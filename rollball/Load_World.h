//-------------		file			Load_World.h		-----------------------------------------------------------------------------------

#ifndef Load_World_H
#define Load_World_H

#include "Rain_Includes.h"

class Load_World
{
	public:
		typedef struct tagVERTEX { 	float x, z;							// coordinate
											float u, v;							// coordinate texture
		} VERTEX;
		typedef struct tagPOLYGON {	VERTEX vertex[4];				// vertici del quadrato
											float height;						// altezza del poligono
											int texture, blend;				// texture e blending (se attivo)
		} POLYGON;

		int num_poligoni, num_salti;			// numero di poligoni che compongono il livello
		POLYGON* pol;						// array che contiene i poligoni
		float x_f1, x_f2, z_f, y_f;			// posizione del traguardo

		char* file_level_name;				//variabile che memorizza il nome del file da cui viene caricato il livello
		char* file_next_level;				// variabile che memorizza il file del prossimo livello
		char level_creator[50];				// nome di chi ha aggiunto il livello

		Load_World (char *);
		~Load_World ();

		// funzioni per la creazione del mondo da un file TXT
		void Setup_World (void);
		void readstr (FILE *, char *);
};

#endif