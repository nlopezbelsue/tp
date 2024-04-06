#include "init.h"

char** RECURSOS;
char** INSTANCIAS_RECURSOS;
int GRADO_MAX_MULTIPROGRAMACION;
char *ALGORITMO_PLANIFICACION;
int QUANTUM;

int procesosTotales_MP=0;
int contadorPeticionesFs=0;
int idProcesoGlobal=0;

bool planificacionFlag=true;

t_queue* colaNew;
t_list* colaExec;
t_queue* colaExit;
t_list* colaBloq;
t_list* estadoBlockRecursos;
t_queue* colaReady_FIFO;
t_list* listaRecursos;
t_list* colaReady;

t_list* tablaGlobal_ArchivosAbiertos;
t_list* listaPeticionesArchivos;

pthread_mutex_t mutex_colaNew;
pthread_mutex_t mutex_ColaReady;
pthread_mutex_t mutex_colaExec;
pthread_mutex_t mutex_colaBloq;
pthread_mutex_t mutex_colaExit;
pthread_mutex_t mutex_MP;
pthread_mutex_t mutex_listaPeticionesArchivos;
pthread_mutex_t mutex_contadorPeticionesFs;
pthread_mutex_t mutex_debug_logger;
pthread_mutex_t mutex_TGAA;

pthread_mutex_t planificacionLargo;
pthread_mutex_t planificacionCorto;

sem_t sem_procesosEnNew;
sem_t sem_procesosReady;
sem_t sem_procesosExit;
sem_t sem_cpuLibre;

pthread_mutex_t* semaforos_io;

pthread_t hilo_planificador_LP;
pthread_t hilo_planificador_corto;
pthread_t hilo_liberador_procesos;

void iniciarNecesidades(){
	tablaGlobal_ArchivosAbiertos = list_create();
	listaPeticionesArchivos = list_create();
    colaNew = queue_create();
    colaReady = list_create();
    colaExec = list_create();
    colaBloq = list_create();
    colaExit = queue_create();

    pthread_mutex_init(&mutex_colaExec, NULL);
    pthread_mutex_init(&mutex_colaNew, NULL);
    pthread_mutex_init(&mutex_ColaReady, NULL);
    pthread_mutex_init(&mutex_colaExec, NULL);
    pthread_mutex_init(&mutex_colaBloq, NULL);
    pthread_mutex_init(&mutex_colaExit, NULL);
    pthread_mutex_init(&mutex_MP, NULL);
    pthread_mutex_init(&mutex_contadorPeticionesFs, NULL);
    pthread_mutex_init(&mutex_debug_logger, NULL);
    pthread_mutex_init(&mutex_TGAA, NULL);

    sem_init(&sem_procesosEnNew,0,0);
    sem_init(&sem_procesosReady,0,0);
    sem_init(&sem_procesosExit,0,0);
    sem_init(&sem_cpuLibre,0,1);

	pthread_t tid[7];
	pthread_create(&tid[0], NULL, conectarMemoria, "MEMORIA");
	pthread_join(tid[0], NULL);
	pthread_create(&tid[1], NULL, conectarFilesystem, "FILESYSTEM");
	pthread_join(tid[1], NULL);
	pthread_create(&tid[2], NULL, conectarCPU, "CPU");
	pthread_join(tid[2], NULL);
	pthread_create(&tid[3], NULL, conectarCPUInterrupt, "CPU");
	pthread_join(tid[3], NULL);

    pthread_create(&hilo_planificador_LP, NULL, (void*)planificadorLargoPlazo, NULL);
    pthread_create(&hilo_planificador_corto, NULL, (void*)planificadorCortoPlazo, NULL);
    pthread_create(&hilo_liberador_procesos, NULL, (void*)liberar_procesos, NULL);

	pthread_create(&tid[4], NULL, iniciarConsola, NULL);
	pthread_create(&tid[5], NULL, escucharFilesystemRef, NULL);
	pthread_create(&tid[6], NULL, escucharCPURef, NULL);

	RECURSOS = config_get_array_value(config, "RECURSOS");
	INSTANCIAS_RECURSOS = config_get_array_value(config, "INSTANCIAS_RECURSOS");
	GRADO_MAX_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION_INI");
	ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(config, "QUANTUM");

    int dim = tamanioArray(RECURSOS);
    semaforos_io = calloc(dim,sizeof(pthread_mutex_t));
    iniciarSemaforoDinamico(semaforos_io, dim);

    pthread_mutex_init(&planificacionLargo, NULL);
    pthread_mutex_init(&planificacionCorto, NULL);

    cargarRecursos();

	pthread_join(tid[4], NULL);
	pthread_join(tid[5], NULL);
	pthread_join(tid[6], NULL);
	pthread_join(hilo_planificador_LP, NULL);
	pthread_join(hilo_planificador_corto,NULL);
	pthread_join(hilo_liberador_procesos,NULL);
}

int tamanioArray(char** array){
    int n=0;
    for(int i=0 ;*(array+i)!= NULL; i++)
        n++;
    return n;
}

int cargarRecursos(){
    listaRecursos = list_create();
    int dim = tamanioArray(RECURSOS);
    char** recursos = RECURSOS;
    char** instancias = INSTANCIAS_RECURSOS;
    for(int i = 0 ; i < dim ; i++){
        Recurso* recurso = malloc(sizeof(Recurso));
        recurso->nombreRecurso = recursos[i];
        recurso->instanciasRecurso = atoi(instancias[i]);
        recurso->indiceSemaforo = i;
        recurso->cola = list_create();
        list_add(listaRecursos,recurso);
    }
    estadoBlockRecursos = listaRecursos;
    return true;
}

void iniciarSemaforoDinamico(pthread_mutex_t* semaforo, int dim){
    for (int i = 0; i <dim ; ++i)
        pthread_mutex_init(&semaforos_io[i],NULL);
}
/*
void cerrar_programa() {

    if(recursosCargados)
        liberarRecursosKernel();
    if(estadosCargados)
        liberarEstadosKernel();
    if(semaforosCargados)
        liberarSemaforos();
    if(tablasFsCargadas)
        liberarManejoFs();
    if(semaforoDinamicoCargado)
        liberarSemaforoDinamico();
    if(configCreado)
        destruirConfig();
    if(cfgCreado)
        destruirCfg();

    cortar_conexiones();
    cerrar_servers();

    if(logsCreados)
        destruirLoggers();

    printf("\nCierre exitoso\n");
}*/
