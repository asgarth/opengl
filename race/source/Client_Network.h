/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#include "includes.h"
#include "network_code.h"
#include <SDL/SDL_net.h>

class Client_Network {
	private:
		UDPsocket udpsock;					// UDP socket
		UDPpacket *in, *out;					// UDP packet
		IPaddress ip;							// IP server
		int current_pkt;						// numero del pacchetto da inviare
		int server_pkt;						// ultimo pkt ricevuto dal server

	public:
		Client_Network (void);
		~Client_Network(void);

		bool init (const char *, int);	// apre ed inizializza il socket
		char* recv (void);					// prova a ricevere un pkt
		bool send (const char *);			// invia la stringa passata
		char* sendWithAck (const char *, int, int);	// invia la stringa e ritorna l'ack se lo riceve, NULL altrimenti
};

#endif
