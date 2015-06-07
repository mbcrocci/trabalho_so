#include <signal.h>

#include "util.h"

int N_SEG = 0;

void terminate (int i)
{
	fprintf (stderr, "\nSERVER TERMINATING\n");

	close (server_fd);
	unlink (SERVER_FIFO);
	unlink (ALERT_FIFO);
	exit (EXIT_SUCCESS);
}

void start_timer (int s)
{
	N_SEG++;
	alarm(1);
}

int main (int argc, char *argv[])
{
	int n, i, timeout;
	char response[REP_BUFF_SIZE], aviso_end[BUFF_SIZE];
	char username[10], password[10];
	char line[20];

	int n_user=0, n_us_play=0, game_started=0;

	FILE *user_fp;

	int self_fifo, alert_fifo;

	// variaveis para ver quem tem mais moedas
	int moedas=0, m_moedas=0, i_moedas=0, s;

	user_t curr_user;

	request_t req;
	response_t rep, alert_rep;

	if (signal (SIGINT, terminate) == SIG_ERR) {
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
	if (access (ALERT_FIFO, F_OK) != 0)
		mkfifo (ALERT_FIFO, 0600);

	alert_fifo = open(ALERT_FIFO, O_WRONLY | O_NONBLOCK);
	// abrir para ler, pq se nao e preciso mandar comando diz 2 vezes
	alert_fifo = open (ALERT_FIFO, O_RDONLY | O_NONBLOCK);

	//manter ligado
	self_fifo = open(SERVER_FIFO, O_WRONLY);

	printf ("SERVER STARTED\n");
	while (1) {
		// (TODO): remover
		show_user_list ();

		// manter o alarm a 0 enquanto o jogo nao comecar
		if (!game_started)
			N_SEG = 0;

		// clear buffers
		memset (&req.command[0], 0, sizeof (req.command));
		for (i = 0; i < 3; i++)
			memset (&req.argument[i][0], 0, sizeof (req.argument[i]));

		memset (&rep.buffer[0], 0, sizeof (rep.buffer));
		memset (&rep.buffer[0], 0, sizeof (alert_rep.buffer));

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

		clearScreen ();
		if (!strcmp (req.command, "AUTHENTICATE")) {
			// (TODO): Ver se ja passou o timeout
			// (TODO): ler fichereiro correctamente

			//if (!strcmp (username, req.argument[0]) &&
			//	!strcmp (password, req.argument[1])) {

			strcpy(rep.buffer, "AUTHENTICATED");

			// adicionar utilizador a lista
			user_list[n_user] = new_user (req.client_pid, req.argument[0]);
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
				signal (SIGALRM, start_timer);
				alarm(1);
				timeout = atoi (req.argument[0]);

				moedas++;

				i = atoi (req.argument[1]);
				if (i < 10){
					random_start ();
					game_started = 1;
				}
				else {
					read_start_file (req.argument[1]);
					game_started = 1;
				}

				// Avisar todos os utilizadores
				strcpy (alert_rep.buffer, "Aviso: novo jogo criado utilize o comando \"jogar\" para comecar");
				alert_fifo = open ("alert_fifo", O_WRONLY | O_NONBLOCK);
				write (alert_fifo, &alert_rep, sizeof (alert_rep));
				close (alert_fifo);

				for (i = 0; i < n_user; i++)
					if (user_list[i].client_pid != curr_user.client_pid)
						kill (user_list[i].client_pid, SIGUSR1);

				strcpy (rep.buffer, "Novo jogo criado. Use o comando \"jogar\", para comecar");
			}
		} else if (!strcmp (req.command, "jogar")) {
			if (N_SEG > timeout)
				strcpy (rep.buffer, "Ja nao pode jogar, espere ate o jogador q comece um novo jogo");
			else if (!game_started)
				strcpy (rep.buffer, "O Jogo ainda nao comecou, utilize o comando \"novo\" para comecar");

			else {
				users_playing[n_us_play] = curr_user;
				n_us_play++;

				update_position (curr_user.client_pid, s_inic_lin, s_inic_col);

				sprintf (rep.buffer, "Encontra-se numa sala %s\nO que pretende fazer?",
						labirinto[curr_user.lin][curr_user.col].descricao);
			}
		} else if (!strcmp (req.command, "sair")) {
			if (!user_is_playing (curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta a jogar, nao pode sair");

			else if (curr_user.lin != s_inic_lin || curr_user.col != s_inic_col)
				strcpy (rep.buffer, "Nao esta na sala de inicio");

			else {
				strcpy (rep.buffer, "Saiu do jogo");
				strcpy (alert_rep.buffer, "O jogador ");
				strcat (alert_rep.buffer, curr_user.nome);
				strcat (alert_rep.buffer, " saiu do jogo e tinha ");

				moedas = 0;
				for (i = 0; i < curr_user.n_obj; i++)
					if (!strcmp (curr_user.saco[i].nome, "moeda"))
						moedas = 0;

				sprintf (alert_rep.buffer, "%s%d moedas", alert_rep.buffer, moedas);
				alert_fifo = open ("alert_fifo", O_WRONLY | O_NONBLOCK);
				write (alert_fifo, &alert_rep, sizeof (alert_rep));
				close (alert_fifo);

				for (i = 0; i < n_us_play; i++)
					kill (users_playing[i].client_pid, SIGUSR1);

				clear_game ();
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
			else if (!user_is_playing (curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta a jogar, nao pode terminar o jogo");

			else {
				strcpy (rep.buffer, "Terminou o jogo");
				strcpy (alert_rep.buffer, "O Jogador 1 terminou o jogo.");

				for (i = 0; i < n_us_play; i++) {
					for (s = 0; i < users_playing[i].n_obj; i++)
						if (!strcmp (users_playing[i].saco[s].nome, "moeda"))
							moedas++;
					if (m_moedas < moedas) {
						m_moedas = moedas;
						i_moedas = i;
					}
					moedas = 0;
				}
				strcat (alert_rep.buffer, " Jogador com mais moedas: ");
				strcat (alert_rep.buffer, users_playing[i_moedas].nome);

				alert_fifo = open ("alert_fifo", O_WRONLY | O_NONBLOCK);
				write (alert_fifo, &alert_rep, sizeof (alert_rep));
				close (alert_fifo);

				for (i = 0; i < n_us_play; i++)
					kill (users_playing[i].client_pid, SIGUSR1);

				clear_game ();

				// meter o labirinto a zero (para o server nao crashar)
				memset(labirinto, 0, sizeof(labirinto[0][0]) * 10 * 10);

				n_us_play = 0;
				game_started = 0;
			}
		} else if (!strcmp (req.command, "desistir")){
			if (user_is_playing (curr_user.client_pid)) {
					strcpy (rep.buffer, "Desistiu");
					strcpy (alert_rep.buffer, "O jogador ");
					strcat (alert_rep.buffer, curr_user.nome);
					strcat (alert_rep.buffer, "abandonou o jogo.");

					alert_fifo = open ("alert_fifo", O_WRONLY | O_NONBLOCK);
					write (alert_fifo, &alert_rep, sizeof (alert_rep));
					close (alert_fifo);

					for (i = 0; i < n_us_play; i++)
						if (users_playing[i].client_pid != curr_user.client_pid)
							kill (users_playing[i].client_pid, SIGUSR1);

					remove_user_playing (curr_user.client_pid); n_us_play--;

			} else strcpy (rep.buffer, "Nao esta a jogar, nao pode desistir");

			if (n_us_play == 0)
				game_started = 0;

		} else if (!strcmp (req.command, "info")) {
			if (user_is_playing (curr_user.client_pid)) {
				sprintf (rep.buffer, "HP: %d\nSaco: ", curr_user.hp);
				for (i = 0; i < curr_user.n_obj; i++) {
					strcat (rep.buffer, curr_user.saco[i].nome);
					strcat (rep.buffer, " ");
				}
			} else strcpy (rep.buffer, "Nao esta a jogar");

		} else if (!strcmp (req.command, "ver")) {
			if (!user_is_playing (curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta a jogar.");

			else if (!strcmp (req.argument[0], "")) {
				strcpy (rep.buffer, "Sala:\n    Portas: ");

				for (i = 0; i < 4; i++)
					if (labirinto[curr_user.lin][curr_user.col].portas[i] == 1) {
						if (i == 0)
							strcat (rep.buffer , "norte ");
						if (i == 1)
							strcat (rep.buffer , "sul ");
						if (i == 2)
							strcat (rep.buffer, "este ");
						if (i == 3)
							strcat (rep.buffer, "oeste ");
				}

				strcat (rep.buffer, "\n    Users: ");
				for (i = 0; i < n_us_play; i++) {
				    if (users_playing[i].lin == curr_user.lin
						&& users_playing[i].col == curr_user.col) {
						strcat (rep.buffer, users_playing[i].nome);
						strcat (rep.buffer, " ");
					}
				}

				strcat (rep.buffer, "\nMonstros: ");
				for (i = 0; i < labirinto[curr_user.lin][curr_user.col].n_mnt; i++) {
						strcat (rep.buffer,
								labirinto[curr_user.lin][curr_user.col].monstros[i].nome);
						strcat (rep.buffer, " ");
				}

				strcat (rep.buffer, "\nObjectos: ");
				for (i = 0; i < labirinto[curr_user.lin][curr_user.col].n_obj; i++) {
						strcat (rep.buffer,
								labirinto[curr_user.lin][curr_user.col].objectos[i].nome);
						strcat (rep.buffer, " ");
				}
			} else if (is_monster_name (req.argument[1])) {
				for (i = 0; i < labirinto[curr_user.lin][curr_user.col].n_mnt; i++) {
					if (!strcmp (req.argument[1], labirinto[curr_user.lin][curr_user.col].monstros[i].nome)) {

						sprintf (rep.buffer, "HP: %d\nF_Ataque: %d\nF_Defesa: %d\n",
								labirinto[curr_user.lin][curr_user.col].monstros[i].hp,
								labirinto[curr_user.lin][curr_user.col].monstros[i].atac,
								labirinto[curr_user.lin][curr_user.col].monstros[i].def);
					}
				}
			} else if (is_object_name (req.argument[1])) {
				for (i = 0; i < labirinto[curr_user.lin][curr_user.col].n_obj; i++) {
					if (!strcmp (req.argument[1], labirinto[curr_user.lin][curr_user.col].objectos[i].nome)) {
						strcpy  (rep.buffer, labirinto[curr_user.lin][curr_user.col].objectos[i].nome);
						strcat (rep.buffer, "\nPeso: ");
						sprintf (rep.buffer, "%s %f", rep.buffer,
						   	labirinto[curr_user.lin][curr_user.col].objectos[i].peso);
					}
				}
			} else strcpy (rep.buffer, "Nao existe na sala");
		} else if (!strcmp (req.command, "mover")) {
			if (!user_is_playing (curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta a jogar");

			else if (!strcmp (req.argument[0], ""))
				strcpy (rep.buffer, "Comando imcompleto!");

			else {
				i = 0;
				if (!strcmp (req.argument[0], "norte"))
					i = mover (curr_user.client_pid, 0);
				else if (!strcmp (req.argument[0], "sul"))
					i = mover (curr_user.client_pid, 1);
				else if (!strcmp (req.argument[0], "este"))
					i = mover (curr_user.client_pid, 2);
				else if (!strcmp (req.argument[0], "oeste"))
					i = mover (curr_user.client_pid, 3);

				if (i == 0) strcpy (rep.buffer, "Nao tem essa porta");
				else {
					// update user para ter nova possicao
					curr_user = find_user (curr_user.client_pid);
					sprintf (rep.buffer,
							"Encontra-se numa sala %s\nO que pretende fazer?",
							labirinto[curr_user.lin][curr_user.col].descricao);
				}
			}

		} else if (!strcmp (req.command, "apanha")) {
			if (!strcmp (req.argument[0], ""))
				strcpy (rep.buffer, "Comando Incompleto");

			else {
				for (i = 0; i < labirinto[curr_user.lin][curr_user.col].n_obj; i++) {
					if (!strcmp (req.argument[0],
								labirinto[curr_user.lin][curr_user.col].objectos[i].nome)) {
					
						apanha_objecto (i, curr_user.client_pid);

						strcpy (rep.buffer, "Apanhou ");
						strcat (rep.buffer, req.argument[1]);
						break;
					} else
						strcpy (rep.buffer, "Objecto nao existe na sala");
				}
			}
		} else if (!strcmp (req.command, "larga")) {
			if (!user_is_playing (curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta a jogar");

			else if (!strcmp (req.argument[0], ""))
				strcpy (rep.buffer, "Comando Incompleto");

			else {
				for (i = 0; i < curr_user.n_obj; i++) {
					if (!strcmp (req.argument[0], curr_user.saco[i].nome)) {
					
						larga_objecto (i, curr_user.client_pid);

						strcpy (rep.buffer, "Largou ");
						strcat (rep.buffer, req.argument[1]);
						break;
					} else
						strcpy (rep.buffer, "Objecto nao existe no saco");
				}


			}
		} else if(!strcmp (req.command, "diz")) {
			if (!user_is_playing(curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta a jogar");
			
			else if (!strcmp (req.argument[0], ""))
				strcpy (rep.buffer, "Nao disse nada");

			else {
				strcpy (alert_rep.buffer, curr_user.nome);
				strcat (alert_rep.buffer, " disse: ");
				strcat (alert_rep.buffer, req.argument[0]);

				alert_fifo = open ("alert_fifo", O_WRONLY | O_NONBLOCK);
				write (alert_fifo, &alert_rep, sizeof (alert_rep));
				close (alert_fifo);

				for (i = 0; i < n_us_play; i++)
					if (curr_user.client_pid != users_playing[i].client_pid
						&& users_playing[i].lin == curr_user.lin
						&& users_playing[i].col == curr_user.col) {

						kill (users_playing[i].client_pid, SIGUSR1);	
				}

			}

		} else if (!strcmp (req.command, "grita")) {
			if (!user_is_playing(curr_user.client_pid))
				strcpy (rep.buffer, "Nao esta jogar nao pode gritar");

			else if (!strcmp (req.argument[0], ""))
				strcpy (rep.buffer, "Nao gritou nada");

			else {
				strcpy (alert_rep.buffer, curr_user.nome);
				strcat (alert_rep.buffer, " gritou: ");
				strcat (alert_rep.buffer, req.argument[0]);

				alert_fifo = open (ALERT_FIFO, O_WRONLY | O_NONBLOCK);
				write (alert_fifo, &alert_rep, sizeof(alert_rep));
				close (alert_fifo);

				for (i = 0; i < n_us_play; i++)
					if (users_playing[i].client_pid != curr_user.client_pid)
						kill (users_playing[i].client_pid, SIGUSR1);

				i = find_user_index (curr_user.client_pid);
				user_list[i].hp -= 3;

				i = find_user_playing_index (curr_user.client_pid);
				users_playing[i].hp -= 3;

				strcpy (rep.buffer, "Gritei");
			}
		} else if (!strcmp (req.command, "quem")) {
			strcpy (rep.buffer, "\nUtilizadores logados: ");
			for(i = 0; i < n_user; i++){
				strcat (rep.buffer, user_list[i].nome);
				strcat (rep.buffer, " ");
			}
			strcat (rep.buffer, "\nUtilizadores a jogar: ");
			for (i = 0; i < n_us_play; i++) { 
				strcat (rep.buffer, users_playing[i].nome);
				strcat (rep.buffer, " ");
			}

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

	close (alert_fifo);
	close (server_fd);
	unlink ("alert_fifo");
	unlink (SERVER_FIFO);
	return 0;
}
