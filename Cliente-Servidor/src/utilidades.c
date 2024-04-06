#include "utilidades.h"

int idProcesoGlobal = 0;

//void liberarPcbCpu(PCB* pcb){
//    //Lo tenemos que hacer distinto pq nosotros No serializamos la TablaArchivosAbiertos y Lista recursos tomados
//    //Entonces sino le estariamos haciendo free a una memoria que nunca reservamos
//    free(pcb->registros);
//    list_destroy_and_destroy_elements(pcb->listaInstrucciones,liberarInstruccion);
//    list_destroy_and_destroy_elements(pcb->tablaSegmentos->segmentos,liberarSegmento);
//    free(pcb->tablaSegmentos);
//    free(pcb);
//}

int calcular_bytes_segun_registro(char* registro){
    int bytes;

    if ((strcmp(registro, "AX") == 0)||(strcmp(registro, "BX") == 0)||(strcmp(registro, "CX") == 0)||(strcmp(registro, "DX") == 0))
        bytes = 4;

    return bytes;
}

void enviarOrden(op_code_cliente orden, int socket, t_log *logger) {
    t_paquete * paquete= crear_paquete(orden, logger);
    paquete->buffer->size+=sizeof (uint32_t);
    void* stream = malloc(paquete->buffer->size);
    uint32_t valor = 0;
    int offset= 0;
    memcpy(stream + offset, &valor, sizeof(uint32_t));
    paquete->buffer->stream = stream;

    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete);
    free(valor);
    free(offset);
}

void* recibir_stream(int* size, uint32_t cliente_socket) { //En realidad devuelve el stream, no el t_buffer
    recv(cliente_socket, size, sizeof(int), MSG_WAITALL);
    void *buffer = malloc(*size);
    recv(cliente_socket, buffer, *size, MSG_WAITALL);
    return buffer;
}

void recibirOrden(int socket){
    int tamanio;
    void* stream = recibir_stream(&tamanio, socket);
    free (stream);
}

void EnviarInterrupt(int socket, uint32_t pid){
	t_paquete* paquete = crear_paquete(INTERRUPCIONCPU,logger);

	char buffer[20];
	sprintf(buffer, "%d", pid);

	agregar_a_paquete2(paquete, buffer, strlen(buffer)+1);

	enviar_paquete(paquete, socket);
	free(paquete);
}

void EnviarMarco(int socket, uint32_t marco){
	t_paquete* paquete = crear_paquete(SOLICITUDMARCO,logger);
	char buffer[20];
	sprintf(buffer, "%d", marco);

	agregar_a_paquete2(paquete, buffer, strlen(buffer)+1);

	enviar_paquete(paquete, socket);
	free(paquete);
}

