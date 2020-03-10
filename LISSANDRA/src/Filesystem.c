/*
 * Filesystem.c
 *
 *  Created on: 1 may. 2019
 *      Author: utnso
 */

#include "Filesystem.h"

void *CrearFileSystem() {
	loggear(logger, LOG_LEVEL_INFO, "Creando fileSystem");

	rutaTablas = string_new();
	string_append(&rutaTablas, lfs_conf.punto_montaje);
	log_info(logger, "Punto montaje %s", lfs_conf.punto_montaje);
	string_append(&rutaTablas, "Tables/");
	CrearDirectorio(rutaTablas);
	log_debug(logger, "Ruta de tablas: %s", rutaTablas);
	rutaBloques = string_new();
	string_append(&rutaBloques, lfs_conf.punto_montaje);
	string_append(&rutaBloques, "Bloques/");
	CrearDirectorio(rutaBloques);
	log_debug(logger, "Ruta de bloques: %s", rutaBloques);
	CargarMetadata();
	CargarBitmap();
	CargarTablas();
	IniciarBloques();

	return (void*) 1;
}

void CargarMetadata() {
	rutaMetadata = string_new();
	string_append(&rutaMetadata, lfs_conf.punto_montaje);
	string_append(&rutaMetadata, "Metadata/");
	CrearDirectorio(rutaMetadata);
	string_append(&rutaMetadata, "Metadata.bin");

	log_debug(logger, "Ruta Metadata: %s", rutaMetadata);

	config_metadata = cargarConfiguracion(rutaMetadata, logger);

	if (config_has_property(config_metadata, "BLOCK_SIZE")) {
		tamanio_bloques = config_get_int_value(config_metadata, "BLOCK_SIZE");
		loggear(logger, LOG_LEVEL_INFO, "Tamanio de bloques: %d",
				tamanio_bloques);
	}

	if (config_has_property(config_metadata, "BLOCKS")) {
		cantidad_bloques = config_get_int_value(config_metadata, "BLOCKS");
		loggear(logger, LOG_LEVEL_INFO, "Cantidad de bloques: %d",
				cantidad_bloques);
	}

	config_destroy(config_metadata);

}

void CargarBitmap() {
	rutaBitmap = string_new();
	string_append(&rutaBitmap, lfs_conf.punto_montaje);
	string_append(&rutaBitmap, "Metadata/");
	CrearDirectorio(rutaBitmap);
	string_append(&rutaBitmap, "Bitmap.bin");

	log_debug(logger, "Ruta Bitmap: %s", rutaBitmap);
//		verifico si abre bitmap en readonly
	int bm = open(rutaBitmap, O_RDONLY);
	if (bm == -1) {
//		si no esta creo el archivo
		log_debug(logger, "Generando Bitmap.bin");
		FILE *file = fopen(rutaBitmap, "wb");
		fclose(file);
	}
	if(bm != -1) close(bm);
//  lo abro en modo WR
	bm = open(rutaBitmap, O_RDWR);
	ftruncate(bm, (cantidad_bloques / 8) + 1);
	bmap = mmap(NULL, cantidad_bloques / 8, PROT_WRITE | PROT_READ, MAP_SHARED,
			bm, 0);
	bitmap = bitarray_create_with_mode(bmap, cantidad_bloques / 8, MSB_FIRST);

	msync(bmap, bm, MS_SYNC);
	struct stat mystat;
	if (fstat(bm, &mystat) < 0) {
		loggear(logger, LOG_LEVEL_ERROR, "Error al establecer fstat");
	}
	fstat(bm, &mystat);

	loggear(logger, LOG_LEVEL_INFO, "Bitmap generado");
	close(bm);
	free(rutaBitmap);
}

int ExisteTabla(const char *tabla) {
	loggear(logger, LOG_LEVEL_INFO, "Validando tabla: %s", tabla);

	char *ruta_tabla = string_from_format("%s%s", rutaTablas, tabla);
	log_info(logger, "Ruta tabla: %s", ruta_tabla);
	FILE *fp = fopen(ruta_tabla, "r");
	free(ruta_tabla);

	if (fp) {
		fclose(fp);
		loggear(logger, LOG_LEVEL_WARNING,
				"La tabla %s existe en el FileSystem", tabla);
		return 1;
	} else {
		return 0;
	}
}

t_metadata* ObtenerMetadataTabla(char *tabla) {
	loggear(logger, LOG_LEVEL_INFO, "Obteniendo metadata de tabla: %s", tabla);

	t_metadata *metadata = malloc(sizeof(t_metadata));
	char *rutaMetadata = string_from_format("%s%s/Metadata", rutaTablas, tabla);
	loggear(logger, LOG_LEVEL_INFO, "Obteniendo metadata en ruta %s",
			rutaMetadata);

	t_config* config = cargarConfiguracion(rutaMetadata, logger);
	metadata->particiones = config_get_int_value(config, "PARTITIONS");
	metadata->compactationTime = config_get_int_value(config,
			"COMPACTATION_TIME");
	char *consistencia = string_new();
	string_append(&consistencia, config_get_string_value(config, "CONSISTENCY"));
	strcpy(metadata->tipoConsistencia, consistencia);
	free(rutaMetadata);
	free(consistencia);

	config_destroy(config);
	return metadata;
}

