//-------------		file			Texture_Manager.h		-----------------------------------------------------------------------------------

#ifndef Texture_Manager_H
#define Texture_Manager_H

#include "Genesis_Includes.h"

#define MAX_TEXT_NUM		1000

class Texture_Manager
{
	private:
		// vettore contenente gli indici che referenziano le texture
		GLuint referenze[MAX_TEXT_NUM];
		GLuint referenza_corrente;
		// contatore texture generate
		GLuint contatore;

		AUX_RGBImageRec *LoadBMP (char *);
		void Genera_Texture (AUX_RGBImageRec *);
		void Cancella_Memoria_Immagine (AUX_RGBImageRec *);

	public:
		Texture_Manager ();
		~Texture_Manager ();

		GLuint Load_BMP_Textures (char *);
		GLuint Numero_Texture_Generate ();
		GLuint Usa_Texture (GLuint);
};

#endif