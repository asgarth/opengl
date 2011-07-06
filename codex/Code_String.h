/*--------------------------------------------------		Code_String.h		----------------------------------------------------------*/

#ifndef Code_String_H
#define Code_String_H

#include "Code_Includes.h"

class Code_String
{
	public:
		Code_String ();
		~Code_String ();

		void Get_String (String_Code*, int, bool);
		void Set_Resolution (int, int);
		void Setup_Code_Text (void);				// riempie l'array delle stringhe
		void Dealloc_Text (void);						// cancella la memoria

	private:
		int scr_w, scr_h;
		char *codez_text[CODE_LENGTH];
		int codez_text_position[CODE_LENGTH][CODE_LENGTH];

		void readstr (FILE *, char *);				// legge una nuova stringa da un file di testo
		void copystr (char*, char*);					// copia una stringa eliminando '\n' e '\t'
};

#endif