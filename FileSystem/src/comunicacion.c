#include "comunicacion.h"

int memoria_fd;
int kernel_fd;
int filesystem_fd;
pthread_mutex_t mutex_peticiones_pendientes;
pthread_mutex_t mutex_ArchivosUsados;
t_list* peticiones_pendientes;
sem_t contador_peticiones;
pthread_mutex_t mutex_recvM;
pthread_mutex_t mutex_recvK;
pthread_t crear_server_filesystem;
t_list *archivosUsados;

int conectarModulo(char *modulo){
	printf("ENTRE A CONECTAR ");
	//int conexion;											PREGUNTAR
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
	ip=config_get_string_value(fileConfigFs,parametro);

	if(!strcmp(charAux,"CPU"))
		strcpy(charAux,"CPU_DISPATCH");

	strcpy(parametro, "PUERTO_");
    strcat(parametro, charAux);
	puerto=config_get_string_value(fileConfigFs,parametro);

	log_info(logger, "IP=%s\n", ip);
	log_info(logger, "PUERTO=%s\n", puerto);


	memoria_fd= crear_conexion(ip, puerto);
	free(logger);
//	paquete(conexion, logger);
//
//	terminar_programa(conexion, logger);

	return memoria_fd;
}

int recibirConexion(char *puerto) {  //distinguir operaciones memoria y kernel

	logger = log_create("modulo.log", "-", 1, LOG_LEVEL_DEBUG);
	pthread_t tid[2];

	filesystem_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir a los clientes");

	if(filesystem_fd == 0){
		log_error(errorLogger, "Fallo al crear el servidor FILESYSTEM, cerrando FILESYSTEM");
		return EXIT_FAILURE;
	}

	kernel_fd = esperar_cliente(filesystem_fd);

	pthread_create(&tid[0], NULL, recibirMemoria, NULL);
	pthread_create(&tid[1], NULL, recibirKernel, NULL);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);

	return EXIT_SUCCESS;
}

void *recibirMemoria(){
	while(1) {//distinguir cada cliente
			pthread_mutex_lock(&mutex_recvM);
			int cod_op = recibir_operacion(memoria_fd);

			switch (cod_op) {

				case LECTURA_REALIZADA:

					pthread_t finalizarLecturaArchivoHilo;
					pthread_create(&finalizarLecturaArchivoHilo, NULL, finalizarLecturaArchivo, &memoria_fd);
					pthread_join(finalizarLecturaArchivoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvM);
					break;

				case ESCRITURA_REALIZADA:

					pthread_t finalizarEscrituraArchivoHilo;
					pthread_create(&finalizarEscrituraArchivoHilo, NULL, finalizarEscrituraArchivo, &memoria_fd);
					pthread_join(finalizarEscrituraArchivoHilo,NULL);
					pthread_mutex_unlock(&mutex_recvM);


					break;

				case PEDIR_SWAP: //Pide lista de bloques en swap

					pthread_t iniciarProcesoHilo;
					pthread_create(&iniciarProcesoHilo, NULL, iniciar_proceso, &memoria_fd);
					pthread_join(iniciarProcesoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvM);

					break;
				case DEVOLVER_SWAP:

					pthread_t finalizarProcesoHilo;
					pthread_create(&finalizarProcesoHilo, NULL,finalizar_proceso,&memoria_fd);
					pthread_join(finalizarProcesoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvM);

					break;
				case ESCRITURA_BLOQUE_SWAP:

					pthread_t escribirSwapHilo;
					pthread_create(&escribirSwapHilo, NULL, escribirEnSwap, &memoria_fd);
					pthread_join(escribirSwapHilo, NULL);
					pthread_mutex_unlock(&mutex_recvM);

					break;

				case LECTURA_BLOQUE_SWAP:

					pthread_t leerSwapHilo;
					pthread_create(&leerSwapHilo, NULL, leerEnSwap, &memoria_fd);
					pthread_join(leerSwapHilo, NULL);
					pthread_mutex_unlock(&mutex_recvM);

					break;

				case -1:
					log_error(logger, "el cliente se desconecto.");

						log_error(logger, "Terminando servidor.");
						return NULL;
					break;
				default:
					log_warning(logger,"Operacion desconocida. No quieras meter la pata");
					break;
			}
		}
}


