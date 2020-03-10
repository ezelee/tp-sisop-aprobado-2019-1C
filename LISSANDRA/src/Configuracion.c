/*
 * Config.c
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#include "Configuracion.h"

void CargarConfigLFS() {
	config = cargarConfiguracion("../config/Lissandra.config", logger);

	if (config == NULL)
		_exit_with_error("No se encontro el archivo de configuracion", config);

	if (config_has_property(config, "PUERTO"))
		lfs_conf.puerto = config_get_string_value(config, "PUERTO");
	remove_quotes(lfs_conf.puerto);
	string_trim_right(&lfs_conf.puerto);

	if (config_has_property(config, "PUNTO_MONTAJE"))
		lfs_conf.punto_montaje = config_get_string_value(config,
				"PUNTO_MONTAJE");
	remove_quotes(lfs_conf.punto_montaje);
	string_trim_right(&lfs_conf.punto_montaje);

	if (config_has_property(config, "RETARDO"))
		lfs_conf.retardo = config_get_int_value(config, "RETARDO");

	if (config_has_property(config, "TAMAÑO_VALUE"))
		lfs_conf.tamano_value = config_get_int_value(config, "TAMAÑO_VALUE");

	if (config_has_property(config, "TIEMPO_DUMP"))
		lfs_conf.tiempo_dump = config_get_int_value(config, "TIEMPO_DUMP");

}

void AplicarRetardo() {
	sleep(lfs_conf.retardo / 1000);
}

void get_modify_config(char* path) {
	config = cargarConfiguracion(path, logger);

	memset(&lfs_conf, 0x00, sizeof(t_lfs_config));

	if (config == NULL)
		_exit_with_error("No se encontro el archivo de configuracion", config);


	if (config_has_property(config, "PUERTO"))
		lfs_conf.puerto = config_get_string_value(config, "PUERTO");
	remove_quotes(lfs_conf.puerto);
	string_trim_right(&lfs_conf.puerto);

	if (config_has_property(config, "PUNTO_MONTAJE"))
		lfs_conf.punto_montaje = config_get_string_value(config,
				"PUNTO_MONTAJE");
	remove_quotes(lfs_conf.punto_montaje);
	string_trim_right(&lfs_conf.punto_montaje);

	if (config_has_property(config, "RETARDO"))
		lfs_conf.retardo = config_get_int_value(config, "RETARDO");

	if (config_has_property(config, "TAMAÑO_VALUE"))
		lfs_conf.tamano_value = config_get_int_value(config, "TAMAÑO_VALUE");

	if (config_has_property(config, "TIEMPO_DUMP"))
		lfs_conf.tiempo_dump = config_get_int_value(config, "TIEMPO_DUMP");
}

