#include "anlex.h"

// Variables globales
int numLinea = 1;
token t;

void error(const char* mensaje) {
    printf("Lin %d: Error Lexico. %s.\n", numLinea, mensaje);    
}

void imprimirToken(int token, FILE *salida) {
    switch(token) {
        case L_LLAVE: fprintf(salida, "L_LLAVE"); break;
        case R_LLAVE: fprintf(salida, "R_LLAVE"); break;
        case L_CORCHETE: fprintf(salida, "L_CORCHETE"); break;
        case R_CORCHETE: fprintf(salida, "R_CORCHETE"); break;
        case COMA: fprintf(salida, "COMA"); break;
        case DOS_PUNTOS: fprintf(salida, "DOS_PUNTOS"); break;
        case PR_TRUE: fprintf(salida, "PR_TRUE"); break;
        case PR_FALSE: fprintf(salida, "PR_FALSE"); break;
        case PR_NULL: fprintf(salida, "PR_NULL"); break;
        case STRING: fprintf(salida, "STRING"); break;
        case NUMBER: fprintf(salida, "NUMBER"); break;
        case EOF_TOKEN: fprintf(salida, "EOF"); break;
        default: fprintf(salida, "UNKNOWN"); break;
    }
}

void getToken(FILE *archivo, FILE *salida) {
    int i = 0;
    char c = 0;
    char lexema[TAMLEX];
    entrada e;
    
    while((c = fgetc(archivo)) != EOF) {
        if (c == ' ' || c == '\t' || c == '\n') {
            if (c == '\n') numLinea++;
            continue;
        }
        
        // Manejo de strings (entre comillas)
        if (c == '"') {
            i = 0;
            lexema[i++] = c;
            while((c = fgetc(archivo)) != EOF) {
                if (c == '"') {
                    lexema[i++] = c;
                    lexema[i] = '\0';
                    t.pe = buscar(lexema);
                    if (t.pe->compLex == -1) {
                        strcpy(e.lexema, lexema);
                        e.compLex = STRING;
                        insertar(e);
                        t.pe = buscar(lexema);
                    }
                    t.compLex = STRING;
                    imprimirToken(t.compLex, salida);
                    break;
                }
                lexema[i++] = c;
            }
            continue;
        }
        
        // Manejo de números
        if (isdigit(c) || c == '-') {
            i = 0;
            lexema[i++] = c;
            while((c = fgetc(archivo)) != EOF && (isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')) {
                lexema[i++] = c;
            }
            lexema[i] = '\0';
            ungetc(c, archivo);
            
            t.pe = buscar(lexema);
            if (t.pe->compLex == -1) {
                strcpy(e.lexema, lexema);
                e.compLex = NUMBER;
                insertar(e);
                t.pe = buscar(lexema);
            }
            t.compLex = NUMBER;
            imprimirToken(t.compLex, salida);
            continue;
        }
        
        // Manejo de símbolos simples
        if (c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ':') {
            char sym[2] = {c, '\0'};
            t.pe = buscar(sym);
            t.compLex = t.pe->compLex;
            imprimirToken(t.compLex, salida);
            continue;
        }
        
        // Manejo de palabras reservadas (true, false, null)
        if (isalpha(c)) {
            i = 0;
            lexema[i++] = c;
            while((c = fgetc(archivo)) != EOF && isalpha(c)) {
                lexema[i++] = c;
            }
            lexema[i] = '\0';
            ungetc(c, archivo);
            
            t.pe = buscar(lexema);
            if (t.pe->compLex != -1) {
                t.compLex = t.pe->compLex;
                imprimirToken(t.compLex, salida);
            } else {
                fprintf(stderr, "Error: Identificador no reconocido '%s'\n", lexema);
            }
            continue;
        }
    }
    
    if (c == EOF) {
        t.compLex = EOF_TOKEN;
        imprimirToken(t.compLex, salida);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <archivo_entrada> <archivo_salida>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    FILE *salida = fopen(argv[2], "w");
    
    if (!entrada || !salida) {
        printf("Error al abrir los archivos\n");
        return 1;
    }

    initTabla();
    initTablaSimbolos();
    
    while (t.compLex != EOF_TOKEN) {
        getToken(entrada, salida);
        fprintf(salida, " ");
    }
    
    fclose(entrada);
    fclose(salida);
    return 0;
}