void *recibirKernel(){
	while(1) {//distinguir cada cliente
		pthread_mutex_lock(&mutex_recvK);
			int cod_op = recibir_operacion(kernel_fd);

			switch (cod_op) {
				case APERTURA_ARCHIVO:

					pthread_t abrirArchivoHilo;
					pthread_create(&abrirArchivoHilo, NULL, abrirArchivo, &kernel_fd);
					pthread_join(abrirArchivoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvK);

					break;

				case CREACION_ARCHIVO:

					pthread_t crearArchivoHilo;
					pthread_create(&crearArchivoHilo, NULL, fileCreate, &kernel_fd);
					pthread_join(crearArchivoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvK);
					break;

				case LECTURA_ARCHIVO:

					pthread_t leerArchivoHilo;
					pthread_create(&leerArchivoHilo, NULL, leerArchivo, &kernel_fd);
					pthread_join(leerArchivoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvK);
					break;

				case ESCRITURA_ARCHIVO:

					pthread_t escribirArchivoHilo;
					pthread_create(&escribirArchivoHilo, NULL, escribirArchivo, &kernel_fd);
					pthread_join(escribirArchivoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvK);
					break;

				case TRUNCACION_ARCHIVO:

			        pthread_t truncarArchivoHilo;
			        pthread_create(&truncarArchivoHilo, NULL,  truncarArchivo, &kernel_fd);
					pthread_join(truncarArchivoHilo, NULL);
					pthread_mutex_unlock(&mutex_recvK);
					break;

				case -1:
					log_error(logger, "el cliente se desconecto.");
					log_error(logger, "Terminando servidor.");
					return NULL;
					break;
				default:
					log_warning(logger,"Operacion desconocida. No quieras meter la pata");
					break;
			}
		}
}

void* finalizar_proceso(void* memoria_cliente){

	int conexion = *((int*) memoria_cliente);

	t_list *listaBloquesADesasignar = list_create();
		listaBloquesADesasignar =	recibirListaUint32_t(conexion);

	t_peticion* peticion_finProceso = crear_peticion_fin_proceso(EJECUTAR_FINALIZAR_PROCESO, listaBloquesADesasignar);
	agregarPeticionAPendientes(peticion_finProceso);
	sem_post(&contador_peticiones);


	return NULL;

}

t_peticion* crear_peticion_fin_proceso(t_operacion_fs operacion, t_list *listaBloquesADesasignar){
    t_peticion* peticion = malloc(sizeof(t_peticion));


    peticion->operacion = operacion;
    peticion->bloquesADesasignarSwap = list_create();
    peticion->bloquesADesasignarSwap = listaBloquesADesasignar;
    return peticion;
}

void *fileCreate(void *cliente_fd){

	int conexion = *((int *) cliente_fd);


	char* nombreArchivo = recibirString(conexion);
	    //log_debug(debug_logger,"nombre archivo: %s", nombreArchivo);
	creacionArchivo(nombreArchivo);

	t_peticion* peticion_open = crear_peticion(EJECUTAR_CREACION,nombreArchivo, 0, 0, 0, NULL);
	agregarPeticionAPendientes(peticion_open);
	sem_post(&contador_peticiones);


	return NULL;

}



void *escribirEnSwap(void* memoria_cliente){

	int conexion = *((int*) memoria_cliente);

	t_datos* unosDatos = malloc(sizeof(t_datos));
	t_list* listaInts = recibirListaIntsYDatos(conexion, unosDatos);
	uint32_t direccionFisica = *(uint32_t*)list_get(listaInts,1);
	//uint32_t tamanio = *(uint32_t*)list_get(listaInts,1);
	//uint32_t pid = *(uint32_t*)list_get(listaInts,2);
	uint32_t punteroArchivo = *(uint32_t*)list_get(listaInts,0);


	t_peticion* peticion_write = crear_peticion(EJECUTAR_ESCRITURA_SWAP,"", direccionFisica, punteroArchivo, 0, unosDatos->datos);
	agregarPeticionAPendientes(peticion_write);
	sem_post(&contador_peticiones);

	return NULL;
}


