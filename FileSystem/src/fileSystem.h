#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include "pthread.h"
#include "utilidades.h"
#include "operacionesFs.h"
#include "loggers_config_fs.h"
#include "comunicacion.h"
#include "init.h"

extern t_log * infoLogger;
extern t_log *errorLogger;
extern t_log *traceLogger;
extern t_list* peticiones_pendientes;


void ejecutar_fopen(char* );
void ejecutarCreacionArchivo(char* );
void ejecutarFTruncate(char *, uint32_t);
void ejecutar_iniciar_proceso(uint32_t );
void ejecutar_finalizar_proceso(t_list* );
void ejecutar_finalizarLecturaArchivo(char *);
void ejecutar_finalizarEscrituraArchivo(char* , uint32_t , uint32_t , uint32_t , char* );
void ejecutarFwrite(char* , uint32_t , uint32_t , uint32_t , uint32_t );
void ejecutarFread(char* , uint32_t  ,uint32_t , uint32_t );
void ejecutar_escrituraSwap(void *, uint32_t );
void ejecutar_lecturaSwap(uint32_t );


extern char *PUERTO;

#endif
