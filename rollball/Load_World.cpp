//-------------		file			Load_World.cpp		-----------------------------------------------------------------------------------

#include "Load_World.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Load_World::Load_World(char *filename) {
	file_level_name = filename;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Load_World::~Load_World () {	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Load_World::Setup_World ()
{
	static float x, z, u, v;
	static char oneline[255];
	FILE *filein;
	filein = fopen (file_level_name, "rt");

	// legge i salti a disposizione per finire il livello
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_SALTI %d\n", &num_salti);
	// numero di poligoni che compongono lo scenario
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_POLIGONI %d\n", &num_poligoni);
	// memoria per poligoni da disegnare
	pol = new POLYGON[num_poligoni];

	// lettura dati dei poligoni
	for (int loop = 0; loop < num_poligoni; loop++) {
		// legge i vertici
		for (int vert = 0; vert < 4; vert++) {
			readstr (filein, oneline);
			sscanf (oneline, "%f %f %f %f", &x, &z, &u, &v);
			pol[loop].vertex[vert].x = x;
			pol[loop].vertex[vert].z = z;
			pol[loop].vertex[vert].u = u;
			pol[loop].vertex[vert].v = v;
		}
		// legge dati addizionali del poligono
		readstr (filein, oneline);
		sscanf (oneline, "%f %d %d", &pol[loop].height, &pol[loop].blend, &pol[loop].texture);
	}

	// lettura delle coordinate dell'arrivo
	readstr (filein, oneline);
	sscanf (oneline, "ARRIVO %f %f %f %f\n", &x_f1, &x_f2, &z_f, &y_f);

	// lettura del creatore cel livello
	readstr (filein, oneline);
	readstr (filein, oneline);
	sscanf (oneline, "%s", &level_creator);

	// lettura del nome del file contenente il prossimo livello
	readstr (filein, oneline);
	readstr (filein, oneline);
	file_next_level = oneline;

	// chiude il file
	fclose (filein);
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Load_World::readstr (FILE *filename, char *string)
{
	do {
		fgets (string, 255, filename);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------