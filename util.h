#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "start.h"

#define SERVER_FIFO "server_fifo"
#define CLIENT_FIFO "client_%d_fifo"
#define ALERT_FIFO "alert_fifo"

#define BUFF_SIZE 25
#define REP_BUFF_SIZE 100

// Variaveis globais
int server_fd, client_fd;
int s_inic_lin, s_inic_col;

typedef struct {
	char endereco[BUFF_SIZE];
	pid_t client_pid;

	char command[BUFF_SIZE]; // comando principal: novo, atacar, mover, etc
	char argument[3][BUFF_SIZE]; // argumentos para o comando principal
} request_t;

typedef struct {
	char buffer[REP_BUFF_SIZE];
} response_t;

// OBJECTOS
#define OBJECT_NUMBER 8
#define OBJECT_NAME_SIZE 10

struct object {
	char nome[OBJECT_NAME_SIZE];
	float peso;
	int raridade, f_ataque, f_defesa, max_uso;
	int hp_diff, def_diff; // "Quando usado" e "Quando Equipado" respctivamente
	int lin, col;
};

typedef struct object object_t;

extern char obj_names[OBJECT_NUMBER][OBJECT_NAME_SIZE];

// Funcoes de objectos
object_t new_object (char name[10], int lin, int col);
int is_object_name (char name[10]);
void apanha_objecto (int i, pid_t client_pid);
void remove_object_sala (int i, int lin, int col);

// MONSTROS
#define MAX_N_MONTROS 27
typedef struct {
    char nome [10];
    int atac, def; // valores de ataque e de defesa (variam)
    int hp;        // vida do monstro
    int agress;    // agressivo=1 ou passivo=0
    int estado;    // quieto=1 ou irrequieto=0
    object_t loot[5]; // 5 pq p boss larga 5 moedas

    int lin, col;
} monstro_t;


// funcoes de montros 
monstro_t new_monster(char *nome, int lin, int col);
int is_monster_name (char name[10]);


#define MAX_N_SALAS 10
typedef struct {
    int portas[4]; // 0-norte, 1-sul, 2-este, 3-oeste
    char descricao[100];

    int n_obj, n_mnt;
    object_t objectos[5];
    monstro_t monstros[3];
} sala;

sala labirinto[10][10];

extern char sala_desc[25][30];

// General functions
void init_random_generator (void);
int random_number (int min, int max);
void clearScreen (void);


// USER
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


// Funcoes de Utilizadores
void show_user_list (void);
user_t new_user (pid_t client_pid, char nome[10]);
int user_is_playing (pid_t client_pid);
int user_is_first (pid_t client_pid);
user_t find_user (pid_t client_pid);
int find_user_index (pid_t client_pid);
int find_user_playing_index (pid_t client_pid);
void remove_user (pid_t client_pid);
void remove_user_playing (pid_t client_pid);
void clear_game (void);
void update_position (pid_t client_pid, int lin, int col);
void show_saco (user_t curr_user);
int mover (pid_t client_pid, int p);
void send_alert (pid_t pid, char mess[80]);


#endif
