#include "operacionesFs.h"
#include <math.h>

t_list *listaFCBs;
t_list *tablaFAT;
//t_bloques* archivoBloques;
t_list *bloquesAEnviar;
fatTable *tFat;
archBloques *archivoBloques;

int TAM_BLOQUE = 1024;

void *crearArchivo(char *nombreArchivo){

	char *pathFCB = configFs->PATH_FCB;
	log_trace(traceLogger, "Entre a crear archivo");
	char *path = string_new();
	string_append(&path, pathFCB);
	string_append(&path, "/");
	string_append(&path, nombreArchivo);
	string_append(&path,".fcb");

	log_info(infoLogger, "%s", path);

	FILE *f = fopen(path, "wb");


	t_config* aux_config = config_create(path);

	config_set_value(aux_config,"NOMBRE_ARCHIVO",nombreArchivo);
	config_set_value(aux_config,"TAMANIO_ARCHIVO","0");
	config_set_value(aux_config,"BLOQUE_INICIAL", "-1");

	config_save(aux_config);


	t_config_fcb* auxFCB = malloc(sizeof(t_config_fcb));


	int tamanio_nombre_archivo = strlen(nombreArchivo);

	auxFCB->NOMBRE_ARCHIVO = malloc( tamanio_nombre_archivo + 1);

	strcpy(auxFCB->NOMBRE_ARCHIVO, nombreArchivo);
	auxFCB->TAMANIO_ARCHIVO = 0;
	auxFCB->BLOQUE_INICIAL = -1;
    auxFCB->fcb_config = aux_config;

    list_add(listaFCBs, auxFCB);

    creacionArchivo(nombreArchivo);
    fclose(f);

    free(path);

    return NULL;
}

void realizarEscrituraArchivo(char *nombreArchivo, uint32_t punteroABloque, void *contenido){ //El puntero a bloque es el numero de entrada

	punteroABloque = punteroABloque / configFs->TAM_BLOQUE;

	int tamListFcb = list_size(listaFCBs);

	archivoBloques = malloc(sizeof(archBloques));
	int blockCount = configFs->CANT_BLOQUES_TOTAL;
	int blockSize = configFs->TAM_BLOQUE;
	archivoBloques->tamanio = blockCount*blockSize;

	archivoBloques->fd = open(configFs->PATH_BLOQUES, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
	if (archivoBloques->fd == -1){
		log_error(errorLogger,"Error al abrir/crear el archivo de bloques");
		return ;
	}

	archivoBloques->archivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

	if(archivoBloques->archivo == MAP_FAILED) {
	   log_error(errorLogger,"Error al mapear el archivo en memoria");
	   close(archivoBloques->fd);
	   return ;
	}

	msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);

	log_trace(traceLogger, "Entre a escribir archivo");

	t_config_fcb * aux_FCB;



	for(int i=0; i<tamListFcb; i++){

		aux_FCB = list_get(listaFCBs, i);

		if(strcmp(aux_FCB->NOMBRE_ARCHIVO, nombreArchivo) == 0){



				t_entradas * auxFAT = recibirEntradaFat(punteroABloque, aux_FCB);
				if(auxFAT == NULL){
					log_error(errorLogger, "Hubo problemas al buscar la entrada en la tabla FAT");
					return;
				}

		int posicion = (punteroABloque + configFs->CANT_BLOQUES_SWAP);

		escribirEnPosicion(archivoBloques, posicion, contenido, configFs->TAM_BLOQUE); //PONER EL TAMAÑO DE BLOQUE

		accesoABloqueArchivo(nombreArchivo, punteroABloque, auxFAT->numEntrada);

		log_info(infoLogger, "Archivo <%s> escrito en el bloque %d", nombreArchivo, punteroABloque);

		//free(contenido);
		msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
		close(archivoBloques->fd);
		free(archivoBloques);
		return;

		}
	}


	return ;
}

