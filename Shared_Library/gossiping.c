/*
 * gossiping.c
 *
 *  Created on: 8 jun. 2019
 *      Author: utnso
 */

#include "gossiping.h"

char* getLocalIp(char *MEM_CONF_IP)
{
	char* localIp;
	struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void* tmpAddrPtr=NULL;

    FILE *f;
    char line[100] , *p , *c;

    if(MEM_CONF_IP != NULL)
    	return MEM_CONF_IP;

	f = fopen("/proc/net/route", "r");

	while(fgets(line , 100 , f)) {
		p = strtok(line , " \t");
		c = strtok(NULL , " \t");
		if(p!=NULL && c!=NULL && strcmp(c , "00000000") == 0) break;
	}

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET && strcmp( ifa->ifa_name , p) == 0) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            localIp = string_from_format(addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return localIp;
}

int connect_to_server_goss(char* IP, char* PUERTO, int proceso, int flag, t_log *logger)
{
	int socket;

	if((socket = definirSocket(logger))<= 0)
		return-1;

	if(conectarseAServidor_w_to(socket, IP, atoi(PUERTO), logger)<=0)
		return -1;

	loggear(logger, LOG_LEVEL_INFO, "GOSSIP| INICIO Handshake(%d)...", proceso);
	enviarMensaje(mem, handshake, 0, NULL, socket, logger, proceso);
	t_mensaje* msg = recibirMensaje(socket, logger);
	destruirMensaje(msg);
	loggear(logger, LOG_LEVEL_INFO, "GOSSIP| FIN Handshake(%d)", proceso);
	return socket;
}

//----------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------//

int loggearLista(t_list *LISTA_CONN,t_log *logger)
{
	t_tipoSeeds *seed;
	int i=0;

	while(i<LISTA_CONN->elements_count)
	{
		seed = list_get(LISTA_CONN, i);

//		loggear(logger,LOG_LEVEL_INFO,"------------------------------");
//		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_NUMERO[%d]: %d",i, seed->numeroMemoria);
//		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_IP[%d]: %s",i, seed->ip);
//		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_PUERTO[%d]: %s",i, seed->puerto);
//		loggear(logger,LOG_LEVEL_INFO,"LISTA_IP_SEEDS_ESTADO[%d]: %d",i, seed->estado);

		i++;
	}
	return 1;
}

int actualizarNumMemoriaListaStruct(t_list *LISTA_CONN,t_log *logger,int posicion,int numMemoria)
{
	t_tipoSeeds *seed;
	t_tipoSeeds *seedsModificado;

	seedsModificado= malloc(sizeof(t_tipoSeeds));
	seed = list_get(LISTA_CONN,posicion);


	if(numMemoria< NUM_CONEX)
		seedsModificado->numeroMemoria = numMemoria;

	memset(seedsModificado->ip,0x0,sizeof(seedsModificado->ip));
	strcpy(seedsModificado->ip,seed->ip);

	memset(seedsModificado->puerto,0x0,sizeof(seedsModificado->puerto));
	strcpy(seedsModificado->puerto,seed->puerto);

	seedsModificado->estado = seed->estado;

	free(list_replace(LISTA_CONN,posicion,seedsModificado));

	loggearLista(LISTA_CONN,logger);

	return 1;
}

int actualizarEstadoListaStruct(t_list *LISTA_CONN,t_log *logger,int posicion,char estado)
{
	t_tipoSeeds *seed;
	t_tipoSeeds *seedsModificado;

	seedsModificado= malloc(sizeof(t_tipoSeeds));
	seed = list_get(LISTA_CONN,posicion);

	seedsModificado->numeroMemoria = seed->numeroMemoria;

	memset(seedsModificado->ip,0x0,sizeof(seedsModificado->ip));
	strcpy(seedsModificado->ip,seed->ip);

	memset(seedsModificado->puerto,0x0,sizeof(seedsModificado->puerto));
	strcpy(seedsModificado->puerto,seed->puerto);

	seedsModificado->estado = estado;

	free(list_replace(LISTA_CONN,posicion,seedsModificado));

	loggearLista(LISTA_CONN,logger);

	return 1;
}