void *leerEnSwap(void* memoria_cliente){

    int conexion = *((int*) memoria_cliente);

    uint32_t posPuntero = recibirValor_uint32(conexion);

    t_peticion* peticion_read = crear_peticion(EJECUTAR_LECTURA_SWAP,"", 0, posPuntero, 0, NULL);
    agregarPeticionAPendientes(peticion_read);
    sem_post(&contador_peticiones);

    return NULL;
}

void* iniciar_proceso(void* memoria_cliente){

	int conexion = *((int*) memoria_cliente);

	log_info(infoLogger, "Encontrando bloques en swap");

	uint32_t datosDeInicioDeProceso = recibirValor_uint32(conexion);

	t_peticion* peticion_initProceso = crear_peticion(EJECUTAR_INICIAR_PROCESO, "", datosDeInicioDeProceso, 0, 0, NULL);
	agregarPeticionAPendientes(peticion_initProceso);
	sem_post(&contador_peticiones);
	return NULL;
}

void* abrirArchivo(void *kernel_cliente){

    int conexion = *((int*) kernel_cliente);
    char* nombreArchivo = recibirString(conexion);
    //log_debug(debug_logger, "Me llego este String: %s",nombreArchivo);


    t_peticion* peticion_open = crear_peticion(EJECUTAR_OPEN,nombreArchivo, 0, 0, 0, NULL);
    agregarPeticionAPendientes(peticion_open);
    sem_post(&contador_peticiones);


    return NULL;
}

void* truncarArchivo(void* kernel_cliente){
    int conexion = *((int*) kernel_cliente);

    t_archivoTruncate* archivoTruncacion = recibir_archivoTruncacion(conexion);
    t_peticion* peticion_truncar = crear_peticionTruncate(EJECUTAR_TRUNCATE,archivoTruncacion->nombreArchivo, 0, 0,archivoTruncacion->nuevoTamanio , 0, NULL);
    agregarPeticionAPendientes(peticion_truncar);
    sem_post(&contador_peticiones);

    return NULL;

}


void* leerArchivo(void* kernel_cliente){
    int conexion = *((int*) kernel_cliente);

    t_archivoRW* archivo = recibir_archivoRW(conexion);
    log_trace(traceLogger, "Peticion Lectura Archivo <%s> - Puntero <%d> - Memoria: <%d>", archivo->nombreArchivo, archivo->posPuntero, archivo->direcFisica);
    //lecturaArchivo(archivo->nombreArchivo,archivo->posPuntero,archivo->direcFisica);

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb = buscarFCBporNombre(archivo->nombreArchivo); //solo agrego a lista los archivos que se lee o escribe
    list_add(archivosUsados, fcb);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    t_peticion* peticion_read = crear_peticion(EJECUTAR_READ,archivo->nombreArchivo, archivo->direcFisica, archivo->posPuntero, archivo->pid, NULL);
    agregarPeticionAPendientes(peticion_read);
    sem_post(&contador_peticiones);


    return NULL;
}

void* escribirArchivo(void * kernel_cliente){

	int conexion = *((int *) kernel_cliente);

	t_archivoRW* archivo = recibir_archivoRW(conexion);

	pthread_mutex_lock(&mutex_ArchivosUsados);

	t_config_fcb* fcb = buscarFCBporNombre(archivo->nombreArchivo);

	list_add(archivosUsados, fcb);

	pthread_mutex_unlock(&mutex_ArchivosUsados);
	//VERLO PORQUE SOLO ESCRIBO UN BLOQUE Y LO HAGO EN BASE A UN PUNTERO PERO NO SE SI ES ESTE

	t_peticion *peticion_write = crear_peticion(EJECUTAR_WRITE, archivo->nombreArchivo,  archivo->direcFisica, archivo->posPuntero, archivo->pid, NULL);

	agregarPeticionAPendientes(peticion_write);

	sem_post(&contador_peticiones);

	return NULL;
}

