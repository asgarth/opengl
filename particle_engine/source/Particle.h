/************************************************************
		Sistemi Intelligenti - Flock

		Sergio Sartorelli
************************************************************/

#ifndef PARTICLE_H
#define PARTICLE_H

#include "includes.h"

#define FADE_MIN			0.2f
#define FADE_MAX			35
#define MAX_SPEED		500
#define PARTICLE_SIDE	0.3f

class Particle {
	public:
		float life;							// vita rimanente
		float fade;							// variabile per decrementare life
		float x, y, z;						// coordinate della particella
		float xs, ys, zs;				// velocità sui 3 assi
		float r, g, b;						// RGB del colore


		Particle();
		~Particle();

		void setup();
		void update( float, Vector* );
		void render( float );
};

#endif
