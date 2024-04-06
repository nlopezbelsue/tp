#include "cicloInstruccion.h"

t_log* info_logger;
t_log* error_logger;

char* nombre_instruccion_actual;

pthread_mutex_t mutex_ciclo;

Instruccion *instruccion;
int interrupciones;

bool cicloInstrucciones=true;

char registroCPU_AX[4];
char registroCPU_BX[4];
char registroCPU_CX[4];
char registroCPU_DX[4];


void copiar_registroPCB_a_los_registrosCPU (RegistrosCPU* registro) {
	memcpy(registroCPU_AX,registro->registro_AX,4);
	memcpy(registroCPU_BX,registro->registro_BX,4);
	memcpy(registroCPU_CX,registro->registro_CX,4);
	memcpy(registroCPU_DX,registro->registro_DX,4);
}

void copiar_registrosCPU_a_los_registroPCB(RegistrosCPU* registro) {
    memcpy(registro->registro_AX,registroCPU_AX,4);
    memcpy(registro->registro_BX,registroCPU_BX,4);
    memcpy(registro->registro_CX,registroCPU_CX,4);
    memcpy(registro->registro_DX,registroCPU_DX,4);
}


void ciclo_de_instruccion(){

	cicloInstrucciones = true;
	info_logger = log_create("info_logger.log","Cpu", true, LOG_LEVEL_INFO);
	interrupciones=0;
    pthread_mutex_init(&mutex_ciclo, NULL);

    while(cicloInstrucciones){    //   && (pcb->programCounter < list_size(pcb->instr)){

        fetch();

        decode();

        execute();

        checkInterrupt();

    }
    sem_post(&semaforoI);

    pthread_mutex_destroy(&mutex_ciclo);
    log_destroy(info_logger);
}

void fetch(){
	t_list* listaInts = list_create();

	log_info(info_logger, "PID: <%d> - FETCH - Program Counter: <%d>", PCB_Actual->id, PCB_Actual->program_counter);
	//cambiar a enviar 2 uint
	list_add(listaInts, &PCB_Actual->id);
	list_add(listaInts, &PCB_Actual->program_counter);


	enviarListaUint32_t(listaInts,memoria_fd, info_logger, SOLICITUDINSTRUCCION);

	list_clean(listaInts);
	list_destroy(listaInts);
}


void decode(){

	t_list* lista;
	op_code_cliente cod = recibir_operacion(memoria_fd);

	if(cod == SOLICITUDINSTRUCCION){        //en memoria mandar enviar instruccion
		instruccion = recibirInstruccion(memoria_fd);

		nombre_instruccion_actual=instruccion->id;
		log_info(info_logger,"instruccion recibida: %s", nombre_instruccion_actual);

	}
	else{
		log_error(error_logger,"FALLO en el recibo de la instruccion");
		cicloInstrucciones=0; //si hay pageFault tmbn tiene que devolver -1
	}

}