void* finalizarLecturaArchivo(void* cliente_socket){
	int conexion = *((int*) cliente_socket);

   t_config_fcb* fcb  = obtenerPrimerArchivoUsado();


    recibirOrden(conexion);
    t_peticion* peticion_read = crear_peticion(EJECUTAR_RESPUESTA_LECTURA,fcb->NOMBRE_ARCHIVO, 0, 0, 0, NULL);
    agregarPeticionAPendientes(peticion_read);
    sem_post(&contador_peticiones);

    return NULL;

}


void* finalizarEscrituraArchivo(void* cliente_socket){
    int conexion = *((int*) cliente_socket);

    t_datos* unosDatos = malloc(sizeof(t_datos));
    t_list* listaInts = recibirListaIntsYDatos(conexion, unosDatos);
    uint32_t direccionFisica = *(uint32_t*)list_get(listaInts,0);
    //uint32_t tamanio = *(uint32_t*)list_get(listaInts,1);
    //uint32_t pid = *(uint32_t*)list_get(listaInts,2);
    uint32_t punteroArchivo = *(uint32_t*)list_get(listaInts,3);

    t_config_fcb* fcb  = obtenerPrimerArchivoUsado();

    char * nombreArchivo = malloc(sizeof(fcb->NOMBRE_ARCHIVO));
    //strcpy(fcb->NOMBRE_ARCHIVO, nombreArchivo);
    t_peticion* peticion_write = crear_peticion(EJECUTAR_RESPUESTA_ESCRITURA,fcb->NOMBRE_ARCHIVO, direccionFisica, punteroArchivo, 0, unosDatos->datos);
    agregarPeticionAPendientes(peticion_write);
    sem_post(&contador_peticiones);

    list_destroy(listaInts);
    return NULL;
}

t_config_fcb* obtenerPrimerArchivoUsado() {

    pthread_mutex_lock(&mutex_ArchivosUsados);
    t_config_fcb* fcb = list_remove(archivosUsados, 0);
    pthread_mutex_unlock(&mutex_ArchivosUsados);

    return fcb;
}

t_peticion* crear_peticionTruncate(t_operacion_fs operacion, char* nombre, uint32_t dir_fisica, uint32_t puntero, uint32_t tamanio, uint32_t pid, void* datos){
	t_peticion* peticion = malloc(sizeof(t_peticion));
	peticion->nombre = string_new();
	string_append(&peticion->nombre, nombre);
    peticion->operacion = operacion;
    peticion->tamanio = tamanio;
    peticion->direccionFisica = dir_fisica;
    peticion->puntero = puntero;
    peticion->pid = pid;
    peticion->datos = datos;

    return peticion;
}

t_peticion* crear_peticion(t_operacion_fs operacion, char* nombre, uint32_t dir_fisica, uint32_t puntero, uint32_t pid, void* datos){
    t_peticion* peticion = malloc(sizeof(t_peticion));
    peticion->nombre = string_new();
    string_append(&peticion->nombre, nombre);
    peticion->operacion = operacion;
    //strcpy(peticion->nombre, nombre);
    peticion->tamanio = configFs->TAM_BLOQUE;
    peticion->direccionFisica = dir_fisica;
    peticion->puntero = puntero;
    peticion->pid = pid;

    peticion->datos = datos;
    return peticion;
}

void agregarPeticionAPendientes(t_peticion* peticion){

    pthread_mutex_lock(&mutex_peticiones_pendientes);

    list_add(peticiones_pendientes, peticion);

    pthread_mutex_unlock(&mutex_peticiones_pendientes);
}

t_peticion* sacoPeticionDePendientes(){
    pthread_mutex_lock(&mutex_peticiones_pendientes);
    t_peticion* peticion = list_remove(peticiones_pendientes, 0);
    pthread_mutex_unlock(&mutex_peticiones_pendientes);
    return peticion;
}


