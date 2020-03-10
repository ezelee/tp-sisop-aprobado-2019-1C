/*
 * Planificador.c
 *
 *  Created on: 21 abr. 2019
 *      Author: utnso
 */

#include "Planificador.h"


void* planificar() {

	while(1) {

		// Encuentro nuevo proceso en READY
		log_info(logger, "PLANIFIC| Iniciando thread");
		sem_wait(&sem_multiprog);
		sem_wait(&sem_ready);
		log_info(logger, "PLANIFIC| Semaforos OK");
//		aplicar_algoritmo_rr();

		pthread_t thread_planificacion;
//		pthread_create(&thread_planificacion, NULL, aplicar_algoritmo_rr(), NULL);
		int hilo_algoritmo = pthread_create(&thread_planificacion, NULL, aplicar_algoritmo_rr, NULL);

		if (hilo_algoritmo == -1) {
			log_error(logger, "THREAD|No se pudo generar el hilo para el algoritmo.");
		}
		log_info(logger, "THREAD|Se generó el hilo para el algoritmo.");
		pthread_join(thread_planificacion,(void**)NULL);
	}
}

int asignar_id_proceso() {
	cont_id_procesos += 1;
	return cont_id_procesos;
}

void crear_proceso(char* line,t_request* request) {

	/* 1. Creación de PCB */
	t_pcb* proceso = malloc(sizeof(t_pcb));
	int id_proceso = asignar_id_proceso();

	/* 2. Agregar a NEW */
	log_info(logger, "PLANIFIC| Proceso %d generado", id_proceso);

	pthread_mutex_lock(&mutex_new);
	agregar_proceso(proceso, lista_new, &sem_new);
	pthread_mutex_unlock(&mutex_new);

	log_info(logger, "PLANIFIC| Proceso %d agregado a NEW", id_proceso);

	/* 3. Carga de PCB */
	log_info(logger, "PLANIFIC| Cargando PCB.");
	proceso->script = string_new();
	proceso->ruta_archivo = string_new();
	proceso->id_proceso = id_proceso;
	proceso->program_counter = 0;
	string_append(&proceso->script, line);
	proceso->cantidad_request = cantidad_request(line);

	if( request->request == _run) {
		string_append(&proceso->ruta_archivo, request->parametro1);
	}

	imprimir_pcb(proceso);

	/* 4. Pasar PCB de NEW a READY */
	pthread_mutex_lock(&mutex_new);
	proceso = sacar_proceso(id_proceso, lista_new, &sem_new);
	pthread_mutex_unlock(&mutex_new);

	pthread_mutex_lock(&mutex_ready);
	agregar_proceso(proceso, lista_ready, &sem_ready);
	pthread_mutex_unlock(&mutex_ready);


	log_info(logger, "PLANIFIC| Proceso %d de NEW a READY.", id_proceso);

}

void agregar_proceso(t_pcb* proceso,t_list* lista, sem_t* sem) {

	list_add(lista, proceso);
	sem_post(sem);
}

t_pcb* sacar_proceso(int id, t_list* lista, sem_t* sem) {

	int buscar_pcb_por_id(t_pcb* pcb) {
		return pcb->id_proceso == id;
	}
	t_pcb* proceso = list_remove_by_condition(lista, (void *)buscar_pcb_por_id);
	sem_wait(sem);

	return proceso;
}

t_pcb* sacar_proceso_rr(t_list* lista) {

	log_info(logger, "PLANIFIC| Buscando PCB por ROUND ROBIN");

//	pthread_mutex_lock(&mutex_lista_ready);
	t_pcb *pcb = list_remove(lista_ready, 0);
//	pthread_mutex_unlock(&mutex_lista_ready);

	return pcb;

}

int cantidad_request(char* buffer) {

	char ** arrays_linea = string_split(buffer,"\n" );
	int i = 0;
	int cantidad_lineas = 0;

	while(arrays_linea[i] != NULL){

		cantidad_lineas++;
		i++;
	}

	if (cantidad_lineas == 0) {
		cantidad_lineas = 1;
	}

	for (int i = 0; arrays_linea[i] != NULL; i++)
		free(arrays_linea[i]);
	if (arrays_linea)
		free(arrays_linea);

	return cantidad_lineas;
}