void ObtenerMetadataCompleto() {

	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(rutaTablas)) == NULL) {
		perror("openndir() error");
	} else {
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

			} else {
				printf("Metadata tabla %s:\n", entry->d_name);
				t_metadata *metadata;
				metadata = ObtenerMetadataTabla(entry->d_name);
				int particiones = metadata->particiones;
				int tiempoCompactacion = metadata->compactationTime;
				char *consistencia = string_new();
				string_append(&consistencia, metadata->tipoConsistencia);
//				printf("CONSISTENCY=%s\n", consistencia);
//				printf("PARTITIONS=%d\n", particiones);
//				printf("COMPACTATION_TIME=%d\n", tiempoCompactacion);
				printf("Consistency\tPartitions\tCompactionTime\n");
				printf("%s\t\t%d\t\t%d\n\n", consistencia, particiones, tiempoCompactacion);
				free(consistencia);
				free(metadata);
			}
		}

		closedir(dir);
	}
}

int CalcularParticion(int clave, int particiones) {
	int particion = clave % particiones;
	return particion;
}

void CrearDirectorioTabla(char *tabla) {
	char *rutaTabla = string_from_format("%s%s", rutaTablas, tabla);

	if (mkdir(rutaTabla, 0777) == -1) {
		//perror("mkdir");
		loggear(logger, LOG_LEVEL_ERROR, "Error creando directorio para: %s",
				tabla);
	} else {
		loggear(logger, LOG_LEVEL_INFO, "Directorio de %s creado", tabla);
	}

	free(rutaTabla);
}

void CrearMetadataTabla(char *tabla, char *consistencia, int particiones,
		int tiempoCompactacion) {
	loggear(logger, LOG_LEVEL_INFO, "Creando metadata para: %s", tabla);
	char *rutaMetadataTabla = string_from_format("%s%s/Metadata", rutaTablas,
			tabla);

	FILE *file = fopen(rutaMetadataTabla, "w");

	char *tipoConsistencia = string_from_format("CONSISTENCY=%s\n",
			consistencia);
	fputs(tipoConsistencia, file);
	char *partitions = string_from_format("PARTITIONS=%d\n", particiones);
	fputs(partitions, file);
	char *compactationTime = string_from_format("COMPACTATION_TIME=%d\n",
			tiempoCompactacion);
	fputs(compactationTime, file);

	free(tipoConsistencia);
	free(partitions);
	free(compactationTime);
	free(rutaMetadataTabla);
	fclose(file);
}

t_tabla* BuscarTablaMemtable(char *nombre) {
	bool findTable(void* element) {
		t_tabla* tabla = element;
		return string_equals_ignore_case(tabla->nombre_tabla, nombre);
	}


	loggear(logger, LOG_LEVEL_INFO, "Buscando %s en Memtable", nombre);
	return list_find(memtable, &findTable);
}

void AlocarTabla(char *tabla, t_registro *registro) {
	t_tabla *listaTabla = malloc(sizeof(t_tabla));
	char *nombre = string_new();
	string_append(&nombre, tabla);
	strcpy(listaTabla->nombre_tabla, nombre);
	listaTabla->lista = list_create();
	list_add(memtable, listaTabla);
	list_add(listaTabla->lista, registro);
	int a = list_size(memtable);
//	printf("Tamanio memtable :%d\n", a);
	free(nombre);

}

int InsertarTabla(t_insert *insert) {


	char *nombre_tabla = string_new();
	string_append(&nombre_tabla, insert->nombreTabla);
	//valido value enviado
	if((strlen(insert->value)) > (lfs_conf.tamano_value)){
		//free(insert);
		free(nombre_tabla);
		return 1;
	}

	//Verifico existencia en el file system
	if (!ExisteTabla(nombre_tabla)) {
		loggear(logger, LOG_LEVEL_WARNING, "%s no existe en el file system",
				nombre_tabla);
		//free(insert);
		free(nombre_tabla);
		return 1;
	}

	//Valido tabla bloqueada
//	int bloqueado = GetEstadoTabla(nombre_tabla);
//	if(bloqueado){
//		//free(insert);
//		free(nombre_tabla);
//		return -1;
//	}

	//Verifico si no tiene datos a dumpear
	t_tabla *tabla;
	tabla = BuscarTablaMemtable(nombre_tabla);

	t_registro *registro = malloc(sizeof(t_registro));
	registro->key = insert->key;
	strcpy(registro->value, insert->value);
	registro->timestamp = insert->timestamp;

	if (tabla == NULL) {
		//Aloco en memtable como nueva tabla
		loggear(logger, LOG_LEVEL_INFO, "%s no posee datos a dumpear",
				nombre_tabla);
		AlocarTabla(nombre_tabla, registro);
	} else {
		//Alocar en su posicion
		loggear(logger, LOG_LEVEL_INFO, "Alocando en su pos correspondiente");
		list_add(tabla->lista, registro);
	}
	free(nombre_tabla);
	return 0;
}