int agregarSeedLista(t_list *LISTA_CONN, char *ipNueva,char *puertoNuevo,int numeroMemoria,t_log *logger)
{
	t_tipoSeeds *seedNuevo;

	seedNuevo= malloc(sizeof(t_tipoSeeds));

	seedNuevo->numeroMemoria = numeroMemoria;

	memset(seedNuevo->ip,0x0,sizeof(seedNuevo->ip));
	strcpy(seedNuevo->ip,ipNueva);

	memset(seedNuevo->puerto,0x0,sizeof(seedNuevo->puerto));
	strcpy(seedNuevo->puerto,puertoNuevo);

	seedNuevo->estado = DESCONECTADO;

	list_add(LISTA_CONN,seedNuevo);

	loggearLista(LISTA_CONN,logger);

	return 1;
}

int actualizaListaSeedConfigStruct(t_list *LISTA_CONN, char *ipNueva,char *puertoNuevo,int numeroMemoria,t_log *logger)
{
	int j=0;
	int existe=0;
	t_tipoSeeds *seed;


	while(j<LISTA_CONN->elements_count)
	{

		seed = list_get(LISTA_CONN,j);

		if(numeroMemoria == DESCONOCIDO)
		{
			if(string_equals_ignore_case(seed->ip, ipNueva) && string_equals_ignore_case(seed->puerto, puertoNuevo))
			{
				existe=1;
				break;
			}
		}
		else
		{
			if(seed->numeroMemoria == numeroMemoria)
			{
				existe=1;
				break;
			}
			else
			{
				if(string_equals_ignore_case(seed->ip, ipNueva) && string_equals_ignore_case(seed->puerto, puertoNuevo))
				{
					actualizarNumMemoriaListaStruct(LISTA_CONN,logger,j,numeroMemoria);
					existe=1;
					break;
				}
			}
		}

		j++;
	}
	if(existe!=1)
	{
		agregarSeedLista(LISTA_CONN,ipNueva,puertoNuevo,numeroMemoria,logger);
	}

	return 1;
}

int crearListaSeedsStruct(char tipoProceso,char *MEM_CONF_IP,char *MEM_CONF_PUERTO,int MEM_CONF_NUMERO_MEMORIA, char **MEM_CONF_IP_SEEDS, char **MEM_CONF_PUERTO_SEEDS, t_log *logger,t_list *LISTA_CONN)
{
	int i=0;

	loggear(logger,LOG_LEVEL_INFO,"GOSSIP| Creando lista de SEEDS/PUERTOS...");
	loggear(logger,LOG_LEVEL_INFO,"GOSSIP| TIPO_PROCESO: %d", tipoProceso);
	if(tipoProceso == gossiping)
	{
		actualizaListaSeedConfigStruct(LISTA_CONN,getLocalIp(MEM_CONF_IP),MEM_CONF_PUERTO,MEM_CONF_NUMERO_MEMORIA,logger);
	}

	while(MEM_CONF_IP_SEEDS[i] != NULL)
	{
		int memoryNumberSeeds;

		memoryNumberSeeds = DESCONOCIDO;

		actualizaListaSeedConfigStruct(LISTA_CONN,MEM_CONF_IP_SEEDS[i],MEM_CONF_PUERTO_SEEDS[i],memoryNumberSeeds,logger);

		i++;
	}

	//loggearLista(LISTA_CONN, logger);

	return 1;
}

