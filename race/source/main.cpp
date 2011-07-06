/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include <SDL/SDL_thread.h>
#include "includes.h"
#include "config_manager.h"
#include "Server_Network.h"
#include "Client_Network.h"
#include "Texture_Manager.h"
#include "Text.h"
#include "Menu.h"
#include "Display.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Sky.h"
#include "Player.h"
#include "Enemy.h"

// FUNZIONI
void renderFrame (void);
void updatePosition (void);
void setup (void);
void calcFrameRate (void);
void eventManager (void);
void clear_memory (void);
int serverThread (void *);
int clientThread (void *);
void set_end_level (bool);
bool get_end_level (void);

// VARIABILI GLOBALI
// game
float version = VERSION;
bool end_game = false;
bool end_level = false;
SDL_mutex *mutex;
// window
int screen_width = 640, screen_height = 480, screen_bpp = 32;	// risoluzione di default
bool fullscreen = false;
// frame rate
int fps = 30;									// FPS (init value)
bool limit_fps = false;						// limita gli fps ad un valore massimo
bool show_fps = true;						// visualizza gli fps durante il gioco
float frame_interval = 0.0;				// intervallo tra un frame e l'altro
// light
float LightPosition[] = {0.0f, 10.0f, 0.0f, 1.0f};		// position
float LightAmbient[] = {0.8f, 0.8f, 0.8f, 1.0f};		// ambient
float LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};		// diffuse
float LightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};		// specular
// fog
float fogColor[]= {0.5f, 0.5f, 0.5f, 1.0f};				// fog color
// gestione camera
Vector v_cam, v_dest;						// vertice per la posizione della camera
float x_offset, y_offset, z_offset;		// variabili di gestione della camera
// player
float player_x, player_y, player_z;
float current_height;

// NETWORK THREAD
char *server_ip_string;
bool start_server = false, start_client = false;
int server_udp_port = 5433;
SDL_Thread *server = NULL;
SDL_Thread *client = NULL;

// OGGETTI GLOBALI
SDL_GL_Init			SDL_Window;										// oggetto di gestione della finestra
Texture_Manager	texture;											// oggetto x caricamento e cambio texture
Frustum				frustum;											// oggetto per controllare cos'è nell'inquadratura
Terrain				ground(&texture, &frustum, TERRAIN_RES);// terreno
Sky					sky(&texture);									// sky
Text					font(&texture);								// oggetto gestione font
Menu					menu(&texture, &font, &ground, &frustum, &sky);	// oggetto per la gestione del menu
Display				display(&texture, &font);					// interfaccia
Player				player;											// oggetto giocatore
Enemy					enemy[MAX_CLIENT+1];							// array altri giocatori

