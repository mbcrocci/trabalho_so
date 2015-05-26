#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_FIFO "server_fifo"
#define CLIENT_FIFO "client_%d_fifo"

#define BUFF_SIZE 25

typedef struct {
	char endereco[BUFF_SIZE];
	pid_t client_pid;

	char command[BUFF_SIZE]; // comando principal: novo, atacar, mover, etc
	char argument[3][BUFF_SIZE]; // argumentos para o comando principal
} request_t;

typedef struct {
	char buffer[BUFF_SIZE];
} response_t;


typedef struct {
	char *nome;
	int lin, col;
} object_t;

#define MAX_USERS 10
typedef struct {
	pid_t client_pid; //  Used to find from the request sent from client
	int username;
	int hp, hp_max;
	//object_t saco[10];
} user_t;

user_t user_list[MAX_USERS];

#define MAX_N_SALAS 10
typedef struct {
	int portas[4]; // 0-norte, 1-sul, 2-este, 3-oeste
	char descricao[100];
} sala;

sala labirinto[10][10];

#define MAX_N_MONTROS 10
typedef struct {
	char *nome;
	int lin, col;
	int hp; // max 100
} monstro_t;


#define PESO_MAX 20