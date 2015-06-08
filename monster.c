#include "util.h"

monstro_t new_monster(char nome[10])
{
    int a,d,s;
    object_t loot[5];

    if(!strcmp ("morcego", nome)){
        a = random_number (1,4);
        d = random_number (3,4);
        s = random_number (4,5);
        return (monstro_t){
            "morcego", .atac=a, .def=d,.hp=s,.agress=1,.estado=0
        };
    }

    if(!strcmp ("escorpiao", nome)){
        a = random_number(1,7);
        d = random_number(5,7);
        s = random_number (7,9);
        loot[0] = new_object ("moeda");
        return (monstro_t){
            "escorpiao", .atac=a, .def=d,.hp=s,.agress=1,.estado=1,
            .loot={*loot}
        };
    }

    if(!strcmp ("lobisomem", nome)){
        a = random_number  (5,7);
        d = random_number (5,7);
        s = random_number (7,9);
        loot[0] = new_object ("faca");

        return (monstro_t){
            "lobisomem", .atac=a, .def=d,.hp=s,.agress=1,.estado=0,
            .loot={*loot}
        };
    }

    if(!strcmp ("urso", nome)){
        a = random_number (8,10);
        d = random_number (10,12);
        loot[0] = new_object ("faca");
        loot[1] = new_object ("moeda");

        return (monstro_t){
            "urso", .atac=a, .def=d,.hp=10,.agress=0,.estado=1,
            .loot={*loot}
        };
    }

    if(!strcmp ("boss", nome)){
        a = random_number (10,12);
        loot[0] = new_object ("moeda");
        loot[1] = new_object ("moeda");
        loot[2] = new_object ("moeda");
        loot[3] = new_object ("moeda");
        loot[4] = new_object ("moeda");

        return (monstro_t){
            "boss", .atac=a, .def=15,.hp=15,.agress=0,.estado=1,
            .loot={*loot}
        };
    }
}

int is_monster_name (char name[10])
{
	if (!strcmp (name, "morcego")
		|| !strcmp (name, "escorpiao")
		|| !strcmp (name, "lobisomem")
		|| !strcmp (name, "urso")
		|| !strcmp (name, "boss")
	) return 0;

	return 1;
}

void remove_monster (int i, int lin, int col)
{
	int j;
	for (j = i; j < labirinto[lin][col].n_mnt - 1; j++)
		labirinto[lin][col].monstros[j] = labirinto[lin][col].monstros[j+1];

	memset (&labirinto[lin][col].monstros[labirinto[lin][col].n_mnt],
			0, sizeof (monstro_t));

	labirinto[lin][col].n_mnt--;	
}
