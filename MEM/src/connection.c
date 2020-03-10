/*
 * connection.c
 *
 *  Created on: 21 sep. 2018
 *      Author: utnso
 */
#include "MEM.h"

#include "connection.h"
#include "error.h"
#include "gossiping.h"
#include "i_kernel_proceso.h"

void aceptar(int socket_fm9, int* descriptor_mas_alto, fd_set* set_master);
void procesar(int n_descriptor, fd_set* set_master);



int connect_to_server(char* IP, char* PUERTO, int proceso, int flag) {

        int socket;

        if((socket = definirSocket(logger))<= 0)
                _exit_with_error(NULL,NULL);

        if(flag!=gossiping)
        {
        	if(conectarseAServidor(socket, IP, atoi(PUERTO), logger)<=0)
        		_exit_with_error(NULL,NULL);
        }
        else
        {
        	if(conectarseAServidor_w_to(socket, IP, atoi(PUERTO), logger)<=0)
        		return -1;
        }

        /*if(conectarseAServidor(socket, IP, atoi(PUERTO), logger)<=0)
        {
        	if(flag!=gossiping)
        	{
        		_exit_with_error(NULL,NULL);
        	}
        	else
        	{
        		return -1;
        	}
        }*/

        loggear(logger, LOG_LEVEL_INFO, "INICIO Handshake(%d)...", proceso);
        enviarMensaje(mem, handshake, 0, NULL, socket, logger, proceso);
        t_mensaje* msg = recibirMensaje(socket, logger);
        if(proceso == lis){
        	tamanio_value = *(int*)msg->content;
        	loggear(logger,LOG_LEVEL_INFO, "el tamanio es %d", tamanio_value);
        }
        destruirMensaje(msg);
        loggear(logger, LOG_LEVEL_INFO, "FIN Handshake(%d)", proceso);
        return socket;
}

int listen_connexions(char* PUERTO)
{
	int socket_fm9 = definirSocket(logger);
	if(bindearSocketYEscuchar(socket_fm9,"",atoi(PUERTO),logger)<= 0)
		_exit_with_error("BIND",NULL);

	fd_set set_master, set_copia;
	FD_ZERO(&set_master);
	FD_SET(socket_fm9, &set_master);
	int descriptor_mas_alto = socket_fm9;

	while (true) {
		set_copia = set_master;
		int i = select(descriptor_mas_alto + 1, &set_copia, NULL, NULL, NULL);
		if (i == -1) {
			loggear(logger,LOG_LEVEL_INFO,"Salio con error %d",i);
			_exit_with_error("SELECT()",NULL);
		}

		int n_descriptor = 0;
		while (n_descriptor <= descriptor_mas_alto) {
			if (FD_ISSET(n_descriptor,&set_copia)) {
				//ACEPTAR CONXIONES
				if (n_descriptor == socket_fm9) {
					aceptar(socket_fm9, &descriptor_mas_alto, &set_master);
				}
				//PROCESAR MENSAJE
				else {
					procesar(n_descriptor,&set_master);
				}
			}
			n_descriptor++;
		}
	}

	return socket_fm9;
}

void aceptar(int socket_fm9, int* descriptor_mas_alto, fd_set* set_master) {
	int client_socket;
	if ((client_socket = aceptarConexiones(socket_fm9,logger)) == -1) {
		loggear(logger,LOG_LEVEL_ERROR,"Error en el accept");
		return;
	}
	FD_SET(client_socket, set_master);
	if (client_socket > *descriptor_mas_alto) {
		*descriptor_mas_alto = client_socket;
	}
}

void procesar(int n_descriptor, fd_set* set_master) {
	t_mensaje* msg;
	if((msg = recibirMensaje(n_descriptor, logger))== NULL) {
		close(n_descriptor);
		FD_CLR(n_descriptor, set_master);
		return;
	}

	loggear(logger, LOG_LEVEL_INFO, "Proceso: %d", msg->header.tipoProceso);
	loggear(logger, LOG_LEVEL_INFO,"Procesar mensaje: %d", msg->header.tipoMensaje);

	switch(msg->header.tipoProceso) {
		case kernel:;
			procesar_KER(msg, n_descriptor, set_master);
			break;
		case mem:;
			switch(msg->header.tipoMensaje) {
				case handshake:;
					enviarMensaje(mem, handshake, 0, NULL, n_descriptor, logger, mem);
					//procesar_CPU(msg, n_descriptor, set_master);
					break;
				case gossipingMsg:;
					char *listasIPs = string_new();

					//TODO: actualizar mi lista de seeds
					listasIPs = string_from_format("%s",msg->content);
					loggear(logger,LOG_LEVEL_DEBUG,"Lista IPs recibida: [%s]",listasIPs);
					procesarMsjGossipingStruct(listasIPs,"|",":",logger,LISTA_CONN);
					//procesarMsjGossiping(listasIPs,"-",":",logger,LISTA_CONN,LISTA_CONN_PORT);

					pthread_mutex_lock(&mutexGossiping);
					listasIPs= string_from_format("%s",armarMensajeListaSEEDSStruct(logger,LISTA_CONN));
					//listasIPs= string_from_format("%s",armarMensajeListaSEEDS(logger,LISTA_CONN,LISTA_CONN_PORT));
					//mensaje = armarMensajeListaSEEDS(logger);
					pthread_mutex_unlock(&mutexGossiping);

					enviarMensaje(mem, gossipingMsg, strlen(listasIPs)+1, listasIPs, n_descriptor, logger, mem);

					free(listasIPs);

					break;
				default:
					loggear(logger,LOG_LEVEL_INFO,"No se reconoce el Menaje");
					break;
			}
			//procesar_CPU(msg, n_descriptor, set_master);
			break;
		default:
			loggear(logger,LOG_LEVEL_INFO,"No se reconoce el proceso");
			close(n_descriptor);
			FD_CLR(n_descriptor, set_master);
			break;
	}
	destruirMensaje(msg);
	loggear(logger,LOG_LEVEL_DEBUG,"Mensaje destruido");
}

