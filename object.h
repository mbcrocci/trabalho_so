#ifndef OBJECT_H_
#define OBJECT_H_

#include <string.h>
#include "util.h"

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

// tamanho 10 porque o ficheiro de criacao so especifica 10 obj
object_t lab_object_list[10];

extern char obj_names[OBJECT_NUMBER][OBJECT_NAME_SIZE];

// funcoes de objectos
object_t new_object (char name[10], int lin, int col);
int is_object_name (char name[10]);
void apanha_objecto (int i, pid_t client_pid);

#endif
