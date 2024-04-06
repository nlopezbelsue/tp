#include "ejecuciones.h"

void ejecutar_FOPEN(PCB* pcb, char* nombreArch, uint32_t modo){
    log_info(info_logger,"PID: <%d> - Abrir Archivo: <%s>",pcb->id,nombreArch);

    pthread_mutex_lock(&mutex_TGAA);
    int pos = buscarArch_TablaGlobalArchivo(nombreArch);

    if(pos == -1){


        t_archivoPeticion* archivoPeticion = malloc(sizeof(t_archivoPeticion));
        t_archivo* archivo = malloc(sizeof(t_archivo));
        archivo->colaWrite = list_create();
        archivo->nombreArchivo = nombreArch;
        archivo->modo = modo;
        archivo->cola = list_create();
        pthread_mutex_init(&archivo->mutex,NULL);
        archivoPeticion->archivo = archivo;
        archivoPeticion->pcb = pcb;
        pthread_mutex_lock(&mutex_listaPeticionesArchivos);
        list_add(listaPeticionesArchivos,archivoPeticion);
        pthread_mutex_unlock(&mutex_listaPeticionesArchivos);

        enviarString(nombreArch, filesystem_fd, APERTURA_ARCHIVO, info_logger);
    }
    else{
        t_archivoPeticion* archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,pos);
        pthread_mutex_lock(&(archivoPeticion->archivo->mutex));
        t_archivoPeticion* peticion = malloc(sizeof (t_archivoPeticion));
        peticion->archivo = archivoPeticion->archivo;
        peticion->archivo->modo = modo;
        peticion->archivo->cola = archivoPeticion->archivo->cola;
        peticion->archivo->colaWrite = archivoPeticion->archivo->colaWrite;
        peticion->pcb = pcb;
        pthread_mutex_lock(&mutex_listaPeticionesArchivos);
        list_add(listaPeticionesArchivos,peticion);
        pthread_mutex_unlock(&mutex_listaPeticionesArchivos);
        pthread_mutex_unlock(&(archivoPeticion->archivo->mutex));

        t_archivoLocal* archivoLocal = malloc(sizeof (t_archivoLocal));
        archivoLocal->archivo = archivoPeticion->archivo;
        archivoLocal->ptro = 0;
        list_add(pcb->archivos_abiertos,archivoLocal);

        log_info(info_logger,"PID: <%d> - Bloqueado por: <%s>",pcb->id,archivoLocal->archivo->nombreArchivo);
        moverProceso_ExecBloq(pcb);
    }
    pthread_mutex_unlock(&mutex_TGAA);
}

void ejecutar_FCLOSE(PCB* pcb, char *nombreArch) {
    eliminarArchivoTablaLocal(nombreArch, pcb);
    log_info(info_logger,"PID: <%d> - Cerrar Archivo: <%s>",pcb->id,nombreArch);
    bool hayProcesosEsperandoPorArchivo = false;
    pthread_mutex_lock(&mutex_listaPeticionesArchivos);
    for (int i = 0; i < list_size(listaPeticionesArchivos); i++) {
        t_archivoPeticion *archivoPeticion = list_get(listaPeticionesArchivos, i);
        if (strcmp(archivoPeticion->archivo->nombreArchivo, nombreArch) == 0) {
            list_remove(listaPeticionesArchivos, i);
            actualizarDuenioTGAA(archivoPeticion->archivo->nombreArchivo, archivoPeticion->pcb);
            moverProceso_BloqReady(archivoPeticion->pcb);

            free(archivoPeticion);

            hayProcesosEsperandoPorArchivo = true;
            break;
        }
    }
    if (!hayProcesosEsperandoPorArchivo)
        eliminarArchivoTGAA(nombreArch);

    log_info(info_logger,"PID: <%d> - Cerrar Archivo: <%s>",pcb->id, nombreArch);
    pthread_mutex_unlock(&mutex_listaPeticionesArchivos);
    free(nombreArch);
    mandarPaquetePCB(pcb);
}


