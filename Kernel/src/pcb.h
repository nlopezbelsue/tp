#ifndef PCB_H_
#define PCB_H_

#include "estructurasCompartidas.h"
#include "utilidades.h"
#include "init.h"

PCB* crearPcb(char*, uint32_t, uint32_t);
void cambiarEstadoPCB(PCB*, EstadoProceso);
RegistrosCPU* crearRegistroCPU();

#endif /* PCB_H_ */
