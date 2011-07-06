/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Text.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Text::Text (Texture_Manager *main_tm) {
	tm = main_tm;											// set texture manager
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Text::~Text () {
	glDeleteLists (small_text_base, 256);		// delete font list
	glDeleteLists (large_text_base, 256);		// delete font list
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init
void Text::init (void) {
	buildSmallFont ();
	buildLargeFont ();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// stampa la stringa passata nelle coordinate specificate
void Text::drawText (int x, int y, const char *string, int set, bool small_text) {
	// controllo parametri
	if (set > 1)		set = 1;
	if (set < 0)		set = 0;

	glPushMatrix ();
		glTranslated (x, y, 0);
		if (small_text) {
			tm -> setTexture (TEXTURE_SMALL_FONT);
			glListBase (small_text_base - 32 + (128 * set));
			glCallLists (strlen(string), GL_BYTE, string);
		}
		else {
			tm -> setTexture (TEXTURE_LARGE_FONT);
			glListBase (large_text_base - 32 + (128 * set));
			glCallLists (strlen(string), GL_BYTE, string);
		}
	glPopMatrix ();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// stampa la stringa passata nelle coordinate specificate
void Text::drawCenterText (int width, int y, const char *string, int set, bool small_text) {
	int x;
	float len;

	len = (float) strlen (string);
	if (small_text)
		x = (width/2) - (int)((len/2.0f)*10);
	else
		x = (width/2) - (int)((len/2.0f)*30);

	drawText (x, y, string, set, small_text);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// costruisce la lista di visualizzazione dei font (SMALL)
void Text::buildSmallFont (void) {
	float cx, cy;
	small_text_base = glGenLists (256);

	for (int loop = 0; loop < 256; loop++) {
		cx = float (loop % 16) / 16.0f;
		cy = float (loop / 16) / 16.0f;
		// inizio lista visualizzazione
		glNewList (small_text_base + loop, GL_COMPILE);
			glBegin (GL_QUADS);
				glTexCoord2f (cx, 1-cy-0.0625f); 			// Texture Coord (Bottom Left)
				glVertex2i (0, 0); 								// Vertex Coord (Bottom Left)
				glTexCoord2f (cx+0.0625f, 1-cy-0.0625f); 	// Texture Coord (Bottom Right)
				glVertex2i (16, 0); 								// Vertex Coord (Bottom Right)
				glTexCoord2f (cx+0.0625f, 1-cy);				// Texture Coord (Top Right)
				glVertex2i (16, 16); 							// Vertex Coord (Top Right)
				glTexCoord2f (cx, 1-cy); 						// Texture Coord (Top Left)
				glVertex2i (0, 16); 								// Vertex Coord (Top Left)
			glEnd();
			glTranslated(10, 0, 0); 							// Move To The Right Of The Character
		glEndList();
	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// costruisce la lista di visualizzazione dei font (LARGE)
void Text::buildLargeFont (void) {
	float cx, cy;
	large_text_base = glGenLists (256);

	for (int loop = 0; loop < 256; loop++) {
		cx = float (loop % 16) / 16.0f;
		cy = float (loop / 16) / 16.0f;
		// inizio lista visualizzazione
		glNewList (large_text_base + loop, GL_COMPILE);
			glBegin (GL_QUADS);
				glTexCoord2f (cx, 1-cy-0.0625f); 			// Texture Coord (Bottom Left)
				glVertex2i (0, 0); 								// Vertex Coord (Bottom Left)
				glTexCoord2f (cx+0.0625f, 1-cy-0.0625f);	// Texture Coord (Bottom Right)
				glVertex2i (32, 0); 								// Vertex Coord (Bottom Right)
				glTexCoord2f (cx+0.0625f, 1-cy);				// Texture Coord (Top Right)
				glVertex2i (32, 32); 							// Vertex Coord (Top Right)
				glTexCoord2f (cx, 1-cy); 						// Texture Coord (Top Left)
				glVertex2i (0, 32); 								// Vertex Coord (Top Left)
			glEnd();
			glTranslated(26, 0, 0); 							// Move To The Right Of The Character
		glEndList();
	}
}
