#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include "utilsClient.h"
#include "estructurasCompartidas.h"
#include "pcb.h"
#include "init.h"
#include "utilsServer.h"
#include "planificador.h"

extern int planificadorLargoAvance;
extern int planificadorCortoAvance;

extern char* path;
extern ColaProcesos *cola2;
extern pthread_mutex_t planificacionLargo;
extern pthread_mutex_t planificacionCorto;

PCB* encontrarProceso(uint32_t, uint32_t*);

void *inicializarProceso(void*);
void *finalizarProceso(void*);
void *iniciarConsola();
void iniciar_proceso(char *);
void pedirProceso(char*);
void *mostrarColasPuntero();

#endif /* SRC_CONSOLA_H_ */
