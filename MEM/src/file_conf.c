/*
 * file_conf.c
 *
 *  Created on: 14 sep. 2018
 *      Author: Emiliano Ibarrola
 */
#include "MEM.h"

#include "file_conf.h"
#include "error.h"

void get_config(char* path) {
	config = cargarConfiguracion(path,logger);

	memset(&MEM_CONF, 0x00, sizeof(t_MEM_CONF));

	if(config == NULL)
		_exit_with_error("No se encontro el archivo de configuracion", config);

	if(config_has_property(config,"PUERTO")) {
		MEM_CONF.PUERTO = config_get_string_value(config, "PUERTO");
		remove_quotes(MEM_CONF.PUERTO);
		loggear(logger,LOG_LEVEL_INFO,"PUERTO: %s", MEM_CONF.PUERTO);
	}
	if(config_has_property(config,"IP_FS")) {
		MEM_CONF.IP_FS = config_get_string_value(config, "IP_FS");
		remove_quotes(MEM_CONF.IP_FS);
		loggear(logger,LOG_LEVEL_INFO,"IP_FS: %s", MEM_CONF.IP_FS);
	}
	if(config_has_property(config,"PUERTO_FS")) {
		MEM_CONF.PUERTO_FS = config_get_string_value(config, "PUERTO_FS");
		remove_quotes(MEM_CONF.PUERTO_FS);
		loggear(logger,LOG_LEVEL_INFO,"PUERTO_FS: %s", MEM_CONF.PUERTO_FS);
	}

	if(config_has_property(config,"IP_SEEDS")) {
		//TODO: implementar config con listas
		char *ipSeeds;
		int i = 0;
		ipSeeds=config_get_string_value(config, "IP_SEEDS");
		MEM_CONF.IP_SEEDS = string_get_string_as_array(ipSeeds);
		while(MEM_CONF.IP_SEEDS[i] != NULL)
		{
			remove_quotes(MEM_CONF.IP_SEEDS[i]);
			loggear(logger,LOG_LEVEL_INFO,"IP_SEEDS: %s", MEM_CONF.IP_SEEDS[i]);
			i++;
		}

	}
	if(config_has_property(config,"PUERTO_SEEDS")) {
		//TODO: implementar config con listas
		char *puertoSeeds;
		int i = 0;
		puertoSeeds=config_get_string_value(config, "PUERTO_SEEDS");
		MEM_CONF.PUERTO_SEEDS = string_get_string_as_array(puertoSeeds);
		while(MEM_CONF.PUERTO_SEEDS[i] != NULL)
		{
			remove_quotes(MEM_CONF.PUERTO_SEEDS[i]);
			loggear(logger,LOG_LEVEL_INFO,"PUERTO_SEEDS: %s", MEM_CONF.PUERTO_SEEDS[i]);
			i++;
		}
	}

	if(config_has_property(config,"MEMORY_NUMBER_SEEDS")) {
			//TODO: implementar config con listas
			char *puertoSeeds;
			int i = 0;
			puertoSeeds=config_get_string_value(config, "MEMORY_NUMBER_SEEDS");
			MEM_CONF.MEMORY_NUMBER_SEEDS = string_get_string_as_array(puertoSeeds);
			while(MEM_CONF.MEMORY_NUMBER_SEEDS[i] != NULL)
			{
				remove_quotes(MEM_CONF.MEMORY_NUMBER_SEEDS[i]);
				loggear(logger,LOG_LEVEL_INFO,"PUERTO_SEEDS: %s", MEM_CONF.MEMORY_NUMBER_SEEDS[i]);
				i++;
			}
		}

	if(config_has_property(config,"RETARDO_MEM")) {
		MEM_CONF.RETARDO_MEM = config_get_int_value(config, "RETARDO_MEM");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_MEM: %d", MEM_CONF.RETARDO_MEM);
	}
	if(config_has_property(config,"RETARDO_FS")) {
		MEM_CONF.RETARDO_FS = config_get_int_value(config, "RETARDO_FS");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_FS: %d", MEM_CONF.RETARDO_FS);
	}
	if(config_has_property(config,"TAM_MEM")) {
		MEM_CONF.TAM_MEM = config_get_int_value(config, "TAM_MEM");
		loggear(logger,LOG_LEVEL_INFO,"TAM_MEM: %d", MEM_CONF.TAM_MEM);
	}
	if(config_has_property(config,"RETARDO_JOURNAL")) {
		MEM_CONF.RETARDO_JOURNAL = config_get_int_value(config, "RETARDO_JOURNAL");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_JOURNAL: %d", MEM_CONF.RETARDO_JOURNAL);
	}
	if(config_has_property(config,"RETARDO_GOSSIPING")) {
		MEM_CONF.RETARDO_GOSSIPING = config_get_int_value(config, "RETARDO_GOSSIPING");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_GOSSIPING: %d", MEM_CONF.RETARDO_GOSSIPING);
	}
	if(config_has_property(config,"MEMORY_NUMBER")) {
		MEM_CONF.MEMORY_NUMBER = config_get_int_value(config, "MEMORY_NUMBER");
		loggear(logger,LOG_LEVEL_INFO,"MEMORY_NUMBER: %d", MEM_CONF.MEMORY_NUMBER);
	}

	if(config_has_property(config,"IP")) {
		MEM_CONF.IP = config_get_string_value(config, "IP");
		remove_quotes(MEM_CONF.IP);
		loggear(logger,LOG_LEVEL_INFO,"IP: %s", MEM_CONF.IP);
	}
}

void get_modify_config(char* path) {
	config = cargarConfiguracion(path,logger);

	memset(&MEM_CONF, 0x00, sizeof(t_MEM_CONF));

	if(config == NULL)
		_exit_with_error("No se encontro el archivo de configuracion", config);

	if(config_has_property(config,"RETARDO_MEM")) {
		MEM_CONF.RETARDO_MEM = config_get_int_value(config, "RETARDO_MEM");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_MEM: %d", MEM_CONF.RETARDO_MEM);
	}
	if(config_has_property(config,"RETARDO_FS")) {
		MEM_CONF.RETARDO_FS = config_get_int_value(config, "RETARDO_FS");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_FS: %d", MEM_CONF.RETARDO_FS);
	}
	if(config_has_property(config,"RETARDO_JOURNAL")) {
		MEM_CONF.RETARDO_JOURNAL = config_get_int_value(config, "RETARDO_JOURNAL");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_JOURNAL: %d", MEM_CONF.RETARDO_JOURNAL);
	}
	if(config_has_property(config,"RETARDO_GOSSIPING")) {
		MEM_CONF.RETARDO_GOSSIPING = config_get_int_value(config, "RETARDO_GOSSIPING");
		loggear(logger,LOG_LEVEL_INFO,"RETARDO_GOSSIPING: %d", MEM_CONF.RETARDO_GOSSIPING);
	}
}
