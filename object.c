object_t new_object (char *name)
{
	int i;
	switch (name) {
	case "sandes":
		return (object_t) {
			.nome=name, .peso=0.5, .raridade=10,
			.f_ataque=0, .f_defesa=0, .max_uso=1,
			.hp_diff=3, .def_diff=0
		}; break;

	case "aspirina":
		return (object_t) {
			.nome=name, .peso=0.1, .raridade=20,
			.f_ataque=0, .f_defesa=0, .max_uso=1,
			.hp_diff=1, .def_diff=0
		}; break;

	case "xarope":
		return (object_t) {
			.nome=name, .peso=1, .raridade=4,
			.f_ataque=0, .f_defesa=0, .max_uso=1,
			.hp_diff=4, .def_diff=0
		}; break;

	case "faca":
		return (object_t) {
			.nome=name, .peso=2, .raridade=5,
			.f_ataque=5, .f_defesa=0, .max_uso=0,
			.hp_diff=0, .def_diff=0
		}; break;

	case "espada":
		return (object_t) {
			.nome=name, .peso=8, .raridade=3,
			.f_ataque=8, .f_defesa=2, .max_uso=0,
			.hp_diff=0, .def_diff=2
		}; break;

	case "granada":
		return (object_t) {
			.nome=name, .peso=1, .raridade=2,
			.f_ataque=30, .f_defesa=0, .max_uso=1,
			.hp_diff=-5, .def_diff=0
		}; break;

	case "escudo":
		return (object_t) {
			.nome=name, .peso=4, .raridade=4,
			.f_ataque=5, .f_defesa=0, .max_uso=0,
			.hp_diff=0, .def_diff=5
		}; break;

	case "moeda":
		return (object_t) {
			.nome=name, .peso=0.1, .raridade=5,
			.f_ataque=0, .f_defesa=0, .max_uso=0,
			.hp_diff=0, .def_diff=0
		}; break;

	default: return NULL;
	}
}
