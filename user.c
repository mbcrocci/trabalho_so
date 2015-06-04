#include "user.h"

// (TODO): APAGAR DEPOIS
// Percorre a lista de utilizadores  ligados e lista dos a jogar
// e mostra o pid
void show_user_list (void)
{
    int i;
    printf ("\n[USER LIST]\t[USERS PLAYING]\n");
    for (i = 0; i < MAX_USERS; i++) {
        if (user_list[i].client_pid != 0)
            printf ("%s", user_list[i].nome);

        if (users_playing[i].client_pid != 0)
            printf (" \t\t%d", users_playing[i].client_pid);

        if (user_list[i].client_pid != 0 || users_playing[i].client_pid != 0)
            printf ("\n");
    }
    printf ("\n");
}

// Cria um novo utilizador
user_t new_user (pid_t client_pid, char nome[10])
{
    object_t saco[10];
	user_t novo;

    memset (&saco[0], 0, sizeof (object_t) *10); // esvaziar o saco de lixo

	strcpy (novo.nome, nome);

    // (TODO): o object tem a mesma lin, col que user
    novo.saco[0] = new_object ("aspirina", 0, 0);
    novo.saco[1] = new_object ("faca", 0, 0);

	novo.peso_saco = novo.saco[0].peso + novo.saco[1].peso;

    novo.client_pid=client_pid;
	novo.hp=20; novo.hp_max=30;
    novo.lin=s_inic_lin; novo.col=s_inic_col;

	return novo;
}

// Ve se um jogador esta a jogar
int user_is_playing (pid_t client_pid)
{
    int i;
    for (i = 0; i < MAX_USERS; i++)
        if (users_playing[i].client_pid == client_pid)
            return 1;

    return 0;
}

// Ve se e o primeiro jogador
int user_is_first (pid_t client_pid)
{
    if (user_list[0].client_pid == client_pid)
        return 1;

    return 0;
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

void clear_game (void)
{
	int i;
	for (i = 0; i < MAX_USERS; i++)
		remove_user_playing (user_list[i].client_pid);
}

void update_position (pid_t client_pid, int lin, int col)
{
	int i;
	for (i = 0; i < MAX_USERS; i++)
		if (user_list[i].client_pid == client_pid)
			break;

	user_list[i].lin = lin;
	user_list[i].col = col;
}

void show_saco (user_t curr_user)
{
    int i;
    for (i = 0; i < 10; i++)
        printf("%s\n", curr_user.saco[i].nome);
}
