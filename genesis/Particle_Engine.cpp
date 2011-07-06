//-------------		file			Particle_Engine.cpp		-----------------------------------------------------------------------------------

#include "Particle_Engine.h"

int loop, loop_engine;						// variabili per gestire i cicli for

// imposta 12 colori sfumati dal rosso al viola
static GLfloat colors [12][3] = {	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
								{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
								{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}	};

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Particle_Engine::Particle_Engine(char *filename) {
	engine_file_name = filename;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Particle_Engine::~Particle_Engine () {	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// setup iniziale dell'array delle particelle

void Particle_Engine::Setup_Particle_Engine ()
{
	Setup_Engine_Number ();

	for (loop_engine = 0; loop_engine < num_particle_engine; loop_engine++)
	{
		for (loop = 0; loop < engine[loop_engine].num_particle; loop++)
		{
			engine[loop_engine].particle[loop].active = true;						// attiva la particella
			engine[loop_engine].particle[loop].life = float (rand()%100) / 100.0f;		// vita della particella
			// imposta il valore con cui la particella muore
			engine[loop_engine].particle[loop].fade = (rand()%engine[loop_engine].fade_MAX) / 1000.0f + engine[loop_engine].fade_MIN;
			switch (engine[loop_engine].tipo)
			{
				case 1:	// impostazione della velocità nelle 3 direzioni
						engine[loop_engine].particle[loop].xi = engine[loop_engine].xspeed + float ( (rand()%200 - 100) / 50.0f);
						engine[loop_engine].particle[loop].yi = engine[loop_engine].yspeed + float ( (rand()%200 - 100) / 50.0f);
						engine[loop_engine].particle[loop].zi = engine[loop_engine].zspeed + float ( (rand()%200 - 100) / 50.0f);
						// impostazione dei colori in modo da avere valori compresi tra 0 e 12 a seconda del valore di loop
						engine[loop_engine].particle[loop].r = colors[ (loop + 1) / (engine[loop_engine].num_particle / 12) ] [0];
						engine[loop_engine].particle[loop].g = colors[ (loop + 1) / (engine[loop_engine].num_particle / 12) ] [1];
						engine[loop_engine].particle[loop].b = colors[ (loop + 1) / (engine[loop_engine].num_particle / 12) ] [2];
					break;
				case 2:	// impostazione della velocità nelle 3 direzioni
						engine[loop_engine].particle[loop].xi = engine[loop_engine].xspeed + float ( (rand()%200 - 100) / 50.0f);
						engine[loop_engine].particle[loop].yi = engine[loop_engine].yspeed + float ( (rand()%200 - 100) / 50.0f);
						engine[loop_engine].particle[loop].zi = engine[loop_engine].zspeed + float ( (rand()%200 - 100) / 50.0f);
					break;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Particle_Engine::Setup_Engine_Number ()
{
	FILE *filein;
	char oneline[255];
	filein = fopen (engine_file_name, "rt");

	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_ENGINE %d\n", &num_particle_engine);
	engine = new ENGINE[num_particle_engine];

	for (loop = 0; loop < num_particle_engine; loop++)
	{
		readstr (filein, oneline);
		sscanf (oneline, "NUMERO_PARTICELLE_ENGINE %d\n", &engine[loop].num_particle);
		engine[loop].particle = new PARTICLES[engine[loop].num_particle];
		readstr (filein, oneline);
		sscanf (oneline, "%d", &engine[loop].tipo);
		readstr (filein, oneline);
		sscanf (oneline, "%f %f %d %f", &engine[loop].slowdown, &engine[loop].lato, &engine[loop].fade_MAX, &engine[loop].fade_MIN);
		engine[loop].lato /= 100.0f;
		engine[loop].slowdown *= 1000;
		readstr (filein, oneline);
		sscanf (oneline, "%f %f %f", &engine[loop].x_coord, &engine[loop].y_coord, &engine[loop].z_coord);
		readstr (filein, oneline);
		sscanf (oneline, "%f %f %f", &engine[loop].xgrav, &engine[loop].ygrav, &engine[loop].zgrav);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Particle_Engine::readstr (FILE *filename, char *string)
{
	do {
		fgets (string, 255, filename);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------