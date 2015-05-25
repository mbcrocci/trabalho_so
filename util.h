#define SERVER_FIFO "server_fifo"
#define CLIENT_FIFO "client_%d_fifo"

typedef struct {
	char endereco[20];
	char str[10];
} request_t;

typedef struct {
	char str[25];
} response_t;


typedef struct {
	char *nome;
	int lin, col;
} object_t;

#define MAX_USERS 10
typedef struct {
	int username;
	int hp, hp_max;
	object_t saco[10];
} user_t;

user_t *user_list;

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