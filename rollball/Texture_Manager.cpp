//-------------		file			Texture_Manager.cpp		-----------------------------------------------------------------------------------

#include "Texture_Manager.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Texture_Manager::Texture_Manager () {	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Texture_Manager::~Texture_Manager () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

AUX_RGBImageRec *Texture_Manager::LoadBMP (char *Filename)                   // carica immagine bitmap
{
      FILE *File=NULL;                                                  // File Handle
      if (!Filename)                                                          // controlla che sia stato dato un filename
            return NULL;

      File = fopen (Filename, "r");                                           // controlla se il file esiste
      if (File) {
            fclose (File);                                                    // chiude l' Handle
            return auxDIBImageLoad (Filename);                    // carica la bitmap e ritorna un puntatore
      }
      return NULL;                                                            // se il caricamento fallisce return NULL
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// genera una texture e restituisce l'indice che la referenzia all'interno del sistema

GLuint Texture_Manager::Load_BMP_Textures (char *Filename)
{
	AUX_RGBImageRec *TextureImage = NULL;						// puntatore di appoggio
	TextureImage = LoadBMP (Filename);

	if (TextureImage) {
		Genera_Texture (TextureImage);
		referenze[contatore] = referenza_corrente;
		Cancella_Memoria_Immagine (TextureImage);
		contatore++;
		return (contatore - 1);
	}
	else {
		MessageBox(NULL, " Caricamento file texture fallito :( ", "Genesis - ERROR", MB_OK | MB_ICONINFORMATION);
		return -1;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Texture_Manager::Genera_Texture (AUX_RGBImageRec *TextureImage)
{
	glGenTextures (1, &referenza_corrente);							// crea la texture con un nome identificativo
	glBindTexture (GL_TEXTURE_2D, referenza_corrente);

	// applica alla texture il filtro MIPMAP
	gluBuild2DMipmaps (GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Texture_Manager::Cancella_Memoria_Immagine (AUX_RGBImageRec *TextureImage)
{
	if (TextureImage) {                                      // se esiste la texture
            if (TextureImage->data)                            // se esiste l'immagine della texture
            	free (TextureImage->data);                   // libera la memoria
            free (TextureImage);                                     // libera la struttura dell'immagine
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLuint Texture_Manager::Numero_Texture_Generate ()
{
	return contatore;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLuint Texture_Manager::Usa_Texture (GLuint text)
{
	glBindTexture (GL_TEXTURE_2D, referenze[text]);
	return referenze[text];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------