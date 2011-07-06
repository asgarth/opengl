/*--------------------------------------------------		Texture_Manager.h		----------------------------------------------------------*/

#ifndef Texture_Manager_H
#define Texture_Manager_H

#include "Code_Includes.h"

#define MAX_TEXT_NUM		1000

typedef struct {	GLubyte *imageData;	// dati immagine (32 bit)
						GLuint bpp;				// profondità del colore
						GLuint width, height;	// larghezza e altezza dell'immagine
						GLuint texID;			// ID della texture
}Texture_Image;

class Texture_Manager
{
	private:
		// vettore contenente gli indici che referenziano le texture
		GLuint referenze[MAX_TEXT_NUM];
		// contatore texture generate
		GLuint contatore;

	public:
		Texture_Manager ();
		~Texture_Manager ();

		bool LoadTGA (char *);
		GLuint Numero_Texture_Generate ();
		GLuint Usa_Texture (GLuint);
};

#endif