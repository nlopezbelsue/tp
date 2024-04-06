#ifndef MAIN_H_
#define MAIN_H_

#include "comunicacion.h"
#include "pthread.h"
#include "pcb.h"
#include "planificador.h"
#include "leerConfigKernel.h"
#include "estructurasCompartidas.h"
#include "consola.h"
#include "init.h"
#include "liberarKernel.h"

extern t_log* info_logger;

void * conectarCPU(void *);
void *conectarCPUInterrupt(void *);
void * conectarMemoria(void *);
void *conectarFilesystem(void *);
void iniciar();

void *escucharMemoriaRef(void *parametro);
void *escucharFilesystemRef(void *parametro);
void *escucharCPURef(void *parametro);


#endif /* MAIN_H_ */
