#include <signal.h>

#include "util.h"
#include "object.h"
#include "user.h"
#include "monster.h"
#include "start.h"

void trata_sig (int i)
{
	fprintf (stderr, "\nSERVER TERMINATING\n");

	close (server_fd);
	unlink (SERVER_FIFO);
	exit (EXIT_SUCCESS);
}

int main (int argc, char *argv[])
{
	int n, i;
	char response[REP_BUFF_SIZE], aviso_end[BUFF_SIZE];
	char username[10], password[10];
	char line[20];

	int n_user=0, n_us_play=0, game_started=0;

	FILE *user_fp;

	int self_fifo;

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
	//manter ligado
	self_fifo = open(SERVER_FIFO, O_WRONLY);

	printf ("SERVER STARTED\n");
	while (1) {
		// (TODO): remover
		show_user_list ();

		// clear buffers
		memset (&req.command[0], 0, sizeof (req.command));
		for (i = 0; i < 3; i++)
			memset (&req.argument[i][0], 0, sizeof (req.argument[i]));

		memset (&rep.buffer[0], 0, sizeof (rep.buffer));

		// READ REQUEST
		n = read (server_fd, &req, sizeof (req));
		if (n < sizeof(req)) {
			fprintf (stderr, "\nRequest imcompleto");
			continue;
		}
		printf ("[READ] Request from %d ... (%d bytes)\n",
			req.client_pid, n);

		curr_user = find_user (req.client_pid);

		// HANDLE REQUEST

		if (!strcmp (req.command, "AUTHENTICATE")) {
			// (TODO): Ver se ja passou o timeout
			// (TODO): ler fichereiro correctamente

			//if (!strcmp (username, req.argument[0]) &&
			//	!strcmp (password, req.argument[1])) {

			strcpy(rep.buffer, "AUTHENTICATED");

			// adicionar utilizador a lista
			user_list[n_user] = new_user (req.client_pid);

			printf ("[SERVIDOR] - Novo jogador [%s] conhecido por"
					" [jogador %d]\n", req.argument[0], n_user);
			n_user++;

			fseek (user_fp, 0, SEEK_SET); // ir para o inicio do ficheiro

		} else if (!strcmp (req.command, "novo")) {
			if (!user_is_first (curr_user.client_pid))
				strcpy (rep.buffer, "Nao e o utilizador 1, logo nao pode iniciar ");

			else if (game_started)
				strcpy (rep.buffer,
					"Ja existe um jogo a decorrer!\n"
					"Utilize o comando [jogar] para entrar");

			else {
				// (TODO): start timeout
				i = atoi (req.argument[1]); // esta a converter bem
				if (i < 10){
					random_start ();
					game_started = 1;
				}
				else
					read_start_file (req.argument[1]);

				// (TODO): AVISAR TODOS OS UTILIZADORES
				strcpy (rep.buffer, "Novo jogo criado. Use o comando \"jogar\", para comecar");
			}
		} else if (!strcmp (req.command, "jogar")) {
			if (!game_started)
				strcpy (rep.buffer, "O Jogo ainda nao comecou, utilize o comando \"novo\" para comecar");

			else {
				users_playing[n_us_play] = curr_user;
				n_us_play++;

				update_position (curr_user.client_pid, s_inic_lin, s_inic_col);

				sprintf (rep.buffer, "Encontra-se numa sala %s\nO que pretende fazer?",
						labirinto[s_inic_lin][s_inic_col].descricao);
			}
		} else if (!strcmp (req.command, "sair")) {
			if (!user_is_playing (curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta a jogar, nao pode sair");

			else if (curr_user.lin != s_inic_lin || curr_user.col != s_inic_col)
				strcpy (rep.buffer, "Nao esta na sala de inicio");

			else {
				strcpy (rep.buffer, "Saiu do jogo");
				// (TODO): Avisar todos os jogadores
				// 		  e dizer quem saiu e quantas moeads tinha

				for (i = 0; i < n_us_play; i++)
					remove_user_playing (users_playing[i].client_pid);

				n_us_play = 0;
				game_started = 0;
			}
		} else if (!strcmp (req.command, "logout")) {
			strcpy (rep.buffer, "LOGOUT");
			remove_user (req.client_pid); n_user--;
			remove_user_playing (req.client_pid); n_us_play--;

		} else if (!strcmp (req.command, "terminar")) {
			// Nao e o primeiro jogador
			if (!user_is_first (curr_user.client_pid))
				strcpy (rep.buffer, "Nao e o primeiro jogador, "
				        "nao pode terminar o jogo");
			else {
				strcpy (rep.buffer, "Terminou o jogo");
				// (TODO): Avisar todos os jogadores
				// 		   e dizer quem tem mais moedas

				for (i = 0; i < n_us_play; i++)
					remove_user_playing (users_playing[i].client_pid);

				n_us_play = 0;
				game_started = 0;
			}
		} else if (!strcmp (req.command, "desistir")){
			if (user_is_playing (curr_user.client_pid)) {
					strcpy (rep.buffer, "Desistiu");
					remove_user_playing (curr_user.client_pid); n_us_play--;

			} else strcpy (rep.buffer, "Nao esta a jogar, nao pode desistir");

		} else if (!strcmp (req.command, "info")) {
			// (TODO): FIX BUG
			sprintf (rep.buffer, "HP: %d\nSaco: %s, %s", curr_user.hp,
					curr_user.saco[0].nome, curr_user.saco[1].nome);

			show_saco (curr_user);

		} else {
			strcpy (rep.buffer, "Commando Invalido!!!");
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

	fclose (user_fp);
	close (server_fd);
	unlink (SERVER_FIFO);
	return 0;
}
