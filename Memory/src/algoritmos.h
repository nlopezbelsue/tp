#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_
#include <stdint.h>
#include <commons/collections/queue.h>

#include "main.h"

void inicializarColaFIFO();
void encolarPaginaFIFO(Pagina*);
Pagina *algoritmoFIFO();
void inicializarListaPaginas();
uint64_t obtenerTiempoActual();
void actualizarTimestamp(Pagina*);
int compararTimestampDescendente(void*, void*);
Pagina *algoritmoLRU();

#endif
