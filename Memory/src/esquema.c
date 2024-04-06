#include "esquema.h"

bool flagComunicacion = false;

espacioContiguoMemoria espacioUsuario;

void finalizacionDeProceso(uint32_t pid){
	t_list* posicionesSwap = list_create();
	TablaDePaginas* tabla = obtenerTablaPorPID(pid);
    for (int j = 0; j < list_size(tabla->paginas); j++) {
        Pagina* pagina = list_get(tabla->paginas, j);
        list_add(posicionesSwap, &(pagina->posSwap));
    }
    liberarModuloSwap(posicionesSwap);//contiene todos las posiciones de SWAP que van a quedar libres
	liberarTablaDePaginas(tabla);
	return;
};

void obtenerModuloSwap(uint32_t cantPaginas){


	enviarValor_uint32(cantPaginas, filesystem_fd, PEDIR_SWAP, info_logger);


}
/*
t_list* obtenerListaSwap(){
    t_list* listaSwap;
    int cod_op = recibir_operacion(filesystem_fd);

	switch (cod_op) {
	case PEDIR_SWAP:{
		listaSwap = recibirListaUint32_t(filesystem_fd);
        break;
    }
    default:
            log_error(error_logger, "No se recibio la lista correctamente, cerrando el programa");
            exit(1);  //TODO: Hay que cerrar como se debe
    }
return listaSwap;
}
*/
void liberarModuloSwap(t_list* listaSwap){
	enviarListaUint32_t(listaSwap, filesystem_fd, info_logger, DEVOLVER_SWAP);
	return;
}

void* recibePedidoDeLectura(uint32_t direccionFisica, uint32_t tamanio, uint32_t pid){	//Devuelve el valor de la direccion fisica pedida
	uint32_t marcoALeer = direccionFisica / TAM_PAGINA;
	Pagina* paginaLeida = obtenerPaginaConMarco(marcoALeer);
	void* datos= malloc(tamanio);
	log_info(info_logger,"PID: <%d> - Accion: <LEER> - Direccion fisica: <%d>", pid,direccionFisica);
	char *algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	if (strcmp(algoritmo, "LRU") == 0)
	            actualizarTimestamp(paginaLeida);
    memcpy(datos, espacio_contiguo + direccionFisica, tamanio);

    return datos;
}

void recibePedidoDeEscritura(int direccionFisica, void* datos, uint32_t tamanio,uint32_t pid){	//Escribir lo indicado a partir de la dirección física pedida
	uint32_t marcoAEscribir = direccionFisica / TAM_PAGINA;
	Pagina* paginaModificada = obtenerPaginaConMarco(marcoAEscribir);
	paginaModificada->modificado = 1;
	char *algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	if (strcmp(algoritmo, "LRU") == 0)
	            actualizarTimestamp(paginaModificada);
	log_info(info_logger,"PID: <%d> - Accion: <ESCRIBIR> - Direccion fisica: <%d>", pid,direccionFisica);
	memcpy(espacio_contiguo + direccionFisica, datos, tamanio);
}


void asignarMarcoAPagina(uint32_t numeroMarco, int numeroPagina, TablaDePaginas* tabla){
	Pagina* pagina = list_get(tabla->paginas, numeroPagina);
	pagina->marco = numeroMarco;
}



TablaDePaginas* obtenerTablaPorPosSwap(uint32_t posSwap) {
    for (int i = 0; i < list_size(tablaGeneral); i++) {
        TablaDePaginas* tabla = list_get(tablaGeneral, i);
        for (int i = 0; i<list_size(tabla->paginas);i++){
        	Pagina* pagina = list_get(tabla->paginas, i);
        	if(pagina->posSwap == posSwap){

				return tabla;
        	}

        }

    }

    return NULL;
}
int obtenerNroPaginaPorPosSwap(uint32_t posSwap) {
	for (int j = 0; j < list_size(tablaGeneral); j++) {
		TablaDePaginas* tabla = list_get(tablaGeneral, j);

		for (int i = 0; i<list_size(tabla->paginas);i++){
			Pagina* pagina = list_get(tabla->paginas, i);
			if(pagina->posSwap == posSwap){

					return i;
			}

		}

	}
	return -1;
}


void recibirDatosDeFs(void *datos, uint32_t posSwap){

	uint32_t marco = buscarMarcoLibre();
	log_info(info_logger, "marco encontrado: %d\n", marco);


	marcarMarcoOcupado(marco);

	memcpy(espacio_contiguo+(marco*TAM_PAGINA),datos, TAM_PAGINA); //HAY QUE VER BIEN COMO ES LO DEL ESPACIO_CONTIGUO

}

