/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Texture_Manager.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Texture_Manager::Texture_Manager (void) {
	num_texture = 0;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Texture_Manager::~Texture_Manager () {
	if (num_texture > 0)
		glDeleteTextures (num_texture - 1, g_Texture);
	delete g_Texture;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// cambia la texture correntemente utilizzata
bool Texture_Manager::setTexture (GLuint text_num) {
	if (text_num > num_texture) {
		printf ("Tentativo cambio texture: %d -> FALLITO\n", text_num);
		return false;
	}

	glBindTexture (GL_TEXTURE_2D, g_Texture[text_num]);
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// crea una texture opengl da un file
bool Texture_Manager::loadTexture (const char *filename) {
	SDL_Surface *image;
	image = IMG_Load_RW (SDL_RWFromFile(filename, "rb"), 1);

	if(!image) {
		printf ("\nErrore creazione texture da file: %s\n", filename);
		return false;
	}

	glGenTextures(1, &texture);						// crea la texture e assegna un id
	glBindTexture(GL_TEXTURE_2D, texture);			// seleziona la texture appena creata

	// applica alla texture il filtro MIPMAP
	gluBuild2DMipmaps (GL_TEXTURE_2D, 3, image -> w, image -> h, GL_RGB, GL_UNSIGNED_BYTE, image -> pixels);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	g_Texture[num_texture] = texture;
	num_texture ++;

	SDL_FreeSurface(image);
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