void CrearBloque(int numero, int bytes) {
	//loggear(logger, LOG_LEVEL_INFO, "Creando bloque %d.bin", numero);
	char *rutaBloque = string_from_format("%s%d.bin", rutaBloques, numero);

	FILE *binFile = fopen(rutaBloque, "r");
	if (binFile) {
		//log_debug(logger, "El bloque %s ya se encuentra creado", rutaBloque);
		fclose(binFile);
		free(rutaBloque);
	} else {
		FILE *bloque = fopen(rutaBloque, "w");
		char *bytesAEscribir = malloc(bytes);
		//memset(bytesAEscribir, '\n', bytes);
		fwrite(bytesAEscribir, bytes, 0, binFile);
		free(rutaBloque);
		free(bytesAEscribir);
		fflush(binFile);
		fclose(bloque);
	}

}

void GuardarEnBloque(char *linea, char *path) {
	loggear(logger, LOG_LEVEL_INFO, "Guardando linea :%s en: %s", linea, path);
	FILE *file = fopen(path, "a+");
	fseek(file, 0L, SEEK_END);
	//int len = ftell(file);
	//fseek(file, len, SEEK_SET);

	fwrite(linea, 1, strlen(linea), file);
	free(linea);
	fclose(file);
}

t_registro* BuscarKey(t_select *selectMsg) {

	//Verifico existencia en el file system
	if (!ExisteTabla(selectMsg->nombreTabla)) {
		t_registro *registroInit = malloc(sizeof(t_registro));
		loggear(logger, LOG_LEVEL_ERROR, "%s no existe en el file system",
				selectMsg->nombreTabla);
		registroInit->key = -1;
		return registroInit;
	}

//	int bloqueado = GetEstadoTabla(selectMsg->nombreTabla);
//	if(bloqueado){
//		t_registro *registroInit = malloc(sizeof(t_registro));
//		registroInit->key = -1;
//		return registroInit;
//	}

	//Inicializo lista donde se concatenaran las restantes
	t_list *listaBusqueda = list_create();
	int size_busqueda;

	//Obtengo metadata
	t_metadata *metadata = ObtenerMetadataTabla(selectMsg->nombreTabla);
	int particiones = metadata->particiones;
	free(metadata);

	//Calculo particion de la key
	int particion = CalcularParticion(selectMsg->key, particiones);

	//Obtengo bloques de la particion
	char *rutaParticion = string_from_format("%s%s/part%d.bin", rutaTablas,
			selectMsg->nombreTabla, particion);
	loggear(logger, LOG_LEVEL_INFO, "configFile %s", rutaParticion);
	t_config *configFile = cargarConfiguracion(rutaParticion, logger);

	int sizeArchivo = config_get_int_value(configFile, "SIZE");
	if (sizeArchivo > 0) { //Escaneo la particion
//		int cantBloques = CalcularBloques(sizeArchivo);
		//char **blocksArray = malloc(sizeof(int) * cantBloques);
		char **blocksArray = config_get_array_value(configFile, "BLOCKS");
		int j = 0;
		while (blocksArray[j] != NULL) {

			t_registro *registro = BuscarKeyParticion(selectMsg->key,
					blocksArray[j]);
			//Si se encontro en particion agrego a la lista de busqueda
			if (registro->timestamp != 0) {
				log_info(logger, "Registro encontrado en particion");
				list_add(listaBusqueda, registro);
			}
			else
				free(registro);
			free(blocksArray[j]);
			j++;
		}
		free(blocksArray);
	}
	free(rutaParticion);
	config_destroy(configFile);

	//Escaneo memtable
	//t_list *listaMemtable = list_create();
	t_list *listaMemtable = BuscarKeyMemtable(selectMsg->key, selectMsg->nombreTabla);
	if (list_is_empty(listaMemtable)) {
		log_debug(logger, "No se encontraron registros en memtable");
		list_destroy(listaMemtable);
	} else {
		log_debug(logger, "Registros encontrados en memtable");
		int sizeMemtable = list_size(listaMemtable);
		for(int i=0; i<sizeMemtable;i++){
			t_registro *reg = list_get(listaMemtable,0);
			t_registro *regCopiar = malloc(sizeof(t_registro));
			memcpy(regCopiar,reg,sizeof(t_registro));
			list_add(listaBusqueda,regCopiar);
			free(list_remove(listaMemtable,0));
		}
		list_destroy(listaMemtable);
	}

	//Escaneo temporales
	t_list *listaTemp;
	listaTemp = BuscarKeyTemporales(selectMsg->key, selectMsg->nombreTabla);

	if (list_is_empty(listaTemp)) {
		log_debug(logger, "No se encontraron registro en .tmp");
		list_destroy(listaTemp);
	} else {
		log_debug(logger, "Registros encontrados en .tmp");
		int sizeTemp = list_size(listaTemp);
		for(int i=0; i<sizeTemp;i++){
			t_registro *reg = list_get(listaTemp,0);
			t_registro *regCopiar = malloc(sizeof(t_registro));
			memcpy(regCopiar,reg,sizeof(t_registro));
			list_add(listaBusqueda,regCopiar);
			free(list_remove(listaTemp,0));
		}
		list_destroy(listaTemp);
	}

	size_busqueda = list_size(listaBusqueda);

	//si la lista esta vacia devuelvo registro con key -1
	if(list_is_empty(listaBusqueda)) {
			t_registro *registroInit = malloc(sizeof(t_registro));
			registroInit->key = -1;
			return registroInit;
		}


	//Busco registro con mayor timestamp
	t_registro *registroInit = list_get(listaBusqueda, 0);
	t_registro *registroAux;

	if (1 < size_busqueda) {
		for (int i = 0; i < size_busqueda ; i++) {
			registroAux = list_get(listaBusqueda, i);
//			log_info(logger, "Elemento %d tiene value %s y timestamp %llu", i,
//					registroAux->value, registroAux->timestamp);

			if (registroInit->timestamp < registroAux->timestamp)
				registroInit = registroAux;
		}
	}


	log_info(logger, "Devolviendo registro key %d value %s timestamp %llu",
			registroInit->key, registroInit->value, registroInit->timestamp);

	t_registro *registroOut = malloc(sizeof(t_registro));
	memcpy(registroOut,registroInit,sizeof(t_registro));

	//libero lista busqueda
	log_info(logger, "Size lista busqueda %d", size_busqueda);
	if (listaBusqueda != NULL) {
		size_busqueda = list_size(listaBusqueda);
		for (int i = 0; size_busqueda > i; i++) {
//			log_info(logger, "Remove busqueda %d", i);
			free(list_remove(listaBusqueda, 0));
		}
		size_busqueda = list_size(listaBusqueda);
//		log_info(logger, "Size busqueda %d", size_busqueda);

	}
	list_destroy(listaBusqueda);

	//free(selectMsg);

	return registroOut;
}

