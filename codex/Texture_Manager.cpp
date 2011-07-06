/*--------------------------------------------------		Texture_Manager.cpp		----------------------------------------------------------*/

#include "Texture_Manager.h"

/*---------------------------------------------------------------------------------------------------------------------------------------*/

Texture_Manager::Texture_Manager () {	}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

Texture_Manager::~Texture_Manager () {	}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

bool Texture_Manager::LoadTGA (char *filename)
{
	Texture_Image *texture = (Texture_Image*) malloc (sizeof (Texture_Image));

	GLubyte TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0};	// header  per immagini TGA non compresse
	GLubyte TGAcompare[12];									// usato per comparare l'header
	GLubyte header[6];												// primi 6 byte utili dell'header
	GLuint bytesPerPixel;											// byte per pixel dell'immagine
	GLuint imageSize;												// dimensione dell'immagine
	GLuint temp;														// variabile temporanea
	GLuint type = GL_RGBA;										// modo di default (32 bpp)

	FILE *file = fopen (filename, "rb");						// file dell'immagine

	if(		file == NULL || 																					// controlla se il file esiste
			fread (TGAcompare, 1, sizeof (TGAcompare), file) != sizeof (TGAcompare) || // controlla se ci sono 12 byte da leggere
			memcmp (TGAheader, TGAcompare, sizeof (TGAheader)) != 0 || 					// controlla se l'header corrisponde
			fread (header, 1, sizeof (header), file) != sizeof (header)) 							// se l'header corrisponde legge altri 6 byte
	{
		if (file == NULL)			// se il file non esiste
			return false;
		else {
			fclose (file);
			return false;
		}
	}

	texture -> width = header [1] * 256 + header [0];			// determina la larghezza dell'immagine (highbyte * 256 + lowbyte)
	texture -> height = header [3] * 256 + header [2];		// determina l'altezza dell'immagine (highbyte * 256 + lowbyte)

	if (	texture -> width <= 0 ||	texture -> height <= 0 || 		// controlla che larghezza e altezza siano valide
			(header[4] != 24 && header[4] != 32))							// controlla che l'immagine sia a 24 o a 32 bit
	{
		fclose(file);
		return false;
	}

	texture -> bpp = header[4];															// prende i BIT per pixel dell'immagine
	bytesPerPixel = texture -> bpp / 8;												// divide per 8 per avere i BYTE per pixel
	imageSize = texture -> width * texture -> height * bytesPerPixel;		// calcola la dimensione dell'immagine

	texture -> imageData = (GLubyte *) malloc (imageSize);						// alloca la memoria necessaria

	if(		texture -> imageData == NULL ||											// la memoria non esiste?
			fread (texture -> imageData, 1, imageSize, file) != imageSize)		// la memoria coincide con quella richiesta?
	{
		if (texture -> imageData != NULL)								// se i dati dell'immagine sono stati caricati libera la memoria
			delete (texture -> imageData);

		fclose(file);
		return false;
	}

	for(GLuint i = 0; i < int (imageSize); i += bytesPerPixel) {			// scorre i dati dell'immagine
		// scambia il PRIMO e il TERZO byte
		temp = texture -> imageData[i];
		texture -> imageData[i] = texture -> imageData[i + 2];
		texture -> imageData[i + 2] = temp;
	}
	fclose (file);

	// costruisce la texture dai dati
	glGenTextures (1, &texture -> texID);						// genera l'ID della texture per openGL
	glBindTexture (GL_TEXTURE_2D, texture -> texID);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear filter
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // linear filter

	if (texture -> bpp == 24) 							//controlla se l'immagine è a 24 o 32 bit
		type = GL_RGB;

	glTexImage2D (GL_TEXTURE_2D, 0, type, texture -> width, texture -> height, 0, type, GL_UNSIGNED_BYTE, texture -> imageData);

	referenze[contatore] = texture -> texID;
	contatore ++;

	delete (texture);
	return true;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

GLuint Texture_Manager::Numero_Texture_Generate ()
{
	return contatore;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

GLuint Texture_Manager::Usa_Texture (GLuint text)
{
	glBindTexture (GL_TEXTURE_2D, referenze[text]);
	return referenze[text];
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
