#include "memoriaInstrucciones.h"


void* espacio_contiguo;
uint32_t *marcosAsignados;
char* charAux[50];

bool memoriaInicializada;
bool tablaPaginasCreada;
bool pagina0Creada;
bool semaforosCreados;

t_list* tablasPaginas;

pthread_mutex_t mutex_espacioContiguo;
pthread_mutex_t mutex_tablasPaginas;
pthread_mutex_t mutex_idPagina;
pthread_mutex_t mutex_espacioDisponible;

t_log* trace_logger;
t_log* debug_logger;
t_log* warning_logger;
t_log* error_logger;
Pagina* pagina0;
t_list* tablasPaginas;
Pagina *paginaParaAnalizar;

int TAM_MEMORIA;
int TAM_PAGINA;
int CANT_MARCOS;

bool inicializarBitmap() {
    TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
    TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
    CANT_MARCOS = TAM_MEMORIA / TAM_PAGINA;

    marcosAsignados = (uint32_t*)malloc(CANT_MARCOS * sizeof(uint32_t));

    if (marcosAsignados == NULL)
        return false;

    for (int i = 0; i < CANT_MARCOS; i++) {
        marcosAsignados[i] = 0;
    }
    return true;
}


void marcarMarcoOcupado(int numeroMarco) {
    if (numeroMarco >= 0 && numeroMarco < CANT_MARCOS) {
        marcosAsignados[numeroMarco] = 1;//bit de presencia en 1
    } else {
        printf("Número de marco inválido\n");
    }
}

void marcarMarcoLibre(int numeroMarco) {
    if (numeroMarco >= 0 && numeroMarco < CANT_MARCOS) {
        marcosAsignados[numeroMarco] = 0;//bit de presencia en 1
    } else {
        printf("Número de marco inválido\n");
    }
}

void liberarMemory() {
    free(marcosAsignados);
    marcosAsignados = NULL;
}


void crearTablaDePaginas(uint32_t size){

    int cantPaginas = size / TAM_PAGINA; //hay que ver si no perjudica el (int)

   obtenerModuloSwap(cantPaginas);

}

void cargarPaginasEnTabla(uint32_t pid, uint32_t size, t_list* listaSwap){
	TablaDePaginas* nuevaTabla = malloc(sizeof (TablaDePaginas));
	  if (nuevaTabla == NULL) {
	        log_info(info_logger, "Error: No se pudo asignar memoria para nuevaTabla\n");
	        exit(EXIT_FAILURE);
	    }

	  int cantPaginas = size / TAM_PAGINA;
	  nuevaTabla->pid = pid;
	    nuevaTabla->paginas = list_create();
	for(int i = 0; i < cantPaginas ; i++){
		Pagina* nuevaPagina = malloc(sizeof(Pagina));
	    if (nuevaPagina == NULL) {
	        printf("Error: No se pudo asignar memoria para nuevaPagina\n");
	        exit(EXIT_FAILURE);
	    }
		nuevaPagina->marco = 0;
		nuevaPagina->presencia = 0;
		nuevaPagina->modificado = 0;
		nuevaPagina->posSwap = *(uint32_t*)list_get(listaSwap,i); //HAY QUE VER COMO SE CONSIGUE DE FS

		list_add(nuevaTabla->paginas, nuevaPagina);
		free(nuevaPagina); //LINEA AGREGADA
	}
	pthread_mutex_lock(&mutex_tablasPaginas);
    list_add(tablaGeneral, nuevaTabla);
//    list_destroy_and_destroy_elements(nuevaTabla->paginas, free); //LINEA AGREGADA
    for(int i=0;i<list_size(tablaGeneral);i++){
    	log_info(info_logger,"Tamaño de tablaGeneral después de agregar: %d\n", list_size(tablaGeneral));
    }
    pthread_mutex_unlock(&mutex_tablasPaginas);
    log_info(info_logger, "PID: <%d> - Tamaño: <%d>", pid, cantPaginas);
}


