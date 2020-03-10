/*
 * consola.c
 *
 *  Created on: 20 oct. 2018
 *      Author: utnso
 */

#include "MEM.h"

#include "commons/string.h"
#include "error.h"
#include "memory.h"
#include "journaling.h"
#include "consola.h"
#include "memory.h"
#include "connection.h"
#include "proceso.h"

bool isNumber(char* s)
{
    for (int i = 0; i < strlen(s); i++)
        if (isdigit(s[i]) == false)
            return false;

    return true;
}

t_tipoComando buscar_enum(char *sval) {
	t_tipoComando result = select_;
	int i = 0;
	char* comandos_str[] = { "select", "insert", "create", "describe", "drop", "journal", "dump", "salir", NULL };
	if (sval == NULL)
		return -2;
	for (i = 0; comandos_str[i] != NULL; ++i, ++result)
		if (string_equals_ignore_case(sval, comandos_str[i]))
			return result;
	return -1;
}

char *character_name_generator(const char *text, int state) {
	static int list_index, len;
	char *name;

	char *character_names[] = { "select", "insert", "create", "describe", "drop", "journal", "dump", "salir",
								"SELECT", "INSERT", "CREATE", "DESCRIBE", "DROP", "JOURNAL", "DUMP", "SALIR", NULL };

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

char **character_name_completion(const char *text, int start, int end) {
	rl_attempted_completion_over = 1;
	return rl_completion_matches(text, character_name_generator);
}

void *crearConsola() {

	char * line;
	rl_attempted_completion_function = character_name_completion;

	while (1) {
		int i;
		line = readline("Ingrese un comando> ");
		if (line) {
			add_history(line);
		}

		char** comando = string_split(line, " ");
		free(line);

		t_tipoComando comando_e = buscar_enum(comando[0]);

		if(comando_e == -2){
			for(i=0;comando[i]!=NULL;i++)
				free(comando[i]);
			if(comando) free(comando);
			continue;
		}

		switch (comando_e) {
		case select_:;
			if (comando[1] == NULL || comando[2] == NULL) {
				printf("error: select {tabla} {key}.\n");
				break;
			}
			if (!isNumber(comando[2])) {
				printf("error: select {tabla} {key}.\n");
				printf("error: {key} debe ser numerico.\n");
				break;
			}
			printf("select...\n");
			char *buffer_select=NULL;
			int largo_buffer = 0;
			int resultSelect = proceso_select(comando[1],atoi(comando[2]),&buffer_select, &largo_buffer);
			if(resultSelect==0){
				t_registro* reg = descomponer_registro(buffer_select);
				printf("[OK] [timestamp:%llu][key:%d] %s\n",reg->timestamp, reg->key, reg->value);
				destruir_registro(reg);
			}
			else
				printf("[ERR] No se pudo obtener el registro \n");
			if(buffer_select!=NULL)
				free(buffer_select);
			break;
		case insert_:;
			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL) {
				printf("error: insert {tabla} {key} {\"value\"}.\n");
				break;
			}
			if (!isNumber(comando[2])) {
				printf("error: insert {tabla} {key} {\"value\"}.\n");
				printf("error: {key} debe ser numerico.\n");
				break;
			}
			char* registroInsertar = string_new();
			for(i=3;comando[i]!=NULL;i++){
				string_append_with_format(&registroInsertar,"%s ",comando[i]);
			}
			registroInsertar[strlen(registroInsertar)-1]=0x00;
			if(strlen(registroInsertar)>tamanio_value){
				free(registroInsertar);
				printf("error: insert {tabla} {key} {\"value\"}.\n");
				printf("error: el tamao del {value} debe ser menor a %d.\n",tamanio_value);
				break;
			}

			printf("insert...\n");
			int escrito = proceso_insert(comando[1],atoll(comando[2]), registroInsertar, 0);
			if(escrito == 0)
				printf("[OK] Se pudo insertar el registro \n");
			else
				printf("[ERR] No se pudo insertar el registro \n");
			free(registroInsertar);
			break;
		case create_:;
			if (comando[1] == NULL || comando[2] == NULL || comando[3] == NULL || comando[4] == NULL) {
				printf("error: create {tabla} {tipo_consistencia} {numero_particiones} {compaction_time}.\n");
				break;
			}
			if (!(string_equals_ignore_case(comando[2],"SC") ||
					string_equals_ignore_case(comando[2],"SHC") ||
						string_equals_ignore_case(comando[2],"EC")) ) {
				printf("error: create {tabla} {tipo_consistencia} {numero_particiones} {compaction_time}.\n");
				printf("error: {tipo_consistencia} ""SC"", ""SHC"" o ""EC""\n");
				break;
			}
			if (!isNumber(comando[3])) {
				printf("error: create {tabla} {tipo_consistencia} {numero_particiones} {compaction_time}.\n");
				printf("error: {numero_particiones} debe ser numerico.\n");
				break;
			}
			if (!isNumber(comando[4])) {
				printf("error: create {tabla} {tipo_consistencia} {numero_particiones} {compaction_time}.\n");
				printf("error: {compaction_time} debe ser numerico.\n");
				break;
			}
			printf("create...\n");
			int creado = proceso_create(comando[1],comando[2], atoi(comando[3]), atoi(comando[4]));
			if(creado == 0)
				printf("[OK] Se pudo crear la tabla \n");
			else
				printf("[ERR] No se pudo crear la tabla \n");
			break;
		case describe_:;
			printf("describe...\n");
			char* buffer_describe=string_new();
			int largo_buffer_describe;
			int describe = proceso_describe(comando[1]!=NULL?comando[1]:"",&buffer_describe, &largo_buffer_describe);
			if(describe == 0){
				printf("[OK] Se pudo realizar el describe\n");
				printf("%s",buffer_describe);
			}
			else
				printf("[ERR] No se pudo realizar el describe\n");
			free(buffer_describe);
			break;
		case drop_:;
			if (comando[1] == NULL ) {
				printf("error: drop {tabla} .\n");
				break;
			}
			printf("drop...\n");
			int drop = proceso_drop(comando[1]);
			if(drop == 0)
				printf("[OK] Se pudo realizar el drop\n");
			else
				printf("[ERR] No se pudo realizar el drop\n");
			break;
		case journal_:;
			printf("journal...\n");
			int journal = proceso_journal();
			if(journal == 0)
				printf("[OK] Se pudo realizar el journal\n");
			else
				printf("[ERR] No se pudo realizar el journal\n");
			break;
		case dump_:;
			printf("Dump (table: %s)...\n",comando[1]!=NULL?comando[1]:"");
			dump_memory_spa(comando[1]!=NULL?comando[1]:"");
			break;
		case salir_:;
			exit_gracefully(EXIT_SUCCESS);
			break;
		default:;
			printf("No se reconoce el comando %s .\n", comando[0]);
		}
		for(i=0;comando[i]!=NULL;i++)
			free(comando[i]);
		if(comando) free(comando);
	}
	return 0;
}

void crearHiloConsola() {

	sigset_t set;
	int s;
	int hilo_consola;

	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	s = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (s != 0)
		_exit_with_error("No se pudo bloquear SIGINT con prthread_sigmask",NULL);

	hilo_consola = pthread_create(&consola, NULL, crearConsola, (void *) &set);
	if (hilo_consola == -1) {
		log_error(logger, "No se pudo generar el hilo para la consola.");
	}
	log_info(logger, "Se generó el hilo para la consola.");
}
