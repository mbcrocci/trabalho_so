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

#include "object.h"
#include "user.h"
#include "monster.h"

#define SERVER_FIFO "server_fifo"
#define CLIENT_FIFO "client_%d_fifo"

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

#define MAX_N_SALAS 10
typedef struct {
	int portas[4]; // 0-norte, 1-sul, 2-este, 3-oeste
	char descricao[100];

	int n_obj; // usado para saber quantos objectos existem na sala
	object_t ojectos[5];
} sala;

sala labirinto[10][10];

extern char sala_desc[25][30];

// General functions
void init_random_generator (void);
int random_number (int min, int max);
void clearScreen (void);

#endif
