#ifndef ANLEX_H
#define ANLEX_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TAMHASH 101
#define TAMLEX 50

// Tokens JSON
#define L_LLAVE 256
#define R_LLAVE 257
#define L_CORCHETE 258
#define R_CORCHETE 259
#define COMA 260
#define DOS_PUNTOS 261
#define PR_TRUE 262
#define PR_FALSE 263
#define PR_NULL 264
#define STRING 265
#define NUMBER 266
#define EOF_TOKEN 267

typedef struct entrada {
    int compLex;
    char lexema[TAMLEX];
} entrada;

typedef struct {
    int compLex;
    entrada *pe;
} token;

// Declaraciones de funciones
void initTabla(void);
void initTablaSimbolos(void);
void insertar(entrada e);
entrada* buscar(const char *clave);
void getToken(FILE *archivo, FILE *salida);
void imprimirToken(int token, FILE *salida);

//Declaraciones externas
extern entrada *tabla;
extern int tamTabla;
extern int elems;
extern int numLinea;
extern token t;

#endif