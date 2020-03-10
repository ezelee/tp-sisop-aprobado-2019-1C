/*
 * Consola.c
 *
 *  Created on: 3 may. 2019
 *      Author: utnso
 */

#include "Consola.h"
#include "parser.h"
#include "parser.c"
#include "commons/string.h"

void *crear_consola() {

	char *line;
	rl_attempted_completion_function = character_name_completion;

	while (1) {
		int i = 0;
		line = readline("Ingrese un comando> ");
		if (line) {
			add_history(line);
		}

		char *lineaCompleta = string_new();
		string_append(&lineaCompleta, line);


		char** comando = string_split(line, " ");
		free(line);

		t_tipoComando comando_e = buscar_enum_lfs(comando[0]);

		if (comando_e == -2) {
			for (i = 0; comando[i] != NULL; i++)
				free(comando[i]);
			if (comando)
				free(comando);
			continue;
		}

		switch (comando_e) {

		case _select:
			printf("CONSOLA: Se ingresó comando SELECT \n");
			if (comando[1] == NULL || comando[2] == NULL) {
				printf("Falta ingresar datos para utilizar select\n");
				break;
			}

			int exists = ExisteTabla(comando[1]);
			if (!exists) {
				printf("La tabla %s no existe.\n", comando[1]);
				break;
			}
			int bloqueado = GetEstadoTabla(comando[1]);
			if (bloqueado) {
				printf("La tabla %s se encuentra bloqueada.\n", comando[1]);
				break;
			}
			t_select *selectMsg = malloc(sizeof(t_select));
			strcpy(selectMsg->nombreTabla, comando[1]);
			selectMsg->key = atoi(comando[2]);
			t_registro *regSelect = BuscarKey(selectMsg);

			if (regSelect->key != -1) {
				log_info(logger, "Registro key %d value %s timestamp %llu",
						regSelect->key, regSelect->value, regSelect->timestamp);
				printf("Key\t Value\n");
				printf("%d \t %s\n", regSelect->key, regSelect->value);
			}
			if (regSelect->key == -1)
				printf("No se encuentra la key en FS.\n");

			free(regSelect);
			free(selectMsg);
			break;

		case _insert:

			printf("CONSOLA: Se ingresó comando INSERT \n");

			char** comandoCopia = string_split(lineaCompleta, "\"");
			char *comandoValue = string_new();
			string_append(&comandoValue, "\"");
			string_append(&comandoValue, comandoCopia[1]);
			string_append(&comandoValue, "\"");

			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL) {
				printf("Faltan ingresar datos para utilizar el comando\n");
				break;
			}
			//valido tamaño del value
			if ((strlen(comando[3])) > lfs_conf.tamano_value) {
				printf(
						"El tamaño del value con %d bytes supera lo permitido de %d bytes\n",
						(strlen(comando[3])), lfs_conf.tamano_value);
				break;
			}

			int existsI = ExisteTabla(comando[1]);
			if (!existsI) {
				printf("La tabla %s no existe.\n", comando[1]);
				break;
			}
			if (comandoCopia[2] == NULL) {
				unsigned long long timestamp = obtenerTimeStamp();
				comandoCopia[2] = malloc(20);
				sprintf(comandoCopia[2], "%llu", timestamp);
			}

			printf("Tabla: %s\n", comando[1]);
			printf("Key: %s\n", comando[2]);
			printf("Value: %s\n", comandoValue);
			printf("Timestamp: %s\n", comandoCopia[2]);
			t_insert *insert = malloc(sizeof(t_insert));
			strcpy(insert->nombreTabla, comando[1]);
			insert->key = atoi(comando[2]);
			strcpy(insert->value, comandoValue);
			insert->timestamp = atoll(comandoCopia[2]);

			int resultInsert = InsertarTabla(insert);
			if (resultInsert == -1) {
				printf("La tabla %s se encuentra bloqueada.\n", comando[1]);
				break;
			}
			if (resultInsert) {
				printf("La tabla %s no se encuentra creada.\n", comando[1]);
			} else {
				printf("Registro insertado correctamente.\n");
			}
			free(comando[4]);
			comando[4] = NULL;
			free(insert);

			free(comandoCopia[2]);
			comandoCopia[2] = NULL;
			for(i=0;comandoCopia[i]!=NULL;i++)
				free(comandoCopia[i]);
			if(comandoCopia) free(comandoCopia);
			free(comandoValue);

			break;

		case _create:

			printf("CONSOLA: Se ingresó comando CREATE \n");

			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL
					|| comando[4] == NULL) {
				printf("Faltan ingresar datos para la creación de la tabla\n");
			} else {
				t_create *msgCreate = malloc(sizeof(t_create));
				strcpy(msgCreate->nombreTabla, comando[1]);
				strcpy(msgCreate->tipo_cons, comando[2]);
				msgCreate->num_part = atoi(comando[3]);
				msgCreate->comp_time = atoi(comando[4]);

				printf("Tabla: %s\n", msgCreate->nombreTabla);
				printf("Tipo consistencia: %s\n", msgCreate->tipo_cons);
				printf("Numero particiones: %d\n", msgCreate->num_part);
				printf("Compactation time: %d\n", msgCreate->comp_time);

				int result_create = CrearTabla(msgCreate);
				if (result_create == -1)
					printf("La tabla %s ya existe en el FS.\n",
							msgCreate->nombreTabla);
				if (result_create == 0)
					printf("Se creo la tabla exitosamente.\n");
				free(msgCreate);
			}
			break;

		case _describe:
			printf("CONSOLA: Se ingresó comando DESCRIBE \n");
			if (comando[1] == NULL) {
				printf("Obteniendo metadata de todas las tablas.\n");
				int tablas = ContarTablas();
				(tablas != 0) ? ObtenerMetadataCompleto() : printf("No hay tablas en el filesystem.\n");
			} else {
				int resultExists = ExisteTabla(comando[1]);
				char *tableName = string_new();
				string_append(&tableName, comando[1]);

				switch (resultExists) {
				case 1:
					printf("Metadata de tabla %s\n", tableName);
					t_metadata *metadata;
					metadata = ObtenerMetadataTabla(tableName);
					int particiones = metadata->particiones;
					int tiempoCompactacion = metadata->compactationTime;
					char *consistencia = string_new();
					string_append(&consistencia, metadata->tipoConsistencia);
//					printf("CONSISTENCY=%s\n", consistencia);
//					printf("PARTITIONS=%d\n", particiones);
//					printf("COMPACTATION_TIME=%d\n", tiempoCompactacion);
					printf("Consistency\tPartitions\tCompactionTime\n");
					printf("%s\t\t%d\t\t%d\n", consistencia, particiones,
							tiempoCompactacion);
					free(consistencia);
					free(metadata);

					break;
				case 0:
					printf("La tabla no existe. \n");
				}
			}
			break;

		case _drop:
			printf("CONSOLA: Se ingresó comando DROP \n");

			if (comando[1] == NULL) {
				printf("Se debe ingresar la tabla a dropear\n");
				break;
			}

			int resultado = DropearTabla(comando[1]);
			if (!resultado)
				printf("Tabla %s eliminada.\n", comando[1]);
			if (resultado == 1)
				printf("La tabla ingresada no existe.\n");
			if (resultado == -1)
				printf("La tabla %s se encuentra bloqueada.\n", comando[1]);

			break;
		case salir_:;
				printf("Cerrando consola...\n");
			exit_gracefully(EXIT_SUCCESS);
			break;
		default:
			printf("No se reconoce el comando %s .\n", comando[0]);
		}
		for(i=0;comando[i]!=NULL;i++)
			free(comando[i]);
		if(comando) free(comando);


		free(lineaCompleta);
	}
	return 0;
}

char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "SELECT", "INSERT", "CREATE", "DESCRIBE",
			"DROP", "SALIR", NULL };

	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	while ((name = character_names[list_index++])) {
		if (strncmp(name, text, len) == 0) {
			return strdup(name);
		}
	}
	return NULL;
}

t_tipoComando buscar_enum_lfs(char *sval) {
	t_tipoComando result = select_;
	int i = 0;
	char* comandos_str[] = { "SELECT", "INSERT", "CREATE", "DESCRIBE", "DROP", "SALIR",
			NULL };
	if (sval == NULL)
		return -2;
	for (i = 0; comandos_str[i] != NULL; ++i, ++result)
		if (0 == strcmp(sval, comandos_str[i]))
			return result;
	return -1;
}

/*void liberar_comando(char ** comando){//, int elementos){
//	for(int i = 0; i < elementos; i++){
//		free(comando[i]);
//	}
//	free(comando);
	for(int i=0;comando[i]!=NULL;i++){
		free(comando[i]);
		log_info(logger, "Liberando comando %s", comando[i]);
	}

	if(comando) free(comando);
}*/
