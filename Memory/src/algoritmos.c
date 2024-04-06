#include "algoritmos.h"

t_queue* colaFIFO;
t_list* paginasConTimestamp;

void inicializarColaFIFO() {
    colaFIFO = queue_create();
}

void encolarPaginaFIFO(Pagina* pagina) {
    queue_push(colaFIFO, pagina);
}

Pagina* algoritmoFIFO() {
    if (queue_is_empty(colaFIFO)) {
        log_warning(logger, "La cola FIFO está vacía.");
        return NULL;
    }
    Pagina* paginaVictima = queue_pop(colaFIFO);
    return paginaVictima;
}

void inicializarListaPaginas() {
    paginasConTimestamp = list_create();
}

uint64_t obtenerTiempoActual() {
    return (uint64_t)time(NULL);
}

void actualizarTimestamp(Pagina* pagina) {
    pagina->timestamp = obtenerTiempoActual();
}

int compararTimestampDescendente(void* pagina1, void* pagina2) {
	if((((Pagina*)pagina2)->timestamp) < ((Pagina*)pagina1)->timestamp){
		return pagina2;
	}

	return pagina1;
}

Pagina* algoritmoLRU() {
    if (list_is_empty(paginasConTimestamp)) {
        log_warning(logger, "La lista de páginas con timestamp está vacía.");
        return NULL;
    }
    if(list_size(paginasConTimestamp)==1){
    	Pagina* paginaVictima = list_get(paginasConTimestamp, 0);
    	return paginaVictima;
    }
    list_sort(paginasConTimestamp, (void*)compararTimestampDescendente);
    Pagina* paginaVictima = list_get(paginasConTimestamp, 0);
    return paginaVictima;
}

