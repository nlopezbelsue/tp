#include "ejecucionInstrucciones.h"

int tam_pagina;

void SET(char* registro, char* valor){
	pthread_mutex_lock(&mutex_ciclo);
    cambiar_valor_del_registroCPU(registro,valor);
    PCB_Actual->program_counter++;
    pthread_mutex_unlock(&mutex_ciclo);
}


void SUM(char* value1, char* value2) {
	pthread_mutex_lock(&mutex_ciclo);
    int suma=0;
	suma+=atoi(obtener_valor_registroCPU(value1));
	suma+=atoi(obtener_valor_registroCPU(value2));

	char buffer[20];
	sprintf(buffer, "%d", suma);

	pthread_mutex_unlock(&mutex_ciclo);
    SET(value1, buffer);

}

void SUB(char* value1, char* value2) {
	pthread_mutex_lock(&mutex_ciclo);
	int resta=0;
	resta+=atoi(obtener_valor_registroCPU(value1));
	resta-=atoi(obtener_valor_registroCPU(value2));

	char buffer[20];
	sprintf(buffer, "%d", resta);

	pthread_mutex_unlock(&mutex_ciclo);
	SET(value1, buffer);

}

void JNZ(char* registro, char* numeroInstruccion)
{
	pthread_mutex_lock(&mutex_ciclo);
	if(atoi(obtener_valor_registroCPU(registro)))
		PCB_Actual->program_counter=atoi(numeroInstruccion);
	else
		PCB_Actual->program_counter++;
	pthread_mutex_unlock(&mutex_ciclo);
}

void SLEEP(char* segundos) {
	pthread_mutex_lock(&mutex_ciclo);
	copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
	PCB_Actual->program_counter++;
	cicloInstrucciones = false;

	t_paquete* paquete= crear_paquete(SLEEPCPU, info_logger);
	uint32_t tiempo=atoi(segundos);
	agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
	agregar_a_paquete2(paquete, &(tiempo), sizeof(uint32_t));
	enviar_paquete(paquete, kernel_fd);
	eliminar_paquete(paquete);
	pthread_mutex_unlock(&mutex_ciclo);



}

void ejecutar_WAIT(char* nombre_recurso) {
	pthread_mutex_lock(&mutex_ciclo);
    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(WAIT, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
    agregar_a_paquete2(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, kernel_fd);//REVISAR
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}

void ejecutar_SIGNAL(char* nombre_recurso) {
	pthread_mutex_lock(&mutex_ciclo);
    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(SIGNAL, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_recurso) + 1;
    agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
    agregar_a_paquete2(paquete, nombre_recurso, largo_nombre);
    enviar_paquete(paquete, kernel_fd);//REVISAR
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}

void ejecutar_MOV_IN(char* registro, int direccion_logica) {
	pthread_mutex_lock(&mutex_ciclo);
    int direccion_fisica = traducir_direccion_logica(direccion_logica);

    if (!(direccion_fisica < 0)) {
    	   char* valor;
           valor = leer_valor_de_memoria(direccion_fisica);
           cambiar_valor_del_registroCPU(registro,valor);
           free(valor);
           PCB_Actual->program_counter++;
    }
    pthread_mutex_unlock(&mutex_ciclo);
}


void ejecutar_MOV_OUT(int direccion_logica, char* registro) {
	pthread_mutex_lock(&mutex_ciclo);
    char* valorDelRegistro = obtener_valor_registroCPU(registro);
    int direccion_fisica = traducir_direccion_logica(direccion_logica);

    if (!(direccion_fisica < 0)) {
        escribir_valor_en_memoria(direccion_fisica, valorDelRegistro);
        PCB_Actual->program_counter++;
    }
    free(valorDelRegistro);
    pthread_mutex_unlock(&mutex_ciclo);
}

void ejecutar_F_OPEN(char* nombre_archivo, char* modo) {
	pthread_mutex_lock(&mutex_ciclo);
    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(F_OPEN, info_logger);
	uint32_t valor=0;

    if(strcmp(modo, "W")==0){
    	valor=1;
    }

    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_archivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombre_archivo, largo_nombre);
	agregar_a_paquete2(paquete, &valor, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}


void ejecutar_F_TRUNCATE(char* nombre_archivo, uint32_t tamanio) {
	pthread_mutex_lock(&mutex_ciclo);
    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(F_TRUNCATE, info_logger);

    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_archivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombre_archivo, largo_nombre);
	agregar_a_paquete2(paquete, &tamanio, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}

void ejecutar_F_SEEK(char* nombre_archivo, uint32_t posicion) {
	pthread_mutex_lock(&mutex_ciclo);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(F_SEEK, info_logger);

    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_archivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombre_archivo, largo_nombre);
	agregar_a_paquete2(paquete, &posicion, sizeof(uint32_t));
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}


