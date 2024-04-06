#include "limpiarFs.h"

void cerrar_programa(){

	log_info(infoLogger, "Liberando recursos del programa...");

    if(logsCreados){
        destruirLoggers();
    }
    if(configCreado){
        destruirConfig();
    }
    if(cfgCreado){
        destruirCfg();
    }
    if(listaFCBsCreado){
        destruirListaFCBs();
    }
    if(tablaFATcreada){
    	destruirTFAT();
    }
    if(semaforosCreados){

    	pthread_mutex_destroy(&mutex_ArchivosUsados);
    }
    if(bitmapCreado){

    	liberar_bitmap();

    }
    if(archivosUsadosCreado){
        list_clean(archivosUsados);
        list_destroy(archivosUsados);
    }
    if(conexionesHechas){
        //Nada que hacer, se hace detach
    }

//    if(listaBloquesCreado){
//        destruirListaBloques();
//    }

}

void liberar_bitmap() {
	list_destroy_and_destroy_elements(bitmap, free);
}


void destruirConfig(){
	config_destroy(fileConfigFs);
}

void destruirCfg(){


//	free(configFs->PATH_BLOQUES);
//	free(configFs->PATH_FAT);
//	free(configFs->PATH_FCB);
    free(configFs);
}
void destruirLoggers(){
    log_destroy(infoLogger);
    //log_destroy(warning_logger);
    log_destroy(errorLogger);
    log_destroy(traceLogger);
    //log_destroy(debug_logger);
}

void destruirTFAT(){

	list_clean(tablaFAT);
	list_destroy(tablaFAT);

}

void destruirListaFCBs(){


	void destruirFcbs(t_config_fcb* unFcb){
		free(unFcb->NOMBRE_ARCHIVO);
		//free(&unFcb->PUNTERO_INDIRECTO);
		//free(&unFcb->PUNTERO_DIRECTO);
		config_destroy(unFcb->fcb_config);
		free(unFcb);
	  }


	list_destroy_and_destroy_elements(listaFCBs, destruirFcbs);
	//list_destroy(listaFCBs);
}
