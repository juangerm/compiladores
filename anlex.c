#include "anlex.h"

// Variables globales
int numLinea = 1;
token t;
int indent = 0;
char linea[1024] = "";
int nuevaLinea = 1;

void error(const char* mensaje) {
    printf("Lin %d: Error Lexico. %s.\n", numLinea, mensaje);    
}

void imprimirToken(int token, FILE *salida) {
    char buffer[64];

    switch (token) {
        case L_LLAVE:
        case L_CORCHETE:
            if (strlen(linea) > 0) {
                for (int i = 0; i < indent; i++) fprintf(salida, "\t");
                fprintf(salida, "%s\n", linea);
                linea[0] = '\0';
            }
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            fprintf(salida, "%s\n", token == L_LLAVE ? "L_LLAVE" : "L_CORCHETE");
            indent++;
            break;

        case R_LLAVE:
        case R_CORCHETE:
            if (strlen(linea) > 0) {
                for (int i = 0; i < indent; i++) fprintf(salida, "\t");
                fprintf(salida, "%s\n", linea);
                linea[0] = '\0';
            }
            indent--;
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            fprintf(salida, "%s\n", token == R_LLAVE ? "R_LLAVE" : "R_CORCHETE");
            break;

        case COMA:
            strcat(linea, " COMA");
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            fprintf(salida, "%s\n", linea);
            linea[0] = '\0';
            break;

        case DOS_PUNTOS:
            strcat(linea, " DOS_PUNTOS");
            break;

        case PR_TRUE:
            strcat(linea, "PR_TRUE");
            break;

        case PR_FALSE:
            strcat(linea, "PR_FALSE");
            break;

        case PR_NULL:
            strcat(linea, "PR_NULL");
            break;

        case STRING:
            if (strlen(linea) > 0) strcat(linea, " ");
            strcat(linea, "STRING");
            break;

        case NUMBER:
            strcat(linea, " NUMBER");
            break;

        case EOF_TOKEN:
            if (strlen(linea) > 0) {
                for (int i = 0; i < indent; i++) fprintf(salida, "\t");
                fprintf(salida, "%s\n", linea);
            }
            fprintf(salida, "EOF\n");
            break;

        default:
            strcat(linea, " UNKNOWN");
            break;
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
        //fprintf(salida, " "); ya se maneja en la funcion imprimir token
    }
    
    fclose(entrada);
    fclose(salida);
    return 0;
}