uint32_t buscarMarcoLibre() {
    for (uint32_t i = 0; i < CANT_MARCOS; ++i) {
        if (marcosAsignados[i] == 0)
            return i;
    }
    return -1;//indica hay un Page Fault
}

int obtenerCantidadPaginas(TablaDePaginas* tabla) {
    if (tabla == NULL) {
        printf("Error: La tabla de páginas es NULL\n");
        exit(EXIT_FAILURE);
    }
    return list_size(tabla->paginas);
}

void liberarPagina(Pagina* pagina) {
    free(pagina); //AVISAR A FS
}

TablaDePaginas* obtenerTablaPorPID(uint32_t pid) {
    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas* tabla = list_get(tablaGeneral, i);
        if (tabla->pid == pid)
            return tabla;
    }
    return NULL;
}

uint32_t obtenerPIDPorTabla(TablaDePaginas* tabla) {
    if (tabla == NULL) {
        printf("Error: La tabla de páginas es NULL\n");
        exit(EXIT_FAILURE);
    }
    return tabla->pid;
}

uint32_t obtenerMarcoDePagina(uint32_t pid, uint32_t numeroPagina){
	TablaDePaginas* tabla = obtenerTablaPorPID(pid);
	Pagina* pagina = list_get(tabla->paginas, numeroPagina);
	if (pagina->presencia == 1){
		log_info(info_logger,"PID: <%d> - Pagina: <%d> - Marco: <%d>", pid, numeroPagina,pagina->marco);
		return pagina->marco;
	}
	else
		return -1;//PF
}

Pagina* obtenerPaginaConMarco(uint32_t marco){
    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas* tabla = list_get(tablaGeneral, i);
        for (int j = 0; j < list_size(tabla->paginas); j++) {
            Pagina* pagina = list_get(tabla->paginas, j);
            if (pagina->marco == marco)
                return pagina;
        }
    }
    printf("El marco solicitado está vacío");
    return NULL;
}

uint32_t obtenerNumeroPaginaConMarco(uint32_t marco){
    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas* tabla = list_get(tablaGeneral, i);
        for (int j = 0; j < list_size(tabla->paginas); j++) {
            Pagina* pagina = list_get(tabla->paginas, j);
            if (pagina->marco == marco && pagina->presencia == 1)
                return j;
        }
    }
    printf("El marco solicitado está vacío");
    return NULL;
}

uint32_t obtenerPIDDesdeMarco(uint32_t marco) {
    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas *tabla = list_get(tablaGeneral, i);
        for (int j = 0; j < list_size(tabla->paginas); j++) {
            Pagina *pagina = list_get(tabla->paginas, j);
            if (pagina->marco == marco && pagina->presencia == 1)
                return tabla->pid;
        }
    }
    return -1;
}

void liberarTablaDePaginas(uint32_t pid) {

	bool compararPorPID(void* elemento, void* pid) {
	    TablaDePaginas* tabla = (TablaDePaginas*)elemento;
	    uint32_t* pidBuscado = (uint32_t*)pid;
	    return tabla->pid == *pidBuscado;
	}
    int indice = -1;
    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas* tabla = list_get(tablaGeneral, i);
        if (compararPorPID(tabla, &pid)) {
            indice = i;
            break;
        }
    }

    if (indice != -1) {
        TablaDePaginas* tabla = list_remove(tablaGeneral, indice);
        free(tabla->paginas);
        free(tabla);
        log_info(info_logger, "Tabla con PID <%d> eliminada de tablaGeneral", pid);
    } else {
        log_info(info_logger, "No se encontró una tabla con PID <%d> en tablaGeneral", pid);
    }

}

bool crearEstructurasAdministrativas(){
    bool comp1 = crearSemaforos();
    bool comp2 = crearEspacioContiguoDeMemoria();
    bool comp3 = inicializarBitmap();
    return comp1 && comp2 && comp3;
}