void ejecutar_F_WRITE(char* nombre_archivo, int direccion_logica) {
	pthread_mutex_lock(&mutex_ciclo);
    int direccion_fisica = traducir_direccion_logica(direccion_logica);
    if (!(direccion_fisica < 0)) {

        copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
        PCB_Actual->program_counter++;
        t_paquete* paquete = crear_paquete(F_WRITE, info_logger);

        agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
        uint32_t largo_nombre = strlen(nombre_archivo) + 1;
		agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
		agregar_a_paquete2(paquete, nombre_archivo, largo_nombre);
        agregar_a_paquete2(paquete, &direccion_fisica, sizeof(uint32_t));
        enviar_paquete(paquete, kernel_fd);
        eliminar_paquete(paquete);
    }
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}

void ejecutar_F_READ(char* nombre_archivo, int direccion_logica) {
	pthread_mutex_lock(&mutex_ciclo);
    int direccion_fisica = traducir_direccion_logica(direccion_logica);
    if (!(direccion_fisica < 0)) {

        copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
        PCB_Actual->program_counter++;
        t_paquete* paquete = crear_paquete(F_READ, info_logger);

        agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
        uint32_t largo_nombre = strlen(nombre_archivo) + 1;
		agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
		agregar_a_paquete2(paquete, nombre_archivo, largo_nombre);
        agregar_a_paquete2(paquete, &direccion_fisica, sizeof(uint32_t));
        enviar_paquete(paquete, kernel_fd);
        eliminar_paquete(paquete);
    }
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}



void ejecutar_F_CLOSE(char* nombre_archivo) {
	pthread_mutex_lock(&mutex_ciclo);

    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    PCB_Actual->program_counter++;
    t_paquete* paquete = crear_paquete(F_CLOSE,info_logger);

    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    uint32_t largo_nombre = strlen(nombre_archivo) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, nombre_archivo, largo_nombre);
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}

char* obtener_valor_registroCPU(char* registro) {


    if (strcmp(registro, "AX") == 0){

         char* valor = (char *) malloc (4 + 1);
         strcpy (valor,"pppp");
         memcpy(valor,registroCPU_AX,4);

        return valor;
    }
    if (strcmp(registro, "BX") == 0) {

         char* valor = (char *) malloc (4 + 1);
         strcpy (valor,"pppp");
        memcpy(valor,registroCPU_BX, 4);

        return valor;
     }
    if (strcmp(registro, "CX") == 0) {

        char* valor = (char *) malloc (4 + 1);
        strcpy (valor,"pppp");
        memcpy(valor, registroCPU_CX, 4);

        return valor;
     }
    if (strcmp(registro, "DX") == 0) {
        char* valor = (char *) malloc (4 + 1);
        strcpy (valor,"pppp");
        memcpy(valor, registroCPU_DX, 4);

        return valor;
     }
}

