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
#include <pthread.h>
#include "utilsClient.h"
#include "utilsServer.h"
#include "utilidades.h"
#include "init.h"
#include "operacionesFs.h"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>

extern t_config *config;

extern int memoria_fd;
extern int kernel_fd;
extern int filesystem_fd;


typedef enum{
    EJECUTAR_OPEN,
    EJECUTAR_CREACION,
    EJECUTAR_TRUNCATE,
    EJECUTAR_READ,
    EJECUTAR_WRITE,
    EJECUTAR_RESPUESTA_LECTURA,
    EJECUTAR_RESPUESTA_ESCRITURA,
	EJECUTAR_INICIAR_PROCESO,
	EJECUTAR_FINALIZAR_PROCESO,
	EJECUTAR_ESCRITURA_SWAP,
	EJECUTAR_LECTURA_SWAP
}t_operacion_fs;

typedef struct{
    t_operacion_fs operacion;
    char* nombre;
    uint32_t tamanio;
    uint32_t direccionFisica;
    uint32_t puntero;
    uint32_t pid;
    void* datos;
    t_list* bloquesADesasignarSwap;
}t_peticion;


int conectarModulo(char *);
t_log* iniciar_logger(char*);
void leer_consola(t_log*);
void paquete(int, t_log*);
void terminar_programa(int, t_log*);
int recibirConexion(char *);
void *recibirMemoria();
void *recibirKernel();
void iterator(char* value);
void* escribirArchivo(void * );
void* truncarArchivo(void *);
void* abrirArchivo(void *);
void* finalizarEscrituraArchivo(void* );
t_peticion* crear_peticion(t_operacion_fs , char* , uint32_t , uint32_t , uint32_t , void* );
t_peticion* crear_peticionTruncate(t_operacion_fs , char* , uint32_t , uint32_t , uint32_t , uint32_t , void* );
t_peticion* crear_peticion_fin_proceso(t_operacion_fs , t_list *);
void agregarPeticionAPendientes(t_peticion* );

t_peticion* sacoPeticionDePendientes();
void manejar_peticion(t_peticion* );
void* truncarArchivo(void* );
void* leerArchivo(void* );
void* iniciar_proceso(void* );
void* finalizar_proceso(void* );
void iniciar_atencion_peticiones();
void atender_peticiones();
void* finalizarLecturaArchivo(void* );
void *escribirEnSwap(void* );
void *leerEnSwap(void* );
void *fileCreate(void *);


#endif
