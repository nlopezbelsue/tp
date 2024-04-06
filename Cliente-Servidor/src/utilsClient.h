#ifndef UTILSCLIENT_H_
#define UTILSCLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <string.h>
#include "estructurasCompartidas.h"

typedef enum
{	MENSAJECLIENTE,
	PAQUETECLIENTE,
	CONTEXTOEJECUCION,
	SOLICITUDINSTRUCCION,
	WAIT,
	SIGNAL,
	F_OPEN,
	F_TRUNCATE,
	F_SEEK,
	F_WRITE,
	F_READ,
	F_CLOSE,
	EXIT,
	SOLICITUDMARCO,
	PAGE_FAULT,
	ACCESO_PEDIDO_LECTURA,
	LECTURA_REALIZADA,
	ACCESO_PEDIDO_ESCRITURA,
	ESCRITURA_REALIZADA,
	HANDSHAKE_CPU,
	INTERRUPCIONCPU,
    APERTURA_ARCHIVO_EXITOSA,
    APERTURA_ARCHIVO_FALLIDA,
	ESCRITURA_ARCHIVO_EXITOSA,
	TRUNCACION_ARCHIVO_EXITOSA,
	LECTURA_ARCHIVO_EXITOSA,
    CREACION_ARCHIVO_EXITOSA,
	SLEEPCPU,
	CARGA_PAGINA,
	INICIALIZAR_PROCESO_MEMORIA,
	FINALIZAR_PROCESO_MEMORIA,
	APERTURA_ARCHIVO,
	TRUNCACION_ARCHIVO,
	LECTURA_ARCHIVO,
	ESCRITURA_ARCHIVO,
	ESTRUCTURAS_INICIALIZADAS,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	CREACION_ARCHIVO,
	LECTURA_ARCHIVO_POSIBLE,
	ESCRITURA_ARCHIVO_POSIBLE,
	PEDIR_SWAP,
	DEVOLVER_SWAP,
	ESCRITURA_BLOQUE_SWAP,
	LECTURA_BLOQUE_SWAP,
	DESALOJOCPU
}op_code_cliente;

typedef struct
{	int size;
	void* stream;
} t_buffer;

typedef struct
{	op_code_cliente codigo_operacion;
	t_buffer* buffer;
} t_paquete;

int crear_conexion(char*, char*);
void enviar_mensaje(char*, int);
t_paquete* crear_paquete(op_code_cliente, t_log*);
void agregar_a_paquete(t_paquete*, void*, int);
void agregar_a_paquete2(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete*, int);
void liberar_conexion(int);
void eliminar_paquete(t_paquete*);

#endif /* UTILSCLIENT_H_ */