void escribirEnPosicion(archBloques* archivoBloques, int indiceBloque,  void* datos, size_t tamDatos) {

    uint32_t posicionEscritura = (indiceBloque * tamDatos);

    memcpy(archivoBloques->archivo + posicionEscritura, datos, tamDatos);


}


void* leer_archivo(char *nombreArchivo, uint32_t puntero){

	puntero = puntero / configFs->TAM_BLOQUE;


	archivoBloques = malloc(sizeof(archBloques));
	int blockCount = configFs->CANT_BLOQUES_TOTAL;
	int blockSize = configFs->TAM_BLOQUE;
	archivoBloques->tamanio = blockCount*blockSize;

	archivoBloques->fd = open(configFs->PATH_BLOQUES, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
	if (archivoBloques->fd == -1){
		log_error(errorLogger,"Error al abrir/crear el archivo de bloques");
		return NULL;
	}

	archivoBloques->archivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

	if(archivoBloques->archivo == MAP_FAILED) {
	   log_error(errorLogger,"Error al mapear el archivo en memoria");
	   close(archivoBloques->fd);
	   return NULL;
	}

	msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);

	log_trace(traceLogger, "Entre a leer archivo");

	int tamListFCB = list_size(listaFCBs);
	t_config_fcb* auxFCB;

	for(int i=0; i<tamListFCB; i++){

		auxFCB = list_get(listaFCBs, i);

		if(strcmp(auxFCB->NOMBRE_ARCHIVO, nombreArchivo) == 0){



				t_entradas * auxFAT = recibirEntradaFat(puntero, auxFCB);

				accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);


				void *contenido;

				contenido = malloc(configFs->TAM_BLOQUE);
				if (contenido == NULL) {
					log_error(errorLogger, "Error al asignar memoria para contenido");
					return NULL;
				}

				size_t tamBloque = configFs->TAM_BLOQUE;

				int offset = (puntero + configFs->CANT_BLOQUES_SWAP) * configFs->TAM_BLOQUE;

				memcpy(contenido, archivoBloques->archivo + offset, tamBloque);

				accesoABloqueArchivo(nombreArchivo, puntero, auxFAT->numEntrada);

				log_info(infoLogger, "Archivo <%s> leido en el bloque %d", nombreArchivo, puntero);


				close(archivoBloques->fd);

				free(archivoBloques);

				return contenido;


		}




	}

	return NULL;
}

void* leerBloqueDeSwap(uint32_t  puntero){



	log_trace(traceLogger, "Entre a leer bloque de swap");

	archivoBloques = malloc(sizeof(archBloques));
	int blockCount = configFs->CANT_BLOQUES_TOTAL;
	int blockSize = configFs->TAM_BLOQUE;
	archivoBloques->tamanio = blockCount*blockSize;

	archivoBloques->fd = open(configFs->PATH_BLOQUES, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
	if (archivoBloques->fd == -1){
		log_error(errorLogger,"Error al abrir/crear el archivo de bloques");
		return NULL;
	}

	archivoBloques->archivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

	if(archivoBloques->archivo == MAP_FAILED) {
	   log_error(errorLogger,"Error al mapear el archivo en memoria");
	   close(archivoBloques->fd);
	   return NULL;
	}

	msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);

	void *contenido;

	contenido = malloc(configFs->TAM_BLOQUE);
	if (contenido == NULL) {
		log_error(errorLogger, "Error al asignar memoria para contenido");
		return NULL;
	}

	size_t tamBloque = configFs->TAM_BLOQUE;

	int offset = (puntero) * configFs->TAM_BLOQUE;

	accesoSwap(puntero);
	memcpy(contenido, archivoBloques->archivo + offset, tamBloque);
	//munmap(archivoBloques->archivo,archivoBloques->tamanio);

	log_info(infoLogger, "Bloque swap <%d> leido", puntero);


	close(archivoBloques->fd);

	free(archivoBloques);


	return contenido;

}

