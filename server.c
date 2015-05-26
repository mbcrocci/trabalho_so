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

int find_user (pid_t client_pid, user_t *user)
{
	int i;
	for (i = 0; i < MAX_USERS; i++)
		if (client_pid == user_list[i].client_pid) {
			*user = user_list[i];
			return 1;
		}

	return 0;
}

user_t new_user (pid_t client_pid)
{
	object_t saco[10];
	memset (&saco[0], 0, sizeof (object_t) *10); // esvaziar o saco de lixo

	// (TODO): OBJECT MODEL IS NOT CORRECT

	// (TODO): Corrigir valores de inicializao
	return (user_t) {.client_pid = client_pid, .hp=20, .hp_max=30, .saco=saco};
}

int main (int argc, char *argv[])
{
	int n, i, n_user=0;
	char response[BUFF_SIZE];
	char username[10], password[10];

	FILE *user_fp;

	user_t curr_user;

	request_t req;
	response_t rep;

	if (signal (SIGINT, trata_sig) == SIG_ERR) {
		perror ("[ERRO] - Impossivel configurar SIGINT\n");
		exit (EXIT_FAILURE);
	}

	// nao vale a penar continuar sem poder abrir o fichero
	if ((user_fp = fopen (argv[1], "r")) == NULL) {
		fprintf (stderr, "[ERRO] - Impossivel abrir o ficheiro: %s", argv[1]);
		exit (EXIT_FAILURE);
	}

	if (access (SERVER_FIFO, F_OK) == 0) {
		fprintf (stderr, "[ERRO] - Nao pode existir mais de um server\n");
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
		memset (&req.command[0], 0, sizeof (req.command));
		for (i = 0; i < 3; i++)
			memset (&req.argument[i][0], 0, sizeof (req.argument[i]));

		memset (&rep.buffer[0], 0, sizeof (rep.buffer));

		// READ REQUEST
		n = read (server_fd, &req, sizeof (req));
		if (n < sizeof(req)) {
			fprintf(stderr, "\nRequest imcompleto");
			continue;
		}
		printf ("[READ] Request from %d ... (%d bytes)\n",
			req.client_pid, n);


		// HANDLE REQUEST
		if (!strcmp (req.command, "hello")) {
			sprintf (response, "Hello %d", req.client_pid);
			strcpy (rep.buffer, response);

		} else if (!strcmp (req.command, "AUTHENTICATE")) {
			// (TODO): Ver se ja passou o timeout
			while (fscanf (user_fp, "%s:%s", username, password) == 2) {
				if (!strcmp (username, req.argument[0]) &&
					!strcmp (password, req.argument[1])) {

					strcpy(rep.buffer, "AUTHENTICATED");

					// adicionar utilizador a lista
					user_list[n_user] = new_user (req.client_pid);
					n_user++;
				break;
				}
			}
			fseek (user_fp, 0, SEEK_SET); // ir para o inicio do ficheiro
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
