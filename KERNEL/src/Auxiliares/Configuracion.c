/*
 * Configuracion.c
 *
 *  Created on: 17 abr. 2019
 *      Author: utnso
 */

#include "Configuracion.h"

void cargar_configuracion_kernel(char * path_config) {

//	config = cargar_configuracion("../config/KERNEL.config", logger);
	config = cargarConfiguracion(path_config,logger);

	if (config_has_property(config, "IP")) {
		kernel_conf.ip = config_get_string_value(config, "IP");
		remove_quotes(kernel_conf.ip);
	} else {
		loguear_error_carga_propiedad("IP");
	}

	if (config_has_property(config, "PUERTO")) {
		kernel_conf.puerto = config_get_string_value(config, "PUERTO");
	} else {
		loguear_error_carga_propiedad("PUERTO");
	}

	if (config_has_property(config, "IP_MEMORIA")) {
		char* arr = config_get_string_value(config, "IP_MEMORIA");
		kernel_conf.ip_memoria = string_get_string_as_array(arr);
		remove_quotes(kernel_conf.ip_memoria[0]);
	} else {
		loguear_error_carga_propiedad("IP_MEMORIA");
	}

	if (config_has_property(config, "PUERTO_MEMORIA")) {
		char* arr2 = config_get_string_value(config, "PUERTO_MEMORIA");
		kernel_conf.puerto_memoria = string_get_string_as_array(arr2);
	} else {
		loguear_error_carga_propiedad("PUERTO_MEMORIA");
	}

	if (config_has_property(config, "QUANTUM")) {
		kernel_conf.quantum = config_get_int_value(config, "QUANTUM");
		loguear_carga_propiedad("QUANTUM", string_itoa(kernel_conf.quantum));
	} else {
		loguear_error_carga_propiedad("QUANTUM");
	}

	if (config_has_property(config, "MULTIPROCESAMIENTO")) {
		kernel_conf.multiprocesamiento = config_get_int_value(config,
				"MULTIPROCESAMIENTO");
		loguear_carga_propiedad("MULTIPROC",
				string_itoa(kernel_conf.multiprocesamiento));
	} else {
		loguear_error_carga_propiedad("MULTIPROC");
	}

	if (config_has_property(config, "METADATA_REFRESH")) {
		kernel_conf.metadata_refresh = config_get_int_value(config,
				"METADATA_REFRESH");
		loguear_carga_propiedad("MET_REFRESH",
				string_itoa(kernel_conf.metadata_refresh));
	} else {
		loguear_error_carga_propiedad("MET_REFRESH");
	}

	if (config_has_property(config, "SLEEP_EJECUCION")) {
		kernel_conf.sleep_ejecucion = config_get_int_value(config,
				"SLEEP_EJECUCION");
		loguear_carga_propiedad("SLEEP_EJEC",
				string_itoa(kernel_conf.sleep_ejecucion));
	} else {
		loguear_error_carga_propiedad("SLEEP_EJEC");
	}

	if (config_has_property(config, "RETARDO_GOSSIPING")) {
		kernel_conf.retardo_gossiping = config_get_int_value(config,
				"RETARDO_GOSSIPING");
		loguear_carga_propiedad("RETARDO_GOSSIPING",
				string_itoa(kernel_conf.retardo_gossiping));
	} else {
		loguear_error_carga_propiedad("RETARDO_GOSSIPING");
	}

}

// Auxiliares
void mostrar_propiedades() {

		printf("Quantum: %d\n", kernel_conf.quantum);
		printf("Multiprocesamiento: %d\n", kernel_conf.multiprocesamiento);
		printf("Metadata Refresh: %d\n", kernel_conf.metadata_refresh);
		printf("Sleep Ejecución: %d\n", kernel_conf.sleep_ejecucion);

		int i=0;
		while(kernel_conf.ip_memoria[i] != NULL)	{

			remove_quotes(kernel_conf.ip_memoria[i]);
			loggear(logger,LOG_LEVEL_INFO,"IP_SEEDS: %s", kernel_conf.ip_memoria[i]);
			loggear(logger,LOG_LEVEL_INFO,"PUERTOS_SEEDS: %s", kernel_conf.puerto_memoria[i]);
			i++;
		}
}

void cargar_configuracion_kernel_inotify(char* path) {
	config = cargarConfiguracion(path, logger);

	memset(&kernel_conf, 0x00, sizeof(t_kernel_conf));

	if (config_has_property(config, "QUANTUM")) {
		kernel_conf.quantum = config_get_int_value(config, "QUANTUM");
		loguear_carga_propiedad("QUANTUM", string_itoa(kernel_conf.quantum));
	} else {
		loguear_error_carga_propiedad("QUANTUM");
	}

	if (config_has_property(config, "MULTIPROCESAMIENTO")) {
		kernel_conf.multiprocesamiento = config_get_int_value(config,
				"MULTIPROCESAMIENTO");
		loguear_carga_propiedad("MULTIPROCESAMIENTO",
				string_itoa(kernel_conf.multiprocesamiento));
	} else {
		loguear_error_carga_propiedad("MULTIPROCESAMIENTO");
	}

	if (config_has_property(config, "METADATA_REFRESH")) {
		kernel_conf.metadata_refresh = config_get_int_value(config,
				"METADATA_REFRESH");
		loguear_carga_propiedad("METADATA_REFRESH",
				string_itoa(kernel_conf.metadata_refresh));
	} else {
		loguear_error_carga_propiedad("METADATA_REFRESH");
	}

	if (config_has_property(config, "SLEEP_EJECUCION")) {
		kernel_conf.sleep_ejecucion = config_get_int_value(config,
				"SLEEP_EJECUCION");
		loguear_carga_propiedad("SLEEP_EJECUCION",
				string_itoa(kernel_conf.sleep_ejecucion));
	} else {
		loguear_error_carga_propiedad("SLEEP_EJECUCION");
	}

	if (config_has_property(config, "RETARDO_GOSSIPING")) {
		kernel_conf.retardo_gossiping = config_get_int_value(config,
				"RETARDO_GOSSIPING");
		loguear_carga_propiedad("RETARDO_GOSSIPING",
				string_itoa(kernel_conf.retardo_gossiping));
	} else {
		loguear_error_carga_propiedad("RETARDO_GOSSIPING");
	}
	//mostrar_propiedades();
}
