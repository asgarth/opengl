//-------------		file			Rain_Fire.h		-----------------------------------------------------------------------------------

#ifndef Rain_Fire_H
#define Rain_Fire_H

#include "Rain_Includes.h"

typedef struct fire_list {	fire_list *next, *prev;
							float x, z, angle;
							float x_sp, z_sp;
							int power;
							bool destroy;
} fire_list;

class Rain_Fire
{
	public:
		fire_list *fire_start;				// inizio della lista dei proiettili
		fire_list *loop_fire;					// variabile per la gestione dei cicli nel main
		bool fire_left;						// il colpo viene sparato a sinistra o a destra

		GLUquadricObj *quadratic;			// puntatore per la gestione delle proprietà degli oggetti 3D
		GLuint fire_gl_list;					// variabili per la base delle liste di visualizzazione
		int fire_type, fire_power;

		float fire_height;					// altezza a cui disegnare i colpi
		// angoli predefiniti (convertiti in radianti)
		float cos_15, sin_15, cos_25, sin_25;

		Rain_Fire (float height);
		~Rain_Fire ();

		void Aggiorna_Fire (float frame, bool pause);
		void Add_Fire (float shipx, float shipz);
		void Add_F_Angle (float x, float z, float vel, int angle);
		void Delete_Fire (fire_list *f);
		void Build_Fire_List (void);
		void Delete_Fire_List (void);
};

#endif