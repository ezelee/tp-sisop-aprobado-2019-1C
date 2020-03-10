/*
 * Error.c
 *
 *  Created on: 4 may. 2019
 *      Author: utnso
 */

#include "LFS.h"
#include "Error.h"
#include "Configuracion.h"

void _exit_with_error(char* error_msg, void * buffer) {

	if (buffer != NULL)
		free(buffer);
	if (error_msg != NULL)
		loggear(logger, LOG_LEVEL_ERROR, error_msg);

	exit_gracefully(EXIT_FAILURE);
}


void exit_gracefully(int return_nr) {


  loggear(logger, LOG_LEVEL_INFO, "********* FIN DEL PROCESO LISSANDRA *********");

  if (!list_is_empty(tablasGlobal)) {
		int sizeGlobal = list_size(tablasGlobal);
		for (int i = 0; i < sizeGlobal; i++) {
			free(list_remove(tablasGlobal, 0));
		}
		list_destroy(tablasGlobal);
	}
  bitarray_destroy(bitmap);
  destroy_config(config);
  pthread_cancel(thread_inotify);
  pthread_cancel(thread_dump);
  pthread_cancel(thread_conexiones);
  pthread_cancel(thread_consola);
  destruir_logger(logger);

  exit(return_nr);
}
