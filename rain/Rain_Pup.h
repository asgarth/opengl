//-------------		file			Rain_Pup.h		-----------------------------------------------------------------------------------

#ifndef Rain_Pup_H
#define Rain_Pup_H

#include "Rain_Includes.h"

typedef struct part_list part_list;

class Rain_Pup
{
	public:
		struct part_list {	part_list *next, *prev;			// struttura per una singola esplosione
							float x, z, ray, angle;
							int type;						// tipo del power_up
							bool destroy;
		};

		part_list *xpl_start, *loop_p_up;
		float xpl_height;					// altezza a cui disegnare

		GLUquadricObj *quadratic;			// puntatore per la gestione delle proprietà degli oggetti 3D

		Rain_Pup (float height);
		~Rain_Pup ();

		void Render_Power_Up (float frame, bool pause);
		void Add_Power_Up (float x, float z);
		void Delete_Power_Up (part_list *p);
};

#endif