/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef TERRAIN_H
#define TERRAIN_H

// definizione texture e riduzione dell'altezza
#define TEXTURE_DATAIL	64.0f
#define HEIGHT_SCALE		0.05f

#include "includes.h"
#include "race_math.h"
#include "Texture_Manager.h"
#include "Frustum.h"

class Terrain {
	private:
		Texture_Manager *tm;		// texture manager
		Frustum *frustum;			// frustum object
		SDL_Surface *map_h;		// structur for height map

		float res;					// terrain resolution

		// function
		float getHeightFromMap (float, float);	// return height from map
		bool loadHeightMap (const char *);		// load jpg file

	public:
		long num_vert, num_normal;	// vertex num
		long size;						// map size
		Vector *vertex;				// vertex array
		Vector *normal;				// normal array
		TexCoord *texcoord;			// coord array

		Terrain (Texture_Manager *, Frustum *, float);
		~Terrain();

		// function
		void render (float, float);		// render terrain
		bool init (const char *);			// init data
		float getHeight (float, float, Angle &);// calculate height from plane
		void clear (void);					// free memory
};

#endif
