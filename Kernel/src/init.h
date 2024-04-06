#ifndef INIT_H_
#define INIT_H_

#include "comunicacion.h"
#include <commons/log.h>
#include <commons/config.h>
#include <string.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <stdbool.h>

#include "main.h"

extern char* ALGORITMO_PLANIFICACION;
extern int GRADO_MAX_MULTIPROGRAMACION;
extern int QUANTUM;
extern int procesosTotales_MP;
extern int contadorPeticionesFs;
extern int idProcesoGlobal;
extern char** RECURSOS;

extern sem_t sem_procesosEnNew;
extern sem_t sem_procesosReady;
extern sem_t sem_procesosExit;
extern sem_t sem_cpuLibre;

extern bool planificacionFlag;

extern pthread_mutex_t mutex_colaNew;
extern pthread_mutex_t mutex_ColaReady;
extern pthread_mutex_t mutex_colaExec;
extern pthread_mutex_t mutex_colaBloq;
extern pthread_mutex_t mutex_colaExit;
extern pthread_mutex_t mutex_MP;
extern pthread_mutex_t mutex_listaPeticionesArchivos;
extern pthread_mutex_t mutex_contadorPeticionesFs;
extern pthread_mutex_t mutex_debug_logger;
extern pthread_mutex_t mutex_TGAA;

extern pthread_mutex_t* semaforos_io;

extern t_queue* colaNew;
extern t_list* colaExec;
extern t_queue* colaExit;
extern t_list* colaBloq;
extern t_queue* colaReady_FIFO;
extern t_list* estadoBlockRecursos;
extern t_list* listaRecursos;
extern t_list* colaReady;
extern t_list* tablaGlobal_ArchivosAbiertos;
extern t_list* listaPeticionesArchivos;

extern pthread_t hilo_planificador_LP;
extern pthread_t hilo_planificador_corto;

extern pthread_mutex_t planificacionLargo;
extern pthread_mutex_t planificacionCorto;

void iniciarNecesidades();
int tamanioArray(char**);
int cargarRecursos();
void iniciarSemaforoDinamico(pthread_mutex_t*, int);
void cerrar_programa();

#endif