char *armarMensajeListaSEEDSStruct(t_log *logger,t_list *LISTA_CONN)
{
	int j=0;
	char *msj;
	char *msjOld;

	t_tipoSeeds *seed;

	while(j<LISTA_CONN->elements_count)
	{

		seed = list_get(LISTA_CONN,j);

		if(j==0)
		{
			msj = string_from_format("%d:%s:%s",seed->numeroMemoria,seed->ip,seed->puerto);
		}
		else
		{
			msjOld=msj;
			msj = string_from_format("%s|%d:%s:%s",msj,seed->numeroMemoria,seed->ip,seed->puerto);
			free(msjOld);
		}

		j++;
	}

	loggear(logger,LOG_LEVEL_INFO,"GOSSIP| MSJ_GOSSIPING: %s", msj);

	return msj;
}

int procesarMsjGossipingStruct(char *mensaje, char *primerParser, char *segundoParser, t_log *logger,t_list *LISTA_CONN)
{
	int i = 0;
	int numMemoria = 0;
	char** parser;
	char** parserIpPuerto;
	char* ip;
	char* puerto;
	char* numeroMemoria;

	if (!string_is_empty(mensaje))
	{

		parser = string_split(mensaje, primerParser);
		i=0;
		while(parser[i]!=NULL)
		{
			parserIpPuerto = string_split(parser[i], segundoParser);


			if(parserIpPuerto[0] != NULL)
			{
				numeroMemoria = string_new();
				numeroMemoria = parserIpPuerto[0];
				numMemoria = atoi(numeroMemoria);
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"GOSSIP| ERROR PROCESO MSJ GOSSIPING");
				for (int j = 0; parserIpPuerto[j] != NULL; j++)
					free(parserIpPuerto[j]);
				if (parserIpPuerto)
					free(parserIpPuerto);

				for (int j = 0; parser[j] != NULL; j++)
					free(parser[j]);
				if (parser)
					free(parser);

				return -1;
			}


			if(parserIpPuerto[1] != NULL)
			{
				ip = string_new();
				ip = parserIpPuerto[1];
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"GOSSIP| ERROR PROCESO MSJ GOSSIPING");
				for (int j = 0; parserIpPuerto[j] != NULL; j++)
					free(parserIpPuerto[j]);
				if (parserIpPuerto)
					free(parserIpPuerto);

				for (int j = 0; parser[j] != NULL; j++)
					free(parser[j]);
				if (parser)
					free(parser);
				return -1;
			}
			if(parserIpPuerto[2] != NULL)
			{
				puerto = string_new();
				puerto = parserIpPuerto[2];
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"GOSSIP| ERROR PROCESO MSJ GOSSIPING");
				for (int j = 0; parserIpPuerto[j] != NULL; j++)
					free(parserIpPuerto[j]);
				if (parserIpPuerto)
					free(parserIpPuerto);

				for (int j = 0; parser[j] != NULL; j++)
					free(parser[j]);
				if (parser)
					free(parser);
				return -1;
			}

			loggear(logger,LOG_LEVEL_INFO,"GOSSIP| MENSAJE_RECIBIDO: NUMERO_MEMORIA: %d | IP: %s | PUERTO: %s", numMemoria,ip,puerto);

			//AGREGAR A LISTA LOCAL DE PROCESO DE MSJ
			pthread_mutex_lock(&mutexGossiping);
			actualizaListaSeedConfigStruct(LISTA_CONN,ip,puerto,numMemoria,logger);
			pthread_mutex_unlock(&mutexGossiping);

			i++;

			for (int j = 0; parserIpPuerto[j] != NULL; j++)
				free(parserIpPuerto[j]);
			if (parserIpPuerto)
				free(parserIpPuerto);

		}
		for (int j = 0; parser[j] != NULL; j++)
			free(parser[j]);
		if (parser)
			free(parser);
		//loggearLista(LISTA_CONN,logger);
	}
	else
	{
		loggear(logger,LOG_LEVEL_INFO,"GOSSIP| ERROR PROCESO MSJ GOSSIPING");
		return-1;
	}

	return 1;
}