void cargarPaginaEnMemoria(TablaDePaginas* tabla, int numeroPagina) {
	char *algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	if(buscarMarcoLibre() != -1){

		uint32_t pidNuevaPagina;
		log_info(info_logger,"Cargando página %d en memoria...\n", numeroPagina);
		Pagina* pagina = list_get(tabla->paginas, numeroPagina);
		pagina->presencia = 1;
		uint32_t marco = buscarMarcoLibre();
		asignarMarcoAPagina(marco, numeroPagina, tabla);
		pidNuevaPagina = obtenerPIDDesdeMarco(marco);
		if(strcmp(algoritmo, "FIFO") == 0)
			encolarPaginaFIFO(pagina);
		if (strcmp(algoritmo, "LRU") == 0){
			actualizarTimestamp(pagina);
			list_add(paginasConTimestamp, pagina);
		}
		pedirDatosAFS(pagina);

		log_info(info_logger, "SWAP IN -  PID: <%d> - Marco: <%d> - Page In: <%d>-<%d>", pidNuevaPagina, marco, pidNuevaPagina, numeroPagina);
//        free(datos);
	}
	else{
		log_info(info_logger, "No hay espacio suficiente para cargar la pagina en memoria.\n");
		log_info(info_logger, "PAGE FAULT. Buscando pagina víctima...\n");
//		t_list* listaSwap = list_create();

		Pagina* pagina = list_get(tabla->paginas, numeroPagina);
		Pagina* paginaVictima = elegirPaginaVictima();
		uint32_t pidVictima = obtenerPIDDesdeMarco(paginaVictima->marco);
		uint32_t numeroPaginaVictima = obtenerNumeroPaginaConMarco(paginaVictima->marco);
		uint32_t pidNuevaPagina;
		uint32_t marco;
//		uint32_t numeroPaginaNueva;
		if(paginaVictima->modificado){
			t_list* listaInts = list_create();
			t_datos* datos;
			uint32_t posSwap = paginaVictima->posSwap;
			uint32_t direccionFisica = paginaVictima->marco*TAM_PAGINA;
			datos->datos = leerMarco(paginaVictima->marco*TAM_PAGINA);
			datos->tamanio = TAM_PAGINA;
			list_add(listaInts, &posSwap);
			list_add(listaInts, &direccionFisica);
			enviarListaIntsYDatos(listaInts,datos, filesystem_fd, info_logger, ESCRITURA_BLOQUE_SWAP);
//			list_destroy_and_destroy_elements(listaInts, free); //LINEA AGREGADA
			log_info(info_logger, "SWAP OUT -  PID: <%d> - Marco: <%d> - Page Out: <%d>-<%d>", pidVictima, paginaVictima->marco, pidVictima, numeroPaginaVictima);
		}
//		list_add(listaSwap, paginaVictima->posSwap);
//		liberarModuloSwap(listaSwap);
		paginaVictima->presencia = 0;
		pagina->presencia = 1;
		marcarMarcoLibre(paginaVictima->marco);
		marco = buscarMarcoLibre();
		asignarMarcoAPagina(marco, numeroPagina, tabla);
		pidNuevaPagina = obtenerPIDDesdeMarco(marco);
//		numeroPaginaNueva = obtenerNumeroPaginaConMarco(pagina->marco);
//		marcarMarcoOcupado(marco);
		marco = pagina->marco;
		if(strcmp(algoritmo, "FIFO") == 0)
			encolarPaginaFIFO(pagina);
		if (strcmp(algoritmo, "LRU") == 0){
			list_add(paginasConTimestamp, pagina);
			list_remove_element(paginasConTimestamp, paginaVictima);
			actualizarTimestamp(pagina);
		}
		pedirDatosAFS(pagina);

		 log_info(info_logger, "SWAP IN -  PID: <%d> - Marco: <%d> - Page In: <%d>-<%d>", pidNuevaPagina, marco, pidNuevaPagina, numeroPagina);
		 log_info(info_logger, "REEMPLAZO - Marco: <%d> - Page Out: <%d>-<%d> - Page In: <%d>-<%d>",pagina->marco,  pidVictima, numeroPaginaVictima, pidNuevaPagina, numeroPagina);
	}
}

Pagina *elegirPaginaVictima(){
	Pagina *paginaVictima;
	char *algoritmo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    if (strcmp(algoritmo, "FIFO") == 0) {
    	paginaVictima = algoritmoFIFO();
    	return paginaVictima;
    } else if (strcmp(algoritmo, "LRU") == 0) {
    	paginaVictima = algoritmoLRU();
    	return paginaVictima;
    } /*else {
        log_warning(logger, "Algoritmo desconocido: %s", algoritmo);
        return -1;
    }*/
    return NULL;
}

void* leerMarco(uint32_t direccionFisica){
	void* datos = malloc(TAM_PAGINA);
	memcpy(datos, espacio_contiguo + direccionFisica, TAM_PAGINA);
	return datos;
}

//void escribirPagina(uint32_t direccionFisica, Pagina* paginaAEScribir){
//	memcpy(espacio_contiguo + direccionFisica, paginaAEScribir->datos, TAM_PAGINA);
//}

void pedirDatosAFS(Pagina* pagina){

	uint32_t posSwap = pagina->posSwap;
	enviarValor_uint32(posSwap, filesystem_fd, LECTURA_BLOQUE_SWAP, info_logger);

}


