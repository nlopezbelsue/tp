#include "limpiarCPU.h"

void cerar_programa()
{
	config_destroy(config);
	log_destroy(info_logger);
	log_destroy(error_logger);
	log_destroy(loggerCpuMem);
	liberar_conexion(memoria_fd);
	liberar_conexion(kernel_fd);
	liberar_conexion(kernel_interrupt_fd);
}
