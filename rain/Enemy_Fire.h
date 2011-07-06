//-------------		file			Enemy_Fire.h		-----------------------------------------------------------------------------------

#ifndef Enemy_Fire_H
#define Enemy_Fire_H

#include "Rain_Includes.h"

typedef struct ef_list ef_list;

class Enemy_Fire
{
	public:
		struct ef_list {	ef_list *next, *prev;
						float x, z, angle_sin, angle_cos;
						bool x_pos, use_x_pos, drawn;
						bool destroy;
		};
		ef_list *start_ef;				// inizio della lista dei proiettili
		ef_list *loop_ef;				// variabile per la gestione dei cicli nel main

		GLUquadricObj *quadratic;			// puntatore per la gestione delle proprietà degli oggetti 3D
		GLuint ef_fire_list_1;				// variabili per la base delle liste di visualizzazione
		float ef_height, fire_ray;			// altezza e raggio dei colpi

		Enemy_Fire (float height, float raggio);
		~Enemy_Fire ();

		void Render_Enemy_Fire (float frame, bool pause);
		void Add_Enemy_Fire (float xs, float zs, float xe, float ze, float dist);
		void Enemy_Fire::Add_EF_M (float x, float z, float angle);
		void Delete_Enemy_Fire (ef_list *e);
		void Build_EF_List (void);
		void Delete_EF_List (void);
};

#endif