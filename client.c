#include "util.h"

int main (void)
{
	char str[BUFF_SIZE], *word[8];
	int n, i, server_fd, client_fd;

	request_t req;
	response_t rep;

	if (access (SERVER_FIFO, F_OK) != 0) {
		printf ("[ERRO] - Nao existe servidor\n");
		exit (1);
	}

	sprintf (req.endereco, CLIENT_FIFO, getpid());
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
			if (!strcmp (word[0], "hello"))
				strcpy (req.buffer, "hello");

			// send request
			n = write (server_fd, &req, sizeof (req));
			fprintf (stderr, "[WRITE] - Sent request .. (%d bytes)\n", n);

			client_fd = open (req.endereco, O_RDONLY);

			n = read (client_fd, &rep, sizeof (rep));
			fprintf (stderr, "[READ] - Response %s .. (%d bytes)\n", rep.buffer, n);

			close (client_fd);

			// clear buffers
			memset (&req.buffer[0], 0, sizeof (req.buffer));
			memset (&rep.buffer[0], 0, sizeof (rep.buffer));
		}
	} while (strcmp (str, "exit"));

	close (server_fd);
	unlink (req.endereco);

	return 0;
}