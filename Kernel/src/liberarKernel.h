#ifndef LIBERARKERNEL_H_
#define LIBERARKERNEL_H_

#include "comunicacion.h"
#include "init.h"

void cerrar_programa();
void liberarEstadosKernel();

void liberarSemaforos();

void liberarSemaforoDinamico();

void cortar_conexiones();

void liberarRecurso(void*);

void liberarRecursosKernel();

void liberarManejoFs();

void liberarArchivoPeticion(void*);

#endif
