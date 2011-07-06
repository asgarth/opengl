/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Server_Network.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Server_Network::Server_Network (void) {
	client_num = 0;
	current_pkt = 1;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Server_Network::~Server_Network () {
	// reset memory
	for (int loop = 0; loop < MAX_CLIENT; loop ++) {
		ip[loop].host = 0;
		ip[loop].port = 0;
		pkt_num[loop] = 0;
	}

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
bool Server_Network::init (int port_num) {
	port = port_num;

	// init socket
	udpsock = SDLNet_UDP_Open (port);			// open server socket
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

	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// recv
char* Server_Network::recv (void) {
	int client = -1;
	char *string, *token;
	int this_pkt;				// salva il numero del pkt ricevuto

	// provo a ricevere un pkt
	if (SDLNet_UDP_Recv (udpsock, in) > 0) {
		// converte il pkt in stringa
		string = (char *) malloc (sizeof(char) * in->len);
		if (string == NULL) { printf("Error allocating memory :(");	return NULL; }
		string = strcpy (string, (char*)in->data);

		// cerco il client che ha mandato il pkt
		for (int loop = 0; loop < client_num; loop++) {
			if (ip[loop].host == in->address.host && ip[loop].port == in->address.port) {
				client = loop;
				break;
			}
		}
		if (client == -1) {	// client non trovato...controllo se è un msg di login
			// creo una copia della stringa
			char *s_temp = (char *) malloc (sizeof(char) * in->len);
			if (s_temp == NULL) { printf("Error allocating memory :(");	return NULL; }
			s_temp = strcpy (s_temp, string);
			token = strtok (s_temp, ":");
			token = strtok (NULL, ":");
			int pkt_code = atoi(token);
			token = strtok (NULL, ":");
			float version = atof(token);
			// free temp memory
			if (s_temp)		free (s_temp);
			if (token)		free (token);
			if (pkt_code == LOGIN && version == VERSION) {	// LOGIN
				if (addLastIP ())									// aggiunge l'ip
					return string;
				else {
					printf ("Limite massimo giocatori raggiunto -> IMPOSSIBILE AGGIUNGERE IP\n");
					return NULL;
				}
			}
			else
				return NULL;
		}
		else {					// ho trovato il client quindi NON è un msg di login
			// creo una copia della stringa
			char *s_temp = (char *) malloc (sizeof(char) * in->len);
			if (s_temp == NULL) { printf("Error allocating memory :(");	return NULL; }
			s_temp = strcpy (s_temp, string);
			// controllo se la stringa è valida e se non è un duplicato
			token = strtok (s_temp, ":");
			this_pkt = atoi(token);
			// free temp memory
			if (s_temp)		free (s_temp);
			if (token)		free (token);

			// controllo pacchetti duplicati
			if (this_pkt > pkt_num[client] || (pkt_num[client] > 55000 && this_pkt < 1000)) {
				pkt_num[client] = this_pkt;
				// ritorna la stringa ricevuta
				return string;
			}
			else
				return NULL;
		}
	}

	// se non ricevo niente ritorno NULL
	return NULL;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// send
bool Server_Network::send (const char *string, int client) {
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

	out->address.host = ip[client].host;
	out->address.port = ip[client].port;
	if(!SDLNet_UDP_Send (udpsock, -1, out)) {
		printf("ERROR - SDLNet_UDP_Send: %s\n",SDLNet_GetError());
		free (s_temp);
		return false;
	}

	free (s_temp);
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// send to all client
bool Server_Network::sendAll (const char *string) {
	for (int loop = 0; loop < client_num; loop++) {
		if (!send (string, loop))
			return false;
	}
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// add ip
bool Server_Network::addLastIP (void) {
	// controlla se ci sono posti liberi
	if (client_num > MAX_CLIENT)
		return false;

	// controlla se l'ip esiste e se lo trova esce senza fare niente
	for (int loop = 0; loop < client_num; loop++) {
		if (ip[loop].host == in->address.host && ip[loop].port == in->address.port)
			return false;
	}

	// se non esiste lo aggiunge
	ip[client_num].host = in->address.host;	// salva l'host
	ip[client_num].port = in->address.port;	// salva la porta
	pkt_num[client_num] = 0;						// setta a zeor il contatore di pkt per quel client
	client_num++;
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna il numero dei client connessi
int Server_Network::getClientNum (void) {
	return client_num - 1;
}
