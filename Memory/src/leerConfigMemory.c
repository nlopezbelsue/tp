#include "leerConfigMemory.h"

t_config *crearConfig(char* configPath){
	t_config* config;

	config = config_create(configPath);

	if(config == NULL){
		printf("La configuracion no se pudo crear exitosamente");
		exit(2);
	}

	return config;
}

