#define OBJECT_NUMBER 8
#define OBJECT_NAME_SIZE 10

typedef struct {
	char nome[OBJECT_NAME_SIZE];
	float peso;
	int raridade, f_ataque, f_defesa, max_uso;
	int hp_diff, def_diff; // "Quando usado" e "Quando Equipado" respct.
	int lin, col;
} object_t;

// tamanho 10 porque o ficheiro de criacao so especifica 10 obj
object_t lab_object_list[10];

// funcoes de objectos
object_t new_object (char *, int, int);

char obj_names[OBJECT_NUMBER][OBJECT_NAME_SIZE] = {
	"sandes", "aspirina", "xarope", "faca",
	"espada", "granada", "escudo", "moeda" };