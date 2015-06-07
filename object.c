#include "util.h"

object_t new_object (char name[10], int lin, int col)
{
    if (!strcmp ("sandes", name))
        return (object_t) {
            "sandes", .peso=0.5, .raridade=10,
            .f_ataque=0, .f_defesa=0, .max_uso=1,
            .hp_diff=3, .def_diff=0, .lin=lin, .col=col
        };

    if (!strcmp ("aspirina", name))
        return (object_t) {
            "aspirina", .peso=0.1, .raridade=20,
            .f_ataque=0, .f_defesa=0, .max_uso=1,
            .hp_diff=1, .def_diff=0, .lin=lin, .col=col
        };

    if (!strcmp ("xarope", name))
        return (object_t) {
            "xarope", .peso=1, .raridade=4,
            .f_ataque=0, .f_defesa=0, .max_uso=1,
            .hp_diff=4, .def_diff=0, .lin=lin, .col=col
        };

    if (!strcmp ("faca", name))
        return (object_t) {
            "faca", .peso=2, .raridade=5,
            .f_ataque=5, .f_defesa=0, .max_uso=0,
            .hp_diff=0, .def_diff=0, .lin=lin, .col=col
        };

    if (!strcmp ("espada", name))
        return (object_t) {
            "espada", .peso=8, .raridade=3,
            .f_ataque=8, .f_defesa=2, .max_uso=0,
            .hp_diff=0, .def_diff=2, .lin=lin, .col=col
        };

    if (!strcmp ("granada", name))
        return (object_t) {
            "granada", .peso=1, .raridade=2,
            .f_ataque=30, .f_defesa=0, .max_uso=1,
            .hp_diff=-5, .def_diff=0, .lin=lin, .col=col
        };

    if (!strcmp ("escudo", name))
        return (object_t) {
            "escudo", .peso=4, .raridade=4,
            .f_ataque=5, .f_defesa=0, .max_uso=0,
            .hp_diff=0, .def_diff=5, .lin=lin, .col=col
        };

    if (!strcmp ("moeda", name))
        return (object_t) {
            "moeda", .peso=0.1, .raridade=5,
            .f_ataque=0, .f_defesa=0, .max_uso=0,
            .hp_diff=0, .def_diff=0, .lin=lin, .col=col
        };
}

void apanha_objecto (int i, pid_t pid)
{
	int u;
	user_t c;
	u = find_user_index (pid);
	c = find_user (pid);

	user_list[u].saco[user_list[u].n_obj] = labirinto[c.lin][c.col].objectos[i];
}
//usa objecto
int usa_objecto (char nome[10], pid_t pid){
	int i, index, o_index;
	index = find_user_index (pid);
	for(i = 0; i < user_list[index].n_obj; i++){
		if (!strcmp (nome, user_list[index].saco[i].nome)){
			user_list[index].hp += user_list[index].saco[i].hp_diff;
		}return 1;
		else{
			printf("objecto nao exsiste\n");
		}return 0;
	}
}

int is_object_name (char name[10])
{
	if (!strcmp (name, "sandes")
		|| !strcmp (name, "aspirina")
		|| !strcmp (name, "xarope")
		|| !strcmp (name, "faca")
		|| !strcmp (name, "espada")
		|| !strcmp (name, "granada")
		|| !strcmp (name, "escudo")
	) return 1;

	return 0;
}
