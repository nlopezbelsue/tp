#include "planificador.h"

int proteccion=1;

void planificadorLargoPlazo(){
    log_info(info_logger, "Kernel - PLANIFICADOR LARGO PLAZO INICIADO.\n");
    while(planificacionFlag){
        pthread_mutex_lock(&planificacionLargo);

        log_info(info_logger, "%d %d %d", memoria_fd,filesystem_fd,cpuDispatch_fd);

        sem_wait(&sem_procesosEnNew);
        if(!planificacionFlag)
        	planificadorLargoAvance=1;

        if(procesosTotales_MP < GRADO_MAX_MULTIPROGRAMACION && queue_size(colaNew) > 0 && planificacionFlag) {
            pthread_mutex_lock(&mutex_colaNew);
            PCB* pcbAReady = queue_peek(colaNew);
            pthread_mutex_unlock(&mutex_colaNew);
            aumentarGradoMP();

            enviar_uint32_y_uint32_y_char(pcbAReady->nombreRecurso,pcbAReady->id,pcbAReady->size,memoria_fd,INICIALIZAR_PROCESO_MEMORIA,info_logger);
            //free(pcbAReady->nombreRecurso);
            int cod_op = recibir_operacion(memoria_fd);

            if(cod_op == INICIALIZAR_PROCESO_MEMORIA){
            	recibirOrden(memoria_fd);
            	moverProceso_NewReady();
            	if(queue_size(colaNew)==0 || GRADO_MAX_MULTIPROGRAMACION == procesosTotales_MP)
            		sem_post(&sem_procesosReady);
            	else
            		sem_post(&sem_procesosEnNew);
            }
            log_info(info_logger,"PID <%d> Enviado a memoria para ser cargado", pcbAReady->id);
        }
        pthread_mutex_unlock(&planificacionLargo);
    }
}

void planificadorCortoPlazo(){
    log_info(info_logger, "Kernel - PLANIFICADOR CORTO PLAZO INICIADO.");
    while(planificacionFlag){
    	if(proteccion){
			pthread_mutex_lock(&planificacionCorto);
			sem_wait(&sem_cpuLibre);
			sem_wait(&sem_procesosReady);
			mostrarEstadoColasAux("READY", colaReady);
			if(!planificacionFlag)
				planificadorCortoAvance=1;
			if(planificacionFlag && list_size(colaReady))
				moverProceso_readyExec();
			if(!list_size(colaReady) && !list_size(colaExec))
				proteccion=0;
			pthread_mutex_unlock(&planificacionCorto);
    	}else{
    		if(list_size(colaReady)){
    			proteccion=1;
    			sem_post(&sem_cpuLibre);
    			sem_post(&sem_procesosReady);
    		}
    	}
    }
}

void liberar_procesos(){
    while (1){
        sem_wait(&sem_procesosExit);
        pthread_mutex_lock(&mutex_colaExit);
        PCB* pcbALiberar = queue_pop(colaExit);
        pthread_mutex_unlock(&mutex_colaExit);

        enviarValor_uint32(pcbALiberar->id,memoria_fd,FINALIZAR_PROCESO_MEMORIA,info_logger);

        liberarPcb(pcbALiberar);
        int cod_op = recibir_operacion(memoria_fd);
		if(cod_op == FINALIZAR_PROCESO_MEMORIA){
			recibirOrden(memoria_fd);
			disminuirGradoMP();
			if(queue_size(colaNew))
				sem_post(&sem_procesosEnNew);
		}
    }
}

void liberarPcb(PCB* pcb) {

    free(pcb->registros);

    list_destroy(pcb->listaInstrucciones);
    list_destroy(pcb->tablaPaginas);
    list_destroy(pcb->archivos_abiertos);
    list_destroy(pcb->recursosTomados);

    free(pcb);
}

void moverProceso_readyExec(){
        pthread_mutex_lock(&mutex_ColaReady);
        pthread_mutex_lock(&mutex_colaExec);

        if(strcmp(ALGORITMO_PLANIFICACION, "PRIORIDADES") == 0){
            list_sort(colaReady,criterioPrioridad);
            mostrarEstadoColas();

            PCB* pcb = list_remove(colaReady,0);
            list_add(colaExec,pcb);
            pthread_mutex_unlock(&mutex_ColaReady);
            pthread_mutex_unlock(&mutex_colaExec);

            mandarPaquetePCB(pcb);
            log_info(info_logger, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcb->id);

        }else{
            PCB *pcbReady = list_remove(colaReady,0);
            list_add(colaExec,pcbReady);

            if(strcmp(ALGORITMO_PLANIFICACION, "ROUND_ROBIN") == 0){
    			pthread_t atenderRR;
    			pthread_create(&atenderRR,NULL,esperarRR,(void*)pcbReady);
    			pthread_detach(atenderRR);
            }
			pthread_mutex_unlock(&mutex_ColaReady);
            pthread_mutex_unlock(&mutex_colaExec);
            mandarPaquetePCB(pcbReady);
            log_info(info_logger, "PID: [%d] - Estado Anterior: READY - Estado Actual: EXEC.", pcbReady->id);
        }
}