void iniciar_atencion_peticiones(){
    pthread_t hilo_peticiones;
    log_info(infoLogger, "Inicio atencion de peticiones");

    pthread_create(&hilo_peticiones, NULL, (void*) atender_peticiones, NULL);
    pthread_detach(hilo_peticiones);
}


void atender_peticiones(){

    while(1){
        sem_wait(&contador_peticiones);

        log_info(infoLogger, "Hay peticion pendiente");

        t_peticion* peticion = sacoPeticionDePendientes();

        manejar_peticion(peticion);
    }
}


void manejar_peticion(t_peticion* peticion){
    t_operacion_fs cop = peticion->operacion;

    switch (cop) {
        case EJECUTAR_OPEN:
            ejecutar_fopen(peticion->nombre);
            free(peticion);
            break;
        case EJECUTAR_CREACION:
            ejecutarCreacionArchivo(peticion->nombre); //hacer otra funcion parecida a las del filesystem.c
            free(peticion);
            break;
        case EJECUTAR_READ:
            ejecutarFread(peticion->nombre, peticion->pid, peticion->puntero, peticion->direccionFisica);
            break;
        case EJECUTAR_TRUNCATE:
        	ejecutarFTruncate(peticion->nombre, peticion->tamanio);
        	free(peticion);
        	break;
        case EJECUTAR_WRITE:
            ejecutarFwrite(peticion->nombre, peticion->pid, peticion->puntero, peticion->tamanio, peticion->direccionFisica);
            free(peticion);
            break;
        case EJECUTAR_RESPUESTA_LECTURA:
        	 ejecutar_finalizarLecturaArchivo(peticion->nombre);
        	 free(peticion);
        	 break;
        case EJECUTAR_RESPUESTA_ESCRITURA:
        	 ejecutar_finalizarEscrituraArchivo(peticion->nombre, peticion->puntero, peticion->tamanio, peticion->direccionFisica, peticion->datos);
        	 free(peticion);
        	 break;
        case EJECUTAR_INICIAR_PROCESO:
        	ejecutar_iniciar_proceso(peticion->direccionFisica);
        	free(peticion);
        	break;
        case EJECUTAR_FINALIZAR_PROCESO:
        	ejecutar_finalizar_proceso(peticion->bloquesADesasignarSwap);
        	list_destroy(peticion->bloquesADesasignarSwap);
        	free(peticion);
        	break;
        case EJECUTAR_ESCRITURA_SWAP:
        	ejecutar_escrituraSwap(peticion->datos, peticion->puntero);
        	free(peticion->datos);
        	free(peticion);
        	break;
        case EJECUTAR_LECTURA_SWAP:
        	ejecutar_lecturaSwap(peticion->puntero);
        	free(peticion);
        	break;

        default:
            log_error(errorLogger, "Algo anduvo mal");
            log_info(infoLogger, "Cop: %d", cop);
            return;
    }
}


t_log* iniciar_logger(char *nombre){
	t_log* nuevo_logger;
	nuevo_logger = log_create(nombre, "tp", 1, LOG_LEVEL_INFO);
	if(nuevo_logger == NULL)
	{
		printf("No se genero de forma correcta el logger");
		exit(1);
	}

	return nuevo_logger;
}

void leer_consola(t_log* logger){	char* leido;

	leido = readline("> ");

	while(*leido != '\0')
	{
		log_info(logger, "%s\n", leido);
		free(leido);
		leido = readline("> ");
	}

	free(leido);
}

void paquete(int conexion, t_log* logger){
	char *leido;
	t_paquete* paquete = crear_paquete(PAQUETECLIENTE,logger);

	leido = readline("> ");
	while(*leido != '\0')
	{   agregar_a_paquete(paquete, leido, strlen(leido)+1);
		free(leido);
		leido = readline("> ");
	}

	enviar_paquete(paquete, conexion);
	free(paquete);
	free(leido);
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

void terminar_programa(int conexion, t_log* logger){
	log_destroy(logger);
	liberar_conexion(conexion);
}

