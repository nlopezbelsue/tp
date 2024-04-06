#include "mmu.h"

uint32_t num_pagina;

int traducir_direccion_logica(int direccion_logica) {

	num_pagina = direccion_logica / tam_pagina;

	int desplazamiento_pagina = direccion_logica % tam_pagina;

	int marco= check_pageFault();

	if(marco == -1) {

		copiar_registrosCPU_a_los_registroPCB(PCB_Actual->registros);

		t_paquete* paquete= crear_paquete(PAGE_FAULT, info_logger);
		agregar_ContextoEjecucion_a_paquete(paquete, PCB_Actual);
		agregar_a_paquete2(paquete, &(num_pagina), sizeof(uint32_t));
		enviar_paquete(paquete, kernel_fd);
		eliminar_paquete(paquete);

		cicloInstrucciones = false;
		log_info(info_logger, "Page Fault PID: <%d> - Página: <%d>”.", PCB_Actual->id, num_pagina);

		return -1;
	}

	int direccion_fisica = marco * tam_pagina + desplazamiento_pagina;

	log_info(info_logger, "direccion fisica = marco * tam_pagina + desplazamiento: %d", direccion_fisica);

	return direccion_fisica;
}


uint32_t check_pageFault(){
	t_list* listaInts = list_create();

	list_add(listaInts, &PCB_Actual->id);
	list_add(listaInts, &num_pagina);


	enviarListaUint32_t(listaInts,memoria_fd, info_logger, SOLICITUDMARCO);

	uint32_t marco;
	op_code_cliente cod = recibir_operacion(memoria_fd);

	if(cod == SOLICITUDMARCO){        //en memoria mandar con algo parecido a enviarInterrupt
		marco = recibirValor_uint32(memoria_fd);

		log_info(logger, "“PID: <%d> - OBTENER MARCO - Página: <%d> - Marco: <%d>”.\n", PCB_Actual->id, num_pagina, marco);

	}
	else{
		printf("%d", cod);
		log_error(error_logger,"FALLO en la obtencion del marco");
		marco=-1; //si hay pageFault tmbn tiene que devolver -1
	}

	list_clean(listaInts);
	list_destroy(listaInts);

	return marco;
}