int existe_FCB(char *nombreArchivo){

	int tam_listFCB = list_size(listaFCBs);
	 t_config_fcb* aux_FCB;
	for(int i=0; i<tam_listFCB; i++){

		 aux_FCB = list_get(listaFCBs, i);

		if(strcmp(aux_FCB->NOMBRE_ARCHIVO, nombreArchivo) == 0){

			aperturaArchivo(nombreArchivo);

			return 1;
		}
	}
	return 0;
}

t_config_fcb* buscarFCBporNombre(char* nombreArchivo){

	int tam_listFCB = list_size(listaFCBs);

	t_config_fcb* aux_FCB;

	for(int i =0; i<tam_listFCB; i++){

		aux_FCB = list_get(listaFCBs, i);

		if(strcmp(aux_FCB->NOMBRE_ARCHIVO, nombreArchivo) == 0){
			log_info(infoLogger, "ENCONTRE EL FCB");
			return aux_FCB;
		}

	}

	log_error(errorLogger, "No esta el FCB");

	return NULL;

}

bool bloquePerteneceAArchivo(uint32_t punteroABloque, t_config_fcb* auxFCB){

	t_entradas* auxFAT;



	auxFAT = list_get(tablaFAT, auxFCB->BLOQUE_INICIAL);

	for(int i=0; i<list_size(tablaFAT); i++){

		if(auxFAT->numEntrada == punteroABloque){

			return true;
		}

		auxFAT = list_get(tablaFAT, auxFAT->entradaSig);

		if(auxFAT->numEntrada == UINT32_MAX){
			return false;
		}
	}

	return false;

}

t_entradas* recibirEntradaFat(int punteroABloque, t_config_fcb* auxFCB){


		t_entradas* auxFAT;
		t_entradas* auxFAT2;

		auxFAT = list_get(tablaFAT, auxFCB->BLOQUE_INICIAL);
		accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);

		int entradaAOperar = 0;

			do{

				if(punteroABloque == entradaAOperar){
					return auxFAT;
				}

			auxFAT2 = list_get(tablaFAT, auxFAT->entradaSig);

			entradaAOperar++;
			accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);

			if(auxFAT->numEntrada == UINT32_MAX){
				log_error(errorLogger, "La entrada no existe");
				return NULL;
			}

			auxFAT = auxFAT2;




		}while(punteroABloque != entradaAOperar);

		return auxFAT;

}



