#ifndef UTILIDADES_H_
#define UTILIDADES_H_

#include "estructurasCompartidas.h"
#include "utilsServer.h"

extern int idProcesoGlobal;

int calcular_bytes_segun_registro(char*);
void enviarOrden(op_code_cliente, int, t_log *);
void* recibir_stream(int*, uint32_t);
void recibirOrden(int);
void EnviarInterrupt(int, uint32_t);
void EnviarMarco(int, uint32_t);
bool agregarIntsYDatosAPaquete(t_list*, t_datos*, t_paquete*);
bool enviarListaIntsYDatos(t_list*,t_datos*, int, t_log*, op_code_cliente);
t_list* recibirListaIntsYDatos(int,t_datos*);
bool agregarUint32_tsAPaquete(t_list*, t_paquete*);
bool enviarListaUint32_t(t_list*, int, t_log*, op_code_cliente);
void enviar_uint32_y_uint32_y_char(char* path, uint32_t valor1, uint32_t valor2, int socket, op_code_cliente orden, t_log *logger);
char* recibirEnteroEnteroChar(int socket_cliente, uint32_t* entero1, uint32_t* entero2);
bool esInstruccionSinParametros(Instruccion *);
bool esInstruccionConUnParametro(Instruccion *);
bool esInstruccionConDosParametros(Instruccion *);
void liberarInstruccion(Instruccion*);
void liberarPagina(Pagina*);
void liberarPcbCpu(PCB*);
PCB* recibir_pcb(int);
void agregar_instrucciones_a_paquete(t_paquete*, t_list*);
void agregar_registros_a_paquete(t_paquete*, RegistrosCPU*);
void agregar_PCB_a_paquete(t_paquete *, PCB*);
bool configTieneTodasLasPropiedades(t_config *cfg, char **properties);
t_list* recibirListaUint32_t(int);
void agregar_ContextoEjecucion_a_paquete(t_paquete *paquete, PCB* pcb);
Instruccion* recibirInstruccion(int conexion);
void agregar_instruccion_a_paquete(t_paquete *paquete, Instruccion* instruccion);
PCB* recibir_contextoEjecucion_y_char(int conexion);
PCB* recibir_contextoEjecucion_y_uint32(int conexion, uint32_t* direccion);
void enviarValor_uint32(uint32_t, int, op_code_cliente, t_log *);
bool enviarString(char*, int, op_code_cliente , t_log* );
bool agregarStringAPaquete(char*, t_paquete*);
char* recibirString(int);
PCB* recibir_contextoEjecucion(int conexion);
PCB* recibir_contextoEjecucion_y_char_y_uint32(int, uint32_t*);
void enviar_archivoTruncacion(t_archivoTruncate*, int, op_code_cliente, t_log*);
void agregar_archivoTruncacion_a_paquete(t_paquete*, t_archivoTruncate*);
t_archivoTruncate* recibir_archivoTruncacion(int);
void enviar_archivoRW(t_archivoRW*, int, op_code_cliente, t_log*);
void agregar_archivoRW_a_paquete(t_paquete*, t_archivoRW*);
t_archivoRW* recibir_archivoRW(int);
void simularRetardoSinMensaje(int);
uint32_t recibirValor_uint32(int);
bool enviarEnteroYString(uint32_t,char*, int, t_log*, op_code_cliente);
bool agregarEnteroYStringAPaquete(uint32_t, char*, t_paquete*);
char* recibirEnteroYString(int,uint32_t*);
void enviar_uint32_y_uint32(uint32_t, uint32_t, int, op_code_cliente, t_log*);
void recibirEnteroYEntero(int, uint32_t*, uint32_t*);
t_datos *recibir_datos(int );
void agregarDatosAPaquete(t_paquete* ,t_datos* );
void enviar_datos(t_datos* , int , op_code_cliente , t_log* );



#endif
