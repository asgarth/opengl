#include "FlockEngine.h"


FlockEngine::FlockEngine( int num ) {
	num_flock = num;
	flock = new Flock[ num ];
}

FlockEngine::~FlockEngine() {
	if ( flock )
		delete[] flock;
}

void FlockEngine::setup( bool use_2D_space ) {
	for( int loop = num_flock; --loop >= 0; )
		flock[ loop ].setup( loop, use_2D_space );
}

void FlockEngine::update( float frame_interval, Vector *dest, bool enable_mouse, bool collision_detection, bool flock_centering, bool flock_direction, bool random ) {
	for( int loop = num_flock; --loop >= 0; ) {
		Vector *increment = new Vector;
		increment->x = 0.0f;		increment->y = 0.0f;		increment->z = 0.0f;

		// FOLLOW MOUSE CURSOR
		if ( enable_mouse ) {
			Vector *temp = followMouse( frame_interval, &flock[ loop ], dest );
			increment->x += temp->x;			increment->y += temp->y;			increment->z += temp->z;
			delete temp;
		}

		// COLLISION DETECTION
		if ( collision_detection ) {
			Vector *temp = collisionAvoidance( frame_interval, &flock[ loop ] );
			increment->x += temp->x;			increment->y += temp->y;			increment->z += temp->z;
			delete temp;
		}

		// FLOCK CENTERING
		if ( flock_centering ) {
			Vector *temp = flockCentering( frame_interval, &flock[ loop ] );
			increment->x += temp->x;			increment->y += temp->y;			increment->z += temp->z;
			delete temp;
		}

		// DIRECTION ALIGNMENT
		if ( flock_direction ) {
			Vector *temp = directionAlignment( frame_interval, &flock[ loop ] );
			increment->x += temp->x;			increment->y += temp->y;			increment->z += temp->z;
			delete temp;
		}

		// RANDOM MOVEMENT
		if ( random && ( rand() % 4 == 0 ) ) {
			flock[ loop ].random.x = ( ( rand() % 100 ) - 50.0f ) * frame_interval * SPEED_RANDOM;
			flock[ loop ].random.y = ( ( rand() % 100 ) - 50.0f ) * frame_interval * SPEED_RANDOM;
			flock[ loop ].random.z = ( ( rand() % 100 ) - 50.0f ) * frame_interval * SPEED_RANDOM;
		}


		// UPDATE POSITION
		flock[ loop ].incrementSpeed( increment );		// increment speed
		flock[ loop ].update( frame_interval );				// update position
		delete increment;													// release memory
	}
}

void FlockEngine::render( float angle ) {
	for( int loop = num_flock; --loop >= 0; )
		flock[ loop ].render( angle );
}

Vector* FlockEngine::followMouse( float frame_interval, Flock *f, Vector *dest ) {
	Vector *v = new Vector;
	v->x = 0.0f;		v->y = 0.0f;		v->z = 0.0f;

	// compute distance
	Vector distance;
	distance.x = fabs( f->pos.x - dest->x );															// x distance
	distance.y = fabs( f->pos.y - dest->y );															// y distance
	distance.z = fabs( f->pos.z - dest->z );															// z distance
	float dist_2D = sqrt( distance.x * distance.x + distance.y * distance.y );	// 2D distance
	float dist = sqrt( dist_2D * dist_2D + distance.z * distance.z );					// 3D distance

	// update speed
	if ( dist > DISTANCE_OFFSET ) {
		v->x += ( ( dest->x - f->pos.x ) * frame_interval ) * SPEED_UP;
		v->y += ( ( dest->y - f->pos.y ) * frame_interval ) * SPEED_UP;
		v->z += ( ( dest->z - f->pos.z ) * frame_interval ) * SPEED_UP;
	}
	// brake (slowdown)
	if ( distance.x < DISTANCE_OFFSET )
		v->x -= f->speed.x * frame_interval * SPEED_DOWN;
	if ( distance.y < DISTANCE_OFFSET )
		v->y -= f->speed.y * frame_interval * SPEED_DOWN;
	if ( distance.z < DISTANCE_OFFSET )
		v->z -= f->speed.z * frame_interval * SPEED_DOWN;

	return v;
}

