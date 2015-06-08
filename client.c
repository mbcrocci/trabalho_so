#include <signal.h>
#include "util.h"

void read_alert (int s)
{
	response_t rep;
	int alert_fifo;

	alert_fifo = open (ALERT_FIFO, O_RDONLY | O_NONBLOCK);
	read (alert_fifo, &rep, sizeof(rep));

	fprintf(stderr, " %s\n>> ", rep.buffer);
	close (alert_fifo);

	memset (&rep.buffer[0], 0, sizeof(rep.buffer));
}

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
	
	signal (SIGUSR1, read_alert);

	if (strcmp (rep.buffer, "AUTHENTICATED")) {
		fprintf (stderr, "Username ou  Password incorretos\n");

		close (server_fd);
		unlink (req.endereco);
		exit (EXIT_SUCCESS);
	}

	do {

			// clear buffers
		memset (&req.command[0], 0, sizeof (req.command));
		for (i = 0; i < 3; i++)
			memset (&req.argument[i][0], 0, sizeof (req.argument[i]));
		memset (&rep.buffer[0], 0, sizeof (rep.buffer));


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
			if (!strcmp (word[0], "logout"))
				strcpy (req.command, "logout");

			if (!strcmp (word[0], "jogar"))
				strcpy (req.command, "jogar");

			if (!strcmp (word[0], "sair"))
				strcpy (req.command, "sair");

			if (!strcmp (word[0], "desistir"))
				strcpy (req.command, "desistir");

			if (!strcmp (word[0], "terminar"))
				strcpy (req.command, "terminar");

			if (!strcmp (word[0], "novo")) {
				strcpy (req.command, "novo");

				if (word[1] != NULL)
					strcpy (req.argument[0], word[1]); // timeout

				if (word[2] != NULL)
					strcpy (req.argument[1], word[2]); // dificuldade ou ficheiro

			}

			if (!strcmp (word[0], "ver")){
				strcpy (req.command, "ver");

				if (word[1] != NULL)
					strcpy (req.argument[0], word[1]);
			}

			if (!strcmp (word[0], "mover")) {
				strcpy (req.command, "mover");

				if (word[1] != NULL)
					strcpy (req.argument[0], word[1]);
			}

			if (!strcmp (word[0], "info"))
				strcpy (req.command, "info");

			if (!strcmp (word[0], "quem"))
				strcpy (req.command, "quem");

			if (!strcmp (word[0], "diz")) {
				strcpy (req.command, "diz");
				if (word[1] != NULL) {
					strcpy (req.argument[0], word[1]);
					i = 2;
					while (word[i] != NULL) {
						strcat (req.argument[0], " ");
						strcat (req.argument[0], word[i]);
						i++;
					}
				}
			}

			if (!strcmp (word[0], "grita")) {
				strcpy (req.command, "grita");
				if (word[1] != NULL) {
					strcpy (req.argument[0], word[1]);
					i = 2;
					while (word[i] != NULL) {
						strcat (req.argument[0], " ");
						strcat (req.argument[0], word[i]);
						i++;
					}

				}
			}

			if (!strcmp (word[0], "apanha")){
				strcpy (req.command, "apanha");
				if (word[1] != NULL)
					strcpy (req.argument[0], word[1]);
			}

			if (!strcmp (word[0], "larga")) {
				strcpy (req.command, "larga");
				if (word[1] != NULL)
					strcpy (req.argument[0], word[1]);
			}

			if (!strcmp (word[0], "usa")) {
				strcpy (req.command, "usa");
				if (word[1] != NULL)
					strcpy (req.argument[0], word[1]);
			}

			if (!strcmp (word[0], "ataca")) {
				strcpy (req.command, "ataca");
				if (word[1] != NULL)
					strcpy (req.argument[0], word[1]);

				if (word[2] != NULL)
					strcpy (req.argument[1], word[2]);
			}

			// send request
			n = write (server_fd, &req, sizeof (req));

			// read response
			client_fd = open (req.endereco, O_RDONLY);

			n = read (client_fd, &rep, sizeof (rep));
			fprintf (stderr,"\n%s\n", rep.buffer);

			close (client_fd);

			// Handle LOGOUT (por algum motivo nao esta a sair normalmente)
			if (!strcmp (rep.buffer, "LOGOUT"))
				break;
		}
	} while (strcmp (str, "logout"));

	close (server_fd);
	unlink (req.endereco);

	return 0;
}
