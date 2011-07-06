#include "Flock.h"

Flock::Flock (void) {
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
}

Flock::~Flock() {

}


void Flock::setup( int flock_id, bool space_2D ) {
	id = flock_id;
	use_2D_space = space_2D;

	pos.x = float ( ( rand() % 100 ) - 50.0f ) / 25.0f;
	pos.y = float ( ( rand() % 100 ) - 50.0f ) / 25.0f;
	if ( use_2D_space )					pos.z = 0.0f;
	else											pos.z = float ( ( rand() % 100 ) - 50.0f ) / 25.0f;

	speed.x = 0.0f;		speed.y = 0.0f;		speed.z = 0.0f;
	random.x = 0.0f;		random.y = 0.0f;		random.z = 0.0f;

	collision.left = false;			collision.right = false;
	collision.up = false;				collision.down = false;
	collision.forward = false;	collision.back = false;

	r = 1.0f;	g = 0.1f;		b = 0.1f;
}

void Flock::update( float frame_interval ) {
	// UPDATE POSITION
	pos.x += speed.x * frame_interval;
	pos.y += speed.y * frame_interval;
	if ( !use_2D_space )
		pos.z += speed.z * frame_interval;
}

void Flock::incrementSpeed( Vector *increment ) {
	// INCREMENT SPEED VECTOR
	speed.x += increment -> x + random.x;
	speed.y += increment -> y + random.y;
	if ( speed.x > MAX_SPEED)		speed.x = MAX_SPEED;
	if ( speed.x < -MAX_SPEED)		speed.x = -MAX_SPEED;
	if ( speed.y > MAX_SPEED)		speed.y = MAX_SPEED;
	if ( speed.y < -MAX_SPEED)		speed.y = -MAX_SPEED;

	if ( !use_2D_space ) {
		speed.z += increment -> z + random.z;
		if ( speed.z > MAX_SPEED)		speed.z = MAX_SPEED;
		if ( speed.z < -MAX_SPEED)		speed.z = -MAX_SPEED;
	}
}

void Flock::render( float angle ) {
	glPushMatrix();
		glTranslatef( pos.x, pos.y, pos.z );
		glRotatef( angle, 0.0f, 1.0f, 0.0f );
		glColor4f( r, g, b, 1.0f );
		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2d( 1, 1 ); 		glVertex3f( FLOCK_SIDE, FLOCK_SIDE, 0.0f );
			glTexCoord2d( 0, 1 ); 		glVertex3f( -FLOCK_SIDE, FLOCK_SIDE, 0.0f );
			glTexCoord2d( 1, 0 ); 		glVertex3f( FLOCK_SIDE, -FLOCK_SIDE, 0.0f );
			glTexCoord2d( 0, 0 ); 		glVertex3f( -FLOCK_SIDE, -FLOCK_SIDE, 0.0f );
		glEnd();

		if ( collision_detected || collision_warning )
			glColor4f( 0.1f, 1.0f, 0.1f, 0.5f );
		else
			glColor4f( 0.1f, 0.1f, 1.0f, 0.5f );

		gluSphere( quadratic, FLOCK_SIDE + 0.05f, 16, 16 );

		//glColor4f( 1.0f, 0.1f, 1.0f, 0.1f );
		//gluSphere( quadratic, 1.5f, 16, 16 );

	glPopMatrix();
}
