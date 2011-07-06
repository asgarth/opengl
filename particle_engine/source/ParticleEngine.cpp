#include "ParticleEngine.h"


ParticleEngine::ParticleEngine( int num ) {
	num_paticle = num;
	particle = new Particle[ num ];
}

ParticleEngine::~ParticleEngine() {
	if ( particle )
		delete[] particle;
}

void ParticleEngine::setup() {
	for( int loop = 0; loop < num_paticle; loop++ ) {
		particle[ loop ].setup();
	}
}

void ParticleEngine::update( float frame_interval, Vector *grav ) {
	for( int loop = 0; loop < num_paticle; loop++ ) {
		particle[ loop ].update( frame_interval, grav );
	}
}

void ParticleEngine::render( float angle ) {
	for( int loop = 0; loop < num_paticle; loop++ ) {
		particle[ loop ].render( angle );
	}
}