void execute() {
    if (strcmp(nombre_instruccion_actual , "SET") == 0) {
        char* registro = instruccion->param1;
        char* valor = instruccion->param2;

        log_info(info_logger, "PID: <%d> - Ejecutando: <SET> - <%s> - <%s>", PCB_Actual->id, registro, valor);

        ////log_debug(debug_logger,"tamanio del nombre del registro: %ld", strlen(registro));

        SET(registro, valor);

    }
    else if (strcmp(nombre_instruccion_actual , "SUM") == 0) {
		char* registro = instruccion->param1;
		char* valor = instruccion->param2;

		log_info(info_logger, "PID: <%d> - Ejecutando: <SUM> - <%s> - <%s>", PCB_Actual->id, registro, valor);

		////log_debug(debug_logger,"tamanio del nombre del registro: %ld", strlen(registro));

		SUM(registro, valor);

	}
    else if (strcmp(nombre_instruccion_actual , "SUB") == 0) {
		char* registro = instruccion->param1;
		char* valor = instruccion->param2;

		log_info(info_logger, "PID: <%d> - Ejecutando: <SUB> - <%s> - <%s>", PCB_Actual->id, registro, valor);

		////log_debug(debug_logger,"tamanio del nombre del registro: %ld", strlen(registro));

		SUB(registro, valor);

	}
    else if (strcmp(nombre_instruccion_actual , "JNZ") == 0) {
    		char* registro = instruccion->param1;
    		char* valor = instruccion->param2;

    		log_info(info_logger, "PID: <%d> - Ejecutando: <JNZ> - <%s> - <%s>", PCB_Actual->id, registro, valor);

    		////log_debug(debug_logger,"tamanio del nombre del registro: %ld", strlen(registro));

    		JNZ(registro, valor);

    	}
    else if (strcmp(nombre_instruccion_actual , "SLEEP") == 0) {
		char* segundos = instruccion->param1;

		log_info(info_logger, "PID: <%d> - Ejecutando: <SLEEP> - <%s>", PCB_Actual->id, segundos);

		////log_debug(debug_logger,"tamanio del nombre del registro: %ld", strlen(registro));

		SLEEP(segundos);

	}
    else if (strcmp(nombre_instruccion_actual, "WAIT") == 0) {
		char* recurso = instruccion->param1;
		log_info(info_logger, "PID: <%d> - Ejecutando <WAIT> - <%s>", PCB_Actual->id, recurso);

		ejecutar_WAIT(recurso);
	}
	else if (strcmp(nombre_instruccion_actual, "SIGNAL") == 0) {
		char* recurso = instruccion->param1;
		log_info(info_logger, "PID: <%d> - Ejecutando <SIGNAL> - <%s>", PCB_Actual->id, recurso);

		ejecutar_SIGNAL(recurso);
	}
    else if (strcmp(nombre_instruccion_actual, "MOV_IN") == 0) {
        char* registro = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <MOV_IN> - <%s> - <%d>", PCB_Actual->id, registro, direccion_logica);

        ejecutar_MOV_IN(registro, direccion_logica);
    }

    else if (strcmp(nombre_instruccion_actual, "MOV_OUT") == 0) {

        int direccion_logica = atoi(instruccion->param1);
        char* registro = instruccion->param2;

        ////log_debug(debug_logger,"tam registro: %ld", strlen(registro));

        log_info(info_logger, "PID: <%d> - Ejecutando: <MOV_OUT> - <%d> - <%s>", PCB_Actual->id, direccion_logica, registro);

        ejecutar_MOV_OUT(direccion_logica, registro);
    }

    else if (strcmp(nombre_instruccion_actual, "F_OPEN") == 0) {
        char* nombre_archivo = instruccion->param1;
        char* modo = instruccion->param2;
        log_info(info_logger, "PID: <%d> - Ejecutando <F_OPEN> - <%s> - <%s>", PCB_Actual->id, nombre_archivo, modo);

        ejecutar_F_OPEN(nombre_archivo, modo);
    }
    else if (strcmp(nombre_instruccion_actual, "F_CLOSE") == 0) {
        char* nombre_archivo = instruccion->param1;
        log_info(info_logger, "PID: <%d> - Ejecutando <F_CLOSE> - <%s>", PCB_Actual->id, nombre_archivo);

        ejecutar_F_CLOSE(nombre_archivo);
    }
    else if (strcmp(nombre_instruccion_actual, "F_SEEK") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_SEEK> - <%s> - <%d>", PCB_Actual->id, nombre_archivo, direccion_logica );

        ejecutar_F_SEEK(nombre_archivo, direccion_logica);
    }
    else if (strcmp(nombre_instruccion_actual, "F_READ") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_READ> - <%s> - <%d>", PCB_Actual->id, nombre_archivo, direccion_logica);

        ejecutar_F_READ(nombre_archivo, direccion_logica);
    }
    else if (strcmp(nombre_instruccion_actual, "F_WRITE") == 0) {
        char* nombre_archivo = instruccion->param1;
        int direccion_logica = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_WRITE> - <%s> - <%d>", PCB_Actual->id, nombre_archivo, direccion_logica);

        ejecutar_F_WRITE(nombre_archivo, direccion_logica);
    }
    else if (strcmp(nombre_instruccion_actual, "F_TRUNCATE") == 0) {
        char* nombre_archivo = instruccion->param1;
        int tamanio = atoi(instruccion->param2);
        log_info(info_logger, "PID: <%d> - Ejecutando: <F_TRUNCATE> - <%s> - <%d>", PCB_Actual->id, nombre_archivo, tamanio );

        ejecutar_F_TRUNCATE(nombre_archivo, tamanio);
    }

    else if (strcmp(nombre_instruccion_actual, "EXIT") == 0) {
        log_info(info_logger, "PID: <%d> - Ejecutando: <EXIT> -", PCB_Actual->id);

        ejecutar_EXIT();
    } else {
        log_info(info_logger, "PID: <%d> - NO EXISTE LA INSTRUCCION %s", PCB_Actual->id, nombre_instruccion_actual); // log_error() ?

    }
    liberarInstruccion(instruccion);
}

void checkInterrupt(){ //queda la parte que lo recibe

	if(interrupciones)
	{
		copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);

		t_paquete* paquete= crear_paquete(INTERRUPCIONCPU, info_logger);
		agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
		enviar_paquete(paquete, kernel_fd);
		eliminar_paquete(paquete);
		interrupciones=0;


		cicloInstrucciones = false;
		log_info(info_logger, "PID: <%d> - Error Interrupcion", PCB_Actual->id);
	}

}