void moverProceso_NewReady(){
    pthread_mutex_lock(&mutex_colaNew);
    PCB* pcbAReady = queue_pop(colaNew);
    pthread_mutex_unlock(&mutex_colaNew);


    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbAReady);
    pthread_mutex_unlock(&mutex_ColaReady);

    //sem_post(&sem_procesosReady);
}

bool criterioPrioridad(PCB* pcb1, PCB* pcb2){
	return pcb1->prioridad < pcb2->prioridad;
}

void mandarPaquetePCB(PCB *pcb){
	t_paquete* paquete= crear_paquete(CONTEXTOEJECUCION, info_logger);
	agregar_ContextoEjecucion_a_paquete(paquete, pcb);
	enviar_paquete(paquete, cpuDispatch_fd);
	eliminar_paquete(paquete);
}

void *esperarRR(void *pcbReady){
	PCB* pcb = (PCB*) pcbReady;
	pcb->tiempoEjecutando = 0;
	while(1){
		if(QUANTUM > 1000){
			usleep(1000);
		}else{
			usleep(10000);
		}

		pcb->tiempoEjecutando++;
		if(pcb->tiempoEjecutando >= QUANTUM){
			//moverProceso_ExecReady(pcb);
			if(list_size(colaExec)){
				PCB* pcb = obtenerPcbExec();
				enviarValor_uint32(pcb->id, cpuInterrupt_fd, INTERRUPCIONCPU, info_logger);
				log_info(info_logger, "PID: <%d> - Desalojado por fin de Quantum", pcb->id);
			}
			return NULL;
		}
	}
}

void moverProceso_ExecBloq(PCB *pcbBuscado){
    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_lock(&mutex_colaBloq);
    list_add(colaBloq,pcbBuscado);

    pthread_mutex_unlock(&mutex_colaExec);
    pthread_mutex_unlock(&mutex_colaBloq);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: BLOQ.", pcbBuscado->id);

    sem_post(&sem_cpuLibre);
    if(list_size(colaReady))
    	sem_post(&sem_procesosReady);
}


void moverProceso_BloqReady(PCB* pcbBuscado){
    pthread_mutex_lock(&mutex_colaBloq);
    eliminarElementoLista(pcbBuscado, colaBloq);
    pthread_mutex_unlock(&mutex_colaBloq);

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady, pcbBuscado);
    pthread_mutex_unlock(&mutex_ColaReady);
    log_info(info_logger, "PID: [%d] - Estado Anterior: BLOQ - Estado Actual: READY.", pcbBuscado->id);
    sem_post(&sem_procesosReady);

}

void liberarRecursosTomados(PCB* pcb){
    for(int i = 0; i < list_size(pcb->recursosTomados); i++){
        Recurso* recursoTomado = list_get(pcb->recursosTomados,i);
        pthread_mutex_lock(&semaforos_io[recursoTomado->indiceSemaforo]);
        recursoTomado->instanciasRecurso++;
        pthread_mutex_unlock(&semaforos_io[recursoTomado->indiceSemaforo]);
        log_info(info_logger,"PID:<%d> - libera Recurso:<%s> - Instancias <%d>", pcb->id, recursoTomado->nombreRecurso, recursoTomado->instanciasRecurso);
        if(!list_is_empty(recursoTomado->cola))
        	moverProceso_BloqrecursoReady(recursoTomado);
    }
    list_clean(pcb->recursosTomados);
}

void moverProceso_ExecExit(PCB *pcbBuscado){

    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: EXIT", pcbBuscado->id);

    sem_post(&sem_cpuLibre);

    pthread_mutex_lock(&mutex_colaExit);
    queue_push(colaExit,pcbBuscado);
    pthread_mutex_unlock(&mutex_colaExit);

    if(!list_is_empty(pcbBuscado->recursosTomados))
        liberarRecursosTomados(pcbBuscado);

    if(!list_is_empty(pcbBuscado->archivos_abiertos))
        eliminarPcbTGAA_Y_actualizarTGAA(pcbBuscado);

    sem_post(&sem_procesosExit);
    if(list_size(colaReady))
     	sem_post(&sem_procesosReady);
}

