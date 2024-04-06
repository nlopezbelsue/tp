#include "init.h"

bool listaFCBsCreado = false;
bool semaforosCreados = false;
bool tablaFATcreada = false;
bool bitmapCreado = false;
bool archivosUsadosCreado = false;
t_list *bitmap;
t_config_fs* configFs;
t_config *fileConfigFs;






int cargar_configuracion(char *path) {


    configFs->IP_MEMORIA = config_get_string_value(fileConfigFs, "IP_MEMORIA");
    log_trace(traceLogger, "IP_MEMORIA Cargada Correctamente: %s", configFs->IP_MEMORIA);

    configFs->PUERTO_MEMORIA = config_get_string_value(fileConfigFs, "PUERTO_MEMORIA");
    log_trace(traceLogger, "PUERTO_MEMORIA Cargada Correctamente: %s", configFs->PUERTO_MEMORIA);

    configFs->PUERTO_ESCUCHA = config_get_string_value(fileConfigFs, "PUERTO_ESCUCHA");
    log_trace(traceLogger, "PUERTO_ESCUCHA Cargada Correctamente: %s", configFs->PUERTO_ESCUCHA);

    configFs->RETARDO_ACCESO_FAT = config_get_int_value(fileConfigFs, "RETARDO_ACCESO_FAT");
    log_trace(traceLogger, "RETARDO_ACCESO_FAT Cargada Correctamente: %d", configFs->RETARDO_ACCESO_FAT);

    configFs->TAM_BLOQUE = config_get_int_value(fileConfigFs, "TAM_BLOQUE");
    log_trace(traceLogger, "TAM_BLOQUE Cargada Correctamente: %d", configFs->TAM_BLOQUE);

    configFs->PATH_BLOQUES = config_get_string_value(fileConfigFs, "PATH_BLOQUES");
    log_trace(traceLogger, "PATH_BLOQUES Cargada Correctamente: %s", configFs->PATH_BLOQUES);

    configFs->PATH_FAT = config_get_string_value(fileConfigFs, "PATH_FAT");
    log_trace(traceLogger, "PATH_FAT Cargada Correctamente: %s", configFs->PATH_FAT);

    configFs->PATH_FCB = config_get_string_value(fileConfigFs, "PATH_FCB");
    log_trace(traceLogger, "PATH_FCB Cargada Correctamente: %s", configFs->PATH_FCB);

    configFs->RETARDO_ACCESO_BLOQUE = config_get_int_value(fileConfigFs, "RETARDO_ACCESO_BLOQUE");
    log_trace(traceLogger, "RETARDO_ACCESO_BLOQUE Cargada Correctamente: %d", configFs->RETARDO_ACCESO_BLOQUE);

    configFs->CANT_BLOQUES_TOTAL = config_get_int_value(fileConfigFs, "CANT_BLOQUES_TOTAL");
    log_trace(traceLogger, "CANT_BLOQUES_TOTAL Cargada Correctamente: %d", configFs->CANT_BLOQUES_TOTAL);

    configFs->CANT_BLOQUES_SWAP = config_get_int_value(fileConfigFs, "CANT_BLOQUES_SWAP");
    log_trace(traceLogger, "CANT_BLOQUES_SWAP Cargada Correctamente: %d", configFs->CANT_BLOQUES_SWAP);


    log_trace(traceLogger, "Archivo de configuracion cargado correctamente");

    return true;
}

bool crear_bitmap_swap(){

	for(int i = 0; i < configFs->CANT_BLOQUES_SWAP; i++){

	    bool *bit = malloc(sizeof(bool));

	    *bit = 1;
	    list_add(bitmap, bit);



	}

	int tamBitmap = list_size(bitmap);

	if(tamBitmap == configFs -> CANT_BLOQUES_SWAP){

		log_trace(traceLogger, "Bitmap creado");

		return true; //Se creo de forma exitosa
	}

	liberar_bitmap();
	return false;
}
//Para saber que bloque es, es la posicion en la que se encuentra en la lista


void crearSemaforos(){
	 pthread_mutex_init(&mutex_ArchivosUsados,NULL);
	 pthread_mutex_init(&mutex_peticiones_pendientes, NULL);
	 pthread_mutex_init(&mutex_recvM, NULL);
	 pthread_mutex_init(&mutex_recvK, NULL);
	 sem_init(&contador_peticiones, 0, 0);

	 semaforosCreados = true;
}


bool iniciarFileSystem(){

	crearSemaforos();
	listaFCBs = list_create();
	listaFCBsCreado = true;
	bitmap = list_create();
	bitmapCreado = true;
	archivosUsados = list_create();
	archivosUsadosCreado = true;

	peticiones_pendientes = list_create();

	tablaFAT = list_create();
	initFAT();



	if(!crearArchivoBloques()){
		log_error(errorLogger, "No se pudo crear el archivo de bloques");
	}

	if(!crear_bitmap_swap()){
		log_error(errorLogger, "No se pudo crear el bitmap");
		return false;
	}

	 iniciar_atencion_peticiones();

	log_trace(traceLogger, "FileSystem iniciado exitosamente");

	return true;
}


void *recibir(){
	recibirConexion(configFs->PUERTO_ESCUCHA);
	return NULL;
}

void *conectarMemoria(){
	conectarModulo("MEMORIA");
	return NULL;
}

void iniciarArchivoDeBloques() {
    crearArchivoBloques();
}

