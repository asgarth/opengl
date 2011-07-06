/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Display.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Display::Display (Texture_Manager *main_tm, Text *f) {
	tm = main_tm;											// set texture manager
	font = f;												// set font manager
	text_index = 0;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Display::~Display () {
	if (mutex)		SDL_DestroyMutex(mutex);		// close mutex
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init
void Display::init (int w, int h, bool fps) {
	// init mutex
	mutex = SDL_CreateMutex ();
	// set screen resolution
	width = w;
	height = h;
	// set fps on screen
	show_fps = fps;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Display::render (int fps, float frame_interval, int speed, int life) {
	static char temp_string[15];					// temp string
	SDL_GL_Init::text_mode (width, height);	// enable 2D text mode
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_TEXTURE_2D);

	// draw msg window
	tm -> setTexture (DISPLAY_TEXTURE);
	glColor4f (1.0, 1.0, 1.0, 0.7);
	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);		glVertex2i (POS_X, height-120);
		glTexCoord2f (1.0, 0.0);		glVertex2i (width+10, height-120);
		glTexCoord2f (1.0, 1.0);		glVertex2i (width+10, height-10);
		glTexCoord2f (0.0, 1.0);		glVertex2i (POS_X, height-10);
	glEnd();

	// draw speed window
	tm -> setTexture (DISPLAY_TEXTURE);
	glColor4f (1.0, 1.0, 1.0, 0.7);
	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);		glVertex2i (width-240, 10);
		glTexCoord2f (1.0, 0.0);		glVertex2i (width+10, 10);
		glTexCoord2f (1.0, 1.0);		glVertex2i (width+10, 65);
		glTexCoord2f (0.0, 1.0);		glVertex2i (width-240, 65);
	glEnd();

	// draw weapons window
	tm -> setTexture (DISPLAY_TEXTURE);
	glColor4f (1.0, 1.0, 1.0, 0.7);
	glBegin (GL_QUADS);
		glTexCoord2f (0.0, 0.0);		glVertex2i (-10, 10);
		glTexCoord2f (1.0, 0.0);		glVertex2i (240, 10);
		glTexCoord2f (1.0, 1.0);		glVertex2i (240, 65);
		glTexCoord2f (0.0, 1.0);		glVertex2i (-10, 65);
	glEnd();

	// draw text
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	glColor4f (1.0, 1.0, 1.0, 1.0);
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}
	// draw string
	for (int loop = 0; loop < MAX_STRING; loop++)
		font->drawText (POS_X + 10, height - 27 - (loop*18), text[loop], 1, true);
	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}

	// draw speed text
	font->drawText (width-205, 48, "SPEED", 0, true);
	font->drawText (width-75, 48, "LIFE", 0, true);
	glColor4f (0.7+(speed/200.0f), 0.3+(speed/500.0f), 0.3+(speed/500.0f), 1.0);
	sprintf (temp_string, "%d", speed);
	font->drawText (width-180-strlen(temp_string)*13, 12, temp_string, 0, false);
	glColor4f (0.3+(speed/500.0f), 0.3+(speed/500.0f), 0.7+(speed/200.0f), 1.0);
	sprintf (temp_string, "%d", life);
	font->drawText (width-55-strlen(temp_string)*13, 12, temp_string, 0, false);

	//draw weapons text
	glColor4f (1.0, 1.0, 1.0, 1.0);
	font->drawText (25, 48, "WEAPON", 0, true);
	font->drawText (140, 48, "SPECIAL", 0, true);


	// draw FPS text
	if (show_fps) {
		glBlendFunc (GL_SRC_ALPHA, GL_ONE);
		glColor4f (0.2, 1.0, 0.4, 1.0);
		sprintf (temp_string, "FPS: %d", fps);
		font->drawText (10, height - 20, temp_string, 0, true);
	}

	// return to 3D mode
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
	glEnable (GL_DEPTH_TEST);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// add text string
void Display::addText (const char *string) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}

	if (strlen(string) > 100)
		return;

	// slide string array
	for (int loop = 1; loop < MAX_STRING; loop++) {
		strcpy (text[loop-1], text[loop]);
	}

	// copy new string at the end
	strcpy (text[MAX_STRING-1], string);

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init
void Display::clear (void) {
	for (int loop = 0; loop < MAX_STRING; loop++) {
		strcpy (text[loop], "");
	}

	if (mutex)		SDL_DestroyMutex(mutex);		// close mutex
}