void cambiar_valor_del_registroCPU(char* registro, char* valor) {
    if (strcmp(registro, "AX") == 0)
        memcpy(registroCPU_AX, valor, 4);

    if (strcmp(registro, "BX") == 0)
        memcpy(registroCPU_BX, valor, 4);

    if (strcmp(registro, "CX") == 0)
        memcpy(registroCPU_CX, valor,4);

    if (strcmp(registro, "DX") == 0)
        memcpy(registroCPU_DX, valor, 4);
}

void escribir_valor_en_memoria(int direccion_fisica, char* valor) {


    t_list* listaInts = list_create();
    t_datos* unosDatos = malloc(sizeof(t_datos));
    unosDatos->tamanio= 4; //cantidad de bytes por consigna para escribir en memoria (coincide con el tamaño de un registro)
    unosDatos->datos = (void*) valor;
    list_add(listaInts, &direccion_fisica);
    list_add(listaInts, &PCB_Actual->id);

    enviarListaIntsYDatos(listaInts, unosDatos, memoria_fd, info_logger, ACCESO_PEDIDO_ESCRITURA);
    list_clean(listaInts);
    list_destroy(listaInts);

    char* valor2 = recibir_confirmacion_de_escritura() ;
    if (strcmp(valor2, "OK") == 0) {
    log_info(info_logger, "PID: <%d> - Accion: <ESCRIBIR> - Pagina:< %d > - Dirección Fisica: <%d> - Valor: <%s>", PCB_Actual->id, num_pagina, direccion_fisica, valor);
    }
    free(unosDatos);
 }

char* recibir_confirmacion_de_escritura(){

        char* valor;
        int cod_op = recibir_operacion(memoria_fd);

		switch (cod_op) {
		case ESCRITURA_REALIZADA:
             recibirOrden(memoria_fd);
             valor= "OK";
			 break;
        }
        return valor;
}

char* leer_valor_de_memoria(int direccion_fisica) {



    t_list* listaInts = list_create();
    uint32_t uint32t_dir_fis = direccion_fisica;
    uint32_t uint32t_tamanio = 4; //por consigna se escribe y lee 4 bytes

    list_add(listaInts, &uint32t_dir_fis);
    list_add(listaInts, &uint32t_tamanio);
    list_add(listaInts, &PCB_Actual->id);

    enviarListaUint32_t(listaInts,memoria_fd, info_logger, ACCESO_PEDIDO_LECTURA);
    char* valor = recibir_valor_de_memoria();
    list_clean(listaInts);
    list_destroy(listaInts);
    log_info(info_logger, "PID: <%d> - Acción: <LEER> - Pagina:< %d > - Dirección Fisica: <%d> - Valor: <%s>", PCB_Actual->id, num_pagina, direccion_fisica, valor);

    return valor;
 }

char*  recibir_valor_de_memoria(){

        char* valor;
        int cod_op = recibir_operacion(memoria_fd);

		switch (cod_op) {
		case LECTURA_REALIZADA:{
            t_datos* unosDatos = malloc(sizeof(t_datos));
            t_list* listaInts = recibirListaIntsYDatos(memoria_fd,unosDatos);
            uint32_t tamanio = *(uint32_t*)list_get(listaInts,1);
            valor = malloc(unosDatos->tamanio+1);
            memcpy(valor,unosDatos->datos,tamanio);
            valor[tamanio] = '\0';
            free(unosDatos->datos);
            free(unosDatos);
            list_clean_and_destroy_elements(listaInts,free);
            list_destroy(listaInts);

            break;
        }
        default:
                log_error(error_logger, "No se recibio el valor correctamente, cerrando el programa");
                exit(1);  //TODO: Hay que cerrar como se debe
        }
    return valor;
}


void ejecutar_EXIT() {
	pthread_mutex_lock(&mutex_ciclo);
    copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);
    t_paquete* paquete = crear_paquete(EXIT, info_logger);
    agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
    enviar_paquete(paquete, kernel_fd);
    eliminar_paquete(paquete);
    cicloInstrucciones = false;
    pthread_mutex_unlock(&mutex_ciclo);
}
