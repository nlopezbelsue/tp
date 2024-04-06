#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <string.h>
#include "utilsClient.h"
#include "utilsServer.h"
#include "cicloInstruccion.h"
#include "ejecucionInstrucciones.h"
#include "utilidades.h"
#include <semaphore.h>

extern t_config *config;
extern int kernel_fd;
extern int kernel_interrupt_fd;
extern int memoria_fd;
extern t_log* loggerCpuMem;
extern sem_t semaforoI;

int conectarModulo(char *);
t_log* iniciar_logger(char*);
void paquete(int, t_log*);
void terminar_programa(int, t_log*);
void recibirConexion(char *);
void recibirConexionInterrupt(char *);
void iterator(char* value);
#endif
