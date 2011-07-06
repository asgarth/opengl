/*--------------------------------------------------		Render_Text.cpp		----------------------------------------------------------*/

#include "Render_Text.h"

/*---------------------------------------------------------------------------------------------------------------------------------------*/

Render_Text::Render_Text ()
{

}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

Render_Text::~Render_Text () {	}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Render_Text::Render_String (String_Code *s, float frame)
{
	int base_size;
	switch (s -> size) {
		case 1:	base_size = base1;
					break;
		case 2:	base_size = base2;
					break;
		case 3:	base_size = base3;
					break;
		case 4:	base_size = base4;
					break;
		default:	base_size = base1;
					break;
	}
	//glPrint (String_Code *s, int base)
	glPrint (s, base_size);
	s -> x_pos += s -> x_vel * frame;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Render_Text::Render_Vertical_String (String_Code *s, float frame)
{
	int base_size;
	switch (s -> size) {
		case 1:	base_size = Vbase1;
					break;
		case 2:	base_size = Vbase2;
					break;
		case 3:	base_size = Vbase3;
					break;
		case 4:	base_size = Vbase4;
					break;
		default:	base_size = Vbase1;
					break;
	}
	//glPrint (String_Code *s, int base)
	glPrint (s, base_size);
	s -> y_pos += s -> x_vel * frame;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Render_Text::Build_Font ()
{
	float cx, cy;
	base1 = Build_Font_List (0.8);
	base2 = Build_Font_List (1.0);
	base3 = Build_Font_List (1.2);
	base4 = Build_Font_List (1.4);

	Vbase1 = Build_Vertical_Font_List (0.8);
	Vbase2 = Build_Vertical_Font_List (1.0);
	Vbase3 = Build_Vertical_Font_List (1.2);
	Vbase4 = Build_Vertical_Font_List (1.4);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

int Render_Text::Build_Font_List (float size)
{
	float cx, cy;
	float temp_size = 16 * size;
	GLuint base = glGenLists (256);
	for (int loop = 0; loop < 256; loop++) {
		cx = float(loop % 16) / 16.0f;
		cy = float(loop / 16) / 16.0f;

		glNewList (base + loop, GL_COMPILE);
			glBegin (GL_QUADS);
				glTexCoord2f(cx, 1-cy-0.0625f); 					// Texture Coord (Bottom Left)
				glVertex2f(0.0, 0.0); 									// Vertex Coord (Bottom Left)
				glTexCoord2f(cx+0.0625f, 1-cy-0.0625f); 		// Texture Coord (Bottom Right)
				glVertex2f(temp_size, 0.0); 							// Vertex Coord (Bottom Right)
				glTexCoord2f(cx+0.0625f, 1-cy);					// Texture Coord (Top Right)
				glVertex2f(temp_size, temp_size); 				// Vertex Coord (Top Right)
				glTexCoord2f(cx, 1-cy); 								// Texture Coord (Top Left)
				glVertex2f(0.0, temp_size); 							// Vertex Coord (Top Left)
			glEnd();
			glTranslatef(10.0 * size, 0.0, 0.0);						// Move To The Right Of The Character
		glEndList();
	}
	return base;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

int Render_Text::Build_Vertical_Font_List (float size)
{
	float cx, cy;
	float temp_size = 16 * size;
	GLuint base = glGenLists (256);
	for (int loop = 0; loop < 256; loop++) {
		cx = float(loop % 16) / 16.0f;
		cy = float(loop / 16) / 16.0f;

		glNewList (base + loop, GL_COMPILE);
			glBegin (GL_QUADS);
				glTexCoord2f(cx, 1-cy-0.0625f); 					// Texture Coord (Bottom Left)
				glVertex2f(0.0, 0.0); 									// Vertex Coord (Bottom Left)
				glTexCoord2f(cx+0.0625f, 1-cy-0.0625f); 		// Texture Coord (Bottom Right)
				glVertex2f(temp_size, 0.0); 							// Vertex Coord (Bottom Right)
				glTexCoord2f(cx+0.0625f, 1-cy);					// Texture Coord (Top Right)
				glVertex2f(temp_size, temp_size); 				// Vertex Coord (Top Right)
				glTexCoord2f(cx, 1-cy); 								// Texture Coord (Top Left)
				glVertex2f(0.0, temp_size); 							// Vertex Coord (Top Left)
			glEnd();
			glTranslatef(0.0, 10.0 * size, 0.0);						// Move To The Right Of The Character
		glEndList();
	}
	return base;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Render_Text::Kill_Font ()
{
	glDeleteLists (base1, 256);					// cancella le liste di visualizzazione
	glDeleteLists (base2, 256);
	glDeleteLists (base3, 256);
	glDeleteLists (base4, 256);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Render_Text::glPrint (String_Code *s, int base)
{
	glPushMatrix ();
		glTranslatef (s -> x_pos, s -> y_pos, 0);
		glListBase (base - 32 + 128);
		if (strlen (s -> string) > 0) {
			glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
			glCallLists (1, GL_BYTE, s -> string);
			glColor4f (s -> r, s -> g, s -> b, 1.0f);
			glCallLists (strlen (s -> string) - 1, GL_BYTE, s -> string + 1);
		}
	glPopMatrix ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
