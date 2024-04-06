#ifndef MAIN_H_
#define MAIN_H_

#include "pthread.h"
#include "comunicacion.h"
#include "leerConfigCPU.h"
#include "estructurasCompartidas.h"
#include "limpiarCPU.h"

void *recibir();
void *conectarMemoria();
void *recibirInterrupt();

#endif /* MAIN_H_ */


