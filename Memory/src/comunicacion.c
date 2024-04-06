#include "comunicacion.h"

int memoria_fd;
int cpu_fd;
int filesystem_fd;
int kernel_fd;
int RETARDO_RESPUESTA;
pthread_mutex_t mutexFS;
uint32_t pidGlobal;
uint32_t sizeGlobal;


int conectarModulo(char *modulo){
	printf("ENTRE A CONECTAR ");
	int conexion;
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


	conexion= crear_conexion(ip, puerto);

	return conexion;
}

int recibirConexion(char *puerto) {
	logger = log_create("modulo.log", "-", 1, LOG_LEVEL_DEBUG);
	pthread_t tid[3];

	memoria_fd = iniciar_servidor(puerto);
	log_info(logger, "Servidor listo para recibir a los clientes");

	cpu_fd = esperar_cliente(memoria_fd);
	pthread_create(&tid[0], NULL, recibirCPU, NULL);
	filesystem_fd = esperar_cliente(memoria_fd);
	pthread_create(&tid[1], NULL, recibirFS, NULL);
	kernel_fd = esperar_cliente(memoria_fd);
	pthread_create(&tid[2], NULL, recibirKernel, NULL);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	pthread_join(tid[2], NULL);

	return EXIT_SUCCESS;
}

void *recibirCPU(void){
		while(1) {
			int cod_op = recibir_operacion(cpu_fd);
			t_list *lista;
			switch (cod_op) {
				case HANDSHAKE_CPU:
					recibirOrden(cpu_fd);
					log_info(logger,"HANDSHAKE con CPU acontecido");
					PaqueteHand(cpu_fd, logger);
					break;
				case SOLICITUDMARCO:
					lista = recibirListaUint32_t(cpu_fd);
					uint32_t marco = obtenerMarcoDePagina(*(uint32_t*)list_get(lista,0), *(uint32_t*)list_get(lista,1));
					enviarValor_uint32(marco, cpu_fd, SOLICITUDMARCO, info_logger);
//					list_clean(lista);
					list_destroy_and_destroy_elements(lista, free); //LINEA MODIFICADA
					break;
				case ACCESO_PEDIDO_LECTURA:
					realizarPedidoLectura(cpu_fd);
					break;
				case ACCESO_PEDIDO_ESCRITURA:
					realizarPedidoEscritura(cpu_fd);
					break;

				case SOLICITUDINSTRUCCION:
					lista = recibirListaUint32_t(cpu_fd);
					t_paquete* paquete = crear_paquete(SOLICITUDINSTRUCCION, info_logger);
					Instruccion* instruccion;
					log_info(logger, "PID: %d PC: %d", *(uint32_t*)list_get(lista,0),*(uint32_t*)list_get(lista,1));
					instruccion = retornarInstruccionACPU(*(uint32_t*)list_get(lista,0),*(uint32_t*)list_get(lista,1));
					usleep(RETARDO_RESPUESTA*1000);
					log_info(info_logger, "instruccion: %s %s %s\n", instruccion->id, instruccion->param1, instruccion->param2);
					agregar_instruccion_a_paquete(paquete, instruccion);
					enviar_paquete(paquete, cpu_fd);
					eliminar_paquete(paquete);
//					list_clean(lista);
					list_destroy_and_destroy_elements(lista, free); // ESTO ERA SOLO LIST_DESTROY
					break;
				case -1:
					log_error(logger, "el cliente se desconecto.");

						log_error(logger, "Terminando servidor.CPU");
						return NULL;
					break;//sacar
				default:
					log_warning(logger,"Operacion desconocida. No quieras meter la pata");
					break;
			}
		}
}

void *recibirFS(){
	while(1) {
		int cod_op = recibir_operacion(filesystem_fd);
		pthread_mutex_lock(&mutexFS);

		t_list *lista = list_create();
		switch (cod_op) {
			case ACCESO_PEDIDO_LECTURA:
				realizarPedidoEscrituraFs(filesystem_fd);

				break;
			case ACCESO_PEDIDO_ESCRITURA:
				realizarPedidoLecturaFs(filesystem_fd);

				break;
			case PEDIR_SWAP:
				lista = recibirListaUint32_t(filesystem_fd);
				cargarPaginasEnTabla(pidGlobal, sizeGlobal, lista);
				list_destroy_and_destroy_elements(lista, free); //LINEA AGREGADA
				break;
			case LECTURA_BLOQUE_SWAP:
				t_datos* unosDatos = malloc(sizeof(t_datos));
				void* datos;

				t_list* listaConSwap = recibirListaIntsYDatos(filesystem_fd,unosDatos);
				uint32_t posSwap = *(uint32_t *)list_get(listaConSwap, 0);
				datos = unosDatos->datos;
				recibirDatosDeFs(datos, posSwap);

				free(unosDatos->datos);
				free(unosDatos);
				list_destroy_and_destroy_elements(listaConSwap, free); //LINEA AGREGADA
				break;
			 case -1:
				 log_error(logger, "el cliente se desconecto.");

				 log_error(logger, "Terminando servidor.FILESYSTEM");
				 return NULL;

			 default:

				log_warning(logger,"Operacion desconocida. No quieras meter la pata %d ", cod_op);
				break;
		}
		pthread_mutex_unlock(&mutexFS);
	}
}


