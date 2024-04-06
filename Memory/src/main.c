#include "main.h"

char *PUERTO;
char *algoritmo;
char* PATH_INSTRUCCIONES;
t_config *config;
t_list* tablaGeneral;
InstruccionPseudo* instrucciones;
t_log* info_logger;
t_list *instruccionesEnMemoria;

int main(int argc, char* argv[]) {
	liberar_conexion(cpu_fd);
	liberar_conexion(kernel_fd);
	liberar_conexion(filesystem_fd);
	info_logger = log_create("info_logger.log","Memory", true, LOG_LEVEL_INFO);
	pthread_t tid[2];
	config = crearConfig(argv[1]);
	PUERTO = config_get_string_value(config, "PUERTO_ESCUCHA");
	algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	PATH_INSTRUCCIONES = config_get_string_value(config, "PATH_INSTRUCCIONES");
	RETARDO_RESPUESTA = config_get_int_value(config, "RETARDO_RESPUESTA");
	tablaGeneral = list_create();
	int marco, marco2;
	instruccionesEnMemoria = list_create();

//	strcpy(parametro, "TAM_PAGINA");
//	crearEspacioContiguoDeMemoria();
	//inicializarBitmap();//Inicializamos memoria
	crearEstructurasAdministrativas();//
	if(strcmp(algoritmo, "FIFO") == 0)
		inicializarColaFIFO();
	else if (strcmp(algoritmo, "LRU") == 0)
		inicializarListaPaginas();
	printf("La cantidad de marcos es: %u\n", CANT_MARCOS);

	pthread_create(&tid[0], NULL, recibir, NULL);
	pthread_join(tid[0], NULL);

	//crearTablaDePaginas(1, 64);

	config_destroy(config);
	if(strcmp(algoritmo, "FIFO") == 0)
		queue_destroy(colaFIFO);
	else if (strcmp(algoritmo, "LRU") == 0)
		list_destroy(paginasConTimestamp);

	list_destroy(tablaGeneral);
	log_destroy(info_logger);
	free(espacio_contiguo);
	return EXIT_SUCCESS;
}

void *recibir(){
	recibirConexion(PUERTO);
	return NULL;
}