void realizarTruncarArchivo(char *nombreArchivo, uint32_t nuevoTamArchivo){

	t_config_fcb* auxFCB;
	t_entradas* auxFAT;

	int cantEntradasFAT = list_size(tablaFAT);

	int tamFcbList = list_size(listaFCBs);

	log_trace(traceLogger, "Entre a truncar archivo");

	int indexListFcb = 0;

	for(indexListFcb = 0; indexListFcb<tamFcbList;indexListFcb++){

		auxFCB = list_get(listaFCBs, indexListFcb);

		if(strcmp(auxFCB->NOMBRE_ARCHIVO, nombreArchivo) ==  0){
			break;
		}
	}

	if(strcmp(auxFCB->NOMBRE_ARCHIVO, nombreArchivo) != 0){

		log_info(infoLogger, "No se encuentra el FCB del archivo");
		return;
	}

	char *path = string_new();

	string_append(&path, configFs->PATH_FCB);
	string_append(&path, "/");
	string_append(&path, nombreArchivo);
	string_append(&path, ".fcb");

	FILE *fcbFile = fopen(path, "ab");

	free(path);

	tFat = malloc(sizeof(fatTable));
	int cantidadEntradas = configFs->CANT_BLOQUES_TOTAL-configFs->CANT_BLOQUES_SWAP;
	int tamanioEntrada= sizeof(t_entradas);
	tFat->tamanioArchivo= cantidadEntradas*tamanioEntrada;


	tFat->fd = open(configFs->PATH_FAT, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);


	if(tFat->fd ==  -1){
		log_error(errorLogger, "Error al abrir la tabla fat");
		return;
	}
	tFat->archivo = mmap(NULL,tFat->tamanioArchivo, PROT_READ | PROT_WRITE, MAP_SHARED, tFat->fd , 0);

	if(tFat->archivo == MAP_FAILED) {
	       log_error(errorLogger,"Error al mapear el archivo en memoria");
	       close(tFat->fd);
	       free(tFat);
	       return ;
	}

	msync(tFat->archivo, tFat->tamanioArchivo, MS_SYNC);



	if(auxFCB->TAMANIO_ARCHIVO == 0){  // SU TAMAÑO ESTA EN 0

		if(auxFCB->TAMANIO_ARCHIVO < nuevoTamArchivo ){ //Quiere aumentar el tamaño del archivo por primera vez


			// CONVIERTO EN BLOQUES

			double bloquesInexactos = (double)nuevoTamArchivo / configFs->TAM_BLOQUE;

			uint32_t cantidadDeBloquesRequeridos;


			    double parteEntera;
			    double decimales = modf(bloquesInexactos, &parteEntera);


			 if(decimales > 0){


				 cantidadDeBloquesRequeridos = (uint32_t)bloquesInexactos + 1;

			 }else{

			      cantidadDeBloquesRequeridos = (uint32_t)bloquesInexactos;

			 }

			//Busco una entrada en la fat libre
			for(int i =0; i<cantEntradasFAT; i++){



				auxFAT = list_get(tablaFAT, i);
				// ENCUENTRA UNA ENTRADA EN LA FAT LIBRE
				if(auxFAT->bloqueLibre){

					auxFAT->bloqueLibre = false;
					auxFCB->BLOQUE_INICIAL = auxFAT->numEntrada;
					auxFCB->TAMANIO_ARCHIVO = nuevoTamArchivo;
				/*
					if(fseek(archivoFAT, auxFAT->numEntrada, SEEK_SET) != 0){
					log_error(errorLogger, "No se puede acceder a la entrada deseada");
					return;
					}
					accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);
					usleep(configFs->RETARDO_ACCESO_FAT * 1000);
					fwrite(&auxFAT, sizeof(t_entradas), 1, archivoFAT);
				*/




					int longitud = snprintf(NULL, 0, "%d", auxFCB->BLOQUE_INICIAL);
					char *cadena = (char *)malloc((longitud + 1) * sizeof(char));
					sprintf(cadena, "%d", auxFCB->BLOQUE_INICIAL);


					config_set_value(auxFCB->fcb_config, "BLOQUE_INICIAL", cadena);

					int longitud2 = snprintf(NULL, 0, "%d", nuevoTamArchivo);
					char *cadena2 = (char *)malloc((longitud2 + 1) * sizeof(char));
					sprintf(cadena2, "%d", nuevoTamArchivo);

					config_set_value(auxFCB->fcb_config, "TAMANIO_ARCHIVO", cadena2);

					config_save(auxFCB->fcb_config);
					free(cadena2);
					free(cadena);
					fclose(fcbFile);
					list_replace(listaFCBs, indexListFcb, auxFCB); //Reemplaza en lista

					cantidadDeBloquesRequeridos--;

					t_entradas* auxFAT2;

					// POR SI TIENE MAS DE 1 BLOQUE A TRUNCAR
					while(cantidadDeBloquesRequeridos != 0){



						for(int j=0;j<cantEntradasFAT; j++){

							auxFAT2 = list_get(tablaFAT,j);

							if(auxFAT2->bloqueLibre){

								auxFAT->entradaSig = auxFAT2->numEntrada;
								auxFAT2->bloqueLibre = false;
								list_replace(tablaFAT, auxFAT->numEntrada, auxFAT);


								cantidadDeBloquesRequeridos--;
								//SOBREESCRIBO LA PRIMER ENTRADA EN FAT QUE ENCUENTRO

								int  offset = sizeof(uint32_t) * 3 * auxFAT->numEntrada;
								memcpy(tFat->archivo + offset, &auxFAT->bloqueLibre, sizeof(uint32_t));
								offset+= sizeof(uint32_t);
								memcpy(tFat->archivo + offset, &auxFAT->numEntrada, sizeof(uint32_t));
								offset += sizeof(uint32_t);
								memcpy(tFat->archivo + offset, &auxFAT->entradaSig, sizeof(uint32_t));

								accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);



								break;
							}

						}

						auxFAT = auxFAT2;
						int  offset = sizeof(uint32_t) * 3 * auxFAT->numEntrada;
						memcpy(tFat->archivo + offset, &auxFAT->bloqueLibre, sizeof(uint32_t));
						offset+= sizeof(uint32_t);
						memcpy(tFat->archivo + offset, &auxFAT->numEntrada, sizeof(uint32_t));
						offset += sizeof(uint32_t);
						memcpy(tFat->archivo + offset, &auxFAT->entradaSig, sizeof(uint32_t));

						accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);



						list_replace(tablaFAT, auxFAT->numEntrada, auxFAT);

					}


					break;

				}


			}

			truncacionArchivo(nombreArchivo, nuevoTamArchivo);
			msync(tFat->archivo, tFat->tamanioArchivo, MS_SYNC);
			close(tFat->fd);
			free(tFat);
			return;
		}

		// QUIERE HACERLO DEL MISMO TAMAÑO
		if(auxFCB->TAMANIO_ARCHIVO > nuevoTamArchivo){ //El nuevo tamaño es negativo

			log_error(infoLogger, "No se puede tener un archivo de tamaño negativo");
			return;
		}

	} //FIN IF tamNuevo == 0

		// SI ES DISTINTO DE 0, OSEA MAYOR A 0
	if(auxFCB->TAMANIO_ARCHIVO != 0){

		uint32_t tamArchivo =  auxFCB->TAMANIO_ARCHIVO;

		if(tamArchivo == nuevoTamArchivo){ //mismo tamaño

			log_info(infoLogger, "El archivo ya tiene ese tamanio");
			return;
		}


		// AUMENTAR TAMAÑO
		if(tamArchivo < nuevoTamArchivo){




			double bloquesInexactos = (double)nuevoTamArchivo / configFs->TAM_BLOQUE;
			uint32_t cantidadDeBloquesNuevos;

			double parteEntera;
			double decimales = modf(bloquesInexactos, &parteEntera);


			 if(decimales > 0){


					 cantidadDeBloquesNuevos = (uint32_t)bloquesInexactos + 1;

			 }else{

					 cantidadDeBloquesNuevos = (uint32_t)bloquesInexactos;

			 }


			double bloquesInexactosActuales = (double)auxFCB->TAMANIO_ARCHIVO / configFs->TAM_BLOQUE;
			uint32_t cantidadDeBloquesActuales;

			double parteEntera2;
			double decimales2 = modf(bloquesInexactosActuales, &parteEntera2);

			 if(decimales2 > 0){


				 cantidadDeBloquesActuales = (uint32_t)bloquesInexactosActuales + 1;

			 }else{

				 cantidadDeBloquesActuales = (uint32_t)bloquesInexactosActuales;

			 }


			uint32_t bloquesAdicionales = cantidadDeBloquesNuevos - cantidadDeBloquesActuales;
			// BLOQUES A ADICIONAR



			/*Cambio en el FCB*/
			auxFCB->TAMANIO_ARCHIVO = nuevoTamArchivo;

			int longitud = snprintf(NULL, 0, "%d", nuevoTamArchivo);
			char *cadena = (char *)malloc((longitud + 1) * sizeof(char));
			sprintf(cadena, "%d", nuevoTamArchivo);


			config_set_value(auxFCB->fcb_config, "TAMANIO_ARCHIVO", cadena);
			config_save(auxFCB->fcb_config);
			free(cadena);
			fclose(fcbFile);
			list_replace(listaFCBs, indexListFcb, auxFCB);



				//ENCUENTRO EL BLOQUE INCIAL DE MI ARCHIVO EN FAT
				auxFAT = list_get(tablaFAT, auxFCB->BLOQUE_INICIAL);


				t_entradas* auxFAT2;


				bool encontrado = false;
				int loop = 0;
					while(!encontrado){

						auxFAT2 = list_get(tablaFAT, loop);

						if(auxFAT->entradaSig == auxFAT2->numEntrada){

							auxFAT = auxFAT2;


							// ESTA EN EL ULTIMO BLOQUE DEL ARCHIVO
							if(auxFAT->entradaSig == UINT32_MAX){

								encontrado = true;
							}
						}
							loop++;
					}
					loop = 0;
					while(bloquesAdicionales != 0){

						auxFAT2 = list_get(tablaFAT, loop);

						if(auxFAT2->bloqueLibre){

							auxFAT->entradaSig = auxFAT2->numEntrada;


							int  offset = sizeof(uint32_t) * 3 * auxFAT->numEntrada;
							memcpy(tFat->archivo + offset, &auxFAT->bloqueLibre, sizeof(uint32_t));
							offset+= sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT->numEntrada, sizeof(uint32_t));
							offset += sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT->entradaSig, sizeof(uint32_t));
							accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);


							list_replace(tablaFAT, auxFAT->numEntrada, auxFAT);

							auxFAT = auxFAT2;

							list_replace(tablaFAT, auxFAT->numEntrada, auxFAT);

							offset = sizeof(uint32_t) * 3 * auxFAT->numEntrada;
							memcpy(tFat->archivo + offset, &auxFAT->bloqueLibre, sizeof(uint32_t));
							offset+= sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT->numEntrada, sizeof(uint32_t));
							offset += sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT->entradaSig, sizeof(uint32_t));
							accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);

							/*Cambio en el archivo FAT*/


							bloquesAdicionales--;


						}


						loop++;

					}
					msync(tFat->archivo, tFat->tamanioArchivo, MS_SYNC);
					close(tFat->fd);
					free(tFat);
					truncacionArchivo(nombreArchivo, nuevoTamArchivo);
					return;




		} //FIN AUMENTAR TAMAÑO

		if(auxFCB->TAMANIO_ARCHIVO > nuevoTamArchivo){ //DISMINUIR TAMAÑO

			double bloquesInexactos = (double)nuevoTamArchivo / configFs->TAM_BLOQUE;
			uint32_t cantidadDeBloquesNuevos;
			double parteEntera2;
			double decimales2 = modf(bloquesInexactos, &parteEntera2);
			if(decimales2 > 0){


				 cantidadDeBloquesNuevos = (uint32_t)bloquesInexactos + 1;

			 }else{

				 cantidadDeBloquesNuevos = (uint32_t)bloquesInexactos;

			 }


			double bloquesInexactosActuales = (double)auxFCB->TAMANIO_ARCHIVO / configFs->TAM_BLOQUE;
			uint32_t cantidadDeBloquesActuales;
			double parteEntera;
			double decimales = modf(bloquesInexactosActuales, &parteEntera);
			if(decimales > 0){


				cantidadDeBloquesActuales = (uint32_t)bloquesInexactosActuales + 1;

			 }else{

				 cantidadDeBloquesActuales = (uint32_t)bloquesInexactos;

			 }
			uint32_t bloquesQueLeTengoQueSacar = cantidadDeBloquesActuales - cantidadDeBloquesNuevos ;
			//BLOQUES QUE LE TENGO QUE SACAR

			//CAMBIO EN EL FCB
			auxFCB->TAMANIO_ARCHIVO = nuevoTamArchivo;

			int longitud2 = snprintf(NULL, 0, "%d", nuevoTamArchivo);
			char *cadena2 = (char *)malloc((longitud2 + 1) * sizeof(char));
			sprintf(cadena2, "%d", nuevoTamArchivo);

			config_set_value(auxFCB->fcb_config, "TAMANIO_ARCHIVO", cadena2);
			config_save(auxFCB->fcb_config);
			free(cadena2);
			list_replace(listaFCBs, indexListFcb, auxFCB);


			//ENCUENTRO EL BLOQUE INCIAL DE MI ARCHIVO EN FAT
			auxFAT = list_get(tablaFAT, auxFCB->BLOQUE_INICIAL);

			t_entradas* auxInicialFAT = auxFAT;

			t_entradas* auxFAT2;

			bool encontrado;

			int loop;

			while(bloquesQueLeTengoQueSacar != 0){

				loop = 0;
				encontrado = false;
				auxFAT = auxInicialFAT;

				while(!encontrado){

					auxFAT2 = list_get(tablaFAT, loop);

					if(auxFAT->entradaSig == auxFAT2->numEntrada){

						// ESTA EN EL ULTIMO BLOQUE DEL ARCHIVO
						if(auxFAT2->entradaSig == UINT32_MAX){

							encontrado = true;

							auxFAT2->bloqueLibre = true;



							int offset = sizeof(uint32_t) * 3 * auxFAT2->numEntrada;
							memcpy(tFat->archivo + offset, &auxFAT2->bloqueLibre, sizeof(uint32_t));
							offset+= sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT2->numEntrada, sizeof(uint32_t));
							offset += sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT2->entradaSig, sizeof(uint32_t));

							accesoFat(auxFAT2->numEntrada, auxFAT2->entradaSig); /*ENTRADA 3*/

							list_replace(tablaFAT, auxFAT2->numEntrada, auxFAT2);


							accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);
							auxFAT->entradaSig = UINT32_MAX;
							offset = sizeof(uint32_t) * 3 * auxFAT->numEntrada;
							memcpy(tFat->archivo + offset, &auxFAT->bloqueLibre, sizeof(uint32_t));
							offset+= sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT->numEntrada, sizeof(uint32_t));
							offset += sizeof(uint32_t);
							memcpy(tFat->archivo + offset, &auxFAT->entradaSig, sizeof(uint32_t));


							list_replace(tablaFAT, auxFAT->numEntrada, auxFAT);


						}else{

							auxFAT = auxFAT2;

						}

					}
					loop++;
				}

				bloquesQueLeTengoQueSacar--;

			}
			accesoFat(auxFAT->numEntrada, auxFAT->entradaSig);
			msync(tFat->archivo, tFat->tamanioArchivo, MS_SYNC);
			close(tFat->fd);
			free(tFat);
			truncacionArchivo(nombreArchivo, nuevoTamArchivo);
			return;
		}
	}




	return;
}

