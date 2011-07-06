//-------------		file			Genesis_World.cpp		-----------------------------------------------------------------------------------

#include "Genesis_World.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Genesis_World::Genesis_World(char *filename) {
	file_world_name = filename;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Genesis_World::~Genesis_World () {	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_World::Conta_Poligoni ()
{
	FILE *filein;
	char oneline[255];
	filein = fopen (file_world_name, "rt");

	int poligoni, poligoni_T;
	int poligoni_da_leggere, blending;

	readstr (filein, oneline);
	readstr (filein, oneline);
	readstr (filein, oneline);
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_TRIANGOLI %d\n", &poligoni_da_leggere);
	for (int loop = 0; loop < poligoni_da_leggere; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0)
			poligoni ++;
		else
			poligoni_T ++;
		readstr (filein, oneline);		readstr (filein, oneline);
		readstr (filein, oneline);		readstr (filein, oneline);
	}

	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_QUADRATI %d\n", &poligoni_da_leggere);
	for (int loop = 0; loop < poligoni_da_leggere; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0)
			poligoni ++;
		else
			poligoni_T ++;
		readstr (filein, oneline);		readstr (filein, oneline);
		readstr (filein, oneline);		readstr (filein, oneline);
		readstr (filein, oneline);
	}

	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_QUADRATI_ANIMATI %d\n", &poligoni_da_leggere);
	for (int loop = 0; loop < poligoni_da_leggere; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0)
			poligoni ++;
		else
			poligoni_T ++;
		readstr (filein, oneline);		readstr (filein, oneline);
		readstr (filein, oneline);		readstr (filein, oneline);
		readstr (filein, oneline);		readstr (filein, oneline);
	}

	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_SFERE %d\n", &poligoni_da_leggere);
	for (int loop = 0; loop < poligoni_da_leggere; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0)
			poligoni ++;
		else
			poligoni_T ++;
		readstr (filein, oneline);		readstr (filein, oneline);
		readstr (filein, oneline);
	}

	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_CILINDRI %d\n", &poligoni_da_leggere);
	for (int loop = 0; loop < poligoni_da_leggere; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0)
			poligoni ++;
		else
			poligoni_T ++;
		readstr (filein, oneline);		readstr (filein, oneline);
	}

	sector.num_poligoni = poligoni;
	sector.num_poligoni_T = poligoni_T;
	fclose (filein);
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_World::Setup_World ()
{
	Conta_Poligoni ();
	float x, y , z, u, v, raggio, raggio_cima, altezza;
	int sphere_x_rot, sphere_y_rot, sphere_z_rot;
	int numero_texture, blending, numero_divisioni;

	int c_poly = 0;
	int c_poly_T = 0;
	int numero_poligoni_corrente;
	FILE *filein;
	char oneline[255];
	filein = fopen (file_world_name, "rt");

	// setup numero ALBERI da disegnare
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_ALBERI_SCENA %d\n", &sector.num_alberi);
	readstr (filein, oneline);
	sscanf (oneline, "QUANTITA'_PIOGGIA_SCENA %d\n", &sector.num_pioggia);
	readstr (filein, oneline);
	sscanf (oneline, "QUANTITA'_NEVE_SCENA %d\n", &sector.num_neve);

	// setup numero totale di POLIGONI da disegnare
	sector.poly = new POLYGON[sector.num_poligoni];
	sector.poly_T = new POLYGON[sector.num_poligoni_T];

	// setup POLIGONI NON TRASPARENTI
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_TRIANGOLI %d\n", &numero_poligoni_corrente);
	for (int loop = 0; loop < numero_poligoni_corrente; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0) {
			// memorizza il poligono come triangolo
			sector.poly[c_poly].tipo_poligono = 1;
			for (int vert = 0; vert < 3; vert++) {
				readstr (filein, oneline);
				sscanf (oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
				sector.poly[c_poly].vertex[vert].x = x;
				sector.poly[c_poly].vertex[vert].y = y;
				sector.poly[c_poly].vertex[vert].z = z;
				sector.poly[c_poly].vertex[vert].u = u;
				sector.poly[c_poly].vertex[vert].v = v;
			}
			readstr (filein, oneline);
			sscanf (oneline, "%d", &numero_texture);
			sector.poly[c_poly].texture = numero_texture;
			sector.poly[c_poly].blending = blending;
			c_poly ++;
		}
		else {
			// memorizza il poligono come triangolo trasparente
			sector.poly_T[c_poly_T].tipo_poligono = 1;
			for (int vert = 0; vert < 3; vert++) {
				readstr (filein, oneline);
				sscanf (oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
				sector.poly_T[c_poly_T].vertex[vert].x = x;
				sector.poly_T[c_poly_T].vertex[vert].y = y;
				sector.poly_T[c_poly_T].vertex[vert].z = z;
				sector.poly_T[c_poly_T].vertex[vert].u = u;
				sector.poly_T[c_poly_T].vertex[vert].v = v;
			}
			readstr (filein, oneline);
			sscanf (oneline, "%d", &numero_texture);
			sector.poly_T[c_poly_T].texture = numero_texture;
			sector.poly_T[c_poly_T].blending = blending;
			c_poly_T ++;
		}
	}
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_QUADRATI %d\n", &numero_poligoni_corrente);
	for (int loop = 0; loop < numero_poligoni_corrente; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0) {
			// memorizza il poligono come quadrato
			sector.poly[c_poly].tipo_poligono = 2;
			for (int vert = 0; vert < 4; vert++) {
				readstr (filein, oneline);
				sscanf (oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
				sector.poly[c_poly].vertex[vert].x = x;
				sector.poly[c_poly].vertex[vert].y = y;
				sector.poly[c_poly].vertex[vert].z = z;
				sector.poly[c_poly].vertex[vert].u = u;
				sector.poly[c_poly].vertex[vert].v = v;
			}
			readstr (filein, oneline);
			sscanf (oneline, "%d", &numero_texture);
			sector.poly[c_poly].texture = numero_texture;
			sector.poly[c_poly].blending = blending;
			c_poly ++;
		}
		else {
			// memorizza il poligono come quadrato trasparente
			sector.poly_T[c_poly_T].tipo_poligono = 2;
			for (int vert = 0; vert < 4; vert++) {
				readstr (filein, oneline);
				sscanf (oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
				sector.poly_T[c_poly_T].vertex[vert].x = x;
				sector.poly_T[c_poly_T].vertex[vert].y = y;
				sector.poly_T[c_poly_T].vertex[vert].z = z;
				sector.poly_T[c_poly_T].vertex[vert].u = u;
				sector.poly_T[c_poly_T].vertex[vert].v = v;
			}
			readstr (filein, oneline);
			sscanf (oneline, "%d", &numero_texture);
			sector.poly_T[c_poly_T].texture = numero_texture;
			sector.poly_T[c_poly_T].blending = blending;
			c_poly_T ++;
		}
	}
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_QUADRATI_ANIMATI %d\n", &numero_poligoni_corrente);
	for (int loop = 0; loop < numero_poligoni_corrente; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0) {
			// memorizza il poligono come quadrato
			sector.poly[c_poly].tipo_poligono = 3;
			for (int vert = 0; vert < 4; vert++) {
				readstr (filein, oneline);
				sscanf (oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
				sector.poly[c_poly].vertex[vert].x = x;
				sector.poly[c_poly].vertex[vert].y = y;
				sector.poly[c_poly].vertex[vert].z = z;
				sector.poly[c_poly].vertex[vert].u = u;
				sector.poly[c_poly].vertex[vert].v = v;
			}
			sector.poly[c_poly].distanza_text_x = sector.poly[c_poly].vertex[0].u - sector.poly[c_poly].vertex[1].u;
			sector.poly[c_poly].distanza_text_y = sector.poly[c_poly].vertex[0].v - sector.poly[c_poly].vertex[2].v;
			readstr (filein, oneline);
			sscanf (oneline, "%d %d %f", 	&sector.poly[c_poly].text_roll_x,
										&sector.poly[c_poly].text_roll_y,
										&sector.poly[c_poly].text_roll_vel);
			readstr (filein, oneline);
			sscanf (oneline, "%d", &numero_texture);
			sector.poly[c_poly].texture = numero_texture;
			sector.poly[c_poly].blending = blending;
			c_poly ++;
		}
		else {
			// memorizza il poligono come quadrato trasparente
			sector.poly_T[c_poly_T].tipo_poligono = 3;
			for (int vert = 0; vert < 4; vert++) {
				readstr (filein, oneline);
				sscanf (oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
				sector.poly_T[c_poly_T].vertex[vert].x = x;
				sector.poly_T[c_poly_T].vertex[vert].y = y;
				sector.poly_T[c_poly_T].vertex[vert].z = z;
				sector.poly_T[c_poly_T].vertex[vert].u = u;
				sector.poly_T[c_poly_T].vertex[vert].v = v;
			}
			readstr (filein, oneline);
			sscanf (oneline, "%d %d %f", 	&sector.poly_T[c_poly_T].text_roll_x,
										&sector.poly_T[c_poly_T].text_roll_y,
										&sector.poly_T[c_poly_T].text_roll_vel);
			readstr (filein, oneline);
			sscanf (oneline, "%d", &numero_texture);
			sector.poly_T[c_poly_T].texture = numero_texture;
			sector.poly_T[c_poly_T].blending = blending;
			c_poly_T ++;
		}
	}
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_SFERE %d\n", &numero_poligoni_corrente);
	for (int loop = 0; loop < numero_poligoni_corrente; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0) {
			// memorizza il poligono come una sfera
			sector.poly[c_poly].tipo_poligono = 4;
			readstr (filein, oneline);
			sscanf (oneline, "%f %f %f %f", &x, &y, &z, &raggio);
			sector.poly[c_poly].vertex[0].x = x;
			sector.poly[c_poly].vertex[0].y = y;
			sector.poly[c_poly].vertex[0].z = z;
			sector.poly[c_poly].raggio = raggio;
			// setup raggio da usare nel frustum
			sector.poly[c_poly].raggio_x_frustum = raggio;

			readstr (filein, oneline);
			sscanf (oneline, "%f %f %f", &sphere_x_rot, &sphere_y_rot, &sphere_z_rot);
			sector.poly[c_poly].sphere_x_rot = sphere_x_rot;
			sector.poly[c_poly].sphere_y_rot = sphere_y_rot;
			sector.poly[c_poly].sphere_z_rot = sphere_z_rot;

			readstr (filein, oneline);
			sscanf (oneline, "%d %d", &numero_texture, &numero_divisioni);
			sector.poly[c_poly].texture = numero_texture;
			sector.poly[c_poly].blending = blending;
			sector.poly[c_poly].numero_divisioni = numero_divisioni;
			c_poly ++;
		}
		else {
			// memorizza il poligono come una sfera trasparente
			sector.poly_T[c_poly_T].tipo_poligono = 4;
			readstr (filein, oneline);
			sscanf (oneline, "%f %f %f %f", &x, &y, &z, &raggio);
			sector.poly_T[c_poly_T].vertex[0].x = x;
			sector.poly_T[c_poly_T].vertex[0].y = y;
			sector.poly_T[c_poly_T].vertex[0].z = z;
			sector.poly_T[c_poly_T].raggio = raggio;
			// setup raggio da usare nel frustum
			sector.poly_T[c_poly_T].raggio_x_frustum = raggio;

			readstr (filein, oneline);
			sscanf (oneline, "%f %f %f", &sphere_x_rot, &sphere_y_rot, &sphere_z_rot);
			sector.poly_T[c_poly_T].sphere_x_rot = sphere_x_rot;
			sector.poly_T[c_poly_T].sphere_y_rot = sphere_y_rot;
			sector.poly_T[c_poly_T].sphere_z_rot = sphere_z_rot;

			readstr (filein, oneline);
			sscanf (oneline, "%d %d", &numero_texture, &numero_divisioni);
			sector.poly_T[c_poly_T].texture = numero_texture;
			sector.poly_T[c_poly_T].blending = blending;
			sector.poly_T[c_poly_T].numero_divisioni = numero_divisioni;
			c_poly_T ++;
		}
	}
	readstr (filein, oneline);
	sscanf (oneline, "NUMERO_CILINDRI %d\n", &numero_poligoni_corrente);
	for (int loop = 0; loop < numero_poligoni_corrente; loop++) {
		readstr (filein, oneline);
		sscanf (oneline, "%d", &blending);
		if (blending == 0) {
			// memorizza il poligono come un cilindro
			sector.poly[c_poly].tipo_poligono = 5;
			readstr (filein, oneline);
			sscanf (oneline, "%f %f %f %f %f %f", &x, &y, &z, &altezza ,&raggio, &raggio_cima);
			sector.poly[c_poly].vertex[0].x = x;
			sector.poly[c_poly].vertex[0].y = y;
			sector.poly[c_poly].vertex[0].z = z;
			sector.poly[c_poly].altezza = altezza;
			sector.poly[c_poly].raggio = raggio;
			sector.poly[c_poly].raggio_cima = raggio_cima;
			// setup raggio da usare nel frustum
			if (raggio > raggio_cima)
				sector.poly[c_poly].raggio_x_frustum = sqrt (raggio*raggio + altezza*altezza);
			else
				sector.poly[c_poly].raggio_x_frustum = sqrt (raggio_cima*raggio_cima + altezza*altezza);

			readstr (filein, oneline);
			sscanf (oneline, "%d %d", &numero_texture, &numero_divisioni);
			sector.poly[c_poly].texture = numero_texture;
			sector.poly[c_poly].blending = blending;
			sector.poly[c_poly].numero_divisioni = numero_divisioni;
			c_poly ++;
		}
		else {
			// memorizza il poligono come un cilindro trasparente
			sector.poly_T[c_poly_T].tipo_poligono = 5;
			readstr (filein, oneline);
			sscanf (oneline, "%f %f %f %f %f %f", &x, &y, &z, &altezza ,&raggio, &raggio_cima);
			sector.poly_T[c_poly_T].vertex[0].x = x;
			sector.poly_T[c_poly_T].vertex[0].y = y;
			sector.poly_T[c_poly_T].vertex[0].z = z;
			sector.poly_T[c_poly_T].altezza = altezza;
			sector.poly_T[c_poly_T].raggio = raggio;
			sector.poly_T[c_poly_T].raggio_cima = raggio_cima;
			// setup raggio da usare nel frustum
			if (raggio > raggio_cima)
				sector.poly_T[c_poly_T].raggio_x_frustum = sqrt (raggio*raggio + altezza*altezza);
			else
				sector.poly_T[c_poly_T].raggio_x_frustum = sqrt (raggio_cima*raggio_cima + altezza*altezza);

			readstr (filein, oneline);
			sscanf (oneline, "%d %d", &numero_texture, &numero_divisioni);
			sector.poly_T[c_poly_T].texture = numero_texture;
			sector.poly_T[c_poly_T].blending = blending;
			sector.poly_T[c_poly_T].numero_divisioni = numero_divisioni;
			c_poly_T ++;
		}
	}

	fclose (filein);
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_World::readstr (FILE *filename, char *string)
{
	do {
		fgets (string, 255, filename);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