void *recibirKernel(){
	while(1) {
		int cod_op = recibir_operacion(kernel_fd);
		switch (cod_op) {
        	case INICIALIZAR_PROCESO_MEMORIA:
        		inicializarProceso(kernel_fd);
        		break;
        	case FINALIZAR_PROCESO_MEMORIA:
        		finalizarProceso(kernel_fd);
        		break;
        	case CARGA_PAGINA:
        		t_list *lista;
        		uint32_t pid;
        		lista = recibirListaUint32_t(kernel_fd);
        		pid = *(uint32_t*)list_get(lista,0);
        		TablaDePaginas* tabla;
        		tabla = obtenerTablaPorPID(pid);
        		cargarPaginaEnMemoria(tabla,*(uint32_t*)list_get(lista,1));
        		sleep(2);
        		enviarOrden(CARGA_PAGINA, kernel_fd, info_logger);
//        		list_clean(lista);
//        		list_destroy(lista); //LINEA AGREGADA
        		break;
			case -1:
				log_error(logger, "el cliente se desconecto.");
				log_error(logger, "Terminando servidor KERNEL");
				return NULL;
				break;//sacar
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
		}
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

void PaqueteHand(int conexion, t_log* logger){
	t_paquete* paquete = crear_paquete(HANDSHAKE_CPU,logger);

	char buffer[20];
	sprintf(buffer, "%d", TAM_PAGINA);

	agregar_a_paquete(paquete, buffer, strlen(buffer)+1);

	enviar_paquete(paquete, conexion);
	free(paquete);
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

void terminar_programa(int conexion, t_log* logger){
	log_destroy(logger);
	liberar_conexion(conexion);
}

void paquete(int conexion, t_log* logger){
	char *leido;
	t_paquete* paquete = crear_paquete(PAQUETECLIENTE,logger);

	leido = readline("> ");
	while(*leido != '\0'){
		agregar_a_paquete(paquete, leido, strlen(leido)+1);
		free(leido);
		leido = readline("> ");
	}

	enviar_paquete(paquete, conexion);
	free(paquete);
	free(leido);
}

void realizarPedidoLectura(int cliente_socket){
    t_list* listaInts = recibirListaUint32_t(cliente_socket);
    uint32_t posicion = *(uint32_t*)list_get(listaInts,0);
    uint32_t tamanio = *(uint32_t*)list_get(listaInts,1);
    uint32_t pid = *(uint32_t*)list_get(listaInts,2);

    pthread_mutex_lock(&mutex_espacioContiguo);
    log_info(info_logger,"Accediendo a Espacio de Usuario para Lectura en la Dirección: <%d> de Tamanio: <%d> para el Proceso con PID: <%d>", posicion, tamanio, pid);
    simularRetardoSinMensaje(RETARDO_RESPUESTA);
    log_info(info_logger,"Se accedió a Espacio de Usuario correctamente");
    void* datos = recibePedidoDeLectura(posicion, tamanio, pid);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    t_datos* unosDatos = malloc(sizeof (t_datos));
    unosDatos->datos = datos;
    unosDatos->tamanio= tamanio;
    enviarListaIntsYDatos(listaInts,unosDatos, cliente_socket, info_logger, LECTURA_REALIZADA);
    free(datos);
    free(unosDatos);
    list_clean_and_destroy_elements(listaInts,free);
    list_destroy(listaInts);
}

void realizarPedidoLecturaFs(int cliente_socket){
    t_list* listaInts = recibirListaUint32_t(cliente_socket);
    uint32_t posicion = *(uint32_t*)list_get(listaInts,0);
    uint32_t tamanio = *(uint32_t*)list_get(listaInts,1);
    uint32_t pid = *(uint32_t*)list_get(listaInts,2);

    pthread_mutex_lock(&mutex_espacioContiguo);
    log_info(info_logger,"Accediendo a Espacio de Usuario para Lectura en la Dirección: <%d> de Tamanio: <%d> para el Proceso con PID: <%d>", posicion, tamanio, pid);
    simularRetardoSinMensaje(RETARDO_RESPUESTA);
    log_info(info_logger,"Se accedió a Espacio de Usuario correctamente");
    void* datos = recibePedidoDeLectura(posicion, tamanio, pid);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    t_datos* unosDatos = malloc(sizeof (t_datos));
    unosDatos->datos = datos;
    unosDatos->tamanio= tamanio;
    enviarListaIntsYDatos(listaInts,unosDatos, cliente_socket, info_logger, ESCRITURA_REALIZADA);
    free(datos);
    free(unosDatos);
    list_clean_and_destroy_elements(listaInts,free);
    list_destroy(listaInts);
}


void realizarPedidoEscritura(int cliente_socket){
    t_datos* unosDatos = malloc(sizeof(t_datos));
    t_list* listaInts = recibirListaIntsYDatos(cliente_socket, unosDatos);
    uint32_t* posicion = list_get(listaInts,0);
    uint32_t* pid = list_get(listaInts,1);
    pthread_mutex_lock(&mutex_espacioContiguo);
    log_info(info_logger,"Accediendo a Espacio de Usuario para Escritura en la Direccion: <%d> para el Proceso con PID: <%d>", *posicion, *pid);
    simularRetardoSinMensaje(RETARDO_RESPUESTA);
    log_info(info_logger,"Se accedio a Espacio de Usuario correctamente");
    recibePedidoDeEscritura(*posicion,unosDatos->datos, unosDatos->tamanio, *pid);
    free(unosDatos->datos);
    free(unosDatos);
    list_clean_and_destroy_elements(listaInts, free);
    list_destroy(listaInts);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    enviarOrden(ESCRITURA_REALIZADA, cliente_socket, info_logger);
}

void realizarPedidoEscrituraFs(int cliente_socket){
    t_datos* unosDatos = malloc(sizeof(t_datos));
    t_list* listaInts = recibirListaIntsYDatos(cliente_socket, unosDatos);
    uint32_t* posicion = list_get(listaInts,0);
    uint32_t* pid = list_get(listaInts,1);
    pthread_mutex_lock(&mutex_espacioContiguo);
    log_info(info_logger,"Accediendo a Espacio de Usuario para Escritura en la Direccion: <%d> para el Proceso con PID: <%d>", *posicion, *pid);
    simularRetardoSinMensaje(RETARDO_RESPUESTA);
    log_info(info_logger,"Se accedio a Espacio de Usuario correctamente");
    recibePedidoDeEscritura(*posicion,unosDatos->datos, unosDatos->tamanio, *pid);
    free(unosDatos->datos);
    free(unosDatos);
    list_clean_and_destroy_elements(listaInts, free);
    list_destroy(listaInts);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    enviarOrden(LECTURA_REALIZADA, cliente_socket, info_logger);
}

void inicializarProceso(int cliente_socket){

    char* file_name=recibirEnteroEnteroChar(cliente_socket,&pidGlobal,&sizeGlobal);
    log_info(info_logger, "%s %d %d", file_name, pidGlobal, sizeGlobal);

    list_add(instruccionesEnMemoria,obtenerArchivoPseudoCodigo(pidGlobal,file_name,PATH_INSTRUCCIONES));
    free(file_name);
    crearTablaDePaginas(sizeGlobal);
	sleep(1);
	log_info(info_logger,"Tamaño de tablaGeneral antes de terminar inicializarProceso: %d\n", list_size(tablaGeneral));

	enviarOrden(INICIALIZAR_PROCESO_MEMORIA,cliente_socket, info_logger);

}

void finalizarProceso(int cliente_socket){
	log_info(info_logger,"Tamaño de tablaGeneral al llegar a finalizarProceso: %d\n", list_size(tablaGeneral));
    pthread_mutex_lock(&mutex_tablasPaginas);
    pthread_mutex_lock(&mutex_espacioContiguo);
	t_list* posicionesSwap = list_create();
    uint32_t pid = recibirValor_uint32(cliente_socket);
	TablaDePaginas* tabla = obtenerTablaPorPID(pid);
	if (tabla!=NULL){
		for (int j = 0; j < list_size(tabla->paginas); j++) {
			Pagina* pagina = list_get(tabla->paginas, j);
			list_add(posicionesSwap, &(pagina->posSwap));
			marcarMarcoLibre(pagina->marco);
		}
		liberarModuloSwap(posicionesSwap);
		list_destroy_and_destroy_elements(posicionesSwap, free); //LINEA AGREGADA
	}
   bool buscarPorPID(ProgramaDeProceso* programa){
	if(programa->pid == pid) return true;
	return false;
   }

   ProgramaDeProceso* programa = list_find(instruccionesEnMemoria, buscarPorPID);
   for(int i=0;i<list_size(programa->instrucciones);i++){
	   Instruccion* instruccion = list_get(programa->instrucciones,i);
	   free(instruccion->id);
	   free(instruccion->param1);
	   free(instruccion->param2);
   }

   list_remove_by_condition(instruccionesEnMemoria,buscarPorPID);
   free(programa);

    if(tabla!=NULL)
    	liberarTablaDePaginas(pid);
    pthread_mutex_unlock(&mutex_tablasPaginas);
    pthread_mutex_unlock(&mutex_espacioContiguo);
    enviarOrden(FINALIZAR_PROCESO_MEMORIA, cliente_socket, info_logger);
    log_info(info_logger, "Proceso finalizado con éxito");
    log_info(info_logger,"Tamaño de tablaGeneral al terminar finalizarProceso: %d\n", list_size(tablaGeneral));
}
