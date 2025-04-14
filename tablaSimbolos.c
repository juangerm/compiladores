#include "anlex.h"

entrada *tabla;
int tamTabla = TAMHASH;
int elems = 0;

int h(const char* k, int m) {
    unsigned h = 0, g;
    for (int i = 0; i < strlen(k); i++) {
        h = (h << 4) + k[i];
        if ((g = h & 0xf0000000)) {
            h ^= (g >> 24);
            h ^= g;
        }
    }
    return h % m;
}

void initTabla() {
    int i;
    tabla = (entrada*)malloc(tamTabla * sizeof(entrada));
    for(i = 0; i < tamTabla; i++) {
        tabla[i].compLex = -1;
    }
}

void insertar(entrada e) {
    int pos;
    if (++elems >= tamTabla / 2) {
        fprintf(stderr, "Error: Tabla llena, necesita implementar rehash\n");
        exit(1);
    }
    pos = h(e.lexema, tamTabla);
    while (tabla[pos].compLex != -1) {
        pos = (pos + 1) % tamTabla;
    }
    tabla[pos] = e;
}

entrada* buscar(const char *clave) {
    int pos = h(clave, tamTabla);
    while (tabla[pos].compLex != -1 && strcmp(tabla[pos].lexema, clave) != 0) {
        pos = (pos + 1) % tamTabla;
    }
    return &tabla[pos];
}

void initTablaSimbolos() {
    insertar((entrada){L_LLAVE, "{"});
	insertar((entrada){R_LLAVE, "}"});
	insertar((entrada){L_CORCHETE, "["});
	insertar((entrada){R_CORCHETE, "]"});
	insertar((entrada){COMA, ","});
	insertar((entrada){DOS_PUNTOS, ":"});
	insertar((entrada){PR_TRUE, "true"});
	insertar((entrada){PR_FALSE, "false"});
	insertar((entrada){PR_NULL, "null"});
}