/*
void escribirEnArchivo(char *nombreArchivo, int punteroArchivo, void *direccionMemoria, FAT *fat) {
    int numeroBloque = fat->entradas[punteroArchivo / TAM_BLOQUE];

    int offsetEnBloque = punteroArchivo % TAM_BLOQUE;//dirección de inicio en el bloque

    FILE *archivoBloques = fopen("archivo_de_bloques.dat", "rb+");
    if (archivoBloques == NULL)return;

    fseek(archivoBloques, numeroBloque * TAM_BLOQUE + offsetEnBloque, SEEK_SET);

    fwrite(direccionMemoria, sizeof(char), TAM_BLOQUE - offsetEnBloque, archivoBloques);

    fclose(archivoBloques);
}
*/

bool iniciarProceso(uint32_t cantidadBloques) {

	bool bit;


	bloquesAEnviar = list_create();

    for(int i=0; i<(configFs->CANT_BLOQUES_SWAP); i++){

    	bit = 1;

    	bool bit2 = *(bool*)list_get(bitmap, i);

    	if(bit2 == bit){

    		int *bloqueAReservar = malloc(sizeof(int));

    		*bloqueAReservar = i;

    		reservarBloque(bloqueAReservar);



    		if(rellenarBloque(i)){
    			bool* bitReplace = list_get(bitmap, i);

    			*bitReplace = 0;

    			list_replace(bitmap, i, bitReplace);

    		}
    		if(cantidadBloques == list_size(bloquesAEnviar)){

    			log_info(infoLogger, "Bloques encontrados"); //Logger a definir

    			enviarListaUint32_t(bloquesAEnviar, memoria_fd, infoLogger, PEDIR_SWAP);
    		    return true;
    		}
    	}


    }

    return false;
}

