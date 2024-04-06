#include "main.h"

void handle_sigint(int sig){
    cerrar_programa();
    printf("test");
    exit(0);
}

//char *path_config = "./FileSystem.config";

int main(int argc, char* argv[]) {
	char* path_config = argv[1];
	printf("%s", path_config);
	signal(SIGINT, handle_sigint);
	if( !init_log_config(path_config) || !cargar_configuracion(path_config)){

		printf("No se pudo cargar la config o los logs\n");

		return EXIT_FAILURE;
	}
  	if(!iniciarFileSystem()){
  		printf("No se pudo inciializar el FileSystem\n");

  		return EXIT_FAILURE;
  	}

  	//pruebasFs();


	pthread_t tid[2];

	pthread_create(&tid[0], NULL, recibir, NULL);
	pthread_create(&tid[1], NULL, conectarMemoria, NULL);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	cerrar_programa();

	return EXIT_SUCCESS;
}

void pruebasFs(){

	crearArchivo("ARCHIVO");

	log_trace(traceLogger, "se ha creado el archivo");

	realizarTruncarArchivo("ARCHIVO", 64);

	log_trace(traceLogger, "Archivo truncado");

//	realizarTruncarArchivo("Hola", 37);
//
//	log_trace(traceLogger, "Archivo aumentado de tamanio");
//
//	realizarTruncarArchivo("Hola", 16);

	log_trace(traceLogger, "Archivo disminuido de tamanio");

	realizarEscrituraArchivo("ARCHIVO", 2, "ES UNA PRUEBA123");

	log_trace(traceLogger, "Archivo escrito");

//	void* datos = leer_archivo("Hola", 1);

//	free(datos);

	iniciarProceso(6);





	log_error(errorLogger, "SOY UN ERROR");
//	escribirBloqueDeSwap("HOLA COMO ESTAS", 0);
//
//	void* data = leerBloqueDeSwap(0);
//
//	free(data);



	//liberar_bitmap();




}
