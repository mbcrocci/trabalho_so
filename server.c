#include <signal.h>
#include <time.h>

#include "util.h"

int server_fd, client_fd;
int s_inic_lin, s_inic_col;

void init_random_generator (void)
{
	srand ((unsigned) time (NULL));
}

int random_number (int min, int max)
{
	return min + rand () % (max-min+1);
}

// (TODO): APAGAR DEPOIS
void show_user_list (void)
{
	int i;
	printf ("\n[USER LIST]\t[USERS PLAYING]\n");
	for (i = 0; i < MAX_USERS; i++) {
		if (user_list[i].client_pid != 0)
			printf ("%d", user_list[i].client_pid);

		if (users_playing[i].client_pid != 0)
			printf (" \t\t%d", users_playing[i].client_pid);

		if (user_list[i].client_pid != 0 || users_playing[i].client_pid != 0)
			printf ("\n");
	}
	printf ("\n");
}

void show_saco (user_t curr_user)
{
	int i;
	for (i = 0; i < 10; i++)
		printf("%s\n", curr_user.saco[i].nome);
}

void trata_sig (int i)
{
	fprintf (stderr, "\nSERVER TERMINATING\n");

	close (server_fd);
	unlink (SERVER_FIFO);
	exit (EXIT_SUCCESS);
}

user_t new_user (pid_t client_pid)
{
	float peso;
	object_t saco[10];
	memset (&saco[0], 0, sizeof (object_t) *10); // esvaziar o saco de lixo

	// (TODO): o object tem a mesma lin, col que user
	saco[0] = new_object ("aspirina", 0, 0);
	saco[1] = new_object ("faca", 0, 0);

	peso = saco[0].peso + saco[1].peso;

	//(TODO): resolver WARNING -> saco

	return (user_t) {.client_pid=client_pid, .hp=20, .hp_max=30,
					 .saco={*saco}, .peso_saco=peso,
					 .lin=s_inic_lin, .col=s_inic_col };
}
user_t find_user (pid_t client_pid)
{
	int i;
	for (i = 0; i < MAX_USERS; i++)
		if (user_list[i].client_pid == client_pid)
			return user_list[i];
}

// remover utilizador da lista de jogadores ligados ao servidor
void remove_user (pid_t client_pid)
{
	int i, j;
	// encontrar o utilizador a remover
	for (i = 0; i < MAX_USERS; i++)
		if (client_pid == user_list[i].client_pid)
			break;

	for (j = i; j < MAX_USERS-1; j++)
		user_list[j] = user_list[j+1];

	// asegurar que o ultimo ultilzador nao repete ao final do vector
	memset (&user_list[MAX_USERS-1], 0, sizeof (user_t));
}

// remover utilizador da lista de jogadores que estao a jogar
void remove_user_playing (pid_t client_pid)
{
	int i, j;
	// encontrar o utilizador a remover
	for (i = 0; i < MAX_USERS; i++)
		if (client_pid == users_playing[i].client_pid)
			break;

	for (j = i; j < MAX_USERS-1; j++)
		users_playing[j] = users_playing[j+1];

	// asegurar que o ultimo ultilzador nao repete ao final do vector
	memset (&user_list[MAX_USERS-1], 0, sizeof (user_t));
}


