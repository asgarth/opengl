//-------------		file			Rain_XPL_DISK.h		-----------------------------------------------------------------------------------

#ifndef Rain_XPL_DISK_H
#define Rain_XPL_DISK_H

#include "Rain_Includes.h"
#include "Rain_Enemy.h"

typedef struct part_list part_list;

class Rain_XPL_DISK
{
	public:
		struct part_list {	part_list *next, *prev;					// struttura per una singola esplosione
							float x, y, z, speed;						// posizione e velocità
							float ray_1, ray_2, ray_3, ray_4, ray_5;	// raggi dei cerchi meno visibili (esterni)
							float life, fade;							// vita e velocità della morte
							float color_r, color_g, color_b;			// colore dell'esplosione
		};

		part_list *xpl_start;					// inizio della lista delle esplosioni
		float xpl_height;						// altezza a cui disegnare i colpi
		GLUquadricObj *quadratic;				// puntatore per la gestione delle proprietà degli oggetti 3D

		Rain_XPL_DISK (float height);
		~Rain_XPL_DISK ();

		void Render_Xpl (float frame, bool pause);
		void Add_Xpl (const enemy_list *e);
		void Delete_Xpl (part_list *p);
};

#endif