bool crearSemaforos(){
    int comprobacionEspacioContiguo = pthread_mutex_init(&mutex_espacioContiguo,NULL);
    int comprobacionEspacioDisponible = pthread_mutex_init(&mutex_espacioDisponible,NULL);
    int comprobacionIdPagina = pthread_mutex_init(&mutex_idPagina,NULL);
    int comprobacionTablasPaginas = pthread_mutex_init(&mutex_tablasPaginas,NULL);
    int comprobacionFSMutex = pthread_mutex_init(&mutexFS, NULL);


    bool noEsIgualACero(int numero){
        return numero != 0;
    }
    if(noEsIgualACero(comprobacionFSMutex)){
    	log_error(error_logger, "No se pudieron inicializar los semaforos");
    	return false;
    }
    if(noEsIgualACero(comprobacionEspacioContiguo) || noEsIgualACero(comprobacionEspacioDisponible)){
        log_error(error_logger, "No se pudieron inicializar los semaforos");
        return false;
    }
    if(noEsIgualACero(comprobacionIdPagina) || noEsIgualACero(comprobacionTablasPaginas)){
        log_error(error_logger, "No se pudieron inicializar los semaforos");
        return false;
    }
    semaforosCreados = true;
    return true;
}

bool crearEspacioContiguoDeMemoria(){

    espacio_contiguo = malloc(TAM_MEMORIA);
    memset(espacio_contiguo,0,TAM_MEMORIA);
    memoriaInicializada = true;
    return true;
}

bool iniciarMemoria(){
    bool estructurasAdministrativas = crearEstructurasAdministrativas();//Crea y comprueba que se hayan inicializado todas las estructuras administrativas
    return estructurasAdministrativas;//Retorna si se pudieron crear las estructuras administrativas
}

ProgramaDeProceso* obtenerArchivoPseudoCodigo(uint32_t pid,char* file_name,char* path_instrucciones){

	ProgramaDeProceso* programa=malloc(sizeof(ProgramaDeProceso));
	programa->pid = pid;
	t_list* instrucciones=list_create();

	char* path=string_new();
	char linea[60];//Buffer dónde se carga la línea

	string_append(&path,path_instrucciones);
	string_append(&path, file_name);

	FILE* archivoPseudocodigo;
	archivoPseudocodigo= fopen(path,"r");
	if(archivoPseudocodigo==NULL) log_info(info_logger, "No se pudo abrir el archivo");

	while(fgets(linea,sizeof(linea),archivoPseudocodigo)){
		Instruccion* instruccion=malloc(sizeof(Instruccion));
		char *operador=malloc(sizeof(linea)),*par1=malloc(sizeof(linea)),*par2=malloc(sizeof(linea));
		operador[0]='\0';
		par1[0]='\0';
		par2[0]='\0';
		sscanf(linea,"%s %s %s",operador,par1,par2);

		instruccion->id=malloc(strlen(operador)+1);
		instruccion->param1=malloc(strlen(par1)+1);
		instruccion->param2=malloc(strlen(par2)+1);
		strcpy(instruccion->id,operador);
		strcpy(instruccion->param1,par1);
		strcpy(instruccion->param2,par2);


		instruccion->idLength = (uint8_t)strlen(instruccion->id);
		instruccion->param1Length = (uint8_t)strlen(instruccion->param1);
		instruccion->param2Length = (uint8_t)strlen(instruccion->param2);
		if(esInstruccionSinParametros(instruccion)) instruccion->cantidadParametros = 0;
		if(esInstruccionConUnParametro(instruccion)) instruccion->cantidadParametros = 1;
		if(esInstruccionConDosParametros(instruccion)) instruccion->cantidadParametros = 2;
		list_add(instrucciones,instruccion);
		free(operador);
		free(par1);
		free(par2);

	}
	fclose(archivoPseudocodigo);

	programa->instrucciones = instrucciones;

	return programa;
}

Instruccion* retornarInstruccionACPU(uint32_t pid,uint32_t pc){
	bool coincideProceso(ProgramaDeProceso* programa){
		return programa->pid == pid;
	}
	ProgramaDeProceso* programa = list_find(instruccionesEnMemoria,coincideProceso);
	Instruccion* instruccion = list_get(programa->instrucciones,pc);

	return instruccion;
}
