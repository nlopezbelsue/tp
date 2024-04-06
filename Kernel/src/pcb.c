#include "pcb.h"

PCB* crearPcb(char* path, uint32_t size, uint32_t prioridad){
	PCB *pcb = malloc(sizeof(PCB));
	pcb->id = idProcesoGlobal;
	pcb->prioridad = prioridad;//ANDA MAL
	pcb->program_counter = 0;
	pcb->registros = crearRegistroCPU();
	pcb->listaInstrucciones = list_create();
	pcb->tablaPaginas = list_create();
	pcb->archivos_abiertos = list_create();
	pcb->recursosTomados = list_create();
	pcb->tiempoEjecutando = 0;
	pcb->estado = NUEVO;
	pcb->size = size;
	pcb->nombreRecurso = malloc(strlen(path)+1);
	strcpy(pcb->nombreRecurso, path);

	idProcesoGlobal++;

  return pcb;
}

RegistrosCPU* crearRegistroCPU() {
	RegistrosCPU* puntero_registros = malloc(sizeof(RegistrosCPU));

    strcpy(puntero_registros->registro_AX, "0000");
    strcpy(puntero_registros->registro_BX, "0000");
    strcpy(puntero_registros->registro_CX, "0000");
    strcpy(puntero_registros->registro_DX, "0000");

    return puntero_registros;
}
