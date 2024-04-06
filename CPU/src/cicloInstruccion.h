#ifndef CICLOINSTRUCCION_H_
#define CICLOINSTRUCCION_H_

#include <stdlib.h>
#include "estructurasCompartidas.h"
#include "ejecucionInstrucciones.h"
#include "comunicacion.h"

extern t_log* info_logger;
extern t_log* error_logger;

extern bool cicloInstrucciones;

extern int interrupciones;

extern PCB* PCB_Actual;
extern pthread_mutex_t mutex_ciclo;

extern uint32_t tam_max_segmento;
extern char registroCPU_AX[4];
extern char registroCPU_BX[4];
extern char registroCPU_CX[4];
extern char registroCPU_DX[4];

void copiar_registroPCB_a_los_registrosCPU (RegistrosCPU*);
void copiar_registrosCPU_a_los_registroPCB(RegistrosCPU*);
void ciclo_de_instruccion();
void fetch();
void decode();
void execute();
void checkInterrupt();
void finalizo();


#endif
