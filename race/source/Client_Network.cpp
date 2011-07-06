/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Client_Network.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Client_Network::Client_Network (void) {
	current_pkt = 1;
	server_pkt = 0;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Client_Network::~Client_Network () {
	// reset memory
	ip.host = 0;
	ip.port = 0;

	// reset socket
	if (udpsock)	SDLNet_UDP_Close (udpsock);	// close socket
	udpsock = NULL;										// reset socket

	// reset pkt
	if (in)			SDLNet_FreePacket(in);			// free packet
	in = NULL;
	if (out)			SDLNet_FreePacket(out);			// free packet
	out = NULL;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init
bool Client_Network::init (const char *ip_addr, int server_port) {
	// init socket
	udpsock = SDLNet_UDP_Open (0);			// open client socket
	if(!udpsock) {
		printf("SDLNet_UDP_Open error: %s\n", SDLNet_GetError());
		return false;
	}

	// init UDP packet
	if(!(out = SDLNet_AllocPacket(PKT_SIZE))) {
		printf("SDLNet_AllocPacket: %s\n",SDLNet_GetError());
		return false;
	}
	if(!(in = SDLNet_AllocPacket(PKT_SIZE))) {
		printf("SDLNet_AllocPacket: %s\n",SDLNet_GetError());
		return false;
	}

	// connect to server
	if(SDLNet_ResolveHost(&ip, ip_addr, server_port) == -1) {	// resolve ip
		printf("SDLNet_ResolveHost: %s\n",SDLNet_GetError());
		return false;
	}
	// bind channel
	if(SDLNet_UDP_Bind (udpsock, 0, &ip) == -1) {					// bind channel
		printf("SDLNet_UDP_Bind: %s\n",SDLNet_GetError());
		return false;
	}

	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// recv
char* Client_Network::recv (void) {
	char *string, *token;
	int this_pkt;				// salva il numero del pkt ricevuto

	if (SDLNet_UDP_Recv (udpsock, in) > 0) {
		// converte il pkt in stringa
		string = (char *) malloc (sizeof(char) * in->len);
		if (string == NULL) { printf("Error allocating memory :(");	return NULL; }
		string = strcpy (string, (char*)in->data);

		// creo una copia della stringa
		char *s_temp = (char *) malloc (sizeof(char) * in->len);
		if (s_temp == NULL) { printf("Error allocating memory :(");	return NULL; }
		s_temp = strcpy (s_temp, string);

		// controllo se la stringa è valida e se non è un duplicato
		token = strtok (s_temp,":");
		this_pkt = atoi(token);

		// free temp memory
		if (s_temp)		free (s_temp);
		if (token)		free (token);

		// controllo pacchetti duplicati
		if (this_pkt > server_pkt || (server_pkt > 55000 && this_pkt < 1000)) {
			server_pkt = this_pkt;
			// ritorna la stringa ricevuta
			return string;
		}
	}

	// se non ricevo niente ritorno NULL
	return NULL;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// send
bool Client_Network::send (const char *string) {
	if (strlen(string)+1 > PKT_SIZE-6)				// -6 perchè devo aggiungerci davanti il numero del pkt
		return false;

	// inizzializza string
	char *s_temp = (char*) malloc (sizeof(char) * (strlen(string) + 7));
	if (s_temp == NULL) { printf("Error allocating memory :(");	return false; }
	sprintf (s_temp, "%d:%s", current_pkt, string);

	current_pkt++;
	if (current_pkt > 60000)
		current_pkt = 1;


	// prepara pkt
	strcpy ((char*)out->data, s_temp);
	out->len = strlen(s_temp)+1;

	// invia al server
	if(!SDLNet_UDP_Send (udpsock, 0, out)) {
		printf("ERROR - SDLNet_UDP_Send: %s\n",SDLNet_GetError());
		free (s_temp);
		return false;
	}

	free (s_temp);
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// send and wait for ack
char* Client_Network::sendWithAck (const char *string, int retry, int timeout) {
	bool ack_received = false;
	char *s_temp, *token, *ret_string;

	for (int i = 0; i < retry; i++) {
		if (!send (string))
			return NULL;
		SDL_Delay (timeout);
		for (int j = 0; j < retry; j++) {
			SDL_Delay (timeout/(j+1));
			// provo a ricevere l'ack
			if ((s_temp = recv ()) != NULL) {	// se ho ricevuto qualcosa
				ret_string = (char *) malloc (sizeof(char) * (strlen(s_temp)+1));
				ret_string = strcpy (ret_string, s_temp);
				// controlla se è l'ack che aspettavo
				token = strtok (s_temp,":");		// salto il numero del pkt
				token = strtok (NULL,":");			// leggo il codice
				int pkt_code = atoi(token);
				token = strtok (NULL,":");			// leggo il numero dell'ack
				int ack_code = atoi(token);
				// libero memoria temp
				if (s_temp)		free (s_temp);
				if (token)		free (token);
				// cotrollo se l'ack è quellO che aspettavo
				if (pkt_code == ACK && ack_code == current_pkt-1)
					return ret_string;
			}
		}
	}

	return NULL;
}

