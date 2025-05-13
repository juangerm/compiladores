#include "anlex.h"

// Variables globales
int numLinea = 1;
token t;
//int indent = 0;
//char linea[1024] = "";
//int nuevaLinea = 1;

void errorLexico(const char* mensaje) {
    printf("Lin %d: Error Lexico. %s.\n", numLinea, mensaje);    
}

int indent = 0;
char bufferLinea[1024] = "";
int necesitaNuevaLinea = 0;

void imprimirLinea(FILE *salida) {
    if (strlen(bufferLinea) > 0) {
        for (int i = 0; i < indent; i++) fprintf(salida, "\t");
        fprintf(salida, "%s\n", bufferLinea);
        bufferLinea[0] = '\0';
        necesitaNuevaLinea = 0;
    }
}

void agregarAToken(const char *texto) {
    if (strlen(bufferLinea) > 0)
        strcat(bufferLinea, " ");
    strcat(bufferLinea, texto);
}

void imprimirToken(int token, FILE *salida) {
    static int nuevaLinea = 1;
    static int indent = 0;
    static int enArray = 0;
    
    switch(token) {
        case L_LLAVE:
            if (!nuevaLinea) fprintf(salida, "\n");
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            fprintf(salida, "L_LLAVE\n");
            indent++;
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            nuevaLinea = 1;
            break;
            
        case L_CORCHETE:
            if (!nuevaLinea) fprintf(salida, " ");
            fprintf(salida, "L_CORCHETE\n");
            enArray = 1;
            //indent++;
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            nuevaLinea = 1;
            break;
            
        case R_LLAVE:
            if (!nuevaLinea) fprintf(salida, "\n");
            indent--;
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            fprintf(salida, "R_LLAVE");
            nuevaLinea = 0;
            enArray = 0;
            break;
            
        case R_CORCHETE:
            if (!nuevaLinea) fprintf(salida, "\n");
            indent--;
            for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            fprintf(salida, "R_CORCHETE");
            nuevaLinea = 0;
            enArray = 0;
            break;
            
        case COMA:
            if (enArray) {
                fprintf(salida, " COMA\n");
                for (int i = 0; i < indent; i++) fprintf(salida, "\t");
            } else {
                fprintf(salida, " COMA\n");
            }
            nuevaLinea = 0;
            break;
            
        case DOS_PUNTOS:
            fprintf(salida, " DOS_PUNTOS");
            nuevaLinea = 0;
            break;
            
        case PR_TRUE:
            fprintf(salida, "PR_TRUE");
            nuevaLinea = 0;
            break;
            
        case PR_FALSE:
            fprintf(salida, "PR_FALSE");
            nuevaLinea = 0;
            break;
            
        case PR_NULL:
            fprintf(salida, "PR_NULL");
            nuevaLinea = 0;
            break;
            
        case STRING:
            if (!nuevaLinea) fprintf(salida, " ");
            fprintf(salida, "STRING");
            nuevaLinea = 0;
            break;
            
        case NUMBER:
            fprintf(salida, " NUMBER");
            nuevaLinea = 0;
            break;
            
        case EOF_TOKEN:
            fprintf(salida, "\n");
            break;
            
        default:
            fprintf(salida, " UNKNOWN");
            nuevaLinea = 0;
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
        
        // Manejo de strings
        if (c == '"') {
            i = 0;
            lexema[i++] = c;
            while((c = fgetc(archivo)) != EOF && c != '"') {
                lexema[i++] = c;
            }
            if (c == '"') lexema[i++] = c;
            lexema[i] = '\0';
            
            t.pe = buscar(lexema);
            if (t.pe->compLex == -1) {
                strcpy(e.lexema, lexema);
                e.compLex = STRING;
                insertar(e);
                t.pe = buscar(lexema);
            }
            t.compLex = STRING;
            return;
        }
        
        // Manejo de números
        if (isdigit(c) || c == '-') {
            i = 0;
            lexema[i++] = c;
            while((c = fgetc(archivo)) != EOF && (isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')) {
                lexema[i++] = c;
            }
            ungetc(c, archivo);
            lexema[i] = '\0';
            
            t.pe = buscar(lexema);
            if (t.pe->compLex == -1) {
                strcpy(e.lexema, lexema);
                e.compLex = NUMBER;
                insertar(e);
                t.pe = buscar(lexema);
            }
            t.compLex = NUMBER;
            return;
        }
        
        // Manejo de símbolos simples
        if (c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || c == ':') {
            char sym[2] = {c, '\0'};
            t.pe = buscar(sym);
            t.compLex = t.pe->compLex;
            return;
        }
        
        // Manejo de palabras reservadas
        if (isalpha(c)) {
            i = 0;
            lexema[i++] = c;
            while((c = fgetc(archivo)) != EOF && isalpha(c)) {
                lexema[i++] = c;
            }
            ungetc(c, archivo);
            lexema[i] = '\0';
            
            t.pe = buscar(lexema);
            if (t.pe->compLex != -1) {
                t.compLex = t.pe->compLex;
            } else {
                errorLexico("Identificador no reconocido");
            }
            return;
        }
    }
    
    t.compLex = EOF_TOKEN;
}
/*
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
    t.compLex = 0;
    while (t.compLex != EOF_TOKEN) {
        getToken(entrada, salida);
        //fprintf(salida, " "); ya se maneja en la funcion imprimir token
    }
    
    fclose(entrada);
    fclose(salida);
    return 0;
}
*/