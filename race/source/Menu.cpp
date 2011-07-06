/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Menu.h"

#define BORDER_OFFSET	15
#define UP_OFFSET			80
#define DOWN_OFFSET		100
#define LEFT_OFFSET		35

#define NUM_MODELS		3

/* ---------------------------------------------------------------------------------------------------------------------- */

Menu::Menu (Texture_Manager *main_tm, Text *text, Terrain *t, Frustum *f, Sky *s) {
	tm = main_tm;
	font = text;
	ground = t;
	frustum = f;
	sky = s;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Menu::~Menu () {
	if (ip_string)			free (ip_string);
	if (port_string)		free (port_string);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init
void Menu::init (int w, int h) {
	index = join_index = opt_index = 0;
	sub_menu = 0;
	opt_menu_model = 1;
	close_menu = false;

	// reset string
	if (ip_string)			free (ip_string);
	if (port_string)		free (port_string);
	// init ip string
	ip_string = (char *) malloc (sizeof(char) * 20);
	if (ip_string == NULL) { printf("Error allocating memory :(");	}
	sprintf (ip_string, "");
	// init ip port
	port_string = (char *) malloc (sizeof(char) * 10);
	if (port_string == NULL) { printf("Error allocating memory :("); }
	sprintf (port_string, "");
	ip_stop_writing = port_stop_writing = false;

	ground->init ("map/terrain_map.jpg");
	sky->init (400.0f, 15.0f, 15.0f, 1.0f, 1.0f);

	width = w;
	height = h;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init
void Menu::clear (void) {
	if (ground)				ground->clear();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// show
void Menu::show (void) {
	while (!close_menu) {
		eventManager ();								// input and event handle
		render_terrain ();
		draw_black_screen();
		switch (sub_menu) {
			case 0:	draw_main();					// MAIN
						break;
			case 1:	draw_select_map();			// PRACTICE
						break;
			case 2:	draw_select_map();			// HOST GAME
						break;
			case 3:	draw_join();					// JOIN GAME
						break;
			case 4:	draw_option();					// OPTIONS
						break;
			case 5:	close_menu = true;			// QUIT
						break;
		}
		calcFrameRate ();
	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Menu::draw_black_screen (void) {
	SDL_GL_Init::text_mode (width, height);// enable 2D text mode
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_TEXTURE_2D);
	// draw black window
	tm -> setTexture (DISPLAY_BLACK_TEXTURE);
	glColor4f (1.0f, 1.0f, 1.0f, 0.7f);
	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);		glVertex2i (BORDER_OFFSET, BORDER_OFFSET);
		glTexCoord2f (1.0, 0.0);		glVertex2i (width-BORDER_OFFSET, BORDER_OFFSET);
		glTexCoord2f (1.0, 1.0);		glVertex2i (width-BORDER_OFFSET, height-BORDER_OFFSET);
		glTexCoord2f (0.0, 1.0);		glVertex2i (BORDER_OFFSET, height-BORDER_OFFSET);
	glEnd();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Menu::draw_main (void) {
	// draw text
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glColor4f (1.0, 1.0, 1.0, 1.0);
	if (index == 0)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else						glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawText (LEFT_OFFSET, height - UP_OFFSET, "Practice", 0, false);

	if (index == 1)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else						glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawText (LEFT_OFFSET, height - 150, "Host Game", 0, false);

	if (index == 2)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else						glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawText (LEFT_OFFSET, height - 220, "Join Game", 0, false);

	if (index == 3)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else						glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawText (LEFT_OFFSET, height - 290, "Options", 0, false);

	if (index == 4)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else						glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawText (LEFT_OFFSET, height - 440, "Quit", 0, false);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Menu::draw_select_map (void) {
	// draw text
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawCenterText (width, height - UP_OFFSET, "Select Map", 0, false);

	// draw map screenshot
	// TEMP - va rifatto bene con possibilità di scegliere
	tm -> setTexture (TERRAIN_GREEN_SCREEN);
	glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);		glVertex2i (width/2-150, height - UP_OFFSET - 30);
		glTexCoord2f (1.0, 0.0);		glVertex2i (width/2+150, height - UP_OFFSET - 30);
		glTexCoord2f (1.0, 1.0);		glVertex2i (width/2+150, height - UP_OFFSET - 255);
		glTexCoord2f (0.0, 1.0);		glVertex2i (width/2-150, height - UP_OFFSET - 255);
	glEnd();

	font -> drawCenterText (width, DOWN_OFFSET, "Start", 0, false);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Menu::draw_join (void) {
	// draw text
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawCenterText (width, height - UP_OFFSET, "Select Server", 0, false);


	if (join_index == 0)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else							glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawText (LEFT_OFFSET, height - 150, "IP:", 0, false);
	font -> drawText (200, height - 150, ip_string, 0, false);

	if (join_index == 1)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else							glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawText (LEFT_OFFSET, height - 220, "Port:", 0, false);
	font -> drawText (200, height - 220, port_string, 0, false);


	if (join_index == 2)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else							glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawCenterText (width, DOWN_OFFSET, "JOIN", 0, false);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Menu::draw_option (void) {
	static char model_string[20];
	sprintf (model_string, "MODEL %d", opt_menu_model);

	// draw text
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawCenterText (width, height - UP_OFFSET, "Select model", 0, false);

	if (opt_index == 0)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else							glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawCenterText (width, height - 150, model_string, 0, false);

	if (opt_index == 1)		glColor4f (1.0, 0.2, 0.2, 1.0);
	else							glColor4f (1.0, 1.0, 1.0, 1.0);
	font -> drawCenterText (width, DOWN_OFFSET, "ACCEPT", 0, false);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render background terrain
void Menu::render_terrain (void) {
	static float angle;
	SDL_GL_Init::render_mode ();				// enable 3D rendering
	glDisable (GL_BLEND);

	frustum->calculateFrustum ();				// calculate frustum

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();							// reset camera

	// camera
	gluLookAt (0.0f, 10.0f, 1.0f,		0.0f, 10.0f, 0.0f,		0.0, 1.0, 0.0);

	// render ground
	glColor3f (1.0f, 1.0f, 1.0f);
	glRotatef (angle, 0.0, 1.0, 0.0);
	ground->render (0.0f, 0.0f);
	// render sky
	glPushMatrix ();
		glTranslatef (0.0f, -100.0f, 0.0f);
		sky->render (SKY_TEXTURE);
	glPopMatrix();

	angle += 10.0f * frame_interval;
	if (angle >= 360.0f)
		angle -= 360.0f;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// return index
int Menu::getSelection (void) {
	return index;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// return ip
char* Menu::getAddress (void) {
	char *temp = (char *) malloc (sizeof(char) * 20);

	temp = strcpy (temp, ip_string);
	return temp;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// return port
int Menu::getPort (void) {
	return atoi(port_string);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// return model number
int Menu::getModel (void) {
	return opt_menu_model;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// gestione tastiera
void Menu::eventManager (void) {
	SDL_Event event;							// variabile SDL per gestione eventi

	//	EVENTS HANDLE
	while (SDL_PollEvent (&event)) {
		switch(event.type) {
			case SDL_QUIT:
				close_menu = true;
				index = 4;							// 4 = QUIT
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_UP) {
					switch (sub_menu) {
						case 0:	index--;								// MAIN
							if (index < 0)
								index = 4;
							break;
						case 3:	join_index--;						// JOIN GAME
							if (join_index < 0)
								join_index = 1;
							break;
						case 4:	opt_index--;						// OPTION
							if (opt_index < 0)
								opt_index = 1;
							break;
					}
				}
				if(event.key.keysym.sym == SDLK_DOWN) {
					switch (sub_menu) {
						case 0:	index++;								// MAIN
							if (index > 4)
								index = 0;
							break;
						case 3:	join_index++;						// JOIN GAME
							if (join_index > 2)
								join_index = 0;
							break;
						case 4:	opt_index++;						// OPTION
							if (opt_index > 1)
								opt_index = 0;
							break;
					}
				}
				if(event.key.keysym.sym == SDLK_LEFT) {
					switch (sub_menu) {
						case 4:	opt_menu_model--;					// OPTION
							if (opt_menu_model < 1)
								opt_menu_model = NUM_MODELS;
							break;
					}
				}
				if(event.key.keysym.sym == SDLK_RIGHT) {
					switch (sub_menu) {
						case 4:	opt_menu_model++;					// OPTION
							if (opt_menu_model > NUM_MODELS)
								opt_menu_model = 1;
							break;
					}
				}
				if(event.key.keysym.sym == SDLK_RETURN) {
					switch (sub_menu) {
						case 0:	sub_menu = index+1;				// MAIN
									break;
						case 4:	if (opt_index == 1)
										sub_menu = 0;
									break;								// OPTION

						default:	close_menu = true;
									break;
					}
				}
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					if (sub_menu != 0) {					// torno al menu principale
						sub_menu = 0;
						// azzera eventuali indirizzi ip
						sprintf (ip_string, "");
						sprintf (port_string, "");
					}
					else {									// quit
						index = 4;
						close_menu = true;
					}
				}

				// CHARACTER HANDLER
				if (sub_menu == 3) {
					// handle delete key
					int len = strlen(ip_string);
					if (join_index == 0 && len > 0 && event.key.keysym.sym == SDLK_BACKSPACE) {
						ip_string[len-1] = ip_string[len];
						ip_stop_writing = false;
					}
					len = strlen(port_string);
					if (join_index == 1 && len > 0 && event.key.keysym.sym == SDLK_BACKSPACE) {
						port_string[len-1] = port_string[len];
						port_stop_writing = false;
					}

					// handle other keys
					char *current_string = NULL;
					if (join_index == 0 && !ip_stop_writing)
						current_string = ip_string;
					if (join_index == 1 && !port_stop_writing)
						current_string = port_string;

					if (current_string == NULL)
						break;

					if (event.key.keysym.sym == SDLK_0)
						sprintf (current_string, "%s%s", current_string, "0");
					if (event.key.keysym.sym == SDLK_1)
						sprintf (current_string, "%s%s", current_string, "1");
					if (event.key.keysym.sym == SDLK_2)
						sprintf (current_string, "%s%s", current_string, "2");
					if (event.key.keysym.sym == SDLK_3)
						sprintf (current_string, "%s%s", current_string, "3");
					if (event.key.keysym.sym == SDLK_4)
						sprintf (current_string, "%s%s", current_string, "4");
					if (event.key.keysym.sym == SDLK_5)
						sprintf (current_string, "%s%s", current_string, "5");
					if (event.key.keysym.sym == SDLK_6)
						sprintf (current_string, "%s%s", current_string, "6");
					if (event.key.keysym.sym == SDLK_7)
						sprintf (current_string, "%s%s", current_string, "7");
					if (event.key.keysym.sym == SDLK_8)
						sprintf (current_string, "%s%s", current_string, "8");
					if (event.key.keysym.sym == SDLK_9)
						sprintf (current_string, "%s%s", current_string, "9");
					if (join_index == 0 && event.key.keysym.sym == SDLK_PERIOD)
						sprintf (current_string, "%s%s", current_string, ".");

					// fixed string length
					if (strlen (ip_string) > 14)
						ip_stop_writing = true;
					if (strlen (port_string) > 4)
						port_stop_writing = true;
				}

				break;
		}
	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// calcola il frame rate ed imposta le relative variabili
void Menu::calcFrameRate (void) {
	static float last_second, last_frame, this_frame;		// variabili per contenere i tempi tra un frame e l'altro
	static int render_num;											// mantiene il numero di frame renderizzati in un sec
	static float wait_time = 16.50;

	this_frame = SDL_GetTicks();									// memorizza il tempo di questo frame
	frame_interval = (this_frame - last_frame) * 0.001f;	// salva l'intervallo in secondi tra i 2 frame ( < 0 )
	// max fps = 60
	//if (frame_interval < wait_time)								// 16,66 = 1000 / 60
	//	SDL_Delay(int(wait_time-frame_interval));

	last_frame = this_frame;							// imposta il tempo dell'ultimo frame al tempo corrente
	render_num++;											// incrementa il numero di frame renderizzati in questo secondo

	// se sono passati 1000 millisecondi (= 1 sec) imposta gli fps e resetta
	if ((this_frame-last_second) > 1000) {
		render_num = 0;									// riazzera il conteggio
		last_second = this_frame;						// memorizza l'ultimo reset

		if (render_num > 62)		wait_time += 0.5f;
		if (render_num > 70)		wait_time += 0.5f;
		if (render_num < 58)	   wait_time -= 0.5f;
		if (render_num < 50)	   wait_time -= 0.5f;
	}

	glFlush ();
	SDL_GL_SwapBuffers ();
}
