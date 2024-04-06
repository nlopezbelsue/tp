#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "estructurasCompartidas.h"
#include "init.h"
#include "utilidades.h"
#include "consola.h"
#include "comunicacion.h"

void planificadorLargoPlazo();
void planificadorCortoPlazo();
void liberar_procesos();
void liberarPcb(PCB* pcb);
void aumentarGradoMP();
void* esperarRR(void*);
void moverProceso_readyExec();
void moverProceso_ExecBloq(PCB *);
void moverProceso_BloqReady(PCB* );
void liberarRecursosTomados(PCB* );
void moverProceso_NewReady();
void moverProceso_ExecExit(PCB *);
void moverProceso_ExecReady(PCB* );
void aumentarGradoMP();
void disminuirGradoMP();
void mostrarEstadoColas();
void mostrarEstadoColasAux(char*, t_list* );
void eliminarElementoLista(PCB*, t_list *);
void eliminarPcbTGAA_Y_actualizarTGAA(PCB* );
void actualizarTGAALuegoDeLiberacionDeArchivo(char* );
void actualizarDuenioTGAA(char* nombreArchivo, PCB* );
void eliminarArchivoTablaLocal(char* , PCB* );
bool criterioPrioridad(PCB*, PCB*);
void mandarPaquetePCB(PCB *);
void *esperarRR(void *);
void eliminarArchivoTGAA(char*);
void recibirEnteroYEntero(int, uint32_t*, uint32_t*);

#endif /* PLANIFICADOR_H_ */
