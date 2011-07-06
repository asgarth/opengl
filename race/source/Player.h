/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef PLAYER_H
#define PLAYER_H

#define ROT_SPEED			5.0f

#include "includes.h"
#include "race_math.h"
#include "Object3D.h"

class Player : public Object3D {
	private:
		float x, y, z;
		float turn_angle;
		int model_number;

		SDL_mutex *mutex;

	public:
		float x_speed, z_speed, speed;
		Angle angle, angle_dest;
		float ray;
		float dest_height;

		Player ();								// costruttore
		~Player();

		// funzioni
		void render (float);
		void updatePosition (float, float);
		void calcAngle (float);
		// init
		bool init (const char *, const char *, int);
		void clear (void);

		// parametri
		void getPosition (float *, float *, float *);	// ritorna la posizione
		float getAngle (void);									// ritorna l'angolo su y
		void setAngle (float);
		float getTurnAngle (void);								// ritorna l'inclinazione
		void setTurnAngle (float);
		int getModelNumber (void);								// ritorna il numero del file del modello 3D
};

#endif
