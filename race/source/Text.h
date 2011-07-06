/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef TEXT_H
#define TEXT_H

#include "includes.h"
#include "race_math.h"
#include "Texture_Manager.h"

class Text {
	private:
		Texture_Manager *tm;								// texture manager
		GLuint small_text_base, large_text_base;	// font display list

		// init font
		void buildSmallFont (void);
		void buildLargeFont (void);

	public:
		Text (Texture_Manager *);
		~Text();

		void init (void);												// init display list
		void drawText (int, int, const char *, int, bool);	// draw text
		void drawCenterText (int, int, const char *, int, bool);// draw text in the middle of the screen
};

#endif
