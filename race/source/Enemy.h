/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef ENEMY_H
#define ENEMY_H

#define ROT_SPEED			5.0f

#include "includes.h"
#include "race_math.h"
#include "Object3D.h"

class Enemy : public Object3D {
	private:
		float x, y, z;
		float turn_angle;
		bool active;
		int model_number;

		SDL_mutex *mutex;

	public:
		float x_speed, z_speed, speed;
		Angle angle, angle_dest;
		float ray;
		float dest_height;

		Enemy ();								// costruttore
		~Enemy();

		// funzioni
		void render (float, float);
		void calcAngle (float);
		// init
		bool init (const char *, const char *, int);
		void clear (void);

		// parametri
		void getPosition (float *, float *, float *);	// ritorna la posizione
		void setPosition (float, float, float);
		float getAngle (void);									// ritorna l'angolo su y
		void setAngle (float);
		float getTurnAngle (void);								// ritorna l'inclinazione
		void setTurnAngle (float);
		int getModelNumber (void);								// ritorna il numero del file del modello 3D
		bool isActive (void);									// ritorna se l'oggetto è attivo
};

#endif