t_list *BuscarKeyMemtable(int key, char *nombre) {
	loggear(logger, LOG_LEVEL_INFO, "Buscando key:%d en memtable de: %s", key,
			nombre);

	t_tabla *tabla;
	tabla = BuscarTablaMemtable(nombre);

	if (tabla == NULL) {
		loggear(logger, LOG_LEVEL_WARNING,
				"La tabla %s no posee datos en memtable", nombre);
		return list_create();
	}

	int findKey(t_registro *registro) {
		return (registro->key == key);
	}

	t_list *listaFiltrado = list_filter(tabla->lista, (void*) findKey);
	t_list *listaEncontrados = list_create();
	for(int i=0; list_size(listaFiltrado)>i;i++){
		t_registro *regEncontrado = list_get(listaFiltrado,i);
		t_registro *reg = malloc(sizeof(t_registro));
		memcpy(reg,regEncontrado,sizeof(t_registro));
		list_add(listaEncontrados,reg);
	}
	list_destroy(listaFiltrado);
	return listaEncontrados;

}

t_list *BuscarKeyTemporales(int key, char *tabla) {
	t_list *listaTmp = list_create();
	char *pathTemps = string_from_format("%s%s", rutaTablas, tabla);
	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(pathTemps)) == NULL) {
		perror("openndir() error");
	}

	t_list *tempBlocksCollection = list_create();

	//busco todos los bloques de los tmp
//	pthread_mutex_lock(&mutex_temp);
	while ((entry = readdir(dir)) != NULL) {
		if (string_ends_with(entry->d_name, ".tmp")) {

			char *pathFile = string_from_format("%s/%s", pathTemps,
					entry->d_name);
			log_info(logger, "Buscando key %d en %s", key, pathFile);
			t_config *config_file = cargarConfiguracion(pathFile, logger);
			int size = config_get_int_value(config_file, "SIZE");
			int cantBloques = CalcularBloques(size);
			int recorrido = 0;


			char **bloques = config_get_array_value(config_file, "BLOCKS");
			config_destroy(config_file);
			while(bloques[recorrido] != NULL){
				recorrido ++;
			}

			for (int i = 0; recorrido > i; i++) {
				list_add(tempBlocksCollection, (int*)atoi(bloques[i]));
				free(bloques[i]);
			}

			free(pathFile);
			free(bloques);
		}
	}

	closedir(dir);
