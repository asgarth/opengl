//-------------		file			Particle_Engine.h		-----------------------------------------------------------------------------------

#ifndef Particle_Engine_H
#define Particle_Engine_H

#include "Genesis_Includes.h"

class Particle_Engine
{
	public:
		GLuint color, delay;							// colore e ritardo nel cambio di colore

		typedef struct tagPARTICLES {	bool active;				// particella attiva (si/no)
										float life;
										float fade;				// varibile per decrementare life
										float r, g, b;				// RGB del colore
										float x, y, z;				// coordinate della particella
										float xi, yi, zi;			// movimento della particella sui 3 assi
		} PARTICLES;
		typedef struct tagENGINE {	PARTICLES* particle;			// struttura per memorizzare i dati delle particelle
									GLuint num_particle;
									float xspeed, yspeed, zspeed;		// velocità sui due assi
									float x_coord, y_coord, z_coord;	// coordinate in cui disegnare il particle engine
									float lato, slowdown, fade_MIN;
									GLuint fade_MAX;
									float xgrav, ygrav, zgrav;			// valori della forza di gravità su x y z
									GLuint tipo;						// tipo di particle engine
		} ENGINE;

		ENGINE* engine;

		char* engine_file_name;				//variabile che memorizza il nome del file da cui viene caricato lo scenario
		GLuint num_particle_engine;


		Particle_Engine (char *);				// costruttore
		~Particle_Engine ();					// destructor

		void Setup_Particle_Engine ();
		void Setup_Engine_Number ();
		void readstr (FILE *, char *);
};

#endif