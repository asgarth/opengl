/************************************************************
		Project: R.A.C.E.
		sergio sartorelli

		SDL_GL_Init ver 0.1		-			31/08/2005

		License: GPL
************************************************************/

#include "SDL_GL_Init.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

SDL_GL_Init::SDL_GL_Init () {
	screen = NULL;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

SDL_GL_Init::~SDL_GL_Init () {
	if (screen)
		SDL_FreeSurface(screen);		// free sdl surface
	SDL_Quit ();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// funzione di inizializzazione della grafica
bool SDL_GL_Init::sdl_init (const char *title, int w, int h, int b, bool fs) {
	width = w;
	height = h;
	bpp = b;
	fullscreen = fs;

	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {			// specifica quali librerie dinamiche caricare
		printf ("Error SDL_Init: %s\n", SDL_GetError());
		return false;
	}

	// ottiene le info sulla scheda video e imposta i valori della flag
	const SDL_VideoInfo *info_hw;			// struttura che memorizza cosa supporta la scheda grafica
	if ((info_hw =SDL_GetVideoInfo()) == NULL) {
		// se è impossibile -> errore
		printf ("Error hardware info: %s\n", SDL_GetError());
		return false;
	}
	if (info_hw -> hw_available)					// se sono supportate le superfici hardware le abilita
		video_flags = SDL_HWSURFACE;
	else													// altrimenti usa quelle software
		video_flags = SDL_SWSURFACE;
	if ( info_hw->blit_hw )							// hardware acceleration
		video_flags |= SDL_HWACCEL;
	if (fullscreen)									// fullscreen
		video_flags |= SDL_FULLSCREEN;
	// aggiunti flag di default x opengl, double buffer e palette hw
	video_flags |= SDL_OPENGL | SDL_HWPALETTE | SDL_GL_DOUBLEBUFFER;		// | SDL_NOFRAME;

	// settaggio attributi specifici opengl
	SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

	// creazione superficie
	if ((screen = SDL_SetVideoMode (width, height, bpp, video_flags)) == NULL) {
		// se la creazione della superficie fallisce -> errore
		printf ("Error init surface: %s\n", SDL_GetError());
		return false;
	}
	SDL_WM_SetCaption (title, NULL);			// imposta il titolo della finestra

	SDL_EnableUNICODE(1);						// abilità unicode
	SDL_ShowCursor (0);							// hide mouse cursor

	return true;									// tutto ok :)
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// funzione di inizzializzazione delle OpenGL
void SDL_GL_Init::opengl_init (void) {
	// setup opengl
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);						// sfondo nero
	glClearDepth (1.0f);												// setup depth buffer
	glDepthFunc (GL_LEQUAL);										// tipo di depth test
	glEnable (GL_DEPTH_TEST);										// abilita il depth test
	glEnable (GL_TEXTURE_2D);										// enable texture mapping
	glEnable (GL_COLOR_MATERIAL);									// possibilità di colorare le texture attivata
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);							// funzione per il blending
	glShadeModel (GL_SMOOTH);										// enable smooth shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// calcolo prospettiva
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// resize gl scene
void SDL_GL_Init::resize_scene (float distance) {
	if (height <= 0)	height = 1;						//se l'altezza è <= 0 diventa 1

	glViewport (0, 0, width, height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluPerspective (45.0f , (GLfloat)width/(GLfloat)height , 0.1f, distance);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// change video mode
void SDL_GL_Init::text_mode (int w, int h) {
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_LIGHTING);
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity();
	glOrtho (0, w, 0, h, -100, 100);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// change video mode
void SDL_GL_Init::render_mode (void) {
	// return to 3D mode
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
	glEnable (GL_LIGHTING);
	glEnable (GL_DEPTH_TEST);

	glDisable (GL_BLEND);
}
