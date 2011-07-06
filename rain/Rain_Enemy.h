//-------------		file			Rain_Enemy.h		-----------------------------------------------------------------------------------

#ifndef Rain_Enemy_H
#define Rain_Enemy_H

#include "Rain_Includes.h"

typedef struct enemy_list {	enemy_list *next, *prev;
							int tipe, energie;
							float x, z, ray, speed;
							float angle, angle_speed;
							float color_r, color_g, color_b;
							float enemy_fire_time;
							bool destroy, drawn, power_up;
} enemy_list;

class Rain_Enemy
{
	public:
		enemy_list *start_enemy;			// inizio della lista dei proiettili
		enemy_list *loop_enemy;				// variabile per la gestione dei cicli nel main

		float enemy_height;
		GLUquadricObj *quadratic;
		int detail_level;

		Rain_Enemy (float height);
		~Rain_Enemy ();

		void Render_Enemy (float frame, bool pause);
		void Add_Enemy (float x, float z, float raggio, int tipe, bool p_up);
		void Delete_Enemy (enemy_list *e);
};

#endif