//	pthread_mutex_unlock(&mutex_temp);

	//leo bloque por bloque y agrego registro si es la key buscada
	int lenghtCollection = list_size(tempBlocksCollection);
	for (int j = 0; lenghtCollection > j; j++) {
		int block;
		block = (int)list_get(tempBlocksCollection, j);
		char *pathBlock = string_from_format("%s%d.bin", rutaBloques, block);
		log_info(logger, "Leyendo bloque: %s", pathBlock);

		//leer bloque y hacer add en listaTmp si es la key buscada
		char linea[100];
		char **elementos;
		FILE *file = fopen(pathBlock, "r");

		while (!feof(file))

		{

//			fgets(linea, 100, file);
			char *rd = fgets(linea,100, file);
			if(rd!=NULL){
			elementos = string_split(linea, ";");
			int cantElementos = ContarElementosArray(elementos);

			if (atoi(elementos[1]) == key) {
				t_registro *registro = malloc(sizeof(t_registro));
				registro->timestamp = atoll(elementos[0]);
				registro->key = atoi(elementos[1]);
				char *value = string_new();
				string_append(&value, elementos[2]);
				value[strcspn(value, "\n")] = 0;
				strcpy(registro->value, value);
				free(value);
				list_add(listaTmp, registro);
				log_info(logger, "Elemento guardado con value %s",
						registro->value);
			}

			for (int i = 0; i < cantElementos; i++) {
				free(elementos[i]);
			}
			free(elementos);
			}
		}
		fclose(file);
		free(pathBlock);
	}

	int sizeTemp = list_size(tempBlocksCollection);
	for(int i=0; i<sizeTemp;i++){
		list_remove(tempBlocksCollection,0);
	}
	list_destroy(tempBlocksCollection);

	free(pathTemps);
	return listaTmp;
}

t_registro* BuscarKeyParticion(int key, char *bloque) {
	t_registro *registro = malloc(sizeof(t_registro));
	loggear(logger, LOG_LEVEL_INFO, "Buscando key : %d en bloque: %s", key,
			bloque);
	char *pathBlock = string_from_format("%s%s.bin", rutaBloques, bloque);
	loggear(logger, LOG_LEVEL_INFO, "Ruta bloque : %s", pathBlock);
	char linea[100];
	char **elementos;

	FILE *file = fopen(pathBlock, "r");


	if (file == NULL) {
		loggear(logger, LOG_LEVEL_ERROR, "Error abriendo archivo %s", file);
	}


	loggear(logger, LOG_LEVEL_INFO, "Archivo %s abierto correctamente",
			pathBlock);
//	t_registro *registro = malloc(sizeof(t_registro));
	fseek(file, 0, SEEK_END); // seek to end of file
	int size = ftell(file); // get current file pointer
	fseek(file, 0, SEEK_SET);
	if(size == 0){
		registro->timestamp = 0;
		free(pathBlock);
		fclose(file);
		return registro;
	}
	while (!feof(file))

	{
//		fgets(linea, 100, file);
		char * rd = fgets(linea,100,file);
		if(rd!=NULL){
			elementos = string_split(linea, ";");
			int cantElementos = ContarElementosArray(elementos);

			if (atoi(elementos[1]) == key) {
				registro->timestamp = atoll(elementos[0]);
				registro->key = atoi(elementos[1]);
				char *value = string_new();
				string_append(&value, elementos[2]);
				value[strcspn(value, "\n")] = 0;
				strcpy(registro->value, value);

				loggear(logger, LOG_LEVEL_INFO, "Timestamp:%llu",
						registro->timestamp);
				loggear(logger, LOG_LEVEL_INFO, "Key:%d", registro->key);
				loggear(logger, LOG_LEVEL_INFO, "Value:%s", registro->value);

				for (int i = 0; i < cantElementos; i++) {
					free(elementos[i]);
				}
				free(value);
				free(elementos);
				free(pathBlock);
				fclose(file);
				return registro;
			} else {
				registro->timestamp = 0;
			}

			for (int i = 0; i < cantElementos; i++) {
				free(elementos[i]);
			}
			free(elementos);
		}
	}

	free(pathBlock);
	fclose(file);
	return registro;
}

int ContarElementosArray(char **cadena) {
	int contador = 0;

	while (cadena[contador] != '\0') {
		contador++;
	}

	return contador;
}

int CrearTabla(t_create *msgCreate) {

	//Verifico existencia en el file system
	if (ExisteTabla(msgCreate->nombreTabla)) {
		loggear(logger, LOG_LEVEL_ERROR, "%s ya existe en el file system",
				msgCreate->nombreTabla);
		return -1;
	}

	AddGlobalList(msgCreate->nombreTabla);
	int particiones = msgCreate->num_part;

	//Creo Directorio
	CrearDirectorioTabla(msgCreate->nombreTabla);

	//Creo archivo metadata
	CrearMetadataTabla(msgCreate->nombreTabla, msgCreate->tipo_cons,
			particiones, msgCreate->comp_time);

	//Creo archivo binarios
	int particionInicial = 0;
	for (int i = 0; i < particiones; i++) {

		char *rutaParticion = string_from_format("%s%s/%s%d.bin", rutaTablas,
				msgCreate->nombreTabla, "part", particionInicial);
		log_debug(logger, "Creando particion %s", rutaParticion);
		FILE *file = fopen(rutaParticion, "w");

		if (file == NULL) {
			//loggear(logger, LOG_LEVEL_ERROR, "Error abriendo archivo %s", file);
			printf("Error abriendo archivo %s\n", rutaParticion);
		}

		char *stringSize = string_from_format("SIZE=%d\n", 0);
		fputs(stringSize, file);
		int bloque = AgregarBloque();
		char *stringBlocks = string_from_format("BLOCKS=[%d]\n", bloque);
		fputs(stringBlocks, file);
		fclose(file);
		free(stringSize);
		free(stringBlocks);
		free(rutaParticion);
		particionInicial++;
	}

	//CreoHiloCompactacion
	int idHilo = crearHiloCompactacion(msgCreate->comp_time, msgCreate->nombreTabla);
	t_datos_hilo_compactacion* compactacion = malloc(sizeof(t_datos_hilo_compactacion));
	compactacion->hilo = idHilo;
	strcpy(compactacion->path_tabla,msgCreate->nombreTabla);
	list_add(listaHilos,compactacion);
	return 0;
}