void reservarBloque(int *numBloque){


	list_add(bloquesAEnviar, numBloque);

}

bool rellenarBloque(int numBloque){

	archivoBloques = malloc(sizeof(archBloques));
    int blockCount = configFs->CANT_BLOQUES_TOTAL;
    int blockSize = configFs->TAM_BLOQUE;
    archivoBloques->tamanio = blockCount*blockSize;

    archivoBloques->fd = open(configFs->PATH_BLOQUES, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
	if (archivoBloques->fd == -1){
        log_error(errorLogger,"Error al abrir/crear el archivo de bloques");
        return false;
	}
    archivoBloques->archivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

    if(archivoBloques->archivo == MAP_FAILED) {
       log_error(errorLogger,"Error al mapear el archivo en memoria");
       close(archivoBloques->fd);
       return false;
    }

    msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
    if (archivoBloques == NULL){
    	log_error(errorLogger, "No se pudo abrir el archivo de bloques");
    	return false;
    }

    int offset = configFs->TAM_BLOQUE * numBloque;

    memset(archivoBloques->archivo + offset, '\0', configFs->TAM_BLOQUE);

    accesoSwap(numBloque);

   // size_t tamRelleno = configFs->TAM_BLOQUE;


   // log_info(infoLogger, "Acceso SWAP: %d", numBloque);
    log_info(infoLogger, "Bloque rellenado");


    msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
    close(archivoBloques->fd);
    free(archivoBloques);

    return true;
}


void finalizarProceso(t_list *bloquesReservadosDelProceso) {




	for(int i =0; i<list_size(bloquesReservadosDelProceso); i++){

		uint32_t *numBloque  = list_get(bloquesReservadosDelProceso, i);

		bool *bit = malloc(sizeof(bool));

		*bit = 1;
		int nBloque = *numBloque;

		list_replace(bitmap, nBloque, bit);
		//free(bit);
		free(numBloque);
	}


	log_info(infoLogger, "Bloques desasignados");



	return;

}


void escribirBloqueDeSwap(void* datos, uint32_t puntero){


	log_trace(traceLogger, "Entre a escribir bloque swap");

	archivoBloques = malloc(sizeof(archBloques));
	int blockCount = configFs->CANT_BLOQUES_TOTAL;
	int blockSize = configFs->TAM_BLOQUE;
	archivoBloques->tamanio = blockCount*blockSize;

	archivoBloques->fd = open(configFs->PATH_BLOQUES, O_CREAT| O_RDWR,  S_IRUSR|S_IWUSR);
	if (archivoBloques->fd == -1){
		log_error(errorLogger,"Error al abrir/crear el archivo de bloques");
		return ;
	}

	archivoBloques->archivo = mmap(NULL,archivoBloques->tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, archivoBloques->fd , 0);

	if(archivoBloques->archivo == MAP_FAILED) {
	   log_error(errorLogger,"Error al mapear el archivo en memoria");
	   close(archivoBloques->fd);
	   return ;
	}

	msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);


	int posicion = puntero ;

	uint32_t posicionEscritura = (posicion * configFs->TAM_BLOQUE);

	memcpy(archivoBloques->archivo + posicionEscritura, datos, configFs->TAM_BLOQUE);

	accesoSwap(puntero);

	log_info(infoLogger, "Bloque en swap %d escrito", puntero);


	msync(archivoBloques->archivo, archivoBloques->tamanio, MS_SYNC);
	close(archivoBloques->fd);
	free(archivoBloques);

	return;


}


