#ifndef MMU_H_
#define MMU_H_

#include "estructurasCompartidas.h"
#include "ejecucionInstrucciones.h"
#include "cicloInstruccion.h"

extern uint32_t num_pagina;

int traducir_direccion_logica(int);
uint32_t check_pageFault();

#endif
