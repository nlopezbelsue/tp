
#ifndef MEMORIAINSTRUCCIONES_H_
#define MEMORIAINSTRUCCIONES_H_
#include "main.h"

//#define TAM_PAGINA config_get_int_value(config,"TAM_PAGINA")

//extern void* espacio_contiguo;
//extern uint32_t espacioDisponible;
//extern bool memoriaInicializada;
//extern bool tablaPaginasCreada;
//extern bool pagina0Creada;
//extern bool semaforosCreados;
//extern t_list* tablasPaginas;
//
//extern pthread_mutex_t mutex_espacioContiguo;
//extern pthread_mutex_t mutex_tablasPaginas;
//extern pthread_mutex_t mutex_idPagina;
//extern pthread_mutex_t mutex_espacioDisponible;
//
//extern t_log* trace_logger;
//extern t_log* debug_logger;
//extern t_log* info_logger;
//extern t_log* warning_logger;
//extern t_log* error_logger;

/*Estructuras de Memoria de instrucciones*/
typedef struct{
	uint32_t pid;//Identificador del proceso
	t_list* instrucciones;//Se listan las instrucciones del programa

}ProgramaDeProceso;//Estructura de un programa de un proceso

/*Esquema de memoria*/
void recibirCreacionDeProceso();
bool inicializarBitmap();//Inicializa el espacio contiguo de memoria con los marcos asignados
void marcarMarcoOcupado(int);//Modificamos el bit de P cuándo se trae una nueva página
void marcarMarcoLibre(int);
Pagina* obtenerPaginaConMarco(uint32_t);
uint32_t obtenerPIDDesdeMarco(uint32_t);
void liberarMemory();//Liberamos el espacio contiguo de memoria
void crearTablaDePaginas(uint32_t );//Creamos una tabla de páginas
//Pagina* crearPagina(uint32_t);//Creamos una página y pide a FS módulo SWAP
void liberarPagina(Pagina*);//Libera una página y notifica a FS para que libere módulo SWAP
int obtenerCantidadPaginas(TablaDePaginas*);
void liberarTablaDePaginas(uint32_t);//Liberamos las páginas y la estructura de la tabla de páginas en sí
TablaDePaginas* obtenerTablaPorPID(uint32_t);
uint32_t obtenerPIDPorTabla(TablaDePaginas*);
uint32_t buscarMarcoLibre();//Retorna en caso de encontrar el número de marco libre. Si no lo encuentra marca que hay que reemplazar con -1
//void asignarPaginasAProceso(PCB*, char*);//
uint32_t obtenerMarcoDePagina(uint32_t, uint32_t);//Obtiene y retorna el Marco pedido a consultar. De no estar retorna un PF mediante -1.
void recibePeticionDeMarco();
bool crearEstructurasAdministrativas();//Crea y comprueba las estructuras administrativas que requerimos
bool crearSemaforos();//Crea y comprueba los semáforos que vamos a necesitar
bool crearEspacioContiguoDeMemoria();//Crea y comprueba el espacio contiguo de memoria dónde escribirán los procesos
//bool crearTablasPaginas();//Crea y comprueba la tabla de página
bool iniciarMemoria();//Crea y comprueba que se haya inicializado memoria
uint32_t obtenerNumeroPaginaConMarco(uint32_t);
void cargarPaginasEnTabla(uint32_t , uint32_t , t_list* );


ProgramaDeProceso* obtenerArchivoPseudoCodigo(uint32_t,char*,char*); 
Instruccion* retornarInstruccionACPU(uint32_t,uint32_t); //Retorna una instrucción a pedido de la CPU. Se requiere el PID y el PC.

#endif