object_t new_object (char name[10], int lin, int col)
{
	if (!strcmp ("sandes", name))
		return (object_t) {
			"sandes", .peso=0.5, .raridade=10,
			.f_ataque=0, .f_defesa=0, .max_uso=1,
			.hp_diff=3, .def_diff=0, .lin=lin, .col=col
		};

	if (!strcmp ("aspirina", name))
		return (object_t) {
			"aspirina", .peso=0.1, .raridade=20,
			.f_ataque=0, .f_defesa=0, .max_uso=1,
			.hp_diff=1, .def_diff=0, .lin=lin, .col=col
		};

	if (!strcmp ("xarope", name))
		return (object_t) {
			"xarope", .peso=1, .raridade=4,
			.f_ataque=0, .f_defesa=0, .max_uso=1,
			.hp_diff=4, .def_diff=0, .lin=lin, .col=col
		};

	if (!strcmp ("faca", name))
		return (object_t) {
			"faca", .peso=2, .raridade=5,
			.f_ataque=5, .f_defesa=0, .max_uso=0,
			.hp_diff=0, .def_diff=0, .lin=lin, .col=col
		};

	if (!strcmp ("espada", name))
		return (object_t) {
			"espada", .peso=8, .raridade=3,
			.f_ataque=8, .f_defesa=2, .max_uso=0,
			.hp_diff=0, .def_diff=2, .lin=lin, .col=col
		};

	if (!strcmp ("granada", name))
		return (object_t) {
			"granada", .peso=1, .raridade=2,
			.f_ataque=30, .f_defesa=0, .max_uso=1,
			.hp_diff=-5, .def_diff=0, .lin=lin, .col=col
		};

	if (!strcmp ("escudo", name))
		return (object_t) {
			"escudo", .peso=4, .raridade=4,
			.f_ataque=5, .f_defesa=0, .max_uso=0,
			.hp_diff=0, .def_diff=5, .lin=lin, .col=col
		};

	if (!strcmp ("moeda", name))
		return (object_t) {
			"moeda", .peso=0.1, .raridade=5,
			.f_ataque=0, .f_defesa=0, .max_uso=0,
			.hp_diff=0, .def_diff=0, .lin=lin, .col=col
		};
}

monstro_t new_monster(char *nome, int lin, int col)
{
	int a,d,s;
	object_t loot[5];

	if(!strcmp ("morcego", nome)){
		a = random_number (1,4);
		d = random_number (3,4);
		s = random_number (4,5);
		return (monstro_t){
			"morcego", .atac=a, .def=d,.hp=s,.agress=1,.estado=0,
			.lin=lin, .col=col
		};
	}

	if(!strcmp ("escorpiao", nome)){
		a = random_number(1,7);
		d = random_number(5,7);
		s = random_number (7,9);
		loot[0] = new_object ("moeda", lin, col);
		return (monstro_t){
			"escorpiao", .atac=a, .def=d,.hp=s,.agress=1,.estado=1,
			.loot=*loot, .lin=lin, .col=col
		};
	}

	if(!strcmp ("lobisomem", nome)){
		a = random_number  (5,7);
		d = random_number (5,7);
		s = random_number (7,9);
		loot[0] = new_object ("faca", lin, col);

		return (monstro_t){
			"lobisomem", .atac=a, .def=d,.hp=s,.agress=1,.estado=0,
			.loot=*loot, .lin=lin, .col=col
		};
	}

	if(!strcmp ("urso", nome)){
		a = random_number (8,10);
		d = random_number (10,12);
		loot[0] = new_object ("faca", lin, col);
		loot[1] = new_object ("moeda", lin, col);

		return (monstro_t){
			"urso", .atac=a, .def=d,.hp=10,.agress=0,.estado=1,
			.loot=*loot, .lin=lin, .col=col
		};
	}

	if(!strcmp ("boss", nome)){
		a = random_number (10,12);
		loot[0] = new_object ("moeda", lin, col);
		loot[1] = new_object ("moeda", lin, col);
		loot[2] = new_object ("moeda", lin, col);
		loot[3] = new_object ("moeda", lin, col);
		loot[4] = new_object ("moeda", lin, col);

		return (monstro_t){
			"boss", .atac=a, .def=15,.hp=15,.agress=0,.estado=1,
			.loot=*loot, .lin=lin, .col=col
		};
	}
}

