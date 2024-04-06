#include "liberarKernel.h"

void cerrar_programa(){
	liberarRecursosKernel();

	liberarEstadosKernel();

	liberarSemaforos();

	liberarManejoFs();

	liberarSemaforoDinamico();

	config_destroy(config);

	cortar_conexiones();

	log_destroy(info_logger);

	printf("\nCierre exitoso\n");
}

void liberarEstadosKernel(){
    pthread_mutex_lock(&mutex_colaNew);
    queue_destroy_and_destroy_elements(colaNew,liberarPcb);
    pthread_mutex_unlock(&mutex_colaNew);

    pthread_mutex_lock(&mutex_ColaReady);
    list_destroy_and_destroy_elements(colaReady,liberarPcb);
    pthread_mutex_unlock(&mutex_ColaReady);

    pthread_mutex_lock(&mutex_colaBloq);
    list_destroy_and_destroy_elements(colaBloq,liberarPcb);
    pthread_mutex_unlock(&mutex_colaBloq);

    pthread_mutex_lock(&mutex_colaExec);
    list_destroy_and_destroy_elements(colaExec,liberarPcb);
    pthread_mutex_unlock(&mutex_colaExec);

    pthread_mutex_lock(&mutex_colaExit);
    queue_destroy_and_destroy_elements(colaExit,liberarPcb);
    pthread_mutex_unlock(&mutex_colaExit);
}

void liberarSemaforos(){
    pthread_mutex_destroy(&mutex_colaNew);
    pthread_mutex_destroy(&mutex_ColaReady);
    pthread_mutex_destroy(&mutex_colaExec);
    pthread_mutex_destroy(&mutex_colaBloq);
    pthread_mutex_destroy(&mutex_colaExit);
    pthread_mutex_destroy(&mutex_TGAA);
    pthread_mutex_destroy(&mutex_listaPeticionesArchivos);
    pthread_mutex_destroy(&mutex_MP);
    pthread_mutex_destroy(&mutex_debug_logger);
    pthread_mutex_destroy(&mutex_contadorPeticionesFs);
    pthread_mutex_destroy(&planificacionLargo);
    pthread_mutex_destroy(&planificacionCorto);

    sem_destroy(&sem_procesosEnNew);
    sem_destroy(&sem_procesosReady);
    sem_destroy(&sem_procesosExit);
    sem_destroy(&sem_cpuLibre);
}

void liberarSemaforoDinamico(){
    int dim = tamanioArray(RECURSOS);
    for(int i = 0; i < dim; i++)
        pthread_mutex_destroy(&semaforos_io[i]);
}

void cortar_conexiones(){
    liberar_conexion(cpuInterrupt_fd);//LES SACO &
    liberar_conexion(cpuDispatch_fd);
    liberar_conexion(memoria_fd);
    liberar_conexion(filesystem_fd);
}

void liberarRecurso(void* recursoVoid){
	Recurso* recurso = (Recurso*) recursoVoid;
    list_destroy_and_destroy_elements(recurso->cola,liberarPcb);
    free(recurso);
}

void liberarManejoFs(){
    list_destroy_and_destroy_elements(tablaGlobal_ArchivosAbiertos,liberarArchivoPeticion);
    list_destroy_and_destroy_elements(listaPeticionesArchivos,free);
}

void liberarArchivoPeticion(void* archivoVoid){
	t_archivoPeticion* archivoPeticion = (t_archivoPeticion*) archivoVoid;
    free(archivoPeticion->archivo->nombreArchivo);
    free(archivoPeticion->archivo);
    free(archivoPeticion);
}

void liberarRecursosKernel(){
    list_destroy_and_destroy_elements(estadoBlockRecursos,liberarRecurso);
}
