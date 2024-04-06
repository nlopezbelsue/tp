#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>
#include "comunicacion.h"
#include "pthread.h"
#include "leerConfigMemory.h"
#include "estructurasCompartidas.h"
#include "esquema.h"
#include <commons/collections/queue.h>

extern t_config* config;

extern int TAM_PAGINA;
extern int TAM_MEMORIA;
extern int CANT_MARCOS;
extern int RETARDO_RESPUESTA;

extern void* espacio_contiguo;

extern bool memoriaInicializada;
extern bool tablaPaginasCreada;
extern bool pagina0Creada;
extern bool semaforosCreados;

extern t_list* tablasPaginas;
extern t_list* paginasConTimestamp;
extern t_list* tablaGeneral;

extern t_queue* colaFIFO;

extern pthread_mutex_t mutex_espacioContiguo;
extern pthread_mutex_t mutex_tablasPaginas;
extern pthread_mutex_t mutex_idPagina;
extern pthread_mutex_t mutex_espacioDisponible;

extern t_log* trace_logger;
extern t_log* debug_logger;
extern t_log* info_logger;
extern t_log* warning_logger;
extern t_log* error_logger;
extern t_list* instruccionesEnMemoria;

extern char* PATH_INSTRUCCIONES;

void *recibir();
void *conectarFS();

#endif /* MAIN_H_ */
