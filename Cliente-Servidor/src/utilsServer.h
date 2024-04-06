#ifndef UTILSSERVER_H_
#define UTILSSERVER_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>

typedef enum{
	MENSAJESERVIDOR,
	PAQUETESERVIDOR
}op_code_server;

extern t_log* logger;

void* recibir_buffer(int*, int);
void* recibir_buffer2(int* size, int socket_cliente);


int iniciar_servidor(char*);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);

#endif /* UTILSSERVER_H_ */
