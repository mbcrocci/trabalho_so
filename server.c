#include <signal.h>

#include "util.h"

int server_fd, client_fd;

void trata_sig (int i)
{
	fprintf (stderr, "\nSERVER TERMINATING\n");

	close (server_fd);
	unlink (SERVER_FIFO);
	exit (EXIT_SUCCESS);
}

user_t find_user (pit_t client_pid)
{
	int i;
	for (i = 0; i < MAX_USERS; i++)
		if (client_pid == user_list[i].client_pid)
			return user_list[i];

	return NULL;
}

int main (int argc, char *argv[])
{
	int n;
	char response[BUFF_SIZE];

	FILE *user_fp;

	user_t curr_user;

	request_t req;
	response_t rep;

	if (signal (SIGINT, trata_sig) == SIG_ERR) {
		perror ("[ERRO] - Impossivel configurar SIGINT\n");
		exit (EXIT_FAILURE);
	}

	user_fp = open (argv[1], 'r');

	if (access (SERVER_FIFO, F_OK) == 0) {
		printf ("[ERRO] - Nao pode existir mais de um server\n");
		exit (EXIT_FAILURE);
	}

	n = mkfifo (SERVER_FIFO, 0600);
	if (n == -1) {
		perror ("\n[ERRO] - mkfifo failed");
		exit (EXIT_FAILURE);
	}

	server_fd  = open(SERVER_FIFO, O_RDONLY);
	if (server_fd == -1) {
		perror ("\n[ERRO] - Impossivel abrir fifo do server");
		exit (EXIT_FAILURE);
	}

	// Fazer labirinto

	printf ("SERVER STARTED\n");

	while (1) {
		// clear buffers
		memset (&req.command[0], 0, sizeof (req.buffer));
		for (i = 0; i < 3; i++)
			memset (&rep.arguement[i][0], 0, sizeof (rep.buffer));


		// READ REQUEST
		n = read (server_fd, &req, sizeof (req));
		if (n < sizeof(req)) {
			fprintf(stderr, "\nRequest imcompleto");
			continue;
		}
		printf ("[READ] Request from %d ... (%d bytes)\n",
			req.client_pid, n);

		if (!strcmp ())

		// HANDLE REQUEST
		if (!strcmp (req.buffer, "hello")) {
			sprintf (response, "Hello %d", req.client_pid);
			strcpy (rep.buffer, response);
		} else {
			strcpy (rep.buffer, "Not a valid command");
		}

		// SEND RESPONSE
		client_fd = open (req.endereco, O_WRONLY);
		if (client_fd == -1)
			perror ("\n[ERRO] - No response");
		else {
			n = write (client_fd, &rep, sizeof(rep));
 			printf ("[SERVIDOR] Enviei resposta ... (%d bytes)\n", n);
			close (client_fd);
		}
	}

	close (server_fd);
	unlink (SERVER_FIFO);
	return 0;
}