t_tipoSeeds* obtener_memoria_random() {

//	t_tipoSeeds *memory = get_memoria_conectada();;
//	log_info(logger, "REFRESH| Numero Memoria: %d", memory->numeroMemoria);
//	return memory;
	int n;
	pthread_mutex_lock(&mutex_memoria_ev);
	int size_ev = list_size(lista_criterio_ev);
	t_tipoSeeds *memory;
	if(size_ev == 0)
	{
		memory = malloc(sizeof(t_tipoSeeds));
		memset(memory, 0x00, sizeof(t_tipoSeeds));
		memory->numeroMemoria = -1;
		pthread_mutex_unlock(&mutex_memoria_ev);
		return memory;
	}
	n = rand() % size_ev;

	memory = list_get(lista_criterio_ev, n);
	pthread_mutex_unlock(&mutex_memoria_ev);
	return memory;

}

void retardo_ejecucion() {

	sleep(kernel_conf.sleep_ejecucion/1000);
}

int procesar_pcb(t_pcb* pcb) {

	int quantum_restante = calcular_quantum(pcb);
	log_info(logger, "PLANIFIC| Quantum a consumir: %d", quantum_restante);

	for (int i=0; quantum_restante > i; i++ ) {

		log_info(logger, "PLANIFIC| --- Consumiendo Quantum ---");
		retardo_ejecucion();

		// Parsear request y procesarlo
		char **linea = string_split(pcb->script, "\n");

//		pthread_mutex_lock(&mutex_request);
		int resultado = ejecutar_request(linea[pcb->program_counter], pcb->id_proceso);
//		pthread_mutex_unlock(&mutex_request);

		// Si el request falla, se termina el proceso
		if(resultado < 0) {

			pthread_mutex_lock(&mutex_exec);
			sacar_proceso(pcb->id_proceso, lista_exec, &sem_exec);
			pthread_mutex_unlock(&mutex_exec);
			sem_post(&sem_multiprog);

			pthread_mutex_lock(&mutex_exit);
			agregar_proceso(pcb, lista_exit, &sem_exit);
			free(pcb->ruta_archivo);
			free(pcb->script);
			pthread_mutex_unlock(&mutex_exit);

			log_info(logger, "PLANIFIC| Proceso %d pasa a EXIT", pcb->id_proceso);
			//libero script
			for(int j=0;linea[j]!=NULL;j++)
				free(linea[j]);
			if(linea) free(linea);
			return EXIT_FAILURE;
		}

		log_info(logger, "PLANIFIC| Resultado del request: %d", resultado);

		pcb->program_counter++;
		log_info(logger, "PLANIFIC| Nuevo Program Counter: %d", pcb->program_counter);

		//libero script
		for(int j=0;linea[j]!=NULL;j++)
			free(linea[j]);
		if(linea) free(linea);

	}

	// Saco proceso de EXEC y evaluo si finalizó o vuelve a READY

	pthread_mutex_lock(&mutex_exec);
	sacar_proceso(pcb->id_proceso, lista_exec, &sem_exec);
	pthread_mutex_unlock(&mutex_exec);

	sem_post(&sem_multiprog);

	if (pcb->program_counter == pcb->cantidad_request) {

		pthread_mutex_lock(&mutex_exit);
		agregar_proceso(pcb, lista_exit, &sem_exit);
		free(pcb->ruta_archivo);
		free(pcb->script);
		pthread_mutex_unlock(&mutex_exit);
		log_info(logger, "PLANIFIC| Proceso %d pasa a EXIT", pcb->id_proceso);

	} else {

		pthread_mutex_lock(&mutex_ready);
		agregar_proceso(pcb, lista_ready, &sem_ready);
		pthread_mutex_unlock(&mutex_ready);
		log_info(logger, "PLANIFIC| Proceso %d vuelve a READY", pcb->id_proceso);

	}

	return EXIT_SUCCESS;

}

t_registro* descomponer_registro(char *buffer)
{
	t_registro* registro = malloc(sizeof(t_registro));
	memcpy(&registro->timestamp,buffer,sizeof(unsigned long long));
	memcpy(&registro->key,buffer+sizeof(unsigned long long),sizeof(int));
	memcpy(&registro->value[0],buffer+sizeof(unsigned long long)+sizeof(int),VALUE);
	return registro;
}

