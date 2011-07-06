//-------------		file			Rain_TNT.h		-----------------------------------------------------------------------------------

#ifndef Rain_TNT_H
#define Rain_TNT_H

#include "Rain_Includes.h"
#include "Rain_Fire.h"
#include "Rain_Enemy.h"

#define TNT_SIDE	0.2f

typedef struct part_list part_list;

class Rain_TNT
{
	public:
		typedef struct particleXPL {	float x, y, z;				// struttura per la singola particella
									float xs, ys, zs;			// velocità di spostamento nelle 3 direzioni
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

		Rain_TNT (float height);
		~Rain_TNT ();

		void Render_Xpl (float frame, bool pause);
		void Add_Xpl (const enemy_list *e, int tipo);
		void Add_Xpl (const fire_list *f, int tipo);
		void Delete_Xpl (part_list *p);
};

#endif