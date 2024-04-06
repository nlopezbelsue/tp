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
#include "/home/utnso/tp-2023-2c-Operaneta/Cliente-Servidor/src/utilsClient.h"
#include "utilsServer.h"
#include "utilidades.h"
#include "init.h"
#include "ejecuciones.h"
#include "main.h"

typedef struct{

	PCB* pcb;
	uint32_t segundosSleep;

}SleepCpu;

extern t_config *config;

extern int cpuDispatch_fd;
extern int cpuInterrupt_fd;
extern int memoria_fd;
extern int filesystem_fd;

int conectarModuloCPU(char *);
int conectarModuloCPUInterrupt(char *);
int conectarModuloMemoria(char *);
int conectarModuloFilesystem(char *);

void actualizarPcbEjecutado(PCB* pcbRecibida);

void escucharMemoria(void);
void escucharFilesystem(void);
void escucharCPU(void);

void actualizarPcbEjecucion(PCB* pcbRecibida);
PCB* obtenerPcbExec();
void manejoDeRecursos(char*, char*);
void waitRecursoPcb(Recurso* recurso, PCB* unaPcb);
void signalRecursoPcb(Recurso* recurso, PCB* unaPcb);
void moverProceso_BloqrecursoReady(Recurso* recurso);
void bloquearProcesoPorRecurso(Recurso* recurso);

void* esperaTiempo(void*);
void* esperaPageFault(void*);

t_log* iniciar_logger(char*);
void leer_consola(t_log*);
void paquete(t_log*, char*);
void terminar_programa(int, t_log*);
int recibirConexion(char *, int);
void iterator(char* value);
#endif
