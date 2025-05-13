#include "anlex.h"

FILE *archivoFuente;

void match(int esperado);
void json();
void element();
void object();
void array();
void attributes_list();
void attribute();
void element_list();
void attribute_value();

void error(const char* mensaje) {
    printf("Error sintáctico en línea %d: %s. Encontrado: %s\n", numLinea, mensaje, t.pe->lexema);
    exit(1); 
}

void match(int esperado) {
    if (t.compLex == esperado) {
        getToken(archivoFuente, NULL);
    } else {
        char mensaje[100];
        sprintf(mensaje, "Se esperaba %d", esperado);
        error(mensaje);
    }
}

void json() {
    getToken(archivoFuente, NULL); 
    element();
    if (t.compLex != EOF_TOKEN)
        error("Se esperaba fin de archivo");
    else
        printf("Fuente sintácticamente correcta\n");
}

void element() {
    if (t.compLex == L_LLAVE) {
        object();
    } else if (t.compLex == L_CORCHETE) {
        array();
    } else {
        error("Se esperaba objeto o arreglo");
    }
}

void object() {
    match(L_LLAVE);
    if (t.compLex == R_LLAVE) {
        match(R_LLAVE);
    } else {
        attributes_list();
        match(R_LLAVE);
    }
}

void attributes_list() {
    attribute();
    while (t.compLex == COMA) {
        match(COMA);
        attribute();
    }
}

void attribute() {
    if (t.compLex == STRING) {
        match(STRING);
        match(DOS_PUNTOS);
        attribute_value(); 
    } else {
        error("Se esperaba una cadena como nombre de atributo");
    }
}

void attribute_value() {
    switch (t.compLex) {
        case STRING: match(STRING); break;
        case NUMBER: match(NUMBER); break;
        case PR_TRUE: match(PR_TRUE); break;
        case PR_FALSE: match(PR_FALSE); break;
        case PR_NULL: match(PR_NULL); break;
        case L_LLAVE: object(); break;
        case L_CORCHETE: array(); break;
        default: error("Valor de atributo no válido");
    }
}

void array() {
    match(L_CORCHETE);
    if (t.compLex == R_CORCHETE) {
        match(R_CORCHETE);
    } else {
        element_list();
        match(R_CORCHETE);
    }
}

void element_list() {
    element();
    while (t.compLex == COMA) {
        match(COMA);
        element();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <archivo fuente>\n", argv[0]);
        return 1;
    }

    archivoFuente = fopen(argv[1], "r");
    if (!archivoFuente) {
        printf("No se pudo abrir el archivo %s\n", argv[1]);
        return 1;
    }

    initTabla();
    initTablaSimbolos();

    json(); 

    fclose(archivoFuente);
    return 0;
}