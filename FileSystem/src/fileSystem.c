#include "fileSystem.h"



int CANT_BLOQUES_TOTAL = 1024;

void ejecutar_fopen(char* nombreArchivo){

	if(existe_FCB(nombreArchivo)){

		log_info(infoLogger, "EXISTE EL ARCHIVO %s", nombreArchivo);

		t_config_fcb *aux_FCB = buscarFCBporNombre(nombreArchivo);
		log_info(infoLogger, "SE ABRIO EL ARCHIVO");
    
		enviarString(nombreArchivo, kernel_fd, APERTURA_ARCHIVO_EXITOSA, infoLogger);
	}else{
		enviarString(nombreArchivo, kernel_fd, APERTURA_ARCHIVO_FALLIDA, infoLogger);
	}
	free(nombreArchivo);
}

void ejecutarCreacionArchivo(char* nombreArchivo){
	crearArchivo(nombreArchivo);
	enviarString(nombreArchivo, kernel_fd, CREACION_ARCHIVO_EXITOSA, infoLogger);
	free(nombreArchivo);
}

void ejecutarFTruncate(char * nombreArchivo, uint32_t nuevoTamArchivo){

	realizarTruncarArchivo(nombreArchivo, nuevoTamArchivo);

	uint32_t size = list_size(peticiones_pendientes);
	enviarEnteroYString(size, nombreArchivo, kernel_fd, infoLogger,TRUNCACION_ARCHIVO_EXITOSA);

	free(nombreArchivo);

}

void ejecutar_iniciar_proceso(uint32_t cantidadDeBloquesAAsginar){

	log_info(infoLogger, "Asignando %d bloques", cantidadDeBloquesAAsginar);

	iniciarProceso(cantidadDeBloquesAAsginar);


}


void ejecutar_escrituraSwap(void *datos, uint32_t puntero){

	escribirBloqueDeSwap(datos, puntero);

}

void ejecutar_lecturaSwap(uint32_t puntero){

	accesoSwap(puntero);

	 t_datos* datosAEnviar = malloc(sizeof(t_datos));

	 datosAEnviar->datos = leerBloqueDeSwap(puntero);
	 datosAEnviar->tamanio = configFs->TAM_BLOQUE;
	 t_list* listaInts = list_create();
	 list_add(listaInts, &puntero);

		//log_debug(debug_logger, "Se solicita a memoria escrbir en df");
	 enviarListaIntsYDatos(listaInts, datosAEnviar, memoria_fd, infoLogger, LECTURA_BLOQUE_SWAP);
	 list_clean(listaInts);
	 list_destroy(listaInts);
	 free(datosAEnviar->datos);
	 free(datosAEnviar);

}


void ejecutar_finalizar_proceso(t_list* listaBloquesADesasignar){

	int size = list_size(listaBloquesADesasignar);

	finalizarProceso(listaBloquesADesasignar);

	log_info(infoLogger, "%d bloques desasignados", size);
}

void ejecutarFwrite(char* nombreArchivo, uint32_t pid, uint32_t puntero, uint32_t tamanio, uint32_t direcFisica){

    escrituraArchivo(nombreArchivo, puntero, direcFisica);

    t_list* listaInts = list_create();
    list_add(listaInts,  &direcFisica);
    list_add(listaInts,  &tamanio);
    list_add(listaInts, &pid);
    list_add(listaInts, &puntero);

    //log_debug(debug_logger, "se solicita a memoria leer en df");
    enviarListaUint32_t(listaInts,memoria_fd,infoLogger, ACCESO_PEDIDO_ESCRITURA);

    list_clean(listaInts);
    list_destroy(listaInts);
    free(nombreArchivo);

}

void ejecutar_finalizarLecturaArchivo(char *nombreArchivo){

	uint32_t size = list_size(peticiones_pendientes);

	 enviarEnteroYString(size, nombreArchivo, kernel_fd, infoLogger, LECTURA_ARCHIVO_EXITOSA);

	 free(nombreArchivo);
}

void ejecutar_finalizarEscrituraArchivo(char* nombreArchivo, uint32_t puntero, uint32_t tamanio, uint32_t direccionFisica, char* datos){

	 escrituraArchivo(nombreArchivo, puntero, direccionFisica);

	 realizarEscrituraArchivo(nombreArchivo, puntero, datos);


	 uint32_t size = list_size(peticiones_pendientes);

	 enviarEnteroYString(size ,nombreArchivo, kernel_fd, infoLogger, ESCRITURA_ARCHIVO_EXITOSA);

	 free(datos);
	 free(nombreArchivo);
}

void ejecutarFread(char* nombreArchivo, uint32_t pid ,uint32_t punteroArchivo, uint32_t direccionMemoria){

	lecturaArchivo(nombreArchivo, punteroArchivo, direccionMemoria);

	 t_datos* datosAEnviar = malloc(sizeof(t_datos));

	 datosAEnviar->datos = leer_archivo(nombreArchivo, punteroArchivo);
	 datosAEnviar->tamanio = configFs->TAM_BLOQUE;
	 t_list* listaInts = list_create();
	 list_add(listaInts, &direccionMemoria);
	 list_add(listaInts, &pid);

	    //log_debug(debug_logger, "Se solicita a memoria escrbir en df");
	 enviarListaIntsYDatos(listaInts,datosAEnviar,memoria_fd,infoLogger,ACCESO_PEDIDO_LECTURA);
	 list_clean(listaInts);
	 list_destroy(listaInts);
	 free(nombreArchivo);
	 free(datosAEnviar->datos);
	 free(datosAEnviar);

}
