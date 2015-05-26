#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int main (int argc, char *argv[])
{
	int n;
	char response[25];

	FILE *user_f;

	request_t req;
	response_t rep;

	if (signal (SIGINT, trata_sig) == SIG_ERR) {
		perror ("[ERRO] - Impossivel configurar SIGINT\n");
		exit (EXIT_FAILURE);
	}

	// user_f = open() // abrir ficheiros com utilizadors e passwords

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
		// READ REQUEST
		n = read (server_fd, &req, sizeof (req));
		if (n < sizeof(req)) {
			fprintf(stderr, "\nRequest imcompleto");
			continue;
		}
		printf ("[SERVIDOR] Recebi pedido ... (%d bytes)\n", n);

		// HANDLE REQUEST
		if (!strcmp (req.str, "hello")) {
			sprintf (response, "Hello %s", req.endereco);
			strcpy (rep.str, response);
		} else {
			strcpy (rep.str, "Not a valid command");
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
