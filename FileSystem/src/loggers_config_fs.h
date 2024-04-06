#ifndef LOGGERS_CONFIG_FS_H_
#define LOGGERS_CONFIG_FS_H_

#include "leerConfigFS.h"
#include "pthread.h"
#include "utilidades.h"
#include "operacionesFs.h"





int init_log_config(char *);
//t_config_fs * init_config_fs();
t_config* iniciar_config(char*);
bool configTieneTodasLasPropiedades(t_config *, char **);
bool check_config_properties(char *);

void creacionArchivo(char* );
void aperturaArchivo(char* );
void truncacionArchivo(char* , uint32_t );
void lecturaArchivo(char* , uint32_t , uint32_t );
void escrituraArchivo(char* , uint32_t , uint32_t );
void accesoFat(uint32_t , uint32_t );
void accesoABloqueArchivo(char* , uint32_t , uint32_t );
void accesoSwap(uint32_t );



#endif /* LOGGERS_CONFIG_FS_H_ */