/* ---------------------------------------------------------------------------------------------------------------------- */
// MAIN
int main (int argv, char *argc[]) {
	// register exit function
	atexit (clear_memory);

	// load config
	if (!loadConfig(&screen_width, &screen_height, &screen_bpp, &fullscreen, &limit_fps, &server_udp_port)) {
		printf ("Error loading config file...using defaults\n");
		screen_width = 800;		screen_height = 600;		screen_bpp = 32;		fullscreen = true;
		limit_fps = false;		server_udp_port = 5433;
	}


	// setup
	setup ();

	// MAIN LOOP
	while (!end_game) {
		// MENU
		menu.init(screen_width, screen_height);
		menu.show ();
		// NETWORK THREAD
		Server_Network *snet = NULL;
		Client_Network *cnet = NULL;
		switch (menu.getSelection()) {
			case 0:													// PRACTICE
							break;
			case 1:		start_server = true;					// HOST GAME
							break;
			case 2:		start_client = true;					// JOIN GAME
							break;
			case 3:		end_level = true;						// OPTION (disabled)
							break;
			case 4:		end_game = end_level = true;		// QUIT
							break;
		}
		menu.clear();
		// SETUP LEVEL
		if (!get_end_level()) {
			// init player
			char model_filename[30];
			sprintf (model_filename, "models/model_%d.txt", menu.getModel());
			if (!player.init (model_filename, "models/shader.txt", menu.getModel())) {
				printf ("Error loading models :(\n");
				exit (-1);
			}
			// START SERVER THREAD
			if (start_server) {
				snet = new Server_Network ();
				server = SDL_CreateThread (serverThread, snet);
				if(!server) { printf("SDL_CreateThread error: %s\n",SDL_GetError());	exit(-1); }
			}
			// START CLIENT THREAD
			if (start_client) {
				cnet = new Client_Network ();
				client = SDL_CreateThread (clientThread, cnet);
				if(!client) { printf("SDL_CreateThread error: %s\n",SDL_GetError());	exit(-1); }
			}
			// init game var
			display.init (screen_width, screen_height, show_fps);
			ground.init ("map/terrain_map.jpg");
			display.addText ("race: setup complete");

		}
		// GAME LOOP
		while (!get_end_level()) {
			eventManager ();							// input and event handle
			updatePosition ();						// update position
			renderFrame ();							// render scene
			calcFrameRate ();							// calculate fps (and limit fps)
		}

		// se il gioco è finito esco direttamente
		if (end_game)
			break;

		// altrimenti chiudo solo il livello normalmente
		// WAIT THREAD
		int return_status = 0;
		if(server) {
			SDL_WaitThread (server, &return_status);
			if (return_status != 0)
				printf ("Thread return status -> ERROR\n");
		}
		if(client) {
			SDL_WaitThread (client, &return_status);
			if (return_status != 0)
				printf ("Thread return status -> ERROR\n");
		}
		server = client = NULL;
		start_server = start_client = false;
		if (snet)		snet->~Server_Network ();	// close network object
		if (cnet)		cnet->~Client_Network ();	// close network object
		// RESET
		set_end_level(false);							// reset level flag
		ground.clear();
		display.clear();
		player.clear();
		for (int loop = 0; loop < MAX_CLIENT+1; loop++)
			if (enemy[loop].isActive())
				enemy[loop].clear();
	}

	return 0;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// funzione di renderizzazione del frame
void renderFrame (void) {
	SDL_GL_Init::render_mode ();				// enable 3D rendering

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();					// reset camera

	// CAMERA - calcola la posizione della camera rispetto al giocatore
	float speed = sqrt (player.x_speed*player.x_speed + player.z_speed*player.z_speed);
	v_dest.x = (fabs(speed * 0.1) + 20.0) * sin ((player.getAngle()-180) * PIOVER180);
	v_dest.y = 3.0 - 15.0 * (player.angle.x * PIOVER180);
	v_dest.z = (fabs(speed * 0.1) + 20.0) * cos ((player.getAngle()-180) * PIOVER180);
	x_offset = (v_dest.x - v_cam.x) * frame_interval * 7.0;
	y_offset = (v_dest.y - v_cam.y) * frame_interval * 14.0;
	z_offset = (v_dest.z - v_cam.z) * frame_interval * 7.0;
	v_cam.x += x_offset;
	v_cam.z += z_offset;
	v_cam.y += y_offset;
	gluLookAt (v_cam.x+player_x, v_cam.y+player_y, -v_cam.z+player_z,		player_x, player_y, player_z,		0.0, 1.0, 0.0);

	// RENDER OBJECTS
	// render ground
	glColor3f (1.0f, 1.0f, 1.0f);
	ground.render (player_x, player_z);
	// render sky
	glPushMatrix ();
		glTranslatef (player_x, -100.0f, player_z);
		sky.render (SKY_TEXTURE);
	glPopMatrix();

	// render player
	glPushMatrix ();
		glTranslatef (player_x, player_y, player_z);
		glRotatef (-player.getAngle(), 0.0, 1.0, 0.0);
		player.render (frame_interval);
	glPopMatrix();

	// render enemy
	float line_width, dist, x_enemy, y_enemy, z_enemy;
	for (int loop = 0; loop < MAX_CLIENT+1; loop++) {
		if (enemy[loop].isActive()) {
			// calcola distanza e dimensione linee
			enemy[loop].getPosition (&x_enemy, &y_enemy, &z_enemy);
			dist = calculateDistance (x_enemy, z_enemy, player_x, player_z);
			line_width = 1.5f + 1.0f / (dist / 15.0f);
			// render
			enemy[loop].render(frame_interval, line_width);
		}
	}

	// draw interface
	display.render (fps, frame_interval, (int)player.speed, 100);	// 100 = life
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// update posizioni e calcolo collisioni
void updatePosition (void) {
	float enemy_x, enemy_y, enemy_z;

	// update local player data
	player.getPosition (&player_x, &player_y, &player_z);
	current_height = ground.getHeight (player_x, player_z, player.angle_dest);		// get height and angle
	player.updatePosition (frame_interval, current_height);

	// update local enemy data
	for (int loop = 0; loop < MAX_CLIENT+1; loop++) {
		if (enemy[loop].isActive()) {
			enemy[loop].getPosition (&enemy_x, &enemy_y, &enemy_z);
			ground.getHeight (enemy_x, enemy_z, enemy[loop].angle_dest);// get height and angle
		}
	}

	// calculate frustum
	frustum.calculateFrustum ();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// setup generale
void setup (void) {
	// init SDL
	if (!SDL_Window.sdl_init ("Project: R.A.C.E.", screen_width, screen_height, screen_bpp, fullscreen)) {
		printf ("SDL_Init error: %s\n", SDL_GetError());
		exit (-1);
	}
	SDL_Window.opengl_init ();					// init opengl
	SDL_Window.resize_scene (400.0f);		// resize window
	printf ("SDL init... OK\n");

	// init SDL_net
	if(SDLNet_Init() == -1) {
		printf("SDLNet_Init error: %s\n", SDLNet_GetError());
		exit(-1);
	}
	printf ("SDLNet init... OK\n");

	// init mutex
	mutex = SDL_CreateMutex ();

	// load texture
	if (!texture.loadTexture ("textures/sdl_small_font.jpg") || !texture.loadTexture ("textures/sdl_large_font.jpg") ||
		 !texture.loadTexture ("textures/terrain.jpg") || !texture.loadTexture ("textures/terrain_detail.jpg") ||
		 !texture.loadTexture ("textures/sky.jpg") || !texture.loadTexture ("textures/blue_display_quad.jpg") ||
		 !texture.loadTexture ("textures/black_display_quad.jpg") || !texture.loadTexture ("textures/terrain_screen.jpg")) {
		printf ("Error loading texture :(\n");
		exit (-1);
	}
	printf ("Loading textures... OK\n");

	// init font
	font.init ();
	printf ("Init font... OK\n");

	// init sky structure
	sky.init (400.0f, 15.0f, 15.0f, 1.0f, 1.0f);

	// setup level light
	glLightfv (GL_LIGHT1, GL_AMBIENT, LightAmbient);		// setup for ambient light
	glLightfv (GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// setup for difuse light
	glLightfv (GL_LIGHT1, GL_SPECULAR, LightSpecular);		// setup for specular light
	glLightfv (GL_LIGHT1, GL_POSITION,LightPosition);		// light position
	glEnable (GL_LIGHT1);
	glDisable (GL_LIGHT0);
	// setup fog
	glClearColor (0.4f, 0.4f, 0.4f, 1.0f);
	glFogi(GL_FOG_MODE, GL_LINEAR);			// fog mode
	glFogfv(GL_FOG_COLOR, fogColor);			// fog color
	glFogf(GL_FOG_DENSITY, 0.35f);			// fog density
	glHint(GL_FOG_HINT, GL_DONT_CARE);		// fog hint value
	glFogf(GL_FOG_START, 305.0f);				// fog start
	glFogf(GL_FOG_END, 310.0f);				// fog end depth
	glEnable(GL_FOG);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// calcola il frame rate ed imposta le relative variabili
void calcFrameRate (void) {
	static Uint32 last_second, last_frame, this_frame;		// variabili per contenere i tempi tra un frame e l'altro
	static float wait_time = 16.50;								// tempo di attesa con fps limitati
	static int render_num;											// mantiene il numero di frame renderizzati in un sec

	this_frame = SDL_GetTicks();									// memorizza il tempo di questo frame
	frame_interval = (this_frame - last_frame) * 0.001f;	// salva l'intervallo in secondi tra i 2 frame ( < 0 )
	// FPS LIMITATI CHIAMANDO SDL_Delay ()
	if (limit_fps && (frame_interval < wait_time))			// 16,66 = 1000 / 60
		SDL_Delay (int(wait_time-frame_interval));

	last_frame = this_frame;							// imposta il tempo dell'ultimo frame al tempo corrente
	render_num++;											// incrementa il numero di frame renderizzati in questo secondo

	// se sono passati 1000 millisecondi (= 1 sec) imposta gli fps e resetta
	if ((this_frame-last_second) > 1000) {
		fps = render_num;									// salva il frame rate
		render_num = 0;									// riazzera il conteggio
		last_second = this_frame;						// memorizza l'ultimo reset

		if (limit_fps) {									// modifica il valore di wait per avere 60 fps
			if (fps > 62)		wait_time += 0.5f;
			if (fps > 70)		wait_time += 0.8f;
			if (fps < 58)	   wait_time -= 0.5f;
			if (fps < 50)	   wait_time -= 1.0f;
		}
	}

	// update screen
	glFlush ();
	SDL_GL_SwapBuffers ();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// funzione di gestione degli eventi
void eventManager (void) {
	SDL_Event event;							// variabile SDL per gestione eventi
	Uint8 *keys;								// variabile controllo tastiera

	//	EVENTS HANDLE
	while (SDL_PollEvent (&event)) {
		if (event.type == SDL_QUIT) {
			set_end_level (true);
			end_game = true;
		}
	}

	//	KEYBOARD HANDLE
	keys = SDL_GetKeyState (NULL);

	// handle movement
	if (keys[SDLK_UP]) {									// UP
		player.x_speed += 250.0 * sin (player.getAngle() * PIOVER180) * frame_interval;
		player.z_speed += 250.0 * cos (player.getAngle() * PIOVER180) * frame_interval;
	}

	if (keys[SDLK_DOWN]) {								// DOWN
		player.x_speed /= 1.0 + 10.0 * frame_interval;
		player.z_speed /= 1.0 + 10.0 * frame_interval;
	}

	if (keys[SDLK_LEFT]) {								// LEFT
		float turn = player.getTurnAngle();
		player.setAngle (player.getAngle()-(100.0 * frame_interval));
		if (turn <= 20.0)
			player.setTurnAngle (turn+(50.0 * frame_interval));
	}
	if (!keys[SDLK_LEFT]) {								// NOT LEFT
		float turn = player.getTurnAngle();
		if (turn >= 0.0)
			player.setTurnAngle (turn-(70.0 * frame_interval));
	}

	if (keys[SDLK_RIGHT]) {								// RIGHT
		float turn = player.getTurnAngle();
		player.setAngle (player.getAngle()+(100.0 * frame_interval));
		if (turn >= -20.0)
			player.setTurnAngle (turn-(50.0 * frame_interval));
	}
	if (!keys[SDLK_RIGHT]) {							// NOT RIGHT
		float turn = player.getTurnAngle();
		if (turn <= 0.0)
			player.setTurnAngle (turn+(70.0 * frame_interval));
	}

	//	quit
	if (keys[SDLK_ESCAPE])								// ESC
		set_end_level(true);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// free memory (destroy objects)
void clear_memory (void) {
	// WRITE CONFIG FILE
	if (!writeConfig(screen_width, screen_height, screen_bpp, fullscreen, limit_fps, server_udp_port))
		printf ("Error writing config file\n");

	printf ("\n----------RELEASE MEMORY----------\n");
	// CLOSE NETWORK THREAD
	if (server != NULL)		SDL_KillThread (server);
	if (client != NULL)		SDL_KillThread (client);
	printf ("Threads closed\n");

	// FREE MEMORY
	font.~Text();
	texture.~Texture_Manager();
	menu.~Menu();
	display.~Display();
	ground.~Terrain();
	sky.~Sky();
	player.~Player();
	printf ("Objetcs Memory Released\n");
	// free enemy
	for (int loop = 0; loop < MAX_CLIENT+1; loop++)
		if (enemy[loop].isActive())
			enemy[loop].~Enemy();
	printf ("Enemy Memory Released\n");

	// CLOSE SDL
	if (mutex)		SDL_DestroyMutex(mutex);	// close mutex
	SDLNet_Quit ();									// sdl_net quit
	SDL_Window.~SDL_GL_Init ();					// sdl quit
	printf ("SDL Quit\n");
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// thread per la gestione del gioco lato server
int serverThread (void *data) {
	bool pkt_received;						// true se sono stati ricevuti pkt in queto ciclo
	bool send_players_info = false;		// se true invia le info sui giocatori
	Uint32 send_info_timeout;				// se scade invia le info sui giocatori
	Uint32 send_timeout[MAX_CLIENT+1];	// timeout per intervallo tra l'invio dei pkt
	// var per gestire LOGIN
	char s_temp[100];
	int enemy_model_number[MAX_CLIENT+1];
	int player_num = 0;						// numero di giocatori connessi
	// var per gestire POSITION_UPDATE
	int id_temp, model_temp;
	float x_temp, y_temp, z_temp, angle_temp, turn_temp;

	// init network
	Server_Network *net = (Server_Network *) data;
	if (!net->init(server_udp_port)) {
		printf("Network init error\n");
		return -1;
	}
	printf("SERVER init... OK\n");
	display.addText ("server started...");

	// SEND/RECV LOOP
	while (true) {
		// controllo se il livello è finito
		if (get_end_level())
			break;

		pkt_received = false;					// reset flag

		// RECV
		char *string, *token;
		if ((string = net->recv ()) != NULL) {
			pkt_received = true;					// paccehtto ricevuto

			// analizzo la stringa ricevuta
			token = strtok (string,":");		// salto il numero del pkt
			int pkt_num = atoi(token);
			token = strtok (NULL,":");			// leggo il codice
			int pkt_code = atoi(token);
			switch (pkt_code) {
				case ACK:						// RICEVO UN MSG DI ACK -> possibile errore
					printf ("Warning - MAIN received ACK msg\n");
					break;
				case LOGIN:						// RICEVUTO LOGIN -> mandare ack
					token = strtok (NULL,":");	// salto la versione
					token = strtok (NULL,":");	// leggo model number
					model_temp = atoi(token);
					// controllo i giocatori connessi
					if (player_num > MAX_CLIENT+1)
						break;
					// se ci sono posti aggiungo il giocatore
					player_num++;
					// preparazione ed invio pkt di ack
					sprintf (s_temp, "0:%d:%d", pkt_num, player_num);
					net->send (s_temp, net->getClientNum());
					// salvo il numero del modello 3D da usare per il giocatore
					enemy_model_number[player_num] = model_temp;
					display.addText ("new player has joined");
					break;
				case POSITION_UPDATE:			// UPDATE POSIZIONE
					token = strtok (NULL,":");	// leggo ID giocatore
					id_temp = atoi(token);
					token = strtok (NULL,":");	// leggo coord X
					x_temp = atof(token);
					token = strtok (NULL,":");	// leggo coord Y
					y_temp = atof(token);
					token = strtok (NULL,":");	// leggo coord Z
					z_temp = atof(token);
					token = strtok (NULL,":");	// leggo angolo su y
					angle_temp = atof(token);
					token = strtok (NULL,":");	// leggo inclinazione
					turn_temp = atof(token);
					// controllo se il giocatore con l'id ricevuto è già attivo
					if (!enemy[id_temp].isActive()) {
						// creo il filename e inizializzo il giocatore
						sprintf (s_temp, "models/model_%d.txt", enemy_model_number[id_temp]);
						if (!enemy[id_temp].init (s_temp, "models/shader.txt", enemy_model_number[id_temp])) {
							printf ("Error loading models :(\n");
							break;
						}
						// setto la variabile per avvertire gli altri client
						send_players_info = true;
					}
					// aggiorno i dati
					enemy[id_temp].setPosition (x_temp, y_temp, z_temp);
					enemy[id_temp].setAngle (angle_temp);
					enemy[id_temp].setTurnAngle (turn_temp);
					break;
				case LOGOUT:						// RICEVUTO LOGOUT -> chiudere giocatore e avvertire gli altri
					token = strtok (NULL,":");	// leggo ID giocatore
					id_temp = atoi(token);
					enemy[id_temp].clear();		// chiudo il giocatore
					display.addText ("player disconnected :(");
					// avverto gli altri
					sprintf (s_temp, "3:%d", id_temp);
					net->sendAll (s_temp);
					break;
			}
			if (string)			free (string);
			if (token)			free (token);
		}

		// SEND
		// invia posizione giocatore - (SERVER)
		// invia dati giocatore
		// controlla timeout
		if (SDL_GetTicks() - send_timeout[0] > SEND_TIMEOUT) {
			player.getPosition (&x_temp, &y_temp, &z_temp);
			angle_temp = player.getAngle ();
			turn_temp = player.getTurnAngle ();
			// prepara il pkt
			sprintf (s_temp, "2:0:%f:%f:%f:%f:%f", x_temp, y_temp, z_temp, angle_temp, turn_temp);
			// invia il pkt
			if (!net->sendAll (s_temp))
				return -1;
			// setta nuovamente il timeout
			send_timeout[0] = SDL_GetTicks();
		}
		// invia dati altri giocatori - (CLIENT)
		for (int loop = 1; loop < MAX_CLIENT+1; loop++) {
			if (enemy[loop].isActive()) {
				// controlla timeout
				if (SDL_GetTicks() - send_timeout[loop] > SEND_TIMEOUT) {
					// prepara il pkt
					enemy[loop].getPosition (&x_temp, &y_temp, &z_temp);
					angle_temp = enemy[loop].getAngle ();
					turn_temp = enemy[loop].getTurnAngle ();
					sprintf (s_temp, "2:%d:%f:%f:%f:%f:%f", loop, x_temp, y_temp, z_temp, angle_temp, turn_temp);
					// invia il pkt
					if (!net->sendAll (s_temp))
						return -1;
					// setta nuovamente il timeout
					send_timeout[loop] = SDL_GetTicks();
				}
			}
		}
		// INVIO DATI GIOCATORI (MODELLO 3D - init value)
		if (send_players_info || (SDL_GetTicks() - send_info_timeout > SEND_INFO_TIMEOUT)) {
			// invia dati giocatore server
			sprintf (s_temp, "4:0:%d", player.getModelNumber());
			// invia il pkt
			if (!net->sendAll (s_temp))
				return -1;

			// invia dati giocatori client
			for (int loop = 1; loop < MAX_CLIENT+1; loop++) {
				if (enemy[loop].isActive()) {
					// prendi il nome del file
					sprintf (s_temp, "4:%d:%d", loop, enemy[loop].getModelNumber());
					// invia il pkt
					if (!net->sendAll (s_temp))
						return -1;
				}
			}
			// reset value
			send_players_info = false;
			send_info_timeout = SDL_GetTicks();
		}

		// se non ho ricevuto niente piccolo sleep
		if (!pkt_received)
			SDL_Delay (SERVER_SLEEP_TIMEOUT);
	}

	// LOGOUT
	printf ("SERVER -> Sending LOGOUT\n");
	sprintf (s_temp, "3:0");
	if (!net->sendAll (s_temp))
		return -1;

	return 0;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// thread per la gestione del gioco lato client
int clientThread (void *data) {
	bool pkt_received;			// true se sono stati ricevuti pkt in queto ciclo
	Uint32 send_timeout;			// timeout per intervallo tra l'invio dei pkt
	char s_temp[100];
	int player_id = -1;			// id giocatore corrente ricevuto dal server
	int id_temp, model_temp;
	float x_temp, y_temp, z_temp, angle_temp, turn_temp;

	// init network
	Client_Network *net = (Client_Network *) data;
	if (!net->init(menu.getAddress(), menu.getPort())) {
		printf("Network init error\n");
		return -1;
	}
	printf("CLIENT init... OK\n");
	display.addText ("client started...");

	// LOGIN
	char *login_ack, *token;
	sprintf (s_temp, "1:%f:%d", version, player.getModelNumber());
	if ((login_ack = net->sendWithAck (s_temp, 3, 100)) == NULL) {
		display.addText ("server not found");
		return -1;
	}
	display.addText ("login confirmed :)");
	// recupero il mio ID contenuto nell'ack al login
	token = strtok (login_ack,":");
	token = strtok (NULL,":");
	token = strtok (NULL,":");
	token = strtok (NULL,":");
	player_id = atoi(token);
	printf ("PLAYER ID: %d\n", player_id);
	// free temp memory
	if (login_ack)		free (login_ack);
	if (token)			free (token);

	SDL_Delay (200);

	// SEND/RECV LOOP
	while (true) {
		// controllo se il livello è finito
		if (get_end_level())
			break;

		pkt_received = false;					// reset flag

		// RECV
		char *string, *token;
		if ((string = net->recv ()) != NULL) {
			pkt_received = true;					// paccehtto ricevuto

			// analizzo la stringa ricevuta
			token = strtok (string,":");		// salto il numero del pkt
			int pkt_num = atoi(token);
			token = strtok (NULL,":");			// leggo il codice
			int pkt_code = atoi(token);
			switch (pkt_code) {
				case POSITION_UPDATE:			// UPDATE POSIZIONE
					token = strtok (NULL,":");	// leggo ID giocatore
					id_temp = atoi(token);
					token = strtok (NULL,":");	// leggo coord X
					x_temp = atof(token);
					token = strtok (NULL,":");	// leggo coord Y
					y_temp = atof(token);
					token = strtok (NULL,":");	// leggo coord Z
					z_temp = atof(token);
					token = strtok (NULL,":");	// leggo angolo su y
					angle_temp = atof(token);
					token = strtok (NULL,":");	// leggo inclinazione
					turn_temp = atof(token);
					// controllo se l'id è il mio
					if (id_temp == player_id)
						break;
					// controllo se il giocatore con l'id ricevuto è già attivo
					if (enemy[id_temp].isActive()) {
						// aggiorno la posizione
						enemy[id_temp].setPosition (x_temp, y_temp, z_temp);
						enemy[id_temp].setAngle (angle_temp);
						enemy[id_temp].setTurnAngle (turn_temp);
					}
					break;
				case LOGOUT:			// LOGOUT giocatore
					token = strtok (NULL,":");	// leggo ID giocatore
					id_temp = atoi(token);
					// controllo se viene dal server
					if (id_temp == 0) {
						set_end_level (true);
						break;
					}
					// se non viene dal server chiudo il giocatore
					enemy[id_temp].clear();		// chiudo il giocatore
					display.addText ("player disconnected :(");
					break;
				case MODEL_INFO:					// INFO GIOCATORE...init()
					token = strtok (NULL,":");	// leggo ID giocatore
					id_temp = atoi(token);
					token = strtok (NULL,":");	// leggo model number
					model_temp = atoi(token);
					// controllo se l'id è il mio
					if (id_temp == player_id)
						break;
					// controllo se il giocatore con l'id ricevuto è già attivo
					if (!enemy[id_temp].isActive()) {
						// creo il filename e inizializzo il giocatore
						sprintf (s_temp, "models/model_%d.txt", model_temp);
						if (!enemy[id_temp].init (s_temp, "models/shader.txt", model_temp)) {
							printf ("Error loading models :(\n");
							break;
						}
					}
					break;
			}
			if (string)			free (string);
			if (token)			free (token);
		}

		// SEND
		// controlla se è scaduto il timeout
		if (SDL_GetTicks() - send_timeout > SEND_TIMEOUT) {
			// recupera il dato
			player.getPosition (&x_temp, &y_temp, &z_temp);
			angle_temp = player.getAngle ();
			turn_temp = player.getTurnAngle ();
			// prepara il pkt
			sprintf (s_temp, "2:%d:%f:%f:%f:%f:%f", player_id, x_temp, y_temp, z_temp, angle_temp, turn_temp);
			// invia il pkt
			if (!net->send (s_temp))
				return -1;
			// setta nuovamente il timeout
			send_timeout = SDL_GetTicks();
		}

		// se non ho ricevuto niente piccolo sleep
		if (!pkt_received)
			SDL_Delay (CLIENT_SLEEP_TIMEOUT);
	}

	// LOGOUT
	printf ("PLAYER %d -> Sending LOGOUT\n", player_id);
	sprintf (s_temp, "3:%d", player_id);
	if (!net->send (s_temp))
		return -1;

	return 0;
}

void set_end_level (bool end) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Error LOCK mutex :(\n");	return;	}

	end_level = end;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Error UNLOCK mutex :(\n");	return;	}
}

bool get_end_level (void) {
	bool temp;

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Error LOCK mutex :(\n");	return false;	}

	temp = end_level;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Error UNLOCK mutex :(\n");	return false;	}

	return temp;
}
