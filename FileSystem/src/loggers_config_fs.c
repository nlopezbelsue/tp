#include "loggers_config_fs.h"
#include <unistd.h>


t_log * infoLogger;
t_log *errorLogger;
t_log *traceLogger;
bool configCreado;
bool logsCreados;
bool cfgCreado;

t_config* iniciar_config(char* path_config)
{
    t_config* nuevo_config;
    if((nuevo_config = config_create(path_config)) == NULL){
        printf("No pude leer la config");
        exit(2);
    }
    return nuevo_config;
}

int init_log_config(char *path_config){

	configFs = malloc(sizeof(t_config_fs));
	if(configFs == NULL){
		return false;
	}
	cfgCreado= true;
    traceLogger = log_create("trace_logger.log","Filesystem", true, LOG_LEVEL_TRACE);
    //debug_logger = log_create("debug_logger.log","Filesystem", true, LOG_LEVEL_DEBUG);
    //warning_logger = log_create("warning_logger.log","Filesystem", true, LOG_LEVEL_WARNING);
    infoLogger = log_create("info_logger.log","Filesystem", true, LOG_LEVEL_INFO);
    errorLogger = log_create("error_logger.log","Filesystem", true, LOG_LEVEL_ERROR);

    logsCreados = true;

	fileConfigFs = iniciar_config(path_config);
	if(fileConfigFs == NULL){
		return false;
	}

	configCreado = true;
	//check_config_properties(path_config)
	return true;

}

/*
t_config_fs* init_config_fs(){

	t_config_fs *config = malloc(sizeof(t_config_fs));
	return config;

}
*/
bool check_config_properties(char *path){
	 t_config *config = config_create(path);
	    if (config == NULL) {
	        printf("No se pudo crear la config");
	        return false;
	    }


	    char *properties[] = {
	            "IP_MEMORIA",
	            "PUERTO_MEMORIA",
	            "PUERTO_ESCUCHA",
	            "PATH_FAT"
	            "PATH_BLOQUES",
	            "PATH_FCB",
				"CANT_BLOQUES_TOTAL",
				"CANT_BLOQUES_SWAP"
				"TAM_BLOQUE",
	      "RETARDO_ACCESO_BLOQUE",
				"RETARDO_ACCESO_FAT",
	            NULL};

	  // Falta alguna propiedad
	    if (!configTieneTodasLasPropiedades(config, properties)) {
	        log_error(errorLogger, "Propiedades faltantes en el archivo de configuracion");
	        return false;
	    }

	    config_destroy(config);

	    return true;


}

bool configTieneTodasLasPropiedades(t_config *cfg, char **properties)
{
    for (uint8_t i = 0; properties[i] != NULL; i++)
    {
        if (!config_has_property(cfg, properties[i]))
            return false;
    }
    return true;
}

void creacionArchivo(char* nombreArchivo){
    log_info(infoLogger,"Crear Archivo: <%s>", nombreArchivo);
}

void aperturaArchivo(char* nombreArchivo){
    log_info(infoLogger,"Abrir Archivo: <%s>", nombreArchivo);
}

void truncacionArchivo(char* nombreArchivo, uint32_t tamanioArchivo){
    log_info(infoLogger,"Truncar Archivo: <%s> - Tamanio: <%d>", nombreArchivo,tamanioArchivo);
}

void lecturaArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t direccionMemoria){
    log_info(infoLogger,  "Leer Archivo: <%s> - Puntero: <%d> - Memoria: <%d>", nombreArchivo, punteroArchivo, direccionMemoria);
}

void escrituraArchivo(char* nombreArchivo, uint32_t punteroArchivo, uint32_t direccionMemoria){
    log_info(infoLogger,"Escribir Archivo: <%s> - Puntero: <%d> - Memoria: <%d> ", nombreArchivo,punteroArchivo, direccionMemoria);
}

void accesoFat(uint32_t numeroEntrada, uint32_t valorEntrada){
	log_info(infoLogger, "Acceso FAT - Entrada: <%d> - Valor: <%d>", numeroEntrada, valorEntrada);
	usleep(configFs->RETARDO_ACCESO_FAT);



}

void accesoABloqueArchivo(char* nombreArchivo, uint32_t numeroBloqueArchivo, uint32_t numeroBloqueFs){
    log_info(infoLogger,"Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%d> - Bloque File System <%d>", nombreArchivo, numeroBloqueArchivo, numeroBloqueFs);
    usleep(configFs->RETARDO_ACCESO_BLOQUE );
}

void accesoSwap(uint32_t numeroBloque){
	log_info(infoLogger, "Acceso SWAP: <%d>", numeroBloque);
}
