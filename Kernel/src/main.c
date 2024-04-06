#include "main.h"

t_config *config;
t_log* info_logger;

int main(int argc, char* argv[]){
	info_logger = log_create("info_logger.log","Kernel", true, LOG_LEVEL_INFO);

	config = crearConfig(argv[1]);

	iniciarNecesidades();

	config_destroy(config);

	cerrar_programa();

	return EXIT_SUCCESS;
}

void *conectarMemoria(void *parametro){
	conectarModuloMemoria(parametro);
	return NULL;
}

void *conectarFilesystem(void *parametro){
	conectarModuloFilesystem(parametro);
	return NULL;
}

void *conectarCPU(void *parametro){
	conectarModuloCPU(parametro);
	return NULL;
}

void *conectarCPUInterrupt(void *parametro){
	conectarModuloCPUInterrupt(parametro);
	return NULL;
}

void *escucharFilesystemRef(void *parametro){
	escucharFilesystem();
	return NULL;
}

void *escucharCPURef(void *parametro){
	escucharCPU();
	return NULL;
}
