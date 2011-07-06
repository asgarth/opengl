//-------------		file			Rain_EXPL.h		-----------------------------------------------------------------------------------

#ifndef Rain_EXPL_H
#define Rain_EXPL_H

#include "Rain_Includes.h"
#include "Rain_Enemy.h"

#define EXPL_SIDE		1.0f

typedef struct part_list part_list;

class Rain_EXPL
{
	public:
		typedef struct particleXPL {	float x, y, z;				// struttura per la singola particella
									float xs, ys, zs;			// velocità di spostamento nelle 3 direzioni
									// angoli per la gestione dell'animazione
									float ax, ay, az, angle;
									float life, fade;			// vita e velocità della morte
		} particleX;
		struct part_list {	part_list *next, *prev;			// struttura per una singola esplosione
							float x, z, speed;
							float color_r, color_g, color_b;	// colore dell'esplosione
							particleX *partX;					// array dinamico contnente le particelle
							int num_part;					// numero di particelle che compongono le esplosioni
							bool destroy;
		};

		part_list *xpl_start;								// inizio della lista delle esplosioni
		float xpl_height;									// altezza a cui disegnare i colpi

		Rain_EXPL (float height);
		~Rain_EXPL ();

		void Render_Xpl (float frame, bool pause);
		void Add_Xpl (const enemy_list *e);
		void Delete_Xpl (part_list *p);
};

#endif