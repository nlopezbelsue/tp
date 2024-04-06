#include "comunicacion.h"

int cpuDispatch_fd;
int cpuInterrupt_fd;
int memoria_fd;
int filesystem_fd;
int accesosEnEscitura = 0;

int conectarModuloCPU(char *modulo){
	printf("ENTRE A CONECTAR ");
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	t_log *logger;

	strcpy(charAux, modulo);
	logger= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(config,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(config,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);

	cpuDispatch_fd= crear_conexion(ip, puerto);

	log_destroy(logger);
//	terminar_programa(conexion, logger);

	return cpuDispatch_fd;
}

int conectarModuloCPUInterrupt(char *modulo){
	printf("ENTRE A CONECTAR ");
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	t_log *logger;

	strcpy(charAux, modulo);
	logger= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(config,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_INTERRUPT");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(config,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);

	cpuInterrupt_fd= crear_conexion(ip, puerto);

	log_destroy(logger);
//	terminar_programa(conexion, logger);

	return cpuInterrupt_fd;
}

int conectarModuloMemoria(char *modulo){
	printf("ENTRE A CONECTAR ");
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	t_log *logger;

	strcpy(charAux, modulo);
	logger= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(config,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(config,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);

	memoria_fd= crear_conexion(ip, puerto);

	log_destroy(logger);
//	terminar_programa(conexion, logger);

	return memoria_fd;
}

int conectarModuloFilesystem(char *modulo){
	printf("ENTRE A CONECTAR ");
	char *ip;
	char *puerto;
	char charAux[50];
    char parametro[20];

	t_log *logger;

	strcpy(charAux, modulo);
	logger= iniciar_logger(strcat(charAux,".log"));

	strcpy(charAux, modulo);

	strcpy(parametro, "IP_");
    strcat(parametro, charAux);
	ip=config_get_string_value(config,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(config,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);


	filesystem_fd= crear_conexion(ip, puerto);

	log_destroy(logger);
//	terminar_programa(conexion, logger);

	return filesystem_fd;
}

void escucharFilesystem(void){
	while(filesystem_fd != -1){
		int cod_op = recibir_operacion(filesystem_fd);

		switch(cod_op){
			case APERTURA_ARCHIVO_EXITOSA: {
				char* nombreArchivo = recibirString(filesystem_fd);
				log_info(info_logger,"Me llego este nombre de archivo: %s", nombreArchivo);
				agregarEntrada_TablaGlobalArchivosAbiertos(nombreArchivo);
				break;
			}

			case APERTURA_ARCHIVO_FALLIDA: {
				char* nombreArchivo = recibirString(filesystem_fd);
				enviarString(nombreArchivo, filesystem_fd, CREACION_ARCHIVO, info_logger);
				break;
			}

			case CREACION_ARCHIVO_EXITOSA: {
				char* nombreArchivo = recibirString(filesystem_fd);
				log_info(info_logger, "creacion archivo exitosa");
				agregarEntrada_TablaGlobalArchivosAbiertos(nombreArchivo);
				//NO VA LIBERACION NOMARCHIVO
				break;
			}

			case TRUNCACION_ARCHIVO_EXITOSA: {
				log_info(info_logger, "Recibi truncacion archivo exitosa");
				char* nombreArchivo = reciboYActualizoContadorPeticionesFs(filesystem_fd);
				desbloquearPcb_porNombreArchivo(nombreArchivo);
				log_info(info_logger, "Recibi truncacion archivo exitosa 2");
				break;
			}

			case LECTURA_ARCHIVO_EXITOSA: {
				char* nombreArchivo = reciboYActualizoContadorPeticionesFs(filesystem_fd);
				desbloquearPcb_porNombreArchivo(nombreArchivo);
				break;
			}

			case ESCRITURA_ARCHIVO_EXITOSA: {

				char* nombreArchivo = reciboYActualizoContadorPeticionesFs(filesystem_fd);
				log_info(info_logger, "Archivo: %s", nombreArchivo);
				desbloquearPcb_porNombreArchivo(nombreArchivo);
				break;
			}

			case -1:
				log_error(info_logger, "Cliente desconectado de %s...", "FS");
				return;
			default:
				log_error(info_logger, "Algo anduvo mal en el server de %s", "FS");
				log_trace(info_logger, "Cop: %d", cod_op);
				return;
		}
	}
}

void escucharCPU(void){
	while(cpuDispatch_fd != -1)	{
		int cod_op = recibir_operacion(cpuDispatch_fd);

		switch(cod_op){
		    case SLEEPCPU:
		    {
				uint32_t segundos;
				PCB* pcbRecibida = recibir_contextoEjecucion_y_uint32(cpuDispatch_fd, &segundos);
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				moverProceso_ExecBloq(pcbActualizada);
				SleepCpu *aux = malloc(sizeof(SleepCpu));
				aux->pcb = pcbActualizada;
				aux->segundosSleep = segundos;
				log_info(info_logger,"PID: <%d> - Bloqueado por <SLEEP>", pcbActualizada->id);
				pthread_t atenderSleep;
				pthread_create(&atenderSleep,NULL,esperaTiempo,(void*)aux);
				pthread_detach(atenderSleep);


		    	break;
		    }
			case WAIT:
			{
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char(cpuDispatch_fd);
				char* nombreArchivo=pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				manejoDeRecursos("WAIT", nombreArchivo);
				break;
			}

			case SIGNAL:{
				PCB* pcbRecibida = recibir_contextoEjecucion_y_char(cpuDispatch_fd);
				char* nombreArchivo=pcbRecibida->nombreRecurso;
				actualizarPcbEjecucion(pcbRecibida);
				manejoDeRecursos("SIGNAL", nombreArchivo);
				break;
			}

			case PAGE_FAULT:{

				pthread_t atenderPageFault;
				pthread_create(&atenderPageFault,NULL,esperaPageFault,NULL);
				pthread_join(atenderPageFault, NULL);

				break;
			}

			case F_OPEN: {
				//recibe largoNombreArchivo /nombreArchivo / pcb
				uint32_t modo;
				PCB* unPcb = recibir_contextoEjecucion_y_char_y_uint32(cpuDispatch_fd, &modo);
				log_info(info_logger, "MODO: %d", modo);
				char* nombreArchivo=unPcb->nombreRecurso;
				actualizarPcbEjecucion(unPcb);
				PCB* pcbActualizada = obtenerPcbExec();
				int posicion = buscarArch_TablaGlobalArchivo(nombreArchivo);
				if(posicion==-1){

					ejecutar_FOPEN(pcbActualizada, nombreArchivo, modo);
				}else{
					t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos, posicion);
					if(archivoPeticion->archivo->modo==1 || list_size(archivoPeticion->archivo->cola)){
						list_add(archivoPeticion->archivo->cola, pcbActualizada);
						pthread_mutex_lock(&mutex_colaExec);
						PCB* pcbABlockedRecurso = list_remove(colaExec,0);
						pthread_mutex_unlock(&mutex_colaExec);
						pcbABlockedRecurso->program_counter--;
						sem_post(&sem_cpuLibre);
						pthread_mutex_lock(&mutex_colaBloq);
						list_add(colaBloq,pcbABlockedRecurso);
						pthread_mutex_unlock(&mutex_colaBloq);
						log_info(info_logger,"PID: <%d> - Bloqueado por: <%s>", pcbActualizada->id, nombreArchivo);
						if(modo)
							list_add(archivoPeticion->archivo->colaWrite, pcbActualizada);
					    if(list_size(colaReady))
					    	sem_post(&sem_procesosReady);
					}else{
						ejecutar_FOPEN(pcbActualizada, nombreArchivo, modo);
					}
				}
				break;
			}

			case F_CLOSE: {
				PCB* unPcb = recibir_contextoEjecucion_y_char(cpuDispatch_fd);
				char* nombreArchivo=unPcb->nombreRecurso;
				actualizarPcbEjecucion(unPcb);
				PCB* pcbActualizada = obtenerPcbExec();
				int posicion = buscarArch_TablaGlobalArchivo(nombreArchivo);
				t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos, posicion);
				//if(archivoPeticion->archivo->modo){
					int i;
					int tamLista = list_size(archivoPeticion->archivo->cola);
					PCB* pcbPrimerWrite = NULL;

					if(list_size(archivoPeticion->archivo->colaWrite)){
						pcbPrimerWrite = list_get(archivoPeticion->archivo->colaWrite, 0);
					}

					if(pcbPrimerWrite != NULL){
						PCB* pcb = list_get(archivoPeticion->archivo->cola, 0);
						for(i=0; i<tamLista && pcb->id != pcbPrimerWrite->id; i++){
							pcb = list_get(archivoPeticion->archivo->cola, i);
						}
					}else{
						i = tamLista;
					}
					if(i != 0){
						for(int j=0; j<i; j++){
							accesosEnEscitura = i;
							PCB* pcb = list_remove(archivoPeticion->archivo->cola, 0);
							pthread_mutex_lock(&mutex_colaBloq);
							eliminarElementoLista(pcb, colaBloq);
							pthread_mutex_unlock(&mutex_colaBloq);
							pthread_mutex_lock(&mutex_ColaReady);
							list_add(colaReady,pcb);
							pthread_mutex_unlock(&mutex_ColaReady);
							sem_post(&sem_procesosReady);
							log_info(info_logger,"PID: <%d> - Estado Anterior: <BLOCKED_ARCHIVO[%s]> - Estado Actual: <READY>",pcb->id,nombreArchivo);
						}
					}else{
						accesosEnEscitura--;
						if(accesosEnEscitura == 0 && list_size(archivoPeticion->archivo->colaWrite)){
							list_remove(archivoPeticion->archivo->colaWrite, 0);
							PCB* pcb = list_remove(archivoPeticion->archivo->cola, 0);
							pthread_mutex_lock(&mutex_colaBloq);
							eliminarElementoLista(pcb, colaBloq);
							pthread_mutex_unlock(&mutex_colaBloq);
							pthread_mutex_lock(&mutex_ColaReady);
							list_add(colaReady,pcb);
							pthread_mutex_unlock(&mutex_ColaReady);
							sem_post(&sem_procesosReady);
							log_info(info_logger,"PID: <%d> - Estado Anterior: <BLOCKED_ARCHIVO[%s]> - Estado Actual: <READY>",pcb->id,nombreArchivo);
						}

					}

				//}
				ejecutar_FCLOSE(pcbActualizada, nombreArchivo);
				break;
			}

			case F_SEEK: {
				uint32_t posicion;
				PCB* unPcb = recibir_contextoEjecucion_y_char_y_uint32(cpuDispatch_fd, &posicion); //cambiar
				char* nombreArchivo=unPcb->nombreRecurso;
				log_info(info_logger, "EL NOMBRE DEL ARCHIVO ES: %s posicion %d", nombreArchivo, posicion);
				actualizarPcbEjecucion(unPcb);
				PCB* pcbActualizada = obtenerPcbExec();
				ejecutar_FSEEK(pcbActualizada, nombreArchivo, posicion);
				break;
			}

			case F_TRUNCATE: {
				uint32_t tamanio=5;
				PCB* unPcb = recibir_contextoEjecucion_y_char_y_uint32(cpuDispatch_fd, &tamanio);
				char* nombreArchivo=unPcb->nombreRecurso;
				actualizarPcbEjecucion(unPcb);
				PCB* pcbActualizada = obtenerPcbExec();
				ejecutar_FTRUNCATE(pcbActualizada, nombreArchivo, tamanio);
				break;
			}

			case F_READ: {
				uint32_t direccionFisica;
				PCB* unPcb = recibir_contextoEjecucion_y_char_y_uint32(cpuDispatch_fd, &direccionFisica);
				char* nombreArchivo=unPcb->nombreRecurso;
				actualizarPcbEjecucion(unPcb);
				PCB* pcbActualizada = obtenerPcbExec();
				ejecutar_FREAD(pcbActualizada, nombreArchivo, direccionFisica);
				break;
			}

			case F_WRITE:
				uint32_t direccionFisica;
				PCB* unPcb = recibir_contextoEjecucion_y_char_y_uint32(cpuDispatch_fd, &direccionFisica);
				char* nombreArchivo=unPcb->nombreRecurso;
				log_info(info_logger, "%d", list_size(colaExec));
				actualizarPcbEjecucion(unPcb);
				PCB* pcbActualizada = obtenerPcbExec();
				int posicion = buscarArch_TablaGlobalArchivo(nombreArchivo);
				t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos, posicion);
				if(archivoPeticion->archivo->modo){
					ejecutar_FWRITE(pcbActualizada, nombreArchivo, direccionFisica);
					log_info(info_logger, "Se realizo el fwrite");
				}else{
					pthread_mutex_lock(&mutex_colaExec);
					eliminarElementoLista(pcbActualizada, colaExec);
					pthread_mutex_unlock(&mutex_colaExec);
					log_info(info_logger, "PID: [%d] - Estado Anterior: EXEC - Estado Actual: EXIT", pcbActualizada->id);
					sem_post(&sem_cpuLibre);
					pthread_mutex_lock(&mutex_colaExit);
					queue_push(colaExit,pcbActualizada);
					pthread_mutex_unlock(&mutex_colaExit);
					if(!list_is_empty(pcbActualizada->recursosTomados))
					    liberarRecursosTomados(pcbActualizada);
					if(!list_is_empty(pcbActualizada->archivos_abiertos))
					    eliminarPcbTGAA_Y_actualizarTGAA(pcbActualizada);
					 sem_post(&sem_procesosExit);
					 if(list_size(colaReady))
					     sem_post(&sem_procesosReady);
					 log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <INVALID_WRITE>",pcbActualizada->id);
				}
				break;

			case INTERRUPCIONCPU:
				PCB* pcbRecibida = recibir_contextoEjecucion(cpuDispatch_fd);
				if(list_size(colaExec)){
					actualizarPcbEjecucion(pcbRecibida);
					PCB* pcbActualizada = obtenerPcbExec();
					moverProceso_ExecReady(pcbActualizada);
				}
				break;

			case EXIT:{
				PCB* pcbRecibida = recibir_contextoEjecucion(cpuDispatch_fd);
				actualizarPcbEjecucion(pcbRecibida);
				PCB* pcbActualizada = obtenerPcbExec();
				log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <SUCCESS>",pcbActualizada->id);
				moverProceso_ExecExit(pcbActualizada);
				break;
			}
			case -1:
				log_error(info_logger, "Cliente desconectado de %s...", "CPU");
				return;
			default:
				log_error(info_logger, "Algo anduvo mal en el server de %s", "CPU");
				log_info(info_logger, "Cop: %d", cod_op);
				return;
		}
	}
}


void actualizarPcbEjecucion(PCB* pcbRecibida){
    //Actualizo los campos PC,Registros de la pcbRecibida. Se lo asigno a la pcb vieja que se encuentra
    pthread_mutex_lock(&mutex_colaExec);
    PCB* pcbExec = list_get(colaExec,0);

    RegistrosCPU* registrosAux = pcbExec->registros;
    pcbExec->program_counter = pcbRecibida->program_counter;
    pcbExec->registros = pcbRecibida->registros;
    pcbRecibida->registros = registrosAux;
    liberarPcbCpu(pcbRecibida);
    pthread_mutex_unlock(&mutex_colaExec);
}

PCB* obtenerPcbExec(){
    pthread_mutex_lock(&mutex_colaExec);
    PCB* unaPcb = list_get(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);
    return unaPcb;
}

void manejoDeRecursos(char* orden, char* recursoSolicitado){
    PCB* unaPcb = obtenerPcbExec();

    bool coincideConSolicitado(Recurso* unRecurso){
        return strcmp(unRecurso->nombreRecurso,recursoSolicitado) == 0;
    }
    Recurso* recursoEncontrado = list_find(estadoBlockRecursos, coincideConSolicitado);
    if(recursoEncontrado == NULL){
        log_info(info_logger,"Recurso <%s> solicitado INEXISTENTE", recursoSolicitado);
        log_info(info_logger,"Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>",unaPcb->id);
        moverProceso_ExecExit(unaPcb);
    }else{
        if(strcmp(orden,"WAIT") == 0){
            waitRecursoPcb(recursoEncontrado, unaPcb);
        } else{
            signalRecursoPcb(recursoEncontrado, unaPcb);
        }
    }
    free(recursoSolicitado);
}

void waitRecursoPcb(Recurso* recurso, PCB* unaPcb){
    recurso->instanciasRecurso--;
    list_add(unaPcb->recursosTomados,recurso);
    log_info(info_logger,"PID: <%d> - Wait: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if (recurso->instanciasRecurso < 0) {
        log_info(info_logger,"PID: <%d> - Bloqueado por: <%s>",unaPcb->id,recurso->nombreRecurso);
        bloquearProcesoPorRecurso(recurso);
    }else{
    	t_paquete* paquete= crear_paquete(CONTEXTOEJECUCION, info_logger);
		agregar_ContextoEjecucion_a_paquete(paquete, unaPcb);
		enviar_paquete(paquete, cpuDispatch_fd);
		eliminar_paquete(paquete);
    }
}

void signalRecursoPcb(Recurso* recurso, PCB* unaPcb){
    recurso->instanciasRecurso++;
    list_remove_element(unaPcb->recursosTomados,recurso);
    log_info(info_logger,"PID: <%d> - Signal: <%s> - Instancias: <%d>", unaPcb->id, recurso->nombreRecurso, recurso->instanciasRecurso);
    if(!list_is_empty(recurso->cola))
        moverProceso_BloqrecursoReady(recurso);
    t_paquete* paquete= crear_paquete(CONTEXTOEJECUCION, info_logger);
	agregar_ContextoEjecucion_a_paquete(paquete, unaPcb);
	enviar_paquete(paquete, cpuDispatch_fd);
	eliminar_paquete(paquete);
}

void bloquearProcesoPorRecurso(Recurso* recurso){
    pthread_mutex_lock(&mutex_colaExec);
    PCB* pcbABlockedRecurso = list_remove(colaExec,0);
    pthread_mutex_unlock(&mutex_colaExec);

    sem_post(&sem_cpuLibre);

    pthread_mutex_lock(&mutex_colaBloq);
	list_add(colaBloq,pcbABlockedRecurso);
	pthread_mutex_unlock(&mutex_colaBloq);

	pthread_mutex_lock(&semaforos_io[recurso->indiceSemaforo]);
	list_add(recurso->cola, pcbABlockedRecurso);
	pthread_mutex_unlock(&semaforos_io[recurso->indiceSemaforo]);

	/*if(deteccion_deadlock()){

	}*/


    log_info(info_logger,"PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCKED_RECURSO[%s]>", pcbABlockedRecurso->id, recurso->nombreRecurso);
}

void moverProceso_BloqrecursoReady(Recurso* recurso){
    pthread_mutex_lock(&semaforos_io[recurso->indiceSemaforo]);
    PCB* pcbLiberada = list_remove(recurso->cola,0);
    pthread_mutex_unlock(&semaforos_io[recurso->indiceSemaforo]);
    uint32_t num;
    PCB* pcbEncontrado = encontrarProceso(pcbLiberada->id, &num);
    if(pcbEncontrado != NULL){

		pthread_mutex_lock(&mutex_colaBloq);
		eliminarElementoLista(pcbLiberada, colaBloq);
		pthread_mutex_unlock(&mutex_colaBloq);

		pthread_mutex_lock(&mutex_ColaReady);
		list_add(colaReady,pcbLiberada);
		pthread_mutex_unlock(&mutex_ColaReady);
		sem_post(&sem_procesosReady);
		log_info(info_logger,"PID: <%d> - Estado Anterior: <BLOCKED_RECURSO[%s]> - Estado Actual: <READY>",pcbLiberada->id,recurso->nombreRecurso);
    }


}

void* esperaTiempo(void* aux1){

	SleepCpu* aux2 = (SleepCpu *) aux1;




    sleep(aux2->segundosSleep);
    moverProceso_BloqReady(aux2->pcb);
    free(aux2);
}

void* esperaPageFault(void*){
	uint32_t numPagina;
	PCB* pcbRecibida = recibir_contextoEjecucion_y_uint32(cpuDispatch_fd, &numPagina);
	actualizarPcbEjecucion(pcbRecibida);
	PCB* pcbActualizada = obtenerPcbExec();

	log_info(info_logger, "Page Fault PID: <%d> - Pagina: <%d>", pcbActualizada->id, numPagina);

	moverProceso_ExecBloq(pcbActualizada);

	t_list* listaInts = list_create();

	list_add(listaInts, &pcbActualizada->id);
	list_add(listaInts, &numPagina);


	enviarListaUint32_t(listaInts,memoria_fd, info_logger, CARGA_PAGINA);

	list_clean(listaInts);
	list_destroy(listaInts);

	op_code_cliente cod = recibir_operacion(memoria_fd);

	if(cod == CARGA_PAGINA){
		recibirOrden(memoria_fd);
		log_info(info_logger, "Se cargo la pagina %d del proceso %d:\n", numPagina, pcbActualizada->id);
		moverProceso_BloqReady(pcbActualizada);
	}
	else{
		printf("%d", cod);
		log_error(info_logger,"FALLO en el recibo de la confirmacion de carga de pagina en memoria");
	}
}

t_log* iniciar_logger(char *nombre){
	t_log* nuevo_logger;
	nuevo_logger = log_create(nombre, "tp", 1, LOG_LEVEL_INFO);
	if(nuevo_logger == NULL){
		printf("No se genero de forma correcta el logger");
		exit(1);
	}

	return nuevo_logger;
}

void leer_consola(t_log* logger){
	char* leido;

	leido = readline("> ");

	while(*leido != '\0'){
		log_info(logger, "%s\n", leido);
		free(leido);
		leido = readline("> ");
	}

	free(leido);
}

void paquete(t_log* logger, char* parametro){
	t_paquete* paquete = crear_paquete(PAQUETECLIENTE,logger);

   agregar_a_paquete(paquete, parametro, strlen(parametro)+1);

	//enviar_paquete(paquete, kernel_fd);
	free(paquete);
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

void terminar_programa(int conexion, t_log* logger)
{	log_destroy(logger);
	liberar_conexion(conexion);
}
