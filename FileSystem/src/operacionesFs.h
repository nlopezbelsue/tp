#ifndef OPERACIONESFS_H_
#define OPERACIONESFS_H_

#include "fileSystem.h"
#include "loggers_config_fs.h"
#include "init.h"
#include "utilsClient.h"
#include <stdint.h>


typedef struct
{
    char* NOMBRE_ARCHIVO;
    uint32_t TAMANIO_ARCHIVO;
    uint32_t BLOQUE_INICIAL;
    t_config* fcb_config;

} t_config_fcb;

typedef struct {
	uint32_t numEntrada;
	uint32_t entradaSig;
	uint32_t bloqueLibre;
} t_entradas;


extern t_log * infoLogger;
extern t_log *errorLogger;
extern t_log *traceLogger;
extern t_config * fileConfigFs;
extern t_list *bitmap;

void *crearArchivo(char *);
void realizarTruncarArchivo(char *, uint32_t);
int existe_FCB(char *);
t_config_fcb* buscarFCBporNombre(char* );
bool iniciarProceso(uint32_t);
t_config_fcb *obtenerPrimerArchivoUsado();
void reservarBloque(int *);
bool rellenarBloque(int );
void finalizarProceso(t_list *);
void realizarEscrituraArchivo(char *, uint32_t , void *);
void* leer_archivo(char *, uint32_t );
bool bloquePerteneceAArchivo(uint32_t , t_config_fcb*);
t_entradas* recibirEntradaFat(int , t_config_fcb* );
void escribirBloqueDeSwap(void* , uint32_t );
void* leerBloqueDeSwap(uint32_t  puntero);
#endif /* OPERACIONESFS_H_ */
