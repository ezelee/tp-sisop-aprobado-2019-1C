/*
 * inotifyMemoria.c
 *
 *  Created on: 12 jun. 2019
 *      Author: utnso
 */

#include <shared.h>

#include "inotify.h"
#include "inotifyMemoria.h"

#include "MEM.h"
#include "error.h"
#include "argparse.h"
#include "file_conf.h"

void *crearInotify()
{
	int inotifyFd = inotifyInit(string_equals_ignore_case(args_configfile,"false")?
			"../MEM.conf":args_configfile);

	if(inotifyFd==-1)
		_exit_with_error("inotify_init",NULL);

	while(1) {/* Read events forever */
		if(inotifyEvent(inotifyFd))
			get_modify_config(string_equals_ignore_case(args_configfile,"false")?
							"../MEM.conf":args_configfile);
	}
}

int crearHiloInotify()
{
	sigset_t set;
	int s;
	int hilo_inotify;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);

	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_inotify = pthread_create(&inotify, NULL, crearInotify, (void *) &set);

	if (hilo_inotify == -1) {
			log_error(logger, "No se pudo generar el hilo para el I-NOTIFY.");
		}
		log_info(logger, "Se generó el hilo para el I-NOTIFY.");

	return 1;
}
