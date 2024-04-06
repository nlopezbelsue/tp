#ifndef COMUNICACION_H_
#define COMUNICACION_H_

#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <string.h>
#include "utilsClient.h"
#include "utilsServer.h"
#include "utilidades.h"
#include "esquema.h"


extern t_config *config;

extern int memoria_fd;
extern int cpu_fd;
extern int filesystem_fd;
extern int kernel_fd;
extern int RETARDO_RESPUESTA;
extern pthread_mutex_t mutexFS;

int conectarModulo(char *);
t_log* iniciar_logger(char*);
void leer_consola(t_log*);
void *recibirCPU(void);
void *recibirFS(void);
void *recibirKernel(void);
void PaqueteHand(int, t_log*);
void Paquete(int, t_log*);
void terminar_programa(int, t_log*);
int recibirConexion(char *);
void iterator(char* value);
void inicializarProceso(int);
void limpiarYEliminarListaAuxiliarPeroSinEliminarContenido(t_list*);
bool enviarTablasPaginas(t_list*, int, t_log*,op_code_cliente);
bool agregarTablasAPaquete(t_list*, t_paquete*);
void finalizarProceso(int);
int finalizarProcesoConPid(uint32_t);
void liberarModuloSwap(t_list*);
void realizarPedidoLectura(int);
void realizarPedidoEscritura(int);
void realizarPedidoEscrituraFs(int cliente_socket);
void realizarPedidoLecturaFs(int cliente_socket);


#endif
