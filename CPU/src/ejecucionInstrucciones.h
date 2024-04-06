#ifndef EJECUCIONINSTRUCCIONES_H_
#define EJECUCIONINSTRUCCIONES_H_

#include "estructurasCompartidas.h"
#include "cicloInstruccion.h"
#include "utilidades.h"
#include "mmu.h"
#include "comunicacion.h"
#include "utilsClient.h"

extern int tam_pagina;

void SET(char*, char*);
void SUM(char*, char*);
void SUB(char*, char*);
void JNZ(char*, char*);
void SLEEP(char*);

char* obtener_valor_registroCPU(char*);
void cambiar_valor_del_registroCPU(char*, char*);

char* recibir_confirmacion_de_escritura();
char* leer_valor_de_memoria(int direccion_fisica);
char*  recibir_valor_de_memoria();

void ejecutar_WAIT(char* );
void ejecutar_SIGNAL(char*);

void ejecutar_MOV_IN(char*, int);
void ejecutar_MOV_OUT(int, char*);

void ejecutar_F_OPEN(char*, char*);
void ejecutar_F_TRUNCATE(char* , uint32_t);
void ejecutar_F_SEEK(char* , uint32_t);
void ejecutar_F_WRITE(char* , int);
void ejecutar_F_READ(char*, int);
void ejecutar_F_CLOSE(char*);


void ejecutar_EXIT();

#endif