int AgregarBloque() {
	pthread_mutex_lock(&mutex_bitmap);
	//Descomentar cuando se encuentre el bitmap del fs disponible
	size_t sizeBitmap = bitarray_get_max_bit(bitmap);
	 int bloque = 1;

	 log_info(logger, "Agregando bloque");

	 int count = 0;

	 while(count < sizeBitmap && bloque != -1)
	 {

	 if(bitarray_test_bit(bitmap, count ) == 0)
	 {
	 bitarray_set_bit(bitmap, count);
	 log_info(logger,"Bloque %d asignado", count);
	 bloque = count;
	 pthread_mutex_unlock(&mutex_bitmap);
	 return bloque;
	 }
	 count ++;
	 }
	 pthread_mutex_unlock(&mutex_bitmap);
	return bloque;
//	return 1;
}

int DropearTabla(char *nombre) {
	//Verifico existencia en el file system
	if (!ExisteTabla(nombre)) {
		loggear(logger, LOG_LEVEL_ERROR, "%s no puede ser dropeada", nombre);
		return 1;
	}

	int bloqueado = GetEstadoTabla(nombre);
	if(bloqueado) return -1;

	DIR *dir;
	struct dirent *entry;

	char *path = string_from_format("%s%s", rutaTablas, nombre);
	char *pathMetadata = string_from_format("%s/Metadata", path);

	if ((dir = opendir(path)) == NULL) {
		perror("openndir() error");
	} else {
		while ((entry = readdir(dir)) != NULL) {
			if (string_ends_with(entry->d_name, ".tmp")
					|| string_ends_with(entry->d_name, ".bin")) {
				char *pathFile = string_from_format("%s/%s", path,
						entry->d_name);

				log_debug(logger, "Liberando %s", pathFile);
				t_config *config_file = cargarConfiguracion(pathFile, logger);

				int size = config_get_int_value(config_file, "SIZE");
				int cantBloques;
				cantBloques = (size != 0) ? CalcularBloques(size) : 1;
				//char **bloques = malloc(sizeof(int) * cantBloques);
				char **bloques = config_get_array_value(config_file, "BLOCKS");

				LiberarBloques(bloques, cantBloques);
				remove(pathFile);
				free(pathFile);

				for (int i = 0; i <= cantBloques; i++) {
					free(bloques[i]);
				}
				free(bloques);
				config_destroy(config_file);
			}
		}
	}

	for(int i = 0; i<listaHilos->elements_count;i++){
		t_datos_hilo_compactacion *compact = list_get(listaHilos,i);
		if(string_equals_ignore_case(compact->path_tabla,nombre)){
			log_info(logger,"Eliminando hilo %s(%d) vs %s",compact->path_tabla,compact->hilo,nombre);
			pthread_cancel(compact->hilo);
			free(list_remove(listaHilos,i));
			break;
		}
	}

	remove(pathMetadata);
	remove(path);
	free(pathMetadata);
	free(path);
	RemoveGlobalList(nombre);
	closedir(dir);
	return 0;
}

int CalcularBloques(int bytes) {
	int count;
	int aux = (bytes / tamanio_bloques);
	count = (bytes % tamanio_bloques == 0) ? aux : (aux + 1);

	return count;
}

void LiberarBloques(char **bloques, int cantBloques) {
	for (int i = 0; i < cantBloques; i++) {
		//printf("Eliminando bloque %d\n", atoi(bloques[i]));
		int pos = atoi(bloques[i]);
		bitarray_clean_bit(bitmap, pos);
		char* ruta = string_new();
		string_append_with_format(&ruta,"%s/%d.bin",rutaBloques,pos);
		FILE *file = fopen(ruta,"w");
		fclose(file);
		free(ruta);
	}
}

void LiberarMetadata(char **bloques, int cant) {
	loggear(logger, LOG_LEVEL_INFO, "Eliminando metadata asociada");
	for (int i = 0; i < cant; i++) {
		char *pathMeta = string_from_format("%s%d.bin", rutaBloques,
				atoi(bloques[i]));
		loggear(logger, LOG_LEVEL_INFO, "Eliminando %s", pathMeta);
		remove(pathMeta);
		free(pathMeta);

	}
}

