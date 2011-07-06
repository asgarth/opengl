/************************************************************
		Sistemi Intelligenti - Flock

		Sergio Sartorelli
************************************************************/

#ifndef FLOCK_H
#define FLOCK_H

#include "includes.h"

typedef struct { bool left, right, up, down, forward, back;			// COLLISION
} Collision;

#define MAX_SPEED				17.0f
#define FLOCK_SIDE				0.25f


class Flock {
	public:
		int id;									// id del flock
		Vector pos;						// coordinate del flock
		Vector speed;					// velocità sui 3 assi
		Vector random;				// velocità casuale
		Collision collision;				// collisione sui 3 assi
		float r, g, b;						// RGB del colore

		bool collision_detected, collision_warning;
		bool use_2D_space;

		GLUquadricObj *quadratic;


		Flock();
		~Flock();

		void setup( int, bool );
		void update( float );
		void incrementSpeed( Vector* );
		void render( float );
};

#endif
