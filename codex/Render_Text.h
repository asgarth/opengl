/*--------------------------------------------------		Render_Text.h		----------------------------------------------------------*/

#ifndef Render_Text_H
#define Render_Text_H

#include "Code_Includes.h"

class Render_Text
{
	public:
		Render_Text ();
		~Render_Text ();

		void Render_String (String_Code *s, float);
		void Render_Vertical_String (String_Code *, float);
		void Build_Font (void);
		void Build_Vertical_Font (void);
		void Kill_Font (void);

	private:
		GLuint base1, base2, base3, base4;					// variabile per liste di viusalizzazione dei font
		GLuint Vbase1, Vbase2, Vbase3, Vbase4;			// variabile per liste di viusalizzazione dei font in verticale

		int Build_Font_List (float);
		int Build_Vertical_Font_List (float);
		void glPrint (String_Code *, int);
};

#endif