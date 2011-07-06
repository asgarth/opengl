/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include "includes.h"
#include "network_code.h"
#include <SDL/SDL_net.h>

class Server_Network {
	private:
		UDPsocket udpsock;					// UDP socket
		UDPpacket *in, *out;					// UDP packet
		IPaddress ip[MAX_CLIENT];			// IP Address list
		int pkt_num[MAX_CLIENT];			// numerazione pacchetti per scartare duplicati
		int port;								// server port number
		int client_num;						// numero client connessi
		int current_pkt;						// numero del pacchetto da inviare

	public:
		Server_Network (void);
		~Server_Network(void);

		bool init (int);						// apre ed inizializza il socket
		char* recv (void);					// prova a ricevere un pkt
		bool send (const char *, int);	// invia la stringa passata al client specificato
		bool sendAll (const char *);		// invia la stringa passata a tutti i client
		bool addLastIP (void);				// aggiunge l'ip dell'ultimo pkt ricevuto
		int getClientNum (void);			// ritorna il numero dei client connessi
};

#endif
