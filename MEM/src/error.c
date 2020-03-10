/*
 * error.c
 *
 *  Created on: 14 sep. 2018
 *      Author: utnso
 */
#include "MEM.h"

#include "file_conf.h"
#include "argparse.h"
#include "memory.h"

#include "error.h"

void _exit_with_error(char* error_msg, void * buffer) {

	if (buffer != NULL)
	  free(buffer);
	if (error_msg != NULL)
	  loggear(logger,LOG_LEVEL_ERROR, error_msg);

	exit_gracefully(EXIT_FAILURE);
}


void exit_gracefully(int return_nr) {
  /*
          Siempre llamamos a esta funcion para cerrar el programa.
          Asi solo necesitamos destruir el logger y usar la llamada al
          sistema exit() para terminar la ejecucion
  */

	liberar_memory_spa();

	destroy_args();
	destroy_config(config);

	loggear(logger, LOG_LEVEL_INFO, "********* FIN DEL PROCESO MEM *********");
	destruir_logger(logger);

	exit(return_nr);
}
