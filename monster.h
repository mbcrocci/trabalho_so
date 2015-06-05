#ifndef MONSTER_H_
#define MONSTER_H_

#include "util.h"

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

// lista de monstros
monstro_t monster_list[MAX_N_MONTROS];

// Functions
monstro_t new_monster(char *nome, int lin, int col);
int is_monster_name (char name[10]);

#endif
