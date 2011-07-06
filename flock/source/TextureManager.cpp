#include "TextureManager.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

TextureManager::TextureManager (void) {
	// reset init array
	for (int loop = 0; loop < MAX_TEXTURES; loop ++)
		init_texture[loop] = false;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

TextureManager::~TextureManager () {
	for (int loop = 0; loop < MAX_TEXTURES; loop ++)
		if (init_texture[loop])
			glDeleteTextures (1, &g_Texture[loop]);

	delete g_Texture;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// cambia la texture correntemente utilizzata
bool TextureManager::setTexture (GLuint text_num) {
	if (text_num > MAX_TEXTURES || !init_texture[text_num]) {
		printf ("Tentativo cambio texture: %d -> FALLITO\n", text_num);
		return false;
	}

	glBindTexture (GL_TEXTURE_2D, g_Texture[text_num]);
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// crea una texture opengl da un file
bool TextureManager::loadTexture (const char *filename, int array_position) {
	SDL_Surface *image;
	image = IMG_Load_RW (SDL_RWFromFile(filename, "rb"), 1);

	if(!image) {
		printf ("\nErrore creazione texture da file: %s\n", filename);
		return false;
	}

	glGenTextures(1, &texture);									// crea la texture e assegna un id
	glBindTexture(GL_TEXTURE_2D, texture);			// seleziona la texture appena creata

	// applica alla texture il filtro MIPMAP
	gluBuild2DMipmaps (GL_TEXTURE_2D, 3, image -> w, image -> h, GL_RGB, GL_UNSIGNED_BYTE, image -> pixels);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	g_Texture[array_position] = texture;
	init_texture[array_position] = true;

	SDL_FreeSurface(image);
	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
