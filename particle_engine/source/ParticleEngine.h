/************************************************************
		Sistemi Intelligenti - Flock

		Sergio Sartorelli
************************************************************/

#ifndef PARTICLE_ENGINE_H
#define PARTICLE_ENGINE_H

#include "includes.h"
#include "Particle.h"

class ParticleEngine {
	public:
		int num_paticle;
		Particle *particle;


		ParticleEngine( int );
		~ParticleEngine();

		void setup();
		void update( float, Vector* );
		void render( float );
};

#endif
