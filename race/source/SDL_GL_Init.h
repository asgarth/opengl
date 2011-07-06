/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef SDL_GL_INIT_H
#define SDL_GL_INIT_H

#include "includes.h"

class SDL_GL_Init {
	private:
		SDL_Surface *screen;								// variabile sdl x la superficie
		Uint32 video_flags;								// flag che conterrà le informazioni sull'inizzializzazione video

		int width, height, bpp;							// risoluzione finestra
		bool fullscreen;									// fullscreen

	public:
		SDL_GL_Init ();			// costruttore
		~SDL_GL_Init();

		// funzioni di inizializzazione
		bool sdl_init (const char *, int, int, int, bool);
		void opengl_init (void);
		void resize_scene (float);

		// funzioni cambio stato
		static void text_mode (int, int);		// use 2D mode
		static void render_mode (void);			// use 3D mode
		static void toggle_fullscreen (void);	// toggle fullscreen mode
};

#endif