void LevantarHilosCompactacionFS() {
	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(rutaTablas)) == NULL) {
		perror("openndir() error");
	} else {
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

			} else {
				//printf("%s\n", entry->d_name);
				char *metadataFile = string_from_format("%s%s/Metadata",
						rutaTablas, entry->d_name);
				//printf("Ruta de metadata: %s\n", metadataFile);
				t_config *config_file = cargarConfiguracion(metadataFile,
						logger);

				int compactationTime = config_get_int_value(config_file,
						"COMPACTATION_TIME");
				free(metadataFile);
				config_destroy(config_file);
				//printf("Tiempo de compactacion tabla %s: %d\n", entry->d_name, compactationTime);
				int idHilo = crearHiloCompactacion(compactationTime, entry->d_name);
				t_datos_hilo_compactacion* compactacion = malloc(sizeof(t_datos_hilo_compactacion));
				compactacion->hilo = idHilo;
				strcpy(compactacion->path_tabla,entry->d_name);
				list_add(listaHilos,compactacion);
			}
		}
		closedir(dir);
	}
}

t_list *ObtenerRegistros(char *tabla, char *extension) {

	DIR *dir;
	struct dirent *entry;

	log_info(logger, "Obteniendo registros %s de %s", extension, tabla);
	if ((dir = opendir(tabla)) == NULL) {
		perror("openndir() error");
	} else {
		t_list *registros = list_create();
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
				//no hacer nada
			} else {
				if (string_ends_with(entry->d_name, extension)) {
					char *pathFile = string_from_format("%s/%s", tabla,
							entry->d_name);
					log_info(logger, "Abriendo config de %s", pathFile);
					t_config *config_file = cargarConfiguracion(pathFile,
							logger);

					int size = config_get_int_value(config_file, "SIZE");

					int cantBloques = CalcularBloques(size);
					int recorrido = 0;
					char **bloques = config_get_array_value(config_file, "BLOCKS");
					while(bloques[recorrido] != NULL) {
						recorrido ++;
					}
					config_destroy(config_file);
					for (int i = 0; recorrido > i; i++) {
						char *bloque = string_from_format("%s%s.bin",
								rutaBloques, bloques[i]);
						//abrirArchivo
						log_info(logger, "ABRIENDO %s", bloque);
						FILE *archivo = fopen(bloque, "r+");
						char linea[100];
						char **elementos;

						strcpy(linea,"");
						//recorrer
						while (!feof(archivo)) {
							char * rd = fgets(linea,100,archivo);
							if(rd!=NULL){
								elementos = string_split(linea, ";");
								int cantElementos = ContarElementosArray(elementos);
								t_registro *registro = malloc(sizeof(t_registro));
								registro->timestamp = atoll(elementos[0]);
								registro->key = atoi(elementos[1]);
								char *value = string_new();
								string_append(&value, elementos[2]);
								value[strcspn(value, "\n")] = 0;
								strcpy(registro->value, value);
								list_add(registros, registro);
								for (int i = 0; cantElementos > i; i++) {
									free(elementos[i]);
								}
								free(value);
								free(elementos);
							}
						}
						free(bloque);
						fclose(archivo);
					}
					int cBloques = (cantBloques==0)?1:cantBloques;
					for(int z=0;z<cBloques;z++)
						free(bloques[z]);
					free(bloques);
					free(pathFile);
				}
			}
		}
		closedir(dir);
		return registros;
	}
	return NULL;
}

t_list *ObtenerRegistrosArchivo(char *tabla, char *archivo, char *extension) {

	DIR *dir;
	struct dirent *entry;
	char *archivoExtension = string_new();

	string_append(&archivoExtension, archivo);
	string_append(&archivoExtension, extension);

	log_info(logger, "Obteniendo registros %s de %s", extension, tabla);
	if ((dir = opendir(tabla)) == NULL) {
		perror("openndir() error");
	} else {
		t_list *registros = list_create();
		while ((entry = readdir(dir)) != NULL) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
				//no hacer nada
			} else {
				if (string_equals_ignore_case(entry->d_name, archivoExtension)) {
					char *pathFile = string_from_format("%s/%s", tabla,
							entry->d_name);
					log_info(logger, "Abriendo config de %s", pathFile);
					t_config *config_file = cargarConfiguracion(pathFile,
							logger);

					int size = config_get_int_value(config_file, "SIZE");

					int cantBloques = CalcularBloques(size);
					char **bloques = config_get_array_value(config_file, "BLOCKS");
					int recorrido = 0;
					while(bloques[recorrido] != NULL){
						recorrido ++;
					}
					config_destroy(config_file);
					for (int i = 0; recorrido > i; i++) {
						char *bloque = string_from_format("%s%s.bin",
								rutaBloques, bloques[i]);
						//abrirArchivo
						log_info(logger, "ABRIENDO %s", bloque);
						FILE *archivo = fopen(bloque, "r+");
						char linea[100];
						char **elementos;

						strcpy(linea,"");
						//recorrer
						while (!feof(archivo)) {
							char *rd = fgets(linea,100,archivo);
							if(rd!=NULL){
								elementos = string_split(linea, ";");
								int cantElementos = ContarElementosArray(elementos);
								t_registro *registro = malloc(sizeof(t_registro));
								registro->timestamp = atoll(elementos[0]);
								registro->key = atoi(elementos[1]);
								char *value = string_new();
								string_append(&value, elementos[2]);
								value[strcspn(value, "\n")] = 0;
								strcpy(registro->value, value);
								list_add(registros, registro);
								for (int i = 0; cantElementos > i; i++) {
									free(elementos[i]);
								}
								free(value);
								free(elementos);
							}
						}
						free(bloque);
						fclose(archivo);
					}
					int cBloques = (cantBloques==0)?1:cantBloques;
					for(int z=0;z<cBloques;z++)
						free(bloques[z]);
					free(bloques);
					free(pathFile);
				}
			}
		}
		closedir(dir);
		free(archivoExtension);
		return registros;
	}
	free(archivoExtension);
	return NULL;
}