bool crearArchivoBloques() {

		archivoBloques = malloc(sizeof(archBloques));
	    int blockCount = configFs->CANT_BLOQUES_TOTAL;
	    int blockSize = configFs->TAM_BLOQUE;
	    archivoBloques->tamanio = blockCount*blockSize;

	    archivoBloques->fd = open(configFs->PATH_BLOQUES, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
		if (archivoBloques->fd == -1){
	        log_error(errorLogger,"Error al abrir/crear el archivo de bloques");
	        return false;
		}

		ftruncate(archivoBloques->fd, archivoBloques->tamanio);
	    archivoBloques->archivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

	    if(archivoBloques->archivo == MAP_FAILED) {
	       log_error(errorLogger,"Error al mapear el archivo en memoria");
	       close(archivoBloques->fd);
	       return false;
	    }

	    msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);

	    log_info(infoLogger, "Archivo de Bloques creado");
	    close(archivoBloques->fd);
	    free(archivoBloques);
	    return true;
	}



void initFAT() {

	int cantEntradasFAT = configFs->CANT_BLOQUES_TOTAL - configFs->CANT_BLOQUES_SWAP;

	t_entradas* auxFAT = malloc(sizeof(t_entradas));


	auxFAT->numEntrada = 0;
	auxFAT->bloqueLibre = false;
	auxFAT->entradaSig = UINT32_MAX;



	list_add(tablaFAT, auxFAT);

	for(int i=1; i<cantEntradasFAT; i++){

		auxFAT =  malloc(sizeof(t_entradas));

		auxFAT->entradaSig = UINT32_MAX;
		auxFAT->numEntrada = i;
		auxFAT->bloqueLibre = true;

		list_add(tablaFAT, auxFAT);


	}

    if(access(configFs->PATH_FAT, F_OK) != -1)
    	return;
    createFAT();
}

bool createFAT() {

	tFat = malloc(sizeof(fatTable));
    int cantidadEntradas = configFs->CANT_BLOQUES_TOTAL-configFs->CANT_BLOQUES_SWAP;
    int tamanioEntrada= sizeof(t_entradas);
	tFat->tamanioArchivo= cantidadEntradas*tamanioEntrada;


    tFat->fd = open(configFs->PATH_FAT, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
	if (tFat->fd == -1){
        log_error(errorLogger,"Error al abrir/crear la tabla fat");
        return false;
	}else{
        //log_debug(debug_logger,"Se creo el archivo de bloques");
    }

	ftruncate(tFat->fd, tFat->tamanioArchivo);
    tFat->archivo = mmap(NULL,tFat->tamanioArchivo, PROT_READ | PROT_WRITE, MAP_SHARED, tFat->fd , 0);

    if(tFat->archivo == MAP_FAILED) {
       log_error(errorLogger,"Error al mapear el archivo en memoria");
       close(tFat->fd);
       return false;
    }

    msync(tFat->archivo, tFat->tamanioArchivo, MS_SYNC);

    t_entradas* auxFAT= malloc(sizeof(t_entradas));

    auxFAT->bloqueLibre = false;
    auxFAT->numEntrada = 0;
    auxFAT->entradaSig = 0;

    int offset = 0;
    memcpy(tFat->archivo + offset, &auxFAT->bloqueLibre, sizeof(uint32_t));
    offset+= sizeof(uint32_t);
    memcpy(tFat->archivo + offset, &auxFAT->numEntrada, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(tFat->archivo + offset, &auxFAT->entradaSig, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	auxFAT->bloqueLibre = true;
	auxFAT->entradaSig = UINT32_MAX;


    for(int i=1; i<cantidadEntradas; i++){
    	auxFAT->numEntrada = i;

    	memcpy(tFat->archivo + offset, &auxFAT->bloqueLibre, sizeof(uint32_t));
    	offset+= sizeof(uint32_t);
    	memcpy(tFat->archivo + offset, &auxFAT->numEntrada, sizeof(uint32_t));
    	offset += sizeof(uint32_t);
   		memcpy(tFat->archivo + offset, &auxFAT->entradaSig, sizeof(uint32_t));
   		offset += sizeof(uint32_t);
    }

   log_trace(traceLogger, "Tabla fat creada");
   msync(tFat->archivo, tFat->tamanioArchivo, MS_SYNC);
    munmap(tFat->archivo,tFat->tamanioArchivo );
   	close(tFat->fd);

    tFat->fd = open(configFs->PATH_FAT, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
  	if (tFat->fd == -1){
          log_error(errorLogger,"Error al abrir/crear la tabla fat");
          return false;
  	}else{
          //log_debug(debug_logger,"Se creo el archivo de bloques");
      }
  	tFat->archivo = mmap(NULL,tFat->tamanioArchivo, PROT_READ | PROT_WRITE, MAP_SHARED, tFat->fd , 0);
   	t_entradas* test1 = malloc(sizeof(t_entradas));
   	t_entradas* test2 = malloc(sizeof(t_entradas));
   	uint32_t offset2 = 0;
   	memcpy(&test1->bloqueLibre,tFat->archivo + offset2, sizeof(uint32_t));
   	offset2 += sizeof(uint32_t);
	memcpy(&test1->numEntrada,tFat->archivo + offset2, sizeof(uint32_t));
	offset2 += sizeof(uint32_t);
	memcpy(&test1->entradaSig,tFat->archivo + offset2, sizeof(uint32_t));
	offset2 += sizeof(uint32_t);
	memcpy(&test2->bloqueLibre,tFat->archivo + offset2, sizeof(uint32_t));
	offset2 += sizeof(uint32_t);
	memcpy(&test2->numEntrada,tFat->archivo + offset2, sizeof(uint32_t));
	offset2 += sizeof(uint32_t);
	memcpy(&test2->entradaSig,tFat->archivo + offset2, sizeof(uint32_t));



   	free(tFat);
    return true;
}

int sizeFAT(int cantBloquesTotal, int cantBloquesSwap){
	return (cantBloquesTotal - cantBloquesSwap) * sizeof(uint32_t);
}