void moverProceso_ExecReady(PCB* pcbBuscado){
    pthread_mutex_lock(&mutex_colaExec);
    eliminarElementoLista(pcbBuscado, colaExec);
    pthread_mutex_unlock(&mutex_colaExec);

    pthread_mutex_lock(&mutex_ColaReady);
    list_add(colaReady,pcbBuscado);
    pthread_mutex_unlock(&mutex_ColaReady);

    log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: READY", pcbBuscado->id);
    sem_post(&sem_procesosReady);
    sem_post(&sem_cpuLibre);

}


void aumentarGradoMP(){
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP++;
    pthread_mutex_unlock(&mutex_MP);
    log_info(info_logger, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}

void disminuirGradoMP(){
    pthread_mutex_lock(&mutex_MP);
    procesosTotales_MP--;
    pthread_mutex_unlock(&mutex_MP);
    log_info(info_logger, "Kernel - GRADO DE MULTIPROGRAMACION: %d.\n", procesosTotales_MP);
}

void mostrarEstadoColas(){
    pthread_mutex_lock(&mutex_debug_logger);
    mostrarEstadoColasAux("Nuevos", colaNew->elements);
    mostrarEstadoColasAux("Bloqueados", colaBloq);
    mostrarEstadoColasAux("Ejecutando", colaExec);
    mostrarEstadoColasAux("Ready", colaReady);
    pthread_mutex_unlock(&mutex_debug_logger);
}

void mostrarEstadoColasAux(char* colaMsg, t_list* cola){
    char* idsPcb = string_new();
    void guardarIds(void* pcbVoid){//ACA TIENE QUE RECIBIR UN VOID *
    	PCB* unaPcb = (PCB*)pcbVoid;
        char* unId = string_itoa(unaPcb->id);
        string_append(&idsPcb, unId);
        string_append(&idsPcb, ", ");
        //free(unId);//CHEQUEAR
    }

    string_append(&idsPcb, "[");
    if(!list_is_empty(cola))
    	list_iterate(cola, guardarIds);

    string_append(&idsPcb, "]");
    log_info(info_logger,"Cola de procesos %s: %s", colaMsg, idsPcb);
    free(idsPcb);
}

void eliminarElementoLista(PCB* pcbBuscado, t_list *lista){
  PCB *pcbAux;
  for (int i = 0; i < list_size(lista); i++)
  {  pcbAux = list_get(lista, i);
     if(pcbBuscado->id == pcbAux->id)
    	 list_remove(lista, i);
  }
}

void eliminarPcbTGAA_Y_actualizarTGAA(PCB* pcb){
    for(int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++){
        pthread_mutex_lock(&mutex_TGAA);
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        pthread_mutex_unlock(&mutex_TGAA);
        if(archivoPeticion->pcb->id == pcb->id)
            actualizarTGAALuegoDeLiberacionDeArchivo(archivoPeticion->archivo->nombreArchivo);
    }
}

void actualizarTGAALuegoDeLiberacionDeArchivo(char* nombreArchivo){
    bool archivoNoSolicitado = true;

    for(int i = 0; i < list_size(listaPeticionesArchivos); i++){
        t_archivoPeticion* archivoPeticion = list_get(listaPeticionesArchivos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0){
            actualizarDuenioTGAA(nombreArchivo,archivoPeticion->pcb);
            archivoNoSolicitado = false;
            break;
        }
    }
    for(int i = 0; i < list_size(listaPeticionesArchivos); i++){
        t_archivoPeticion* archivoPeticion = list_get(listaPeticionesArchivos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0 && archivoNoSolicitado){
        	eliminarArchivoTGAA(nombreArchivo);
        }

    }
}

void eliminarArchivoTGAA(char* nombreArchivo){
    pthread_mutex_lock(&mutex_TGAA);
    for(int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++){
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0){
            list_remove(tablaGlobal_ArchivosAbiertos,i);
            list_destroy(archivoPeticion->archivo->cola);
            list_destroy(archivoPeticion->archivo->colaWrite);
            free(archivoPeticion->archivo->nombreArchivo);
            free(archivoPeticion->archivo);
            free(archivoPeticion);
        }

    }
    pthread_mutex_unlock(&mutex_TGAA);
}


void actualizarDuenioTGAA(char* nombreArchivo, PCB* pcbNuevoDuenio){
    pthread_mutex_lock(&mutex_TGAA);
    for(int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++){
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo, nombreArchivo) == 0){
            archivoPeticion->pcb = pcbNuevoDuenio;
        }
    }

    pthread_mutex_unlock(&mutex_TGAA);
}

void eliminarArchivoTablaLocal(char* nombreArchivo, PCB* pcb){
    for(int i = 0; i < list_size(pcb->archivos_abiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->archivos_abiertos, i);
        if(strcmp(archivoLocal->archivo->nombreArchivo, nombreArchivo) == 0){
            list_remove(pcb->archivos_abiertos, i);
            free(archivoLocal);
        }
    }
}