void processGossipingStruct(t_log *logger,t_list *LISTA_CONN,char tipoProceso) {
	int i=0;
	int contadorLista=0;
	int socketReceptor=0;
	char *mensaje;
	t_mensaje* msjRecibido;
	t_tipoSeeds *seed;

	if(pthread_mutex_trylock(&mutexprocessGossiping))
	{
		loggear(logger,LOG_LEVEL_INFO,"GOSSIP| ERROR_MUTEX_GOSSIPING");
		return;
	}

	loggear(logger,LOG_LEVEL_INFO,"GOSSIP| Se inicio proceso Gossiping...");

	pthread_mutex_lock(&mutexGossiping);
	mensaje= armarMensajeListaSEEDSStruct(logger,LISTA_CONN);
	pthread_mutex_unlock(&mutexGossiping);

	pthread_mutex_lock(&mutexGossiping);
	contadorLista = LISTA_CONN->elements_count;
	pthread_mutex_unlock(&mutexGossiping);

	//CONEXION_CON_CADA_MEMORIA
	if(tipoProceso== gossiping)
		i=1;

	while(i < contadorLista)
	{
		pthread_mutex_lock(&mutexGossiping);
		seed = list_get(LISTA_CONN,i);
		pthread_mutex_unlock(&mutexGossiping);

		loggear(logger,LOG_LEVEL_DEBUG,"GOSSIP| CONEXION LISTA SEEDS NUMERO: %d",i);
		loggear(logger,LOG_LEVEL_INFO,"GOSSIP| LISTA_MEMORY_NUMBER_SEEDS: %d", seed->numeroMemoria);
		loggear(logger,LOG_LEVEL_INFO,"GOSSIP| LISTA_IP_SEEDS: %s", seed->ip);
		loggear(logger,LOG_LEVEL_INFO,"GOSSIP| LISTA_PUERTO_SEEDS: %s", seed->puerto);

		socketReceptor=connect_to_server_goss(seed->ip,seed->puerto,mem,gossiping,logger);

		if(socketReceptor>0)
		{
			int envioMsj;

			//ENVIAR_LISTA_SEEDS
			envioMsj = enviarMensaje(mem,gossipingMsg,strlen(mensaje)+1,mensaje,socketReceptor,logger,mem);

			if(envioMsj < 1 )
				loggear(logger,LOG_LEVEL_INFO,"GOSSIP| NO SE PUDO ENVIAR MSJ %d",envioMsj);
			else
				loggear(logger,LOG_LEVEL_INFO,"GOSSIP| MSJ ENVIADO CON EXITO %d",envioMsj);

			//RECIBIR_LISTA_SEEDS
			msjRecibido = recibirMensaje(socketReceptor,logger);

			//PROCESAR_LISTA_SEEDS
			if(msjRecibido != NULL)
			{
				loggear(logger,LOG_LEVEL_INFO,"GOSSIP| MSJ RECIBIDO CON EXITO %d",envioMsj);
				procesarMsjGossipingStruct(msjRecibido->content,"|",":",logger,LISTA_CONN);
				destruirMensaje(msjRecibido);
				pthread_mutex_lock(&mutexGossiping);
				actualizarEstadoListaStruct(LISTA_CONN,logger,i,CONECTADO);
				pthread_mutex_unlock(&mutexGossiping);
			}
			else
			{
				loggear(logger,LOG_LEVEL_INFO,"GOSSIP| ERROR MSJ %d",envioMsj);
				pthread_mutex_lock(&mutexGossiping);
				actualizarEstadoListaStruct(LISTA_CONN,logger,i,DESCONECTADO);
				pthread_mutex_unlock(&mutexGossiping);
			}

			close(socketReceptor);
		}
		else
		{
			loggear(logger,LOG_LEVEL_INFO,"GOSSIP| FALLÓ_CONEXION: %d", socketReceptor);
			pthread_mutex_lock(&mutexGossiping);
			actualizarEstadoListaStruct(LISTA_CONN,logger,i,DESCONECTADO);
			pthread_mutex_unlock(&mutexGossiping);
		}
		i++;
	}
	free (mensaje);

	pthread_mutex_unlock(&mutexprocessGossiping);

}