int ContarTablas() {
	DIR *dir;
	struct dirent *entry;
	int count = 0;

	char *path = string_from_format("%s", rutaTablas);

	if ((dir = opendir(path)) == NULL) {
		perror("openndir() error");
	}

	while ((entry = readdir(dir)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

		} else {
			count++;
		}
	}

	closedir(dir);
	free(entry);
	free(path);
	return count;
}

void CargarTablas(){
	DIR *dir;
	struct dirent *entry;

	char *path = string_from_format("%s", rutaTablas);

	if ((dir = opendir(path)) == NULL) {
		perror("openndir() error");
	}

	while ((entry = readdir(dir)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {

		} else {
			char *nombre = string_new();
			string_append(&nombre, entry->d_name);
			AddGlobalList(nombre);
			free(nombre);
		}
	}

	free(entry);
	free(path);
	closedir(dir);

}

void AddGlobalList(char *nombre){
	t_tcb *tabla = malloc(sizeof(t_tcb));
	tabla->bloqueado = 0;
	tabla->contadorTmp = 1;
	strcpy(tabla->nombre_tabla, nombre);
	list_add(tablasGlobal, tabla);
//	log_info(logger, "Nueva tabla ingresada");

}

int GetFreeBlocks() {
	size_t sizeBitmap = bitarray_get_max_bit(bitmap);
	int count = 0;
	for (int i = 0; i < sizeBitmap; i++) {
		if (bitarray_test_bit(bitmap, i) == 0) {
			count++;
		}
	}
	return count;
}

void IniciarBloques() {
	int bloque = 0;
	for (int i = 0; i < cantidad_bloques; i++) {
		CrearBloque(bloque, 0);
		bloque++;
	}
}

void CrearDirectorio(char *directory){
	struct stat st = {0};
	if(stat(directory, &st) == -1 ){
		if(mkdir(directory, 0777) == -1){
			perror("mkdir");
			loggear(logger, LOG_LEVEL_ERROR, "Error creando directorio: %s", directory);
		} else {
		loggear(logger, LOG_LEVEL_INFO, "Directorio %s creado", directory);
		}
	} else {
		loggear(logger, LOG_LEVEL_ERROR, "El directorio %s ya existe", directory);
	}
}

void aplicar_retardo() {
	sleep(lfs_conf.retardo/1000);
}

void RemoveGlobalList(char *tabla){
	int size = list_size(tablasGlobal);
	int pos = 0;
	for (int j = 0; size > j; j++){
		t_tcb *tcb = list_get(tablasGlobal, j);
		if(string_equals_ignore_case(tcb->nombre_tabla, tabla)) pos = j;
	}

	list_remove(tablasGlobal, pos);
}

char* descomponer_registro(t_registro *registro) {
	char *linea = string_new();
	char *key = string_new();
	char *value = string_new();
	string_append(&value, registro->value);
	char *keyOut = string_itoa(registro->key);
	string_append(&key, keyOut);
	char *timestamp = malloc(20);
	sprintf(timestamp, "%llu", registro->timestamp);
	string_append(&linea, timestamp);
	string_append(&linea, ";");
	string_append(&linea, key);
	string_append(&linea, ";");
	string_append(&linea, value);
	string_append(&linea, "\n");
	free(keyOut);
	free(key);
	free(timestamp);
	free(value);
	return linea;
}

char* descomponer_metadata(t_metadata *metadata, char *nombre) {
	char *buffer = string_new();
	string_append(&buffer, nombre);
	string_append(&buffer, ";");
	string_append(&buffer, metadata->tipoConsistencia);
	string_append(&buffer, ";");
	char *particiones = string_itoa(metadata->particiones);
	string_append(&buffer, particiones);
	string_append(&buffer, ";");
	char *compactationTiempo = string_itoa(metadata->compactationTime);
	string_append(&buffer, compactationTiempo);
	free(particiones);
	free(compactationTiempo);
	return buffer;
}
