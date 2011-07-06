//-------------		file			Genesis_World.h		-----------------------------------------------------------------------------------

#ifndef Genesis_World_H
#define Genesis_World_H

#include "Genesis_Includes.h"

class Genesis_World
{
	public:
		typedef struct tagVERTEX { 	float x, y, z;
									float u, v;
		} VERTEX;
		typedef struct tagPOLYGON { 	int tipo_poligono;
										VERTEX vertex[4];
										int texture, blending, numero_divisioni;
										float altezza, raggio, raggio_cima, raggio_x_frustum;
										int sphere_x_rot, sphere_y_rot, sphere_z_rot;
										int text_roll_x, text_roll_y;
										float text_roll_vel, x_roll, y_roll, distanza_text_x, distanza_text_y;
		} POLYGON;
		typedef struct tagSECTOR{ 	int num_poligoni, num_poligoni_T;
									int num_alberi, num_pioggia, num_neve;
									POLYGON* poly;
									POLYGON* poly_T;
		} SECTOR;

		SECTOR	sector;

		char* file_world_name;				//variabile che memorizza il nome del file da cui viene caricato lo scenario

		Genesis_World (char *);				// costruttore
		virtual ~Genesis_World ();			// destructor

		// funzioni per la creazione del mondo da un file TXT
		void Conta_Poligoni ();
		void Setup_World ();
		void readstr (FILE *, char *);
};

#endif