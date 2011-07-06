/************************************************************
		Sistemi Intelligenti - Flock

		Sergio Sartorelli
************************************************************/

#ifndef FLOCK_ENGINE_H
#define FLOCK_ENGINE_H

#include "includes.h"
#include "Flock.h"

#define SPEED_UP					2.5f
#define SPEED_DOWN			8.0f
#define DISTANCE_OFFSET	3.0f

#define COLLISION_DISTANCE		(FLOCK_SIDE * 2 + 0.2f)
#define COLLISION_ESCAPE			4.0f

#define CENTERING							0.2f
#define CENTERING_DISTANCE		3.0f

#define ALIGNMENT						2.0f
#define ALIGNMENT_DISTANCE	3.0f

#define SPEED_RANDOM				0.5f


class FlockEngine {
	public:
		int num_flock;
		Flock *flock;


		FlockEngine( int );
		~FlockEngine();

		void setup( bool );
		void update( float, Vector*, bool, bool, bool, bool, bool );
		void render( float );
		Vector* followMouse( float, Flock*, Vector* );
		Vector* collisionAvoidance( float, Flock* );
		Vector* flockCentering( float, Flock* );
		Vector* directionAlignment( float, Flock* );
};

#endif
