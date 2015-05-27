#include "util.h"

int main (void)
{
	char str[BUFF_SIZE], *word[8]; // shell
	char username[10], password[10];

	int n, i, server_fd, client_fd;

	request_t req;
	response_t rep;

	if (access (SERVER_FIFO, F_OK) != 0) {
		printf ("[ERRO] - Nao existe servidor\n");
		exit (1);
	}

	req.client_pid = getpid ();
	sprintf (req.endereco, CLIENT_FIFO, getpid ());
	if (mkfifo (req.endereco, 0600)) {
		perror ("\n[ERRO] mkfifo FIFO client");
		exit (EXIT_FAILURE);
	}

	server_fd = open (SERVER_FIFO, O_WRONLY);
	if (server_fd == -1) {
		fprintf (stderr, "\nO servidor nao esta a correr\n");
		unlink (req.endereco);
		exit (EXIT_FAILURE);
	}

	// COMECAR POR AUTENTICAR USER
	printf ("Username: "); fgets (username, sizeof (username), stdin);
	fflush (stdin);
	printf ("Password: "); fgets (password, sizeof (password), stdin);

	// remover os \n
	strtok (username, "\n");
	strtok (password, "\n");

	strcpy (req.command, "AUTHENTICATE");
	strcpy (req.argument[0], username);
	strcpy (req.argument[1], password);

	write (server_fd, &req, sizeof (req));
	client_fd = open (req.endereco, O_RDONLY);
	read (client_fd, &rep, sizeof (rep));
	close (client_fd);

	if (strcmp (rep.buffer, "AUTHENTICATED")) {
		fprintf (stderr, "Username ou  Password incorretos\n");

		close (server_fd);
		unlink (req.endereco);
		exit (EXIT_SUCCESS);
	}

	// clear buffers
	memset (&req.command[0], 0, sizeof (req.command));
	for (i = 0; i < 3; i++)
		memset (&req.argument[i][0], 0, sizeof (req.argument[i]));
	memset (&rep.buffer[0], 0, sizeof (rep.buffer));

	do {
		printf (">> ");
		fgets (str, BUFF_SIZE, stdin);

		str[strlen (str)-1] = '\0';
		word[0] = strtok (str, " ");
		i = 0;
		do {
			i++;
			word[i] = strtok (NULL, " ");
		} while (word[i] != NULL);

		if (word[0] != NULL) {
			if (!strcmp (word[0], "exit"))
				break;

			if (!strcmp (word[0], "hello"))
				strcpy (req.command, "hello");

			if (!strcmp (word[0], "novo")) {
				strcpy (req.command, "novo");
				strcpy (req.argument[0], word[1]); // timeout
				strcpy (req.argument[1], word[2]); // dificuldade ou ficheiro
			}

			// send request
			n = write (server_fd, &req, sizeof (req));
			fprintf (stderr, "[WRITE] - Sent request .. (%d bytes)\n", n);

			client_fd = open (req.endereco, O_RDONLY);

			n = read (client_fd, &rep, sizeof (rep));
			fprintf (stderr, "[READ] -  %s .. (%d bytes)\n", rep.buffer, n);

			close (client_fd);

			// clear buffers
			memset (&req.command[0], 0, sizeof (req.command));
			for (i = 0; i < 3; i++)
				memset (&req.argument[i][0], 0, sizeof (req.argument[i]));
			memset (&rep.buffer[0], 0, sizeof (rep.buffer));

		}
	} while (strcmp (str, "exit"));

	close (server_fd);
	unlink (req.endereco);

	return 0;
}