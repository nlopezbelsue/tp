#ifndef TPSO_LOGGERS_PLAN_AUX_H
#define TPSO_LOGGERS_PLAN_AUX_H

#include "planificador.h"
#include "init.h"
#include "comunicacion.h"

void eliminarElementoLista(PCB* , t_list*);

void ejecutar_FOPEN(PCB*, char*, uint32_t);
void ejecutar_FCLOSE(PCB*, char*);
void ejecutar_FSEEK(PCB*, char*, uint32_t);
void ejecutar_FTRUNCATE(PCB*, char*, uint32_t);
void ejecutar_FREAD(PCB*,char*, uint32_t);
void ejecutar_FWRITE(PCB*, char*, uint32_t);
int buscarArch_TablaGlobalArchivo(char*);
uint32_t buscarPosPunteroTablaLocal(char*, PCB*);
void agregarEntrada_TablaGlobalArchivosAbiertos(char*);
void desbloquearPcb_porNombreArchivo (char*);
char* reciboYActualizoContadorPeticionesFs(int);

#endif
