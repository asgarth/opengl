/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "includes.h"
#include "texture.h"

#define MAX_TEXTURES		100

class Texture_Manager {
	private:
		unsigned int g_Texture[MAX_TEXTURES];
		GLuint texture;
		GLuint num_texture;									// numero di texture inizzializzate, usato x indicizzare l'array

	public:
		Texture_Manager (void);								// costruttore
		~Texture_Manager();									// destructor

		// funzioni
		bool setTexture (GLuint text_num);
		bool loadTexture (const char *filename);		// crea una texture da un'immagine
};

#endif