void random_start (void)
{
	int lin, col, p, i, s;
	char obj_name[10];

	init_random_generator ();

	// gerar portas nas salas
	for (lin = 0; lin < 10; lin++) {
		for (col = 0; col < 10; col++) {
			for (p = 0; p < 4; p++) {
				if (lin == 0 && p == 0) // primeira linha nao tem norte
					labirinto[lin][col].portas[0] = 0;

				else if (lin == 9 && p == 1) // ultima linha nao tem sul
					labirinto[lin][col].portas[1] = 0;

				else if (col == 0 && p == 2) // primeira col nao tem este
					labirinto[lin][col].portas[2] = 0;

				else if (col == 9 && p == 3) // ultima col nao tem oeste
					labirinto[lin][col].portas[3] = 0;

				else labirinto[lin][col].portas[p] = random_number(0, 1);
			}
			// descricao aleatoria
			// (TODO): trocar 13 para o numero de descricoes
			s = random_number (0, 13);
			strcpy (labirinto[lin][col].descricao, sala_desc[s]);
		}
	}

	// random sala inicial
	s_inic_lin = random_number (0, 9);
	s_inic_col = random_number (0, 9);

	for (i = 0; i < 10; i++) {
		// random sala
		lin = random_number (0, 9);
		col = random_number (0, 9);

		// random object
		p = random_number (0, 7);
		strcpy (obj_name, obj_names[p]);

		lab_object_list[i] = new_object (obj_name, lin, col);
	}

	for(i = 0; i < MAX_N_MONTROS; i++){
		lin = random_number (0, 9);
		col = random_number (0, 9);

		// 10 morcegos
		if(i < 10)
			monster_list[i] = new_monster ("morcego", lin, col);

		// 10 escorpioes
		else if(9 < i && i < 20)
			monster_list[i] = new_monster ("escorpiao", lin, col);

		// 5 lobisomens
		else if(19 < i && i < 24)
			monster_list[i] = new_monster ("lobisomem", lin, col);

		// 3 ursos
		else if(i < 23 && i < 25)
			monster_list[i] = new_monster ("urso", lin, col);

		// 1 boss
		else if(i == 25)
			monster_list[i] = new_monster ("boss", lin, col);
	}
}

void read_start_file (char *filename)
{
	// Ficheiro
	int f_lin = 0; // linha do ficheiro
	char c;
	FILE *start_fp;

	// labirinto
	int lin = 0, col = 0, p = 0;

	// monstro
	char *m_name;
	int m=0, m_lin, m_col;

	//object
	char *o_name;
	int o=0, o_lin, o_col;

	if ((start_fp = fopen (filename, "r")) == NULL)
		return;

	// contruir as portas do labirinto
	for (f_lin = 0; f_lin < 10; f_lin++) {
		while ((c = fgetc (start_fp)) != '\n') { // ler linha
			switch (c) {
				case ' ': col++; p = 0; break;
				case '.':
					labirinto[lin][col].portas[p] = 0; // nao existe porta
					p++; break;
				case 'P':
					labirinto[lin][col].portas[p] = 1; // existe porta
					p++; break;
			}
		}
		col = 0 ;
		lin++;
	}
	// ao sair do loop dever se encontrar na 10 linha do ficheiro
	if (f_lin != 10) f_lin++;

	// (TODO) Corrigir fscanfs;2

	while (fscanf (start_fp, "%s %d %d", m_name, &m_lin, &m_col) == 3) {
		if (f_lin == 20)
			break;

		// criar monstro e adicionar a lista
		//monster_list[m] = new_monster (m_name, m_lin, m_col);
		f_lin++;
	}

	while (fscanf (start_fp, "%s %d %d", o_name, &o_lin, &o_col) == 3) {
		if (f_lin == 30)
			break;

		lab_object_list[o] = new_object (o_name, o_lin, o_col);
		f_lin++;
	}

	fscanf (start_fp, "%d %d", &s_inic_lin, &s_inic_col);

	fclose (start_fp);
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
		printf ("\n");
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

		if (!strcmp (req.command, "logout")) {
			strcpy (rep.buffer, "LOGOUT");
			remove_user (req.client_pid);
			remove_user_playing (req.client_pid);

		} else if (!strcmp (req.command, "AUTHENTICATE")) {
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
			if (user_list[0].client_pid != curr_user.client_pid)
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
				users_playing[n_us_play] = find_user (curr_user.client_pid);
				n_us_play++;

				curr_user.lin = s_inic_lin;
				curr_user.col = s_inic_col;

				sprintf (rep.buffer, "Encontra-se numa sala %s\nO que pretende fazer?",
						labirinto[s_inic_lin][s_inic_col].descricao);
			}
		} else if (!strcmp (req.command, "sair")) {
			if (curr_user.lin != s_inic_lin || curr_user.col != s_inic_col)
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
		} else if (!strcmp (req.command, "terminar")) {
			// Nao e o primeiro jogador
			if (curr_user.client_pid != user_list[0].client_pid)
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
			// ver se esta a jogar
			for (i = 0; i < n_us_play; i++) {
				if (users_playing[i].client_pid == curr_user.client_pid) {
					strcpy (rep.buffer, "Desistiu");
					remove_user_playing (curr_user.client_pid);
					break;
				}
				else
					strcpy (rep.buffer, "Nao esta a jogar, nao pode desistir");
			}
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
