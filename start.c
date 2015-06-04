#include "start.h"

void random_start (void)
{
	int lin, col, p, i, s;
	char obj_name[10];

	// possiveis descricoes para a sala
	char sala_desc[25][30] = {
	"baixa","com corredor baixo", "com corredor apertado", "fria",
	"alta", "com corredor ventoso" , "escura",
	"com corredor escorregadio", "humida", "com corredor molhado",
	"estreita", "com corredor lamacento", "quente"
	};
	// possiveis nomes para o object
	char obj_names[OBJECT_NUMBER][OBJECT_NAME_SIZE] = {
	"sandes", "aspirina", "xarope", "faca",
	"espada", "granada", "escudo", "moeda" };

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