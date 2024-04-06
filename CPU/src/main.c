#include "main.h"

#include "cicloInstruccion.h"

char *PUERTO_DISPATCH;
char *PUERTO_INTERRUPT;
t_config *config;

int main(int argc, char* argv[]) {

	sem_init(&semaforoI, 0, 1);

	config = crearConfig(argv[1]);
	info_logger = log_create("info_logger.log","Cpu", true, LOG_LEVEL_INFO);
	error_logger = log_create("error_logger.log","Cpu", true, LOG_LEVEL_ERROR);
	PUERTO_DISPATCH = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
	PUERTO_INTERRUPT = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");

	pthread_t tid[3];
	pthread_create(&tid[1], NULL, conectarMemoria, NULL);


	pthread_create(&tid[0], NULL, recibir, NULL);
	pthread_join(tid[1], NULL);
	pthread_create(&tid[2], NULL, recibirInterrupt, NULL);

	pthread_join(tid[0], NULL);
	pthread_join(tid[2], NULL);



	cerar_programa();

	return EXIT_SUCCESS;
}

void *recibir(){
	recibirConexion(PUERTO_DISPATCH);
	return NULL;
}

void *recibirInterrupt(){
	recibirConexionInterrupt(PUERTO_INTERRUPT);
	return NULL;
}

void *conectarMemoria(){
	conectarModulo("MEMORIA");
	return NULL;
}
