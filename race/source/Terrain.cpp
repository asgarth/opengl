/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Terrain.h"

// definizione funzioni per multitexturing senza vertex array
PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB	= NULL;
PFNGLACTIVETEXTUREARBPROC				glActiveTextureARB	= NULL;

/* ---------------------------------------------------------------------------------------------------------------------- */

Terrain::Terrain (Texture_Manager *main_tm, Frustum *f, float terrain_resolution) {
	tm = main_tm;							// set texture manager
	frustum = f;							// set frustum object
	res = terrain_resolution;			// set terrain resolution
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// clear all
Terrain::~Terrain () {
	if (vertex)		delete vertex;						// free vertex memory
	if (texcoord)	delete texcoord;					// free texture memory
	if (normal)		delete normal;						// free normals memory

	if (map_h)		SDL_FreeSurface(map_h);			// free sdl surface
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render terrain
void Terrain::render (float x, float z) {
	// multitexturing functions
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) SDL_GL_GetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) SDL_GL_GetProcAddress("glMultiTexCoord2fARB");

	// enable multitexturing
	glActiveTextureARB (GL_TEXTURE0_ARB);
	glEnable (GL_TEXTURE_2D);
	tm -> setTexture (TERRAIN_GREEN);					// carica la texture del terreno
	glActiveTextureARB (GL_TEXTURE1_ARB);
	glEnable (GL_TEXTURE_2D);
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi (GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
	tm -> setTexture (TERRAIN_DETAIL);					// carica la texture del dettaglio
	// scala la seconda texture (quella del dettaglio)
	glMatrixMode (GL_TEXTURE);								// entra nel modello delle texture
		glLoadIdentity ();									// resetta la matrice corrente e scala la texture
		glScalef (TEXTURE_DATAIL, TEXTURE_DATAIL, 1);
	glMatrixMode (GL_MODELVIEW);							// torna alla matrice del modello (NORMALE)

	// draw
	bool poly_is_near = false;
	static float res_to_check = res;
	glBegin (GL_TRIANGLES);
	for (int loop = 0; loop < num_vert; loop += 4) {
		glNormal3fv (&normal[loop/4].x);					// normal
		// controlla se il poligono è vicino
		if (	fabs(vertex[loop].x - x) < res_to_check || fabs(vertex[loop+1].x - x) < res_to_check ||
				fabs(vertex[loop+2].x - x) < res_to_check || fabs(vertex[loop+3].x - x) < res_to_check)
			if (	fabs(vertex[loop].z - z) < res_to_check || fabs(vertex[loop+1].z - z) < res_to_check ||
					fabs(vertex[loop+2].z - z) < res_to_check || fabs(vertex[loop+3].z - z) < res_to_check)
				poly_is_near = true;
		// triangle 1 - check frustum
		if (	poly_is_near ||
				frustum -> pointInFrustum(vertex[loop].x, vertex[loop].y, vertex[loop].z) ||
				frustum -> pointInFrustum(vertex[loop+1].x, vertex[loop+1].y, vertex[loop+1].z) ||
				frustum -> pointInFrustum(vertex[loop+2].x, vertex[loop+2].y, vertex[loop+2].z)) {
			// draw triangle
			glMultiTexCoord2fARB (GL_TEXTURE0_ARB, texcoord[loop].u, texcoord[loop].v);
			glMultiTexCoord2fARB (GL_TEXTURE1_ARB, texcoord[loop].u, texcoord[loop].v);
			glVertex3fv (&vertex[loop].x);					// vertex
			glMultiTexCoord2fARB (GL_TEXTURE0_ARB, texcoord[loop+1].u, texcoord[loop+1].v);
			glMultiTexCoord2fARB (GL_TEXTURE1_ARB, texcoord[loop+1].u, texcoord[loop+1].v);
			glVertex3fv (&vertex[loop+1].x);					// vertex
			glMultiTexCoord2fARB (GL_TEXTURE0_ARB, texcoord[loop+2].u, texcoord[loop+2].v);
			glMultiTexCoord2fARB (GL_TEXTURE1_ARB, texcoord[loop+2].u, texcoord[loop+2].v);
			glVertex3fv (&vertex[loop+2].x);					// vertex
		}
		// triangle 2 - check frustum
		if (	poly_is_near ||
				frustum -> pointInFrustum(vertex[loop].x, vertex[loop].y, vertex[loop].z) ||
				frustum -> pointInFrustum(vertex[loop+2].x, vertex[loop+2].y, vertex[loop+2].z) ||
				frustum -> pointInFrustum(vertex[loop+3].x, vertex[loop+3].y, vertex[loop+3].z)) {
			// draw triangle
			glMultiTexCoord2fARB (GL_TEXTURE0_ARB, texcoord[loop].u, texcoord[loop].v);
			glMultiTexCoord2fARB (GL_TEXTURE1_ARB, texcoord[loop].u, texcoord[loop].v);
			glVertex3fv (&vertex[loop].x);					// vertex
			glMultiTexCoord2fARB (GL_TEXTURE0_ARB, texcoord[loop+2].u, texcoord[loop+2].v);
			glMultiTexCoord2fARB (GL_TEXTURE1_ARB, texcoord[loop+2].u, texcoord[loop+2].v);
			glVertex3fv (&vertex[loop+2].x);					// vertex
			glMultiTexCoord2fARB (GL_TEXTURE0_ARB, texcoord[loop+3].u, texcoord[loop+3].v);
			glMultiTexCoord2fARB (GL_TEXTURE1_ARB, texcoord[loop+3].u, texcoord[loop+3].v);
			glVertex3fv (&vertex[loop+3].x);					// vertex
		}
	}
	glEnd ();

	// disable multitexturing
	glActiveTextureARB (GL_TEXTURE1_ARB);
	glDisable (GL_TEXTURE_2D);
	glActiveTextureARB (GL_TEXTURE0_ARB);
	glDisable (GL_TEXTURE_2D);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// calculate height at (x,y) with plane
float Terrain::getHeight (float x_float, float z_float, Angle &a) {
	Vector v1, v2, v3, normal, ab, bc;
	Plane plane;
	bool up;
	int res_int = (int) res;
	int x = (int) (x_float);
	int z = (int) (z_float);
	int xs, zs;
	xs = (int) ((x+size/2) / res_int) * res_int - (size/2);
	zs = (int) ((z+size/2) / res_int) * res_int - (size/2);
	// FIND PLANE EQUATION (Ax + By + Cz + D)
	// find triangle - START
	// mi servono i vertici della diagonale -> 0 e 2
	v1.x = xs;
	v1.z = zs;
	v1.y = getHeightFromMap (v1.x+size/2, v1.z+size/2);
	v3.x = xs + res;
	v3.z = zs + res;
	v3.y = getHeightFromMap (v3.x+size/2, v3.z+size/2);
	// calcolo la retta passante per i punti
	float rect_z = v3.z * ((xs-v1.x)/(v3.x-v1.x));
	if (z_float > rect_z)
		up = true;
	else
		up = false;

	// find triangle - END
	if (up) {
		v2.x = xs;
		v2.z = zs + res;
		v2.y = getHeightFromMap (v2.x+size/2, v2.z+size/2);
	}
	else {
		v2.x = xs + res;
		v2.z = zs;
		v2.y = getHeightFromMap (v2.x+size/2, v2.z+size/2);
	}
	// calculate plane equation
	vectorSubtract (v1, v2, ab);
	vectorSubtract (v2, v3, bc);
	crossProduct (ab, bc, normal);
	makePlane (v1, normal, plane);

	// calculate height
	float height = (plane.d - (plane.a * x_float) - (plane.c * z_float)) / plane.b;

	// calculate angle x
	Vector temp_vec;
	temp_vec.x = 1.0; temp_vec.y = 0.0; temp_vec.z = 0.0;
	float temp_angle_x = vectorAngle (temp_vec, normal);
	a.x = 90.0 - temp_angle_x;
	// calculate angle z
	temp_vec.x = 0.0; temp_vec.y = 0.0; temp_vec.z = 1.0;
	float temp_angle_z = vectorAngle (temp_vec, normal);
	a.z = 90.0 - temp_angle_z;

	// return height
	return height;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init data
bool Terrain::init (const char *filename) {
	// try to load map
	printf ("\nLOADING TERRAIN...\n", filename);
	printf ("\tMap file: %s\n", filename);
	if (!loadHeightMap (filename))
		return false;

	int v_index = 0;					// index for vertex array
	int n_index = 0;					// index for normal array
	float x, z;							// temp coord

	SDL_LockSurface(map_h);
	size = map_h -> w;

	num_vert = (long) (map_h -> w * map_h -> h * 4 / (res * res));
	vertex = new Vector[num_vert];			// allocate vertex memory
	texcoord = new TexCoord[num_vert];		// allocate texture memory
	num_normal = (long) (map_h -> w * map_h -> h / (res * res));
	normal = new Vector[num_normal];			// allocate normal memory
	printf ("\tVertex num: %d\n", num_vert);

	// loop for init vertex
	for (int i = 0; i < map_h -> w; i += (int) res) {			// width
		for (int j = 0; j < map_h -> h; j += (int) res) {		// height
			for (int n_tri = 0; n_tri < 4; n_tri++) {				// polygon
				// x, y
				x = (float) i + ( ( n_tri == 1 || n_tri == 2) ? res : 0.0f );
				z = (float) j + ( ( n_tri == 2 || n_tri == 3) ? res : 0.0f );

				// set vertex
				vertex[v_index].x = (x - ( map_h -> w / 2 ));
				vertex[v_index].z = (z - ( map_h -> h / 2 ));
				vertex[v_index].y = getHeightFromMap (x, z);

				// set texture coord
				texcoord[v_index].u = x / map_h -> w;
				texcoord[v_index].v = z / map_h -> h;

				v_index++;			// go to next vertex
			}
			// calculate normal coord
			Vector ab, bc;
			vectorSubtract (vertex[v_index-3], vertex[v_index-2], ab);
			vectorSubtract (vertex[v_index-2], vertex[v_index-1], bc);
			crossProduct (ab, bc, normal[n_index]);
			normalize (normal[n_index]);

			n_index++;
		}
	}
	SDL_UnlockSurface(map_h);

	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// return height at (x,y) from map (POSITIVE VALUE)
float Terrain::getHeightFromMap (float x_float, float z_float) {
	int x = (int) x_float;
	int z = (int) z_float;

	if (x < 0 || z < 0)
		return 0.0;

	Uint8 *dst = (Uint8 *)map_h -> pixels;
	// calcola la posizione sulla texture (evitando overflow)
	int nPos = ( ( x % map_h -> w )  + ( ( z % map_h -> h ) * map_h -> w ) ) * 3;
	float flR = (float) dst[nPos];						// Get The Red Component
	float flG = (float) dst[nPos + 1];					// Get The Green Component
	float flB = (float) dst[nPos + 2];					// Get The Blue Component

	// Calculate The Height Using The Luminance Algorithm
	return (( 0.299f * flR + 0.587f * flG + 0.114f * flB ) * HEIGHT_SCALE);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// load jpg image
bool Terrain::loadHeightMap (const char *filename) {
	map_h = IMG_Load (filename);
	if (map_h)
		return true;
	return false;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// clear memory
void Terrain::clear (void) {
	if (vertex)		delete vertex;						// free vertex memory
	if (texcoord)	delete texcoord;					// free texture memory
	if (normal)		delete normal;						// free normals memory

	if (map_h)		SDL_FreeSurface(map_h);			// free sdl surface
}
