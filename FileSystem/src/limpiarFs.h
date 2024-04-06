#ifndef LIMPIARFS_H_
#define LIMPIARFS_H_

#include "fileSystem.h"

extern t_log * infoLogger;
extern t_log *errorLogger;
extern t_log *traceLogger;

//extern t_config_fs *configFs;
//extern t_config * fileConfigFs;

extern t_list *listaFCBs;
extern bool logsCreados;
extern bool configCreado;
extern bool cfgCreado;
extern bool conexionesHechas;
extern bool semaforosCreados;
extern bool tablaFATcreada;
extern bool listaFCBsCreado;
extern bool bitmapCreado;
extern bool archivosUsadosCreado;
extern bool listaBloquesCreado;

void destruirLoggers();
void cerrar_programa();
void destruirConfig();
void destruirCfg();
void destruirListaFCBs();
void liberar_bitmap();
void destruirTFAT();

#endif /* LIMPIARFS_H_ */
