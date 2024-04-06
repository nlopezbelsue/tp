#ifndef INIT_H_
#define INIT_H_


#include "pthread.h"
#include "utilidades.h"
#include <stdint.h>
#include <semaphore.h>

#include "loggers_config_fs.h"
#include <fcntl.h>
#include <sys/mman.h>
#include "limpiarFs.h"

typedef enum {
    PATH_SUPERBLOQUE,
    PATH_BITMAP,
    PATH_BLOQUES,
    PATH_FCB
}tipo_path;

typedef struct{
	char *IP_MEMORIA;
	char *PUERTO_MEMORIA;
	char *PUERTO_ESCUCHA;
	char *PATH_FAT;
	char *PATH_BLOQUES;
	char *PATH_FCB;
	int CANT_BLOQUES_TOTAL;
	int CANT_BLOQUES_SWAP;
	int TAM_BLOQUE;
	int RETARDO_ACCESO_BLOQUE;
	int RETARDO_ACCESO_FAT;
}t_config_fs;


typedef struct {

	void* archivo;
	int tamanioArchivo;
	int fd;

}fatTable;

typedef struct{

	void* archivo;
	int tamanio;
	int fd;

} archBloques;



extern t_list* listaFCBs;
extern t_list* tablaFAT;
extern char* path_config;
extern t_config_fs *configFs;
extern t_log * infoLogger;
extern t_log *errorLogger;
extern t_log *traceLogger;
extern t_list *bitmap;
extern t_list* peticiones_pendientes;
extern archBloques *archivoBloques;

extern fatTable *tFat;

extern t_list* archivosUsados;

extern sem_t contador_peticiones;
extern pthread_mutex_t mutex_peticiones_pendientes;
extern pthread_mutex_t mutex_recvK;
extern pthread_mutex_t mutex_recvM;
extern pthread_mutex_t mutex_ArchivosUsados;

int cargar_configuracion(char *path);
void crearListaFCBs();
bool iniciarFileSystem();
bool crear_bitmap_swap();
bool crearArchivoBloques();
void iniciarArchivoDeBloques();
void *recibir();
void *conectarMemoria();
void initFAT();
bool createFAT();
int sizeFAT(int , int );
void crearSemaforos();
void escribirEnPosicion(archBloques* , int ,  void* , size_t );
#endif /* INIT_H_ */
