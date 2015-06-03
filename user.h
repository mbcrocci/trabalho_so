#ifndef USER_H_
#define USER_H_

#include "util.h"

#define PESO_MAX 20
#define MAX_USERS 10

typedef struct {
    pid_t client_pid; //  Used to find from the request sent from client
    int hp, hp_max;
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
user_t new_user (pid_t client_pid);
int user_is_playing (pid_t client_pid);
int user_is_first (pid_t client_pid);
user_t find_user (pid_t client_pid);
void remove_user (pid_t client_pid);
void remove_user_playing (pid_t client_pid);
void update_position (pid_t client_pid, int lin, int col);
void show_saco (user_t curr_user);

#endif