void ejecutar_FSEEK(PCB* pcb, char* nombreArchivo, uint32_t posicion){

    for(int i = 0; i < list_size(pcb->archivos_abiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->archivos_abiertos, i);
        if(strcmp(archivoLocal->archivo->nombreArchivo, nombreArchivo) == 0)
            archivoLocal->ptro = posicion;
    }
    mandarPaquetePCB(pcb);
    log_info(info_logger,"PID: <%d> - Actualizar puntero Archivo: <%s> - Puntero <%d>",pcb->id, nombreArchivo, posicion);
    free(nombreArchivo);
}

void ejecutar_FTRUNCATE(PCB* pcb, char* nombreArchivo, uint32_t tamanio){
    t_archivoTruncate* archivoParaFs = malloc(sizeof(t_archivoTruncate));
    archivoParaFs->nombreArchivoLength = strlen(nombreArchivo) + 1;
    archivoParaFs->nombreArchivo = nombreArchivo;
    archivoParaFs->nuevoTamanio = tamanio;

    moverProceso_ExecBloq(pcb);
    enviar_archivoTruncacion(archivoParaFs,filesystem_fd,TRUNCACION_ARCHIVO,info_logger);
    aumentarContadorPeticionesFs();

    log_info(info_logger,"PID: <%d> - Archivo: <%s> - Tamanio: <%d>", pcb->id, nombreArchivo, tamanio);
    free(archivoParaFs->nombreArchivo);
    free(archivoParaFs);
}

void actualizarPunteroLocal(char* nombreArchivo, PCB* pcb, uint32_t cantidadBytes){
    for(int i = 0; i < list_size(pcb->archivos_abiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->archivos_abiertos,i);
        if(strcmp(archivoLocal->archivo->nombreArchivo,nombreArchivo) == 0){
            archivoLocal->ptro += cantidadBytes;
        }else{
            log_error(info_logger, "El archivo <%s> no se encuentra en la tabla local del PID <%d>", nombreArchivo, pcb->id);
        }
    }
}


void ejecutar_FREAD(PCB* pcb, char* nombreArchivo, uint32_t direccionFisica){
    t_archivoRW* archivoParaFs = malloc(sizeof(t_archivoRW));
    archivoParaFs->nombreArchivoLength = strlen(nombreArchivo) + 1;
    archivoParaFs->nombreArchivo = nombreArchivo;
    archivoParaFs->posPuntero = buscarPosPunteroTablaLocal(nombreArchivo,pcb);
    archivoParaFs->direcFisica = direccionFisica;
    archivoParaFs->cantidadBytes = 4;
    archivoParaFs->pid = pcb->id;

    log_info(info_logger,"PID: <%d> - Leer Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamanio <%d>",pcb->id,nombreArchivo,archivoParaFs->posPuntero, direccionFisica,4);

    actualizarPunteroLocal(nombreArchivo,pcb,4);

    moverProceso_ExecBloq(pcb);
    enviar_archivoRW(archivoParaFs,filesystem_fd,LECTURA_ARCHIVO,info_logger);
    aumentarContadorPeticionesFs();
}

void ejecutar_FWRITE(PCB* pcb, char* nombreArchivo, uint32_t direccionFisica){
    t_archivoRW* archivoParaFs = malloc(sizeof(t_archivoRW));
    archivoParaFs->nombreArchivoLength = strlen(nombreArchivo) + 1;
    archivoParaFs->nombreArchivo = nombreArchivo;
    archivoParaFs->posPuntero = buscarPosPunteroTablaLocal(nombreArchivo,pcb);
    archivoParaFs->direcFisica = direccionFisica;
    archivoParaFs->cantidadBytes = 4;
    archivoParaFs->pid = pcb->id;

    log_info(info_logger,"PID: <%d> - Escribir Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamanio <%d>",pcb->id,nombreArchivo,archivoParaFs->posPuntero, direccionFisica,4);

    actualizarPunteroLocal(nombreArchivo,pcb,4);

    moverProceso_ExecBloq(pcb);
    enviar_archivoRW(archivoParaFs,filesystem_fd,ESCRITURA_ARCHIVO,info_logger);
    aumentarContadorPeticionesFs();
}

