/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "includes.h"
#include "race_math.h"
#include "Texture_Manager.h"
#include "Text.h"

#define MAX_STRING	6
#define POS_X			(width - 340)

class Display {
	private:
		int width, height;		// screen resolution
		Texture_Manager *tm;		// texture manager
		Text *font;					// font manager
		SDL_mutex *mutex;

		char text[MAX_STRING][100];
		int text_index;

	public:
		bool show_fps;

		Display (Texture_Manager *, Text *);	// costruttore
		~Display();

		void init (int, int, bool);
		void render (int, float, int, int);		// render func
		void addText (const char *);				// add text line
		void clear (void);
};

#endif