bool agregarIntsYDatosAPaquete(t_list* listaInts, t_datos* datos, t_paquete* paquete){

    paquete->buffer->size+= sizeof(uint32_t)*list_size(listaInts);
    paquete->buffer->size+= datos->tamanio + sizeof(uint32_t);

    paquete->buffer->size += sizeof(uint8_t);
    void* stream = malloc(paquete->buffer->size);
    int offset=0;

    void copiarElementos(uint32_t* unEntero){
        memcpy(stream + offset, unEntero, sizeof(uint32_t));
        offset+= sizeof(uint32_t);
    }
    uint8_t cantidad_ints = list_size(listaInts);
    memcpy(stream + offset, &cantidad_ints, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    list_iterate(listaInts,copiarElementos);
    memcpy(stream + offset, &datos->tamanio, sizeof(uint32_t));
    offset+= sizeof(uint32_t);
    memcpy(stream + offset, datos->datos, datos->tamanio);
    paquete->buffer->stream = stream;
    return true;

}

bool enviarEnteroYString(uint32_t entero,char* string, int socket_cliente, t_log* logger, op_code_cliente codigo)

{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarEnteroYStringAPaquete(entero,string, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

bool agregarEnteroYStringAPaquete(uint32_t entero, char* string, t_paquete* paquete)
{

    paquete->buffer->size+= sizeof(uint32_t);
    uint32_t tamanioString = strlen(string) +1;
    paquete->buffer->size+= tamanioString + sizeof(uint32_t);


    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    int offset=0; //desplazamiento



    memcpy(stream + offset, &entero, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &tamanioString, sizeof(uint32_t));
    offset+= sizeof(uint32_t);
    memcpy(stream + offset, string, tamanioString);
    paquete->buffer->stream = stream;

    return true;

}



char* recibirEnteroYString(int socket_cliente,uint32_t* entero)
{
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);

    memcpy(&entero, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento+=sizeof(uint32_t);

    uint32_t tamanioString =0;
    memcpy(&tamanioString, buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento+=sizeof(uint32_t);
    char* string = malloc(tamanioString);
    memcpy(string, buffer + desplazamiento, tamanioString);

    free(buffer);
    return string;

}


bool enviarListaIntsYDatos(t_list* listaInts,t_datos* datos, int socket_cliente, t_log* logger, op_code_cliente codigo)

{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarIntsYDatosAPaquete(listaInts, datos, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

t_list* recibirListaIntsYDatos(int socket_cliente,t_datos* datos){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    t_list* listaInts = list_create();
    uint8_t cantidad_ints = 0;
    memcpy(&cantidad_ints, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    for (int i = 0; i < cantidad_ints; ++i) {
        uint32_t* nuevoEntero = malloc(sizeof(uint32_t));
        memcpy(nuevoEntero, buffer + desplazamiento, sizeof (uint32_t));
        desplazamiento+=sizeof(uint32_t);
        list_add(listaInts, nuevoEntero);
    }

    memcpy(&datos->tamanio, buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento+=sizeof(uint32_t);
    datos->datos = malloc(datos->tamanio);
    memcpy(datos->datos, buffer + desplazamiento, datos->tamanio);

    free(buffer);
    return listaInts;

}

bool agregarUint32_tsAPaquete(t_list* listaInts, t_paquete* paquete)
{

    paquete->buffer->size+= sizeof(uint32_t)*list_size(listaInts);
    paquete->buffer->size += sizeof(uint8_t);

    void* stream = malloc(paquete->buffer->size);
    int offset=0;

    void copiarElementos(uint32_t* unEntero){
        memcpy(stream + offset, unEntero, sizeof(uint32_t));
        offset+= sizeof(uint32_t);
    }
    int cantidad_ints = list_size(listaInts);
    memcpy(stream + offset, &cantidad_ints, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    list_iterate(listaInts,copiarElementos);
    paquete->buffer->stream = stream;
    return true;
}

bool enviarListaUint32_t(t_list* listaInts, int socket_cliente, t_log* logger, op_code_cliente codigo)
{
    t_paquete* paquete = crear_paquete(codigo, logger);
    if(!agregarUint32_tsAPaquete(listaInts, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

t_list* recibirListaUint32_t(int socket_cliente){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    t_list* listaInts = list_create();
    int cantidad_ints = 0;
    memcpy(&cantidad_ints, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);

    for (int i = 0; i < cantidad_ints; ++i) {
        uint32_t* nuevoEntero = malloc(sizeof(uint32_t));
        memcpy(nuevoEntero, buffer + desplazamiento, sizeof (uint32_t));
        desplazamiento+=sizeof(uint32_t);
        list_add(listaInts, nuevoEntero);
        free(nuevoEntero); //linea agregada
    }
    free(buffer);
    return listaInts;
}


bool esInstruccionSinParametros(Instruccion * instruccion){
    return (strcmp(instruccion->id, "EXIT") == 0);
}

bool esInstruccionConUnParametro(Instruccion * instruccion){
    bool f_close = (strcmp(instruccion->id, "F_CLOSE") == 0);
    bool wait = (strcmp(instruccion->id, "WAIT") == 0);
    bool signal = (strcmp(instruccion->id, "SIGNAL") == 0);
    bool sleep =(strcmp(instruccion->id, "SLEEP") == 0);

    return  f_close || wait || signal || sleep;
}


bool esInstruccionConDosParametros(Instruccion * instruccion){

    bool set = (strcmp(instruccion->id, "SET") == 0);
    bool sum = (strcmp(instruccion->id, "SUM") == 0);
    bool sub = (strcmp(instruccion->id, "SUB") == 0);
    bool mov_in = (strcmp(instruccion->id, "MOV_IN") == 0);
    bool mov_out = (strcmp(instruccion->id, "MOV_OUT") == 0);
    bool jnz = (strcmp(instruccion->id, "JNZ") == 0);

    bool f_open = (strcmp(instruccion->id, "F_OPEN") == 0);
    bool f_truncate = (strcmp(instruccion->id, "F_TRUNCATE") == 0);
    bool f_seek = (strcmp(instruccion->id, "F_SEEK") == 0);
    bool f_write = (strcmp(instruccion->id, "F_READ") == 0);
	bool f_read = (strcmp(instruccion->id, "F_WRITE") == 0);

    return set || sum || sub || mov_in || mov_out || jnz ||f_open || f_truncate || f_seek || f_write || f_read;
}

void liberarInstruccion(Instruccion * instruccion){
    if(esInstruccionConUnParametro(instruccion))
        free(instruccion->param1);

    if(esInstruccionConDosParametros(instruccion)){
        free(instruccion->param1);
        free(instruccion->param2);
    }

    free(instruccion->id);
    free(instruccion);
}

void liberarPagina(Pagina* pagina)
{
	free(pagina);
}

void liberarPcbCpu(PCB* pcb){
    free(pcb->registros);
    free(pcb);
}


PCB* recibir_pcb(int conexion) {
    //Pido el stream del buffer en el que viene serilizada la pcb
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));
    t_list *instrucciones = list_create();
    TablaDePaginas* tablaPaginasRecv = malloc(sizeof(TablaDePaginas));
    t_list *paginas = list_create();

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->tiempoEjecutando), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->prioridad), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

	memcpy(&(PcbRecv->estado), buffer + desplazamiento, sizeof(EstadoProceso));
	desplazamiento += sizeof(EstadoProceso);

    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU, INSTRUCCIONES, TABLA DE PAGINAS
    //REGISTROS CPU (primero tiene los 3 tamaños fijos y luego los datos)

    uint32_t tamanioAx = 0;
    uint32_t tamanioEax = 0;
    uint32_t tamanioRax = 0;

    memcpy(&tamanioAx, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&tamanioEax, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&tamanioRax, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_BX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_CX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_DX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;

    PcbRecv->registros = registros;

    //Instrucciones
    //Al ser una lista primero recibimos el tamanio y luego la lista
    uint32_t tamanioListaInstrucciones = 0;

    memcpy(&tamanioListaInstrucciones, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanioListaInstrucciones; i++) {
        Instruccion *instruccion = malloc(sizeof(Instruccion));

        //INSTRUCCION: ID LENGTH, ID, CANTIDAD_PARAMETROS, PARAM 1 LENGTH, PARAM 1, ...

        //CANTIDAD_PARAMETROS
        memcpy(&(instruccion->cantidadParametros), buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento += sizeof(uint8_t);

        //ID LENGTH
        memcpy(&(instruccion->idLength), buffer + desplazamiento, sizeof(uint8_t));
        desplazamiento += sizeof(uint8_t);

        //ID
        instruccion->id = malloc(instruccion->idLength + 1);
        memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength + 1);
        desplazamiento += instruccion->idLength + 1;



        if (instruccion->cantidadParametros == 1) {
            //PARAM 1 LENGTH
            memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 1
            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
            desplazamiento += instruccion->param1Length + 1;
        }

        if (instruccion->cantidadParametros == 2) {
            //PARAM 1 LENGTH
            memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 1
            instruccion->param1 = malloc(instruccion->param1Length + 1);
            memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
            desplazamiento += instruccion->param1Length + 1;

            //PARAM 2 LENGTH
            memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
            desplazamiento += sizeof(uint8_t);

            //PARAM 2
            instruccion->param2 = malloc(instruccion->param2Length + 1);
            memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
            desplazamiento += instruccion->param2Length + 1;
        }

        list_add(instrucciones,instruccion);
    }
    PcbRecv->listaInstrucciones = instrucciones;

    //TABLA PAGINAS
    //PID DEL PROCESO LIGADO A LA TDP
    uint32_t pid = 0;

    memcpy(&pid, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Al ser una lista primero recibimos el tamanio

    uint32_t tamanioListaPaginas = 0;

    memcpy(&tamanioListaPaginas, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for(int i = 0 ; i<tamanioListaPaginas; i++){
        Pagina* pagina = malloc(sizeof(Pagina));

        //La página está compuesta por: marco, presencia, modificado, posSwap

        //ID
//		memcpy(&(pagina->id), buffer + desplazamiento, sizeof(uint32_t));
//		desplazamiento += sizeof(uint32_t);

        //Marco
        memcpy(&(pagina->marco), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //Presencia
        memcpy(&(pagina->presencia), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //Modificado
        memcpy(&(pagina->modificado), buffer + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        //posSwap
		memcpy(&(pagina->posSwap), buffer + desplazamiento, sizeof(uint32_t));
		desplazamiento += sizeof(uint32_t);

        list_add(paginas,pagina);
    }
    tablaPaginasRecv->paginas = paginas;
    tablaPaginasRecv->pid = PcbRecv->id;
    PcbRecv->tablaPaginas = tablaPaginasRecv;
    free(buffer);

    return PcbRecv;
}

void agregar_instrucciones_a_paquete(t_paquete *paquete, t_list *instrucciones) {

    uint32_t cantidad_instrucciones = list_size(instrucciones);

    agregar_a_paquete2(paquete, &cantidad_instrucciones, sizeof(uint32_t));

    for (int i = 0; i < cantidad_instrucciones; i++) {
        Instruccion* instruccion = list_get(instrucciones, i);

        agregar_a_paquete2(paquete, &(instruccion->cantidadParametros), sizeof(uint8_t));
        agregar_a_paquete2(paquete, &(instruccion->idLength), sizeof(uint8_t));
        agregar_a_paquete2(paquete, instruccion->id, instruccion->idLength + 1);


        if(instruccion->cantidadParametros == 1){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);          }

        if(instruccion->cantidadParametros == 2){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);
            agregar_a_paquete2(paquete,  &(instruccion->param2Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param2, instruccion->param2Length+1);        }

    }
}

void agregar_registros_a_paquete(t_paquete* paquete, RegistrosCPU* registro) {

    //Los valores a almacenar en los registros siempre tendrán la misma longitud que el registro,
    // es decir que si el registro es de 16 bytes siempre se escribirán 16 caracteres.

    uint32_t tamanioAx = 5;

    agregar_a_paquete2(paquete, &(tamanioAx), sizeof(uint32_t));

	agregar_a_paquete2(paquete, (registro->registro_AX), tamanioAx);
	agregar_a_paquete2(paquete, (registro->registro_BX), tamanioAx);
	agregar_a_paquete2(paquete, (registro->registro_CX), tamanioAx);
	agregar_a_paquete2(paquete, (registro->registro_DX), tamanioAx);

}

void agregar_tablaPaginas_a_paquete(t_paquete* paquete, TablaDePaginas* tablaPaginas){
	uint32_t pid = tablaPaginas->pid;
	agregar_a_paquete2(paquete, &pid, sizeof(uint32_t));
	uint32_t cantidad_paginas = list_size(tablaPaginas->paginas);

	agregar_a_paquete2(paquete, &cantidad_paginas, sizeof(uint32_t));

	for (int i = 0; i < cantidad_paginas; i++) {
		Pagina *paginas = list_get(tablaPaginas->paginas, i);
//		agregar_a_paquete(paquete, &(paginas->id), sizeof(uint32_t));
		agregar_a_paquete2(paquete, &(paginas->marco), sizeof(uint32_t));
		agregar_a_paquete2(paquete, &(paginas->presencia), sizeof(uint32_t));
		agregar_a_paquete2(paquete, &(paginas->modificado), sizeof(uint32_t));
		agregar_a_paquete2(paquete, &(paginas->posSwap), sizeof(uint32_t));
	}

}

void agregar_PCB_a_paquete(t_paquete *paquete, PCB* pcb) {
    //PCB(UINTS_32T): ID, PROGRAMAM COUNTER, ESTIMACION RAFAGA, RAFAGA ANTERIOR, TIEMPO LLEGADA READY, TIEMPO ENVIO EXEC
    agregar_a_paquete2(paquete, &(pcb->id), sizeof(uint32_t));
    agregar_a_paquete2(paquete, &(pcb->tiempoEjecutando), sizeof(uint32_t));
    agregar_a_paquete2(paquete, &(pcb->prioridad), sizeof(uint32_t));
    agregar_a_paquete2(paquete, &(pcb->program_counter), sizeof(uint32_t));
    agregar_a_paquete2(paquete, &(pcb->estado), sizeof (EstadoProceso));
    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU, INSTRUCCIONES, TABLA SEGMENTOS
    agregar_registros_a_paquete(paquete, pcb->registros);
    agregar_instrucciones_a_paquete(paquete, pcb->listaInstrucciones);
    agregar_tablaPaginas_a_paquete(paquete, pcb->tablaPaginas);
}

PCB* recibir_pcb_sleep(int conexion, uint32_t* segundos) {
	//Pido el stream del buffer en el que viene serilizada la pcb
	    uint32_t tamanioBuffer;
	    uint32_t desplazamiento = 0;
	    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

	    //Inicializo todas las estructuras que necesito
	    PCB *PcbRecv = malloc(sizeof(PCB));
	    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));


	    //ID
	    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
	    desplazamiento += sizeof(uint32_t);

	    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
	    desplazamiento += sizeof(uint32_t);

	    memcpy(segundos, buffer + desplazamiento, sizeof(uint32_t));
	    desplazamiento += sizeof(uint32_t);

	    //la parte de la PCB que no son uint32_t
	    //PCB: REGISTROS CPU

	    //REGISTROS CPU (primero tiene los 3 tamaños fijos y luego los datos)

	    uint32_t tamanioAx = 0;

	    memcpy(&tamanioAx, buffer + desplazamiento, sizeof(uint32_t));
	    desplazamiento += sizeof(uint32_t);


	    //Una vez obtenidos los tamaños, obtenemos los registros
	    memcpy(&(registros->registro_AX), buffer + desplazamiento, tamanioAx);
	    desplazamiento += tamanioAx;
	    memcpy(&(registros->registro_BX), buffer + desplazamiento, tamanioAx);
	    desplazamiento += tamanioAx;
	    memcpy(&(registros->registro_CX), buffer + desplazamiento, tamanioAx);
	    desplazamiento += tamanioAx;
	    memcpy(&(registros->registro_DX), buffer + desplazamiento, tamanioAx);
	    desplazamiento += tamanioAx;

	    PcbRecv->registros = registros;

	    free(buffer);

	    return PcbRecv;
}

void agregar_PCB_a_paquete_Sleep(t_paquete *paquete, PCB* pcb, uint32_t segundos) { //llamar con la direccion
	//PCB(UINTS_32T): ID
	agregar_a_paquete2(paquete, &(pcb->id), sizeof(uint32_t));
	agregar_a_paquete2(paquete, &(pcb->program_counter), sizeof(uint32_t));
	agregar_a_paquete2(paquete, &(segundos), sizeof(uint32_t));
	//la parte de la PCB que no son uint32_t
	//PCB: REGISTROS CPU
	agregar_registros_a_paquete(paquete, pcb->registros);
}

bool configTieneTodasLasPropiedades(t_config *cfg, char **properties){
    for (uint8_t i = 0; properties[i] != NULL; i++)
    	if (!config_has_property(cfg, properties[i]))
    		return false;
    return true;
}


void agregar_ContextoEjecucion_a_paquete(t_paquete *paquete, PCB* pcb) {
    //PCB(UINTS_32T): ID
    agregar_a_paquete2(paquete, &(pcb->id), sizeof(uint32_t));
    agregar_a_paquete2(paquete, &(pcb->program_counter), sizeof(uint32_t));
    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU
    agregar_registros_a_paquete(paquete, pcb->registros);
}

PCB* recibir_contextoEjecucion(int conexion) {
    //Pido el stream del buffer en el que viene serilizada la pcb
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    //Inicializo todas las estructuras que necesito
    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));


    //ID
    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //Program counter
    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    //la parte de la PCB que no son uint32_t
    //PCB: REGISTROS CPU

    //REGISTROS CPU (primero tiene los 3 tamaños fijos y luego los datos)

    uint32_t tamanioAx = 0;

    memcpy(&tamanioAx, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);


    //Una vez obtenidos los tamaños, obtenemos los registros
    memcpy(&(registros->registro_AX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_BX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_CX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_DX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;

    PcbRecv->registros = registros;

    free(buffer);

    return PcbRecv;
}

void agregar_instruccion_a_paquete(t_paquete *paquete, Instruccion* instruccion) {

        agregar_a_paquete2(paquete, &(instruccion->cantidadParametros), sizeof(uint8_t));
        agregar_a_paquete2(paquete, &(instruccion->idLength), sizeof(uint8_t));
        agregar_a_paquete2(paquete, instruccion->id, instruccion->idLength + 1);


        if(instruccion->cantidadParametros == 1){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);          }

        if(instruccion->cantidadParametros == 2){

            agregar_a_paquete2(paquete, &(instruccion->param1Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param1, instruccion->param1Length+1);
            agregar_a_paquete2(paquete,  &(instruccion->param2Length), sizeof(uint8_t));
            agregar_a_paquete2(paquete, instruccion->param2, instruccion->param2Length+1);        }

}

Instruccion* recibirInstruccion(int conexion){
	uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void *buffer = recibir_buffer(&tamanioBuffer, conexion);
	Instruccion* instruccion = malloc(sizeof(Instruccion));

	memcpy(&(instruccion->cantidadParametros), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	memcpy(&(instruccion->idLength), buffer + desplazamiento, sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);

	instruccion->id = malloc(instruccion->idLength + 1);
	memcpy(instruccion->id, buffer + desplazamiento, instruccion->idLength + 1);
	desplazamiento += instruccion->idLength + 1;



	if (instruccion->cantidadParametros == 1) {
		memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param1 = malloc(instruccion->param1Length + 1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
		desplazamiento += instruccion->param1Length + 1;
	}

	if (instruccion->cantidadParametros == 2) {
		memcpy(&(instruccion->param1Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param1 = malloc(instruccion->param1Length + 1);
		memcpy(instruccion->param1, buffer + desplazamiento, instruccion->param1Length + 1);
		desplazamiento += instruccion->param1Length + 1;

		memcpy(&(instruccion->param2Length), buffer + desplazamiento, sizeof(uint8_t));
		desplazamiento += sizeof(uint8_t);

		instruccion->param2 = malloc(instruccion->param2Length + 1);
		memcpy(instruccion->param2, buffer + desplazamiento, instruccion->param2Length + 1);
		desplazamiento += instruccion->param2Length + 1;
	}
	return instruccion;
}

PCB* recibir_contextoEjecucion_y_char(int conexion) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanioAx = 0;

    memcpy(&tamanioAx, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_BX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_CX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_DX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;

    PcbRecv->registros = registros;

    uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	PcbRecv->nombreRecurso = malloc(tamanioPalabra + 1);
	memcpy(PcbRecv->nombreRecurso, buffer + desplazamiento, tamanioPalabra + 1);
	desplazamiento += tamanioPalabra + 1;

    free(buffer);

    return PcbRecv;
}

PCB* recibir_contextoEjecucion_y_uint32(int conexion, uint32_t* direccion) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanioAx = 0;

    memcpy(&tamanioAx, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_BX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_CX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_DX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;

    PcbRecv->registros = registros;

	memcpy(direccion, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

PCB* recibir_contextoEjecucion_y_char_y_uint32(int conexion, uint32_t* numero) {
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_buffer(&tamanioBuffer, conexion);

    PCB *PcbRecv = malloc(sizeof(PCB));
    RegistrosCPU *registros = malloc(sizeof(RegistrosCPU));

    memcpy(&(PcbRecv->id), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(PcbRecv->program_counter), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanioAx = 0;

    memcpy(&tamanioAx, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(registros->registro_AX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_BX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_CX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;
    memcpy(&(registros->registro_DX), buffer + desplazamiento, tamanioAx);
    desplazamiento += tamanioAx;

    PcbRecv->registros = registros;

    uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	PcbRecv->nombreRecurso= malloc(tamanioPalabra);
	memcpy(PcbRecv->nombreRecurso, buffer + desplazamiento, tamanioPalabra);
	desplazamiento += tamanioPalabra;

	memcpy(numero, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

    free(buffer);

    return PcbRecv;
}

void enviarValor_uint32(uint32_t valor, int socket, op_code_cliente orden, t_log *logger)
{
    t_paquete * paquete= crear_paquete(orden, logger);
    paquete->buffer->size = sizeof(uint32_t);
    void* stream = malloc(paquete->buffer->size);
    int offset = 0;
    memcpy(stream + offset, &valor, sizeof(uint32_t));
    paquete->buffer->stream = stream;
    enviar_paquete(paquete,socket);
    log_info(logger, "se envio el paquete");
    eliminar_paquete(paquete);
}

void simularRetardoSinMensaje(int retardo){
    usleep(retardo*1000);
}

uint32_t recibirValor_uint32(int socket) {

    t_paquete *paquete = malloc(sizeof (t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream =recibir_stream(&paquete->buffer->size, socket);
    uint32_t valor = -1;
    memcpy(&valor, paquete->buffer->stream, sizeof(uint32_t));
    eliminar_paquete(paquete);

    return valor;
}

void enviar_uint32_y_uint32(uint32_t valor1, uint32_t valor2, int socket, op_code_cliente orden, t_log *logger){
    t_paquete * paquete= crear_paquete(orden, logger);
    agregar_a_paquete2(paquete, &valor1, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor2, sizeof(uint32_t));
    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete);
}

void recibirEnteroYEntero(int socket_cliente, uint32_t* entero1, uint32_t* entero2){
	uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void *buffer = recibir_buffer(&tamanioBuffer, socket_cliente);

	memcpy(entero1, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(entero2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	return;
}

void enviar_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, int socket, op_code_cliente orden, t_log *logger){
    t_paquete * paquete= crear_paquete(orden, logger);
    agregar_a_paquete2(paquete, &valor1, sizeof(uint32_t));
    agregar_a_paquete2(paquete, &valor2, sizeof(uint32_t));

    uint32_t largo_nombre = strlen(path) + 1;
	agregar_a_paquete2(paquete, &largo_nombre, sizeof(uint32_t));
	agregar_a_paquete2(paquete, path, largo_nombre);

    enviar_paquete(paquete,socket);
    eliminar_paquete(paquete);
}

char* recibirEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2){
	uint32_t tamanioBuffer;
	uint32_t desplazamiento = 0;
	void *buffer = recibir_buffer(&tamanioBuffer, socket_cliente);

	memcpy(entero1, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(entero2, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	uint32_t tamanioPalabra=0;
	memcpy(&tamanioPalabra, buffer + desplazamiento, sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	char* palabraRecibida = malloc(tamanioPalabra + 1);
	memcpy(palabraRecibida, buffer + desplazamiento, tamanioPalabra + 1);
	desplazamiento += tamanioPalabra + 1;

	return palabraRecibida;
}

bool enviarString(char* string, int socket_cliente, op_code_cliente codigoOperacion, t_log* logger){
    t_paquete* paquete = crear_paquete(codigoOperacion, logger);
    if(!agregarStringAPaquete(string, paquete)){
        log_error(logger, "Hubo un error cuando se intento agregar las instrucciones al paquete");
        return false;
    }
    enviar_paquete(paquete, socket_cliente);
    log_info(logger, "Se envio el paquete");
    eliminar_paquete(paquete);
    return true;
}

bool agregarStringAPaquete(char* string, t_paquete* paquete){
    int offset=0; //desplazamiento
    uint8_t tamanio = strlen(string) + 1;
    paquete->buffer->size+= tamanio + sizeof(uint8_t);
    void* stream = malloc(paquete->buffer->size); //Reservo memoria para el stream
    memcpy(stream + offset, &tamanio, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, string, tamanio);
    paquete->buffer->stream = stream;
    //log_info(logger, "Se agrego el paquete");
    return true;
}
char* recibirString(int socket_cliente){
    int tamanio;
    int desplazamiento = 0;
    void *buffer = recibir_stream(&tamanio, socket_cliente);
    uint8_t tamanioString = 0;
    char* string;
    memcpy(&tamanioString, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento+=sizeof(uint8_t);
    string= malloc(tamanioString);
    memcpy(string, buffer + desplazamiento, tamanioString);
    free(buffer);
    return string;
}

void enviar_archivoTruncacion(t_archivoTruncate* archivoTruncate, int conexion, op_code_cliente codigo, t_log* logger){
    t_paquete* paquete= crear_paquete(codigo, logger);
    agregar_archivoTruncacion_a_paquete(paquete,archivoTruncate);
    enviar_paquete(paquete, conexion);

    eliminar_paquete(paquete);
}

void agregar_archivoTruncacion_a_paquete(t_paquete* paquete, t_archivoTruncate* archivoTruncate){
	agregar_a_paquete2(paquete, &(archivoTruncate->nombreArchivoLength), sizeof(uint32_t));
    agregar_a_paquete2(paquete, archivoTruncate->nombreArchivo, archivoTruncate->nombreArchivoLength);
    agregar_a_paquete2(paquete,&(archivoTruncate->nuevoTamanio), sizeof (uint32_t));
}

t_archivoTruncate* recibir_archivoTruncacion(int conexion){
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_stream(&tamanioBuffer, conexion);

    t_archivoTruncate* archivoTruncacion = malloc(sizeof(t_archivoTruncate));

    memcpy(&(archivoTruncacion->nombreArchivoLength), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);

    archivoTruncacion->nombreArchivo = malloc(archivoTruncacion->nombreArchivoLength);
    memcpy(archivoTruncacion->nombreArchivo, buffer + desplazamiento, archivoTruncacion->nombreArchivoLength);
    desplazamiento += archivoTruncacion->nombreArchivoLength;

    memcpy(&(archivoTruncacion->nuevoTamanio), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);
    free(buffer);
    return archivoTruncacion;
}

void enviar_datos(t_datos* datos, int conexion, op_code_cliente codigo, t_log* logger){

	t_paquete* paquete = crear_paquete(codigo, logger);
	agregarDatosAPaquete(paquete, datos);
}

void agregarDatosAPaquete(t_paquete* paquete,t_datos* datos){

	agregar_a_paquete2(paquete, &datos->tamanio, sizeof(uint32_t));
	agregar_a_paquete2(paquete, &datos->datos, (datos->tamanio + 1));

}

t_datos *recibir_datos(int conexion){

	uint32_t tamBuffer;

	uint32_t offset=0;
	void* buffer = recibir_stream(&tamBuffer, conexion);

	t_datos* datos = malloc(sizeof(t_datos));

	memcpy(&(datos->datos), buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(&(datos->datos), buffer + offset, datos->tamanio);
	free(buffer);

	return datos;
}


void enviar_archivoRW(t_archivoRW* archivoRW, int conexion, op_code_cliente codigo, t_log* logger){
    t_paquete* paquete= crear_paquete(codigo, logger);
    agregar_archivoRW_a_paquete(paquete,archivoRW);
    enviar_paquete(paquete, conexion);
    free(archivoRW->nombreArchivo);
    free(archivoRW);
    eliminar_paquete(paquete);
}

void agregar_archivoRW_a_paquete(t_paquete* paquete, t_archivoRW* archivoRw){
    agregar_a_paquete2(paquete, &(archivoRw->nombreArchivoLength), sizeof(uint32_t));
    agregar_a_paquete2(paquete, archivoRw->nombreArchivo, archivoRw->nombreArchivoLength);
    agregar_a_paquete2(paquete,&(archivoRw->posPuntero),sizeof (uint32_t));
    agregar_a_paquete2(paquete,&(archivoRw->direcFisica), sizeof (uint32_t));
    agregar_a_paquete2(paquete,&(archivoRw->cantidadBytes), sizeof (uint32_t));
    agregar_a_paquete2(paquete,&(archivoRw->pid), sizeof (uint32_t));
}

t_archivoRW* recibir_archivoRW(int conexion){
    uint32_t tamanioBuffer;
    uint32_t desplazamiento = 0;
    void *buffer = recibir_stream(&tamanioBuffer, conexion);

    t_archivoRW* archivoRw = malloc(sizeof(t_archivoRW));

    memcpy(&(archivoRw->nombreArchivoLength), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);

    archivoRw->nombreArchivo = malloc(archivoRw->nombreArchivoLength);
    memcpy(archivoRw->nombreArchivo, buffer + desplazamiento, archivoRw->nombreArchivoLength);
    desplazamiento += archivoRw->nombreArchivoLength;

    memcpy(&(archivoRw->posPuntero), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(archivoRw->direcFisica), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(archivoRw->cantidadBytes), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(archivoRw->pid), buffer + desplazamiento, sizeof (uint32_t));
    desplazamiento += sizeof(uint32_t);
    free(buffer);
    return archivoRw;
}