void agregarEntrada_TablaGlobalArchivosAbiertos(char* nomArch){

    pthread_mutex_lock(&mutex_listaPeticionesArchivos);

    for(int i = 0 ; i < list_size(listaPeticionesArchivos); i++){
        t_archivoPeticion* archivoPeticion = list_get(listaPeticionesArchivos,i);
        if(strcmp(archivoPeticion->archivo->nombreArchivo,nomArch)== 0){
            list_remove(listaPeticionesArchivos,i);

            pthread_mutex_lock(&mutex_TGAA);
            list_add(tablaGlobal_ArchivosAbiertos,archivoPeticion);
            pthread_mutex_unlock(&mutex_TGAA);

            t_archivoLocal* archivoLocal = malloc(sizeof (t_archivoLocal));
            archivoLocal->archivo = archivoPeticion->archivo;
            archivoLocal->ptro = 0;
            list_add(archivoPeticion->pcb->archivos_abiertos,archivoLocal);
            mandarPaquetePCB(archivoPeticion->pcb);
            //free(archivoLocal);
            break;
        }
    }
    pthread_mutex_unlock(&mutex_listaPeticionesArchivos);
    free(nomArch);

}

void desbloquearPcb_porNombreArchivo (char* nombArch) {
    bool desbloqueado = false;
    pthread_mutex_lock(&mutex_TGAA);
    for (int i = 0; i < list_size(tablaGlobal_ArchivosAbiertos); i++) {
        t_archivoPeticion *archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos, i);
        if (strcmp(archivoPeticion->archivo->nombreArchivo, nombArch) == 0) {
            moverProceso_BloqReady(archivoPeticion->pcb);
            desbloqueado = true;
        }
    }
    pthread_mutex_unlock(&mutex_TGAA);
    if(desbloqueado == false)
        log_error(info_logger,"Me enviaron un nombre de archivo que no coincide con ninguno de mis archivos: %s", nombArch);
    free(nombArch);
}

char* reciboYActualizoContadorPeticionesFs(int cliente_socket){
    uint32_t cantidadPeticiones = 0;
    char* nombreArchivo = recibirEnteroYString(cliente_socket,&cantidadPeticiones); //TODO modificar este recibir
    pthread_mutex_lock(&mutex_contadorPeticionesFs);
    contadorPeticionesFs = cantidadPeticiones;
    pthread_mutex_unlock(&mutex_contadorPeticionesFs);
    return nombreArchivo;
}

void aumentarContadorPeticionesFs(){
    pthread_mutex_lock(&mutex_contadorPeticionesFs);
    contadorPeticionesFs++;
    pthread_mutex_unlock(&mutex_contadorPeticionesFs);
}

int buscarArch_TablaGlobalArchivo(char* nomArch){
    int size = list_size(tablaGlobal_ArchivosAbiertos);
    t_archivoPeticion* archivoPeticion;

    for (int i = 0; i < size ; i++){
        archivoPeticion = list_get(tablaGlobal_ArchivosAbiertos,i);
        if (strcmp(archivoPeticion->archivo->nombreArchivo, nomArch) == 0)
            return i;
    }
    return -1;
}

uint32_t buscarPosPunteroTablaLocal(char* nombreArchivo, PCB* pcb){
    for(int i = 0; i < list_size(pcb->archivos_abiertos); i++){
        t_archivoLocal* archivoLocal = list_get(pcb->archivos_abiertos,i);
        if(strcmp(archivoLocal->archivo->nombreArchivo,nombreArchivo) == 0){
            return archivoLocal->ptro;
        }else{
            log_error(info_logger, "El archivo <%s> no se encuentra en la tabla local del PID <%d>", nombreArchivo, pcb->id);
        }
    }
}
