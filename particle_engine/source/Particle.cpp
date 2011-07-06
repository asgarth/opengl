#include "Particle.h"


Particle::Particle (void) {

}

Particle::~Particle() {

}


void Particle::setup() {
	life = float ( rand() % 100 ) / 100.0f;
	fade = float ( rand() % FADE_MAX ) / 10000.0f + FADE_MIN;
	x = 0.0f;		y = 0.0f;		z = 0.0f;
	xs = ( float ( rand() % MAX_SPEED ) - MAX_SPEED / 2 ) / 100.0f;
	ys = ( float ( rand() % MAX_SPEED ) - MAX_SPEED / 2 ) / 100.0f;
	zs = ( float ( rand() % MAX_SPEED ) - MAX_SPEED / 2 ) / 100.0f;
	r = float ( rand() % 100 ) / 100.0f;
	g = float ( rand() % 100 ) / 100.0f;
	b = float ( rand() % 100 ) / 100.0f;
}

void Particle::update( float frame_interval, Vector *grav ) {
	// speed
	x += xs * frame_interval;
	y += ys * frame_interval;
	z += zs * frame_interval;

	// gravity
	xs -= grav->x * frame_interval;
	ys -= grav->y * frame_interval;
	zs -= grav->z * frame_interval;

	// life
	life -= fade * frame_interval;
	if ( life < 0.0f )
		setup();
}

void Particle::render( float angle ) {
	glPushMatrix();
		glTranslatef( x, y, z );
		glRotatef( angle, 0.0f, 1.0f, 0.0f );
		glColor4f( r, g, b, life );
		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2d( 1, 1 ); 		glVertex3f( PARTICLE_SIDE, PARTICLE_SIDE, 0.0f );
			glTexCoord2d( 0, 1 ); 		glVertex3f( -PARTICLE_SIDE, PARTICLE_SIDE, 0.0f );
			glTexCoord2d( 1, 0 ); 		glVertex3f( PARTICLE_SIDE, -PARTICLE_SIDE, 0.0f );
			glTexCoord2d( 0, 0 ); 		glVertex3f( -PARTICLE_SIDE, -PARTICLE_SIDE, 0.0f );
		glEnd();
	glPopMatrix();
}