int ejecutar_request(char* linea, int id_proceso) {

	unsigned long long t_inicioSelect, t_finSelect;
	unsigned long long t_inicioInsert, t_finInsert;
	log_info(logger, "PLANIFIC| Request a ejecutar: %s", linea);
	t_request* request = parsear(linea, logger);
	int resultado, cliente;
	t_tipoSeeds* memoria;
	t_metadata* tabla;

	switch (request->request) {

		case _select:;
			// SELECT [NOMBRE_TABLA] [KEY]
			// SELECT TABLA1 1

		t_inicioSelect = obtenerTimeStamp();

			// VALIDAR EXISTENCIA DE TABLA
			tabla = buscar_tabla(request->parametro1);

			if(tabla == NULL) {
				log_info(logger, "PLANIFIC| SELECT: La tabla no existe.");
				metric_select.contador++;
				t_finSelect = obtenerTimeStamp();
//				log_info(logger, "PLANIFIC| SELECT: DURACION: %d segundos", t_finSelect - t_inicioSelect);
				metric_select.duracion += (t_finSelect - t_inicioSelect) / 1000;
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				return -1;
			}

			// PREPARANDO REQUEST
			log_info(logger, "PLANIFIC| SELECT: Preparando... ");
			t_select* req_select = malloc(sizeof(t_select));
			memset(req_select, 0x00, sizeof(t_select));

			req_select->id_proceso = id_proceso;
			strcpy(req_select->nombreTabla, request->parametro1);
			req_select->key = atoi(request->parametro2);

			log_info(logger, "PLANIFIC| SELECT: %s, %d", req_select->nombreTabla, req_select->key);

			// BUSCANDO MEMORIA
			log_info(logger, "PLANIFIC| SELECT: Buscando memoria del criterio %s", tabla->tipoConsistencia);
			memoria = get_memoria_por_criterio(tabla->tipoConsistencia, req_select->key);
			if (memoria->numeroMemoria <0)
			{
				log_info(logger, "PLANIFIC| SELECT - MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				free(req_select);
				return -1;
			}
			cliente = conectar_a_memoria(memoria);

			if(cliente < 0){
				log_info(logger, "PLANIFIC| SELECT - NO PUDO CONECTARSE A MEM.");
				remover_memoria(memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
//				close(cliente);
				free(req_select);
				return -1;
			}

			// REQUEST
			log_info(logger, "PLANIFIC| SELECT: Enviando a MEMORIA");
			int resultado_mensaje_select = enviarMensajeConError(kernel, selectMsg, sizeof(t_select), req_select, cliente, logger, mem, 0);
			log_info(logger, "PLANIFIC| SELECT: Resultado de enviar mensaje: %d", resultado_mensaje_select);

			/*if(resultado_mensaje_select <1)
			{
				log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				free(tabla);
				return -1;
			}*/
			//TODO: VER SI NO PUEDE MANDAR NADA A MEMORIA

			// RESPUESTA
			log_info(logger, "PLANIFIC| SELECT: RECIBIENDO...");
			t_mensaje* resultado_req_select = recibirMensaje(cliente, logger);
			
			if(resultado_req_select == NULL)
			{
				log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				remover_memoria(memoria);
//				free (memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				close(cliente);
				free(req_select);
				return 0;
			}
			
			//JOURNALING
			log_info(logger, "PLANIFIC| SELECT: Resultado de MSJ a MEM %d",resultado_req_select->header.error);
			if(resultado_req_select->header.error==-100)
			{
				log_info(logger, "PLANIFIC| SELECT - MEMORIA COMPLETA - INICIANDO JOURNALING PARA %s",tabla->tipoConsistencia);
				//HACER JOURNALLING A LAS MEMORIAS
				int resultadoJournal = enviarjournalSegunCriterio(tabla->tipoConsistencia);

				if(resultadoJournal > 0)
				{
//					free (memoria);
					// VOLVER A ENVIAR EL MSJ SELECT
					memoria = get_memoria_por_criterio(tabla->tipoConsistencia, req_select->key);
					if (memoria->numeroMemoria <0)
					{
						log_info(logger, "PLANIFIC| SELECT: POST-JOURNAL MEMORIA: %d",memoria->numeroMemoria);
						free (memoria);
						free(tabla);
						if(request->parametro1) free(request->parametro1);
						if(request->parametro2) free(request->parametro2);
						if(request->parametro3) free(request->parametro3);
						if(request->parametro4) free(request->parametro4);
						if(request) free(request);
						close(cliente);
						free(req_select);
						return -1;
					}
					cliente = conectar_a_memoria(memoria);

					if(cliente < 0){
						log_info(logger, "PLANIFIC| SELECT POST-JOURNAL - NO PUDO CONECTARSE A MEM.");
						remover_memoria(memoria);
						free(tabla);
						if(request->parametro1) free(request->parametro1);
						if(request->parametro2) free(request->parametro2);
						if(request->parametro3) free(request->parametro3);
						if(request->parametro4) free(request->parametro4);
						if(request) free(request);
//						close(cliente);
						free(req_select);
						return -1;
					}

					// REQUEST
					log_info(logger, "PLANIFIC| SELECT: POST-JOURNAL Enviando a MEMORIA");
					resultado_mensaje_select = enviarMensajeConError(kernel, selectMsg, sizeof(t_select), req_select, cliente, logger, mem, 0);
					log_info(logger, "PLANIFIC| SELECT: POST-JOURNAL Resultado de enviar mensaje: %d", resultado_mensaje_select);

					/*if(resultado_mensaje_select <1)
					{
						log_info(logger, "PLANIFIC| SELECT - POST-JOURNAL NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
						free (memoria);
						free(tabla);
						return -1;
					}*/
					log_info(logger, "PLANIFIC| SELECT: RECIBIENDO 2...");
					resultado_req_select = recibirMensaje(cliente, logger);
				}

			}

			
			if(resultado_req_select == NULL)
			{
				log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				remover_memoria(memoria);
//				free (memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				close(cliente);
				free(req_select);
				return 0;
			}
//			int largo_buffer = resultado_req_select->header.longitud;
			char *buffer = malloc(sizeof(t_registro));
			memset(buffer, 0x00,sizeof(t_registro));
			memcpy(buffer, resultado_req_select->content, resultado_req_select->header.longitud);

			t_registro* reg = descomponer_registro(buffer);

			log_info(logger, "PLANIFIC| SELECT: RESPUESTA KEY:%d, VALOR:%s TIMESTAMP:%llu", reg->key, reg->value,reg->timestamp);

			// METRICAS
			metric_select.contador++;
			t_finSelect = obtenerTimeStamp();
			metric_select.duracion += (t_finSelect - t_inicioSelect) / 1000;
//			log_info(logger, "PLANIFIC| SELECT: DURACION %d segundos", duracion);

			if(resultado_req_select->header.error == -100)
			{
				resultado = resultado_req_select->header.error; // Cambiar por lo que devuelve la memoria.
			}else{
				resultado = 0;
			}

			// LIBERAR MEMORIA
			free(tabla);
			free(req_select);
			destruirMensaje(resultado_req_select);
			free(reg);
			free(buffer);
			close(cliente);


			break;

		case _insert:
			// INSERT [NOMBRE_TABLA] [KEY] “[VALUE]”
			// INSERT TABLA1 3 “Mi nombre es Lissandra”
			t_inicioInsert = obtenerTimeStamp();
			if (memoria_sc != NULL)
				log_info(logger, "VALIDACION| MEMORIA SC: %d", memoria_sc->numeroMemoria);

			tabla = buscar_tabla(request->parametro1);

			if(tabla == NULL) {
				log_info(logger, "PLANIFIC| INSERT: La tabla no existe.");
				metric_insert.contador++;
				t_finInsert = obtenerTimeStamp();
				metric_insert.duracion += (t_finInsert - t_inicioInsert) / 1000;
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				return -1;
			}

			log_info(logger, "PLANIFIC| INSERT: La tabla %s.", tabla->nombreTabla);

			log_info(logger, "PLANIFIC| INSERT: Preparando...");
			t_insert* req_insert = malloc(sizeof(t_insert));
			memset(req_insert, 0x00, sizeof(t_insert));
			req_insert->id_proceso = id_proceso;
			strcpy(req_insert->nombreTabla, request->parametro1);
			req_insert->timestamp = obtenerTimeStamp();
			req_insert->key = atoi(request->parametro2);
			strcpy(req_insert->value, request->parametro3);

			log_info(logger, "PLANIFIC| INSERT: %s, %llu, %d, %s", req_insert->nombreTabla, req_insert->timestamp, req_insert->key, req_insert->value);

			//BUSCAR MEMORIA POR CRITERIO Y CONECTARSE
			memoria = get_memoria_por_criterio(tabla->tipoConsistencia, req_insert->key);

			if (memoria->numeroMemoria <0)
			{
				log_info(logger, "PLANIFIC| INSERT - MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				free(req_insert);
				return -1;
			}

			if (memoria_sc != NULL)
				log_info(logger, "VALIDACION| MEMORIA SC: %d", memoria_sc->numeroMemoria);

			cliente = conectar_a_memoria(memoria);

			if(cliente < 0){
				log_info(logger, "PLANIFIC| INSERT - NO PUDO CONECTARSE A MEM.");
				remover_memoria(memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
//				close(cliente);
				free(req_insert);
				return -1;
			}

			log_info(logger, "PLANIFIC| INSERT: Enviando a MEMORIA");

			if (memoria_sc != NULL)
				log_info(logger, "VALIDACION| Memoria SC tiene: %d", memoria_sc->numeroMemoria);

			log_info(logger, "PLANIFC| INSERT: Memoria Asignada tiene: %d", memoria->numeroMemoria);
			int resultado_mensaje_insert = enviarMensajeConError(kernel, insert, sizeof(t_insert), req_insert, cliente, logger, mem, 0);
			log_info(logger, "PLANIFC| INSERT: Resultado de enviar mensaje: %d", resultado_mensaje_insert);

			//TODO: VER SI NO PUEDE MANDAR NADA A MEMORIA
			/*if(resultado_mensaje_insert <1)
			{
				log_info(logger, "PLANIFIC| INSERT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				free(tabla);
				return -1;
			}*/


			t_mensaje* resultado_req_insert = recibirMensaje(cliente, logger);
			
			if(resultado_req_insert == NULL)
			{
				log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				remover_memoria(memoria);
//				free (memoria);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				close(cliente);
				free(req_insert);
				return 0;
			}

			//JOURNALLING
			log_info(logger, "PLANIFIC| INSERT: Resultado de MSJ a MEM %d",resultado_req_insert->header.error);
			if(resultado_req_insert->header.error==-100)
			{
				log_info(logger, "PLANIFIC| INSERT: MEMORIA COMPLETA - INICIANDO JOURNALING PARA %s",tabla->tipoConsistencia);
				//HACER JOURNALLING A LAS MEMORIAS
				int resultado = enviarjournalSegunCriterio(tabla->tipoConsistencia);

				if(resultado > 0)
				{
//					free (memoria);
					//VOLVER A ENVIAR EL MSJ - INSERT
					memoria = get_memoria_por_criterio(tabla->tipoConsistencia, req_insert->key);
					if (memoria->numeroMemoria <0)
					{
						log_info(logger, "PLANIFIC| INSERT: POST-JOURNAL MEMORIA: %d",memoria->numeroMemoria);
						free (memoria);
						free(tabla);
						if(request->parametro1) free(request->parametro1);
						if(request->parametro2) free(request->parametro2);
						if(request->parametro3) free(request->parametro3);
						if(request->parametro4) free(request->parametro4);
						if(request) free(request);
						close(cliente);
						free(req_insert);
						return -1;
					}
					cliente = conectar_a_memoria(memoria);

					if(cliente < 0){
						log_info(logger, "PLANIFIC| INSERT POST-JOURNAL - NO PUDO CONECTARSE A MEM.");
						remover_memoria(memoria);
						free(tabla);
						if(request->parametro1) free(request->parametro1);
						if(request->parametro2) free(request->parametro2);
						if(request->parametro3) free(request->parametro3);
						if(request->parametro4) free(request->parametro4);
						if(request) free(request);
//						close(cliente);
						free(req_insert);
						return -1;
					}

					log_info(logger, "PLANIFC| INSERT: POST-JOURNAL Memoria Asignada tiene: %d", memoria->numeroMemoria);
					resultado_mensaje_insert = enviarMensajeConError(kernel, insert, sizeof(t_insert), req_insert, cliente, logger, mem, 0);
					log_info(logger, "PLANIFC| INSERT: POST-JOURNAL Resultado de enviar mensaje INSERT: %d", resultado_mensaje_insert);

					/*if(resultado_mensaje_insert <1)
					{
						log_info(logger, "PLANIFIC| INSERT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
						free (memoria);
						free(tabla);
						return -1;
					}*/

					log_info(logger, "PLANIFIC| INSERT: RECIBIENDO 2...");
					resultado_req_insert = recibirMensaje(cliente, logger);

				}

			}


			
			if(resultado_req_insert == NULL)
			{
				log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				remover_memoria(memoria);
//				free (memoria);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				close(cliente);
				free(req_insert);
				return 0;
			}
			
			resultado = resultado_req_insert->header.error;
			log_info(logger, "PLANIFIC| INSERT: Resultado: %d", resultado);

			free(tabla);
			free(req_insert);
			destruirMensaje(resultado_req_insert);

			metric_insert.contador++;
			t_finInsert = obtenerTimeStamp();
			metric_insert.duracion += (t_finInsert - t_inicioInsert) / 1000;

			close(cliente);
			break;

		case _create:;
			// CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
			// CREATE TABLA1 SC 4 60000

			log_info(logger, "PLANIFIC| CREATE: Preparando...");

			t_create* req_create = malloc(sizeof(t_create));
			memset(req_create, 0x00, sizeof(t_create));

			req_create->id_proceso = id_proceso;
			strcpy(req_create->nombreTabla, request->parametro1);
			strcpy(req_create->tipo_cons, request->parametro2);
			req_create->num_part = atoi(request->parametro3);
			req_create->comp_time = atoi(request->parametro4);

			log_info(logger, "PLANIFIC| CREATE: %s, %s, %d, %d", req_create->nombreTabla, req_create->tipo_cons, req_create->num_part, req_create->comp_time);

			//BUSCA CUALQUIER MEMORIA CONECTADA - EL CREATE LO HACE SI O SI
			memoria = get_memoria_asociada();

			if (memoria->numeroMemoria <0)
			{
				log_info(logger, "PLANIFIC| CREATE: MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				free(req_create);
				return -1;
			}
			cliente = conectar_a_memoria(memoria);

			if(cliente < 0){
				log_info(logger, "PLANIFIC| CREATE - NO PUDO CONECTARSE A MEM.");
				remover_memoria(memoria);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
//				close(cliente);
				free(req_create);
				return -1;
			}

			log_info(logger, "PLANIFIC| CREATE: Enviando  a MEMORIA");
			int resultado_mensaje = enviarMensajeConError(kernel, create, sizeof(t_create), req_create, cliente, logger, mem, 0);
			log_info(logger, "PLANIFIC| CREATE: Resultado de enviar mensaje: %d", resultado_mensaje);

			//TODO: VER SI NO PUEDE MANDAR NADA A MEMORIA

			/*if(resultado_mensaje <1)
			{
				log_info(logger, "PLANIFIC| CREATE - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				return -1;
			}*/

			t_mensaje* resultado_req_create = recibirMensaje(cliente, logger);
			
			if(resultado_req_create == NULL)
			{
				log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				remover_memoria(memoria);
//				free (memoria);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				close(cliente);
				free(req_create);
				return 0;
			}
			
			resultado = resultado_req_create->header.error;
			log_info(logger, "PLANIFIC| CREATE: Resultado: %d", resultado);

			if (resultado == 0) {
				log_info(logger, "PLANIFIC| CREATE: Solicitando describe_global");
				//LOS MUTEX SE HACEN CUANDO HACE EL LIST_ADD
//				pthread_mutex_lock(&mutex_metadata);
//				limpiar_metadata();
				describe_global(cliente);
//				pthread_mutex_unlock(&mutex_metadata);
			}

			resultado = resultado_req_create->header.error; // Cambiar por lo que devuelve la memoria.

//			free (memoria);
			free(req_create);
			destruirMensaje(resultado_req_create);
			close(cliente);

			break;

		case _describe:
			// DESCRIBE
			// DESCRIBE TABLA1

			log_info(logger, "PLANIFIC| DESCRIBE: Preparando...");

			t_describe* req_describe = malloc(sizeof(t_describe));
			memset(req_describe, 0x00, sizeof(t_describe));
			req_describe->id_proceso = id_proceso;
			strcpy(req_describe->nombreTabla, "");


			if (string_is_empty(request->parametro1)) {
				log_info(logger, "PLANIFIC| DESCRIBE: Describe GLOBAL.");

				//TODO: VER SI SEPUEDE CON MEMORIA ASOCIADA
				//OBTENGO UNA MEMORIA CUALQUIERA
				memoria = get_memoria_asociada();
				if (memoria->numeroMemoria <0)
				{
					log_info(logger, "PLANIFIC| DESCRIBE: MEMORIA: %d",memoria->numeroMemoria);
					free (memoria);
					if(request->parametro1) free(request->parametro1);
					if(request->parametro2) free(request->parametro2);
					if(request->parametro3) free(request->parametro3);
					if(request->parametro4) free(request->parametro4);
					if(request) free(request);
					free(req_describe);
					return -1;
				}
				cliente = conectar_a_memoria(memoria);

				if(cliente < 0){
					log_info(logger, "PLANIFIC| CREATE - NO PUDO CONECTARSE A MEM.");
					remover_memoria(memoria);
					if(request->parametro1) free(request->parametro1);
					if(request->parametro2) free(request->parametro2);
					if(request->parametro3) free(request->parametro3);
					if(request->parametro4) free(request->parametro4);
					if(request) free(request);
//					close(cliente);
					free(req_describe);
					return -1;
				}


				describe_global(cliente);
				resultado = 0;
				close(cliente);

			} else {
				log_info(logger, "PLANIFIC| DESCRIBE: %s", req_describe->nombreTabla);
				log_info(logger, "PLANIFIC| DESCRIBE: Describe %s.", request->parametro1);
				log_info(logger, "PLANIFIC| DESCRIBE: Parámetro 1: %s", request->parametro1);
				strcpy(req_describe->nombreTabla, request->parametro1);

				//OBTENGO UNA MEMORIA DE ACUERDO A LA TABLA
				tabla = buscar_tabla(request->parametro1);
				if(tabla == NULL) {
					log_info(logger, "PLANIFIC| DESCRIBE: La tabla no existe.");
					if(request->parametro1) free(request->parametro1);
					if(request->parametro2) free(request->parametro2);
					if(request->parametro3) free(request->parametro3);
					if(request->parametro4) free(request->parametro4);
					if(request) free(request);
					free(req_describe);
					return -1;
				}
				int keyCualquiera=rand();
				memoria = get_memoria_por_criterio(tabla->tipoConsistencia, keyCualquiera);

				if (memoria->numeroMemoria <0)
				{
					log_info(logger, "PLANIFIC| DESCRIBE: MEMORIA: %d",memoria->numeroMemoria);
					free (memoria);
					free(tabla);
					if(request->parametro1) free(request->parametro1);
					if(request->parametro2) free(request->parametro2);
					if(request->parametro3) free(request->parametro3);
					if(request->parametro4) free(request->parametro4);
					if(request) free(request);
					free(req_describe);
					return -1;
				}
				log_info(logger, "PLANIFIC| DESCRIBE:  MEMORIA ASIGNADA: %d",memoria->numeroMemoria);
				cliente = conectar_a_memoria(memoria);

				if(cliente < 0){
					log_info(logger, "PLANIFIC| INSERT POST-JOURNAL - NO PUDO CONECTARSE A MEM.");
					remover_memoria(memoria);
					free(tabla);
					if(request->parametro1) free(request->parametro1);
					if(request->parametro2) free(request->parametro2);
					if(request->parametro3) free(request->parametro3);
					if(request->parametro4) free(request->parametro4);
					if(request) free(request);
//					close(cliente);
					free(req_describe);
					return -1;
				}


				log_info(logger, "PLANIFIC| DESCRIBE: Enviando a MEMORIA");
				int resultado_mensaje_describe = enviarMensaje(kernel, describe, sizeof(t_describe), req_describe, cliente, logger, mem);
				log_info(logger, "PLANIFIC| DESCRIBE: Resultado de enviar mensaje: %d", resultado_mensaje_describe);
				//TODO: VER SI NO PUEDE MANDAR NADA A MEMORIA
				/*if(resultado_mensaje_describe <1)
				{
					log_info(logger, "PLANIFIC| CREATE - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
					free (memoria);
					free(tabla);
					return -1;
				}*/

				t_mensaje* msg_describe = recibirMensaje(cliente, logger);
				
				if(msg_describe == NULL)
				{
					log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
					remover_memoria(memoria);
//					free (memoria);
					free(tabla);
					if(request->parametro1) free(request->parametro1);
					if(request->parametro2) free(request->parametro2);
					if(request->parametro3) free(request->parametro3);
					if(request->parametro4) free(request->parametro4);
					if(request) free(request);
					close(cliente);
					free(req_describe);
					return 0;
				}
				
				char* buffer_describe= string_new();
				string_append(&buffer_describe, msg_describe->content);

				log_info(logger, "PLANIFIC| DESCRIBE: RESULTADO METADATA TABLA %s| Metadata: %s",req_describe->nombreTabla, buffer_describe);
				resultado = msg_describe->header.error;
				//guardar_metadata(buffer_describe);
//				free (memoria);
				free(tabla);
				destruirMensaje(msg_describe);
				free(buffer_describe);
				close(cliente);
			}



			free(req_describe);

			break;

		case _drop:
			// DROP [NOMBRE_TABLA]
			// DROP TABLA1

			tabla = buscar_tabla(request->parametro1);

			if(tabla == NULL) {
				log_info(logger, "PLANIFIC| DROP: La tabla no existe.");
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				return -1;
			}

			log_info(logger, "PLANIFIC| DROP: Preparando...");
			t_drop* req_drop = malloc(sizeof(t_drop));
			memset(req_drop, 0x00, sizeof(t_drop));

			req_drop->id_proceso = id_proceso;
			strcpy(req_drop->nombreTabla, request->parametro1);

			log_info(logger, "PLANIFIC| DROP: OK. %s", req_drop->nombreTabla);

			//SELECCIONO EL CRITERIO QUE LE CORRESPONDE POR LA TABLA, le paso cualquier KEY
			int keyCualquiera=rand();
			memoria = get_memoria_por_criterio(tabla->tipoConsistencia, keyCualquiera);
			//memoria = obtener_memoria_random(); // cambiar para hacerlo dinamico para los criterios

			if (memoria->numeroMemoria <0)
			{
				log_info(logger, "PLANIFIC| DROP: MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				free(req_drop);
				return -1;
			}
			log_info(logger, "PLANIFIC| DROP: MEMORIA ASIGNADA: %d",memoria->numeroMemoria);
			cliente = conectar_a_memoria(memoria);

			if(cliente < 0){
				log_info(logger, "PLANIFIC| INSERT POST-JOURNAL - NO PUDO CONECTARSE A MEM.");
				remover_memoria(memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
//				close(cliente);
				free(req_drop);
				return -1;
			}

			log_info(logger, "PLANIFIC| DROP: Enviando a MEMORIA");
			int resultado_mensaje_drop = enviarMensajeConError(kernel, drop, sizeof(t_drop), req_drop, cliente, logger, mem, 0);
			log_info(logger, "PLANIFIC| DROP: Resultado de enviar mensaje: %d", resultado_mensaje_drop);

			//TODO: VER SI NO PUEDE MANDAR NADA A MEMORIA
			/*if(resultado_mensaje_drop <1)
			{
				log_info(logger, "PLANIFIC| CREATE - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				free (memoria);
				free(tabla);
				return -1;
			}*/

			t_mensaje* resultado_req_drop = recibirMensaje(cliente, logger);
			
			if(resultado_req_drop == NULL)
			{
				log_info(logger, "PLANIFIC| SELECT - NO SE PUDO CONECTAR MEMORIA: %d",memoria->numeroMemoria);
				remover_memoria(memoria);
//				free (memoria);
				free(tabla);
				if(request->parametro1) free(request->parametro1);
				if(request->parametro2) free(request->parametro2);
				if(request->parametro3) free(request->parametro3);
				if(request->parametro4) free(request->parametro4);
				if(request) free(request);
				close(cliente);
				free(req_drop);
				return 0;
			}
			
			resultado = resultado_req_drop->header.error;
			log_info(logger, "PLANIFIC| DROP: Resultado: %d", resultado);


			if (resultado == 0) {
				log_info(logger, "PLANIFIC| DROP: Solicitando describe_global");
				//LOS MUTEX SE HACEN CUANDO HACE EL LIST_CLEAN
//				pthread_mutex_lock(&mutex_metadata);
//				limpiar_metadata();
				describe_global(cliente);
//				pthread_mutex_unlock(&mutex_metadata);
			}

			resultado = resultado_req_drop->header.error; // Cambiar por lo que devuelve la memoria.

//			free (memoria);
			free(tabla);
			free(req_drop);
			destruirMensaje(resultado_req_drop);
			close(cliente);

			break;

		default:
			// No entra por acá porque se valida antes el enum != -1
			log_info(logger, "PLANIFIC|No se reconoce operación: %s", request->request);
			break;
	}

	log_info(logger, "PLANIFIC| ***** RESULTADO: %d *****", resultado);

	if(request->parametro1) free(request->parametro1);
	if(request->parametro2) free(request->parametro2);
	if(request->parametro3) free(request->parametro3);
	if(request->parametro4) free(request->parametro4);
	if(request) free(request);

	return resultado;
}


void* aplicar_algoritmo_rr() {
	pthread_mutex_lock(&mutex_ready);
	t_pcb* pcb = sacar_proceso_rr(lista_ready);
	pthread_mutex_unlock(&mutex_ready);

	if (pcb == NULL) {
		log_info(logger, "PLANIFIC| No se encontró el PCB");
	} else {
		log_info(logger, "PLANIFIC| Proceso %d removido", pcb->id_proceso);
	}

	pthread_mutex_lock(&mutex_exec);
	agregar_proceso(pcb, lista_exec, &sem_exec);
	pthread_mutex_unlock(&mutex_exec);

	imprimir_listas(); // SACAR

	log_info(logger, "PLANIFIC| Proceso %d pasa a EXEC", pcb->id_proceso);

//	int resultado = procesar_pcb(pcb);
	procesar_pcb(pcb);
	log_info(logger, "PLANIFIC| Fin hilo planificación proceso N° %d", pcb->id_proceso);

	imprimir_listas();
//	return NULL;
//	return (void*)EXIT_SUCCESS;
//	return (void*)resultado;

	pthread_exit(NULL);
//	return NULL;
}

void imprimir_listas() {

	log_info(logger, "LISTA NEW: %d", lista_new->elements_count);
	log_info(logger, "LISTA READY: %d", lista_ready->elements_count);
	log_info(logger, "LISTA EXEC: %d", lista_exec->elements_count);
	log_info(logger, "LISTA EXIT: %d", lista_exit->elements_count);
}

int calcular_quantum(t_pcb* pcb) {

	int quantum = kernel_conf.quantum;
	int requests_restantes = pcb->cantidad_request - pcb->program_counter;
//	log_info(logger, "PLANIFIC| Request restantes: %d", requests_restantes);

	int quantum_restante = (pcb->program_counter % quantum) == 0 ? quantum: quantum - (pcb->program_counter % quantum);

	if(requests_restantes < quantum_restante) {
		quantum_restante = requests_restantes;
	}

	log_info(logger, "PLANIFIC| Quantum disponible: %d", quantum_restante);

	return quantum_restante;

}
