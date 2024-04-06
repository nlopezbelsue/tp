#ifndef ESQUEMA_H_
#define ESQUEMA_H_
#include "memoriaInstrucciones.h"
#include "algoritmos.h"
#include "main.h"

typedef struct {
    TablaDePaginas* procesos;
    size_t numProcesos;
} TablaGeneral;

typedef struct{
	int tamanioMemoria;
	void *espacioDisponible;
	int memoriaIniciada;
}espacioContiguoMemoria;

extern espacioContiguoMemoria espacioUsuario;
extern TablaDePaginas tablaDePaginas;
extern int tam_pagina;
extern bool flagComunicacion;

void asignarEspacioContiguoDeMemoria();
void liberarModuloSwap(t_list *);
void creacionDeProceso(uint32_t, uint32_t);
void finalizacionDeProceso(uint32_t);
void* recibePedidoDeLectura(uint32_t, uint32_t, uint32_t);
void recibePedidoDeEscritura(int, void*, uint32_t, uint32_t);
uint32_t accesoTablaPaginas(const TablaDePaginas*, uint32_t, uint32_t);
void* lecturaEspacioUsuario(void**,void*);
char* escrituraEspacioUsuario(void**,void*,void*);
void asignarMarcoAPagina(uint32_t, int, TablaDePaginas*);
void cargarPaginaEnMemoria(TablaDePaginas*, int);
int obtenerDatoDesdePagina(TablaDePaginas*, int, int);
void PageFault();
void pedirPaginaAlFS();
Pagina *elegirPaginaVictima();
void* leerMarco(uint32_t);
//void escribirPagina(uint32_t, Pagina*);
void pedirDatosAFS(Pagina*);
t_list* obtenerListaSwap();
void* recibirDatosDesdeFS();
void obtenerModuloSwap();
void recibirDatosDeFs(void *, uint32_t );

TablaDePaginas* obtenerTablaPorPosSwap(uint32_t );
int obtenerNroPaginaPorPosSwap(uint32_t );


#endif /* ESQUEMA_H_ */
