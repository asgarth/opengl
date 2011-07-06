/************************************************************
		Sistemi Intelligenti - Flock

		Sergio Sartorelli
************************************************************/

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "includes.h"

#define MAX_TEXTURES		100

class TextureManager {
	private:
		unsigned int g_Texture[MAX_TEXTURES];	// memoria texture
		bool init_texture[MAX_TEXTURES];			// texture inizializzate
		GLuint texture;

	public:
		TextureManager (void);						// costruttore
		~TextureManager();							// destructor

		// funzioni
		bool setTexture (GLuint text_num);
		bool loadTexture (const char *, int);	// crea una texture da un'immagine e la salva nella posizione indicata
};

#endif
