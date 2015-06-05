#ifndef USER_H_
#define USER_H_

#include "util.h"

#define PESO_MAX 20
#define MAX_USERS 10

typedef struct {
    pid_t client_pid; //Usado para encontrar/comparar users
	char nome[10];
    int hp, hp_max;
	int n_obj;
    object_t saco[10];
    float peso_saco;

    int lin, col;
} user_t;

// contem os utilizadores ligados ao server
user_t user_list[MAX_USERS];

// contem os utilizadores a jogar
user_t users_playing[MAX_USERS];


// Functions
void show_user_list (void);
user_t new_user (pid_t client_pid, char nome[10]);
int user_is_playing (pid_t client_pid);
int user_is_first (pid_t client_pid);
user_t find_user (pid_t client_pid);
int find_user_index (pid_t client_pid);
void remove_user (pid_t client_pid);
void remove_user_playing (pid_t client_pid);
void clear_game (void);
void update_position (pid_t client_pid, int lin, int col);
void show_saco (user_t curr_user);
int mover (pid_t client_pid, int p);

#endif