Vector* FlockEngine::collisionAvoidance( float frame_interval, Flock *f ) {
	Vector *v = new Vector;
	v->x = 0.0f;		v->y = 0.0f;		v->z = 0.0f;

	// reset flock collision data
	f->collision_detected = false;
	f->collision.left = false;			f->collision.right = false;		f->collision.up = false;
	f->collision.down = false;		f->collision.forward = false;	f->collision.back = false;

	// check collision
	for( int loop = num_flock; --loop >= 0; ) {
		if ( loop != f->id ) {
			Vector col;
			col.x = f->pos.x - flock[loop].pos.x;		col.y = f->pos.y - flock[loop].pos.y;		col.z = f->pos.z - flock[loop].pos.z;
			if ( fabs( col.x ) < COLLISION_DISTANCE && fabs( col.y ) < COLLISION_DISTANCE && fabs( col.z ) < COLLISION_DISTANCE ) {
				// check
				if ( fabs( col.x ) < (FLOCK_SIDE * 2) && fabs( col.y ) < (FLOCK_SIDE * 2) && fabs( col.z ) < (FLOCK_SIDE * 2) ) {
					f->collision_detected = true;
					if ( col.x > 0.0f ) {
						f->collision.left = true;
						if ( f->speed.x < flock[loop].speed.x )
							f->speed.x = flock[loop].speed.x / COLLISION_ESCAPE;
					}
					if ( col.x < 0.0f ) {
						f->collision.right = true;
						if ( f->speed.x > flock[loop].speed.x )
							f->speed.x = flock[loop].speed.x / COLLISION_ESCAPE;
					}
					if ( col.y > 0.0f ) {
						f->collision.down = true;
						if ( f->speed.y < flock[loop].speed.y )
							f->speed.y = flock[loop].speed.y / COLLISION_ESCAPE;
					}
					if ( col.y < 0.0f ) {
						f->collision.up = true;
						if ( f->speed.y > flock[loop].speed.y )
							f->speed.y = flock[loop].speed.y / COLLISION_ESCAPE;
					}
					if ( col.z > 0.0f ) {
						f->collision.back = true;
						if ( f->speed.z < flock[loop].speed.z )
							f->speed.z = flock[loop].speed.z / COLLISION_ESCAPE;
					}
					if ( col.z < 0.0f ) {
						f->collision.forward = true;
						if ( f->speed.z > flock[loop].speed.z )
							f->speed.z = flock[loop].speed.z / COLLISION_ESCAPE;
					}
				}
				// avoid
				else {
					if ( col.x > 0.0f )
						if ( f->speed.x < flock[loop].speed.x )
							f->speed.x = flock[loop].speed.x;
					if ( col.x < 0.0f )
						if ( f->speed.x > flock[loop].speed.x )
							f->speed.x = flock[loop].speed.x;
					if ( col.y > 0.0f )
						if ( f->speed.y < flock[loop].speed.y )
							f->speed.y = flock[loop].speed.y;
					if ( col.y < 0.0f )
						if ( f->speed.y > flock[loop].speed.y )
							f->speed.y = flock[loop].speed.y;
					if ( col.z > 0.0f )
						if ( f->speed.z < flock[loop].speed.z )
							f->speed.z = flock[loop].speed.z;
					if ( col.z < 0.0f )
						if ( f->speed.z > flock[loop].speed.z )
							f->speed.z = flock[loop].speed.z;
				}
			}
		}
	}

	return v;
}

Vector* FlockEngine::flockCentering( float frame_interval, Flock *f ) {
	Vector *v = new Vector;
	v->x = 0.0f;		v->y = 0.0f;		v->z = 0.0f;

	for( int loop = num_flock; --loop >= 0; ) {
		if ( loop != f->id ) {
			Vector dist;
			dist.x = f->pos.x - flock[loop].pos.x;		dist.y = f->pos.y - flock[loop].pos.y;		dist.z = f->pos.z - flock[loop].pos.z;

			if ( fabs( dist.x ) < ( CENTERING_DISTANCE ) && fabs( dist.y ) < ( CENTERING_DISTANCE ) && fabs( dist.z ) < ( CENTERING_DISTANCE ) ) {
				v->x -= ( f->pos.x - flock[loop].pos.x ) * frame_interval * CENTERING;
				v->y -= ( f->pos.y - flock[loop].pos.y ) * frame_interval * CENTERING;
				v->z -= ( f->pos.z - flock[loop].pos.z ) * frame_interval * CENTERING;
			}
		}
	}

	return v;
}

Vector* FlockEngine::directionAlignment( float frame_interval, Flock *f ) {
	Vector *v = new Vector;
	v->x = 0.0f;		v->y = 0.0f;		v->z = 0.0f;

	Vector avg;
	avg.x = 0.0f;		avg.y = 0.0f;		avg.z = 0.0f;
	int flock_count = 0;
	for( int loop = num_flock; --loop >= 0; ) {
		if ( loop != f->id ) {
			Vector dist;
			dist.x = fabs (f->pos.x - flock[loop].pos.x);
			dist.y = fabs (f->pos.y - flock[loop].pos.y);
			dist.z = fabs (f->pos.z - flock[loop].pos.z);
			if ( dist.x < ALIGNMENT_DISTANCE && dist.y < ALIGNMENT_DISTANCE && dist.z < ALIGNMENT_DISTANCE ) {
				flock_count++;
				avg.x += flock[loop].speed.x;
				avg.y += flock[loop].speed.y;
				avg.z += flock[loop].speed.z;
			}
		}
	}
	if ( flock_count > 0 ) {
		// average
		avg.x /= (float)flock_count;
		avg.y /= (float)flock_count;
		avg.z /= (float)flock_count;
		// align
		v->x -= ( f->speed.x - avg.x ) * ALIGNMENT * frame_interval;
		v->y -= ( f->speed.y - avg.y ) * ALIGNMENT * frame_interval;
		v->z -= ( f->speed.z - avg.z ) * ALIGNMENT * frame_interval;
	}

	return v;
}
