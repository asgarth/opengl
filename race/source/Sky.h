/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef SKY_H
#define SKY_H

#include "includes.h"
#include "race_math.h"
#include "Texture_Manager.h"

class Sky {
	private:
		Texture_Manager *tm;			// texture manager

		Vector *vertex;				// vertex array
		TexCoord *texcoord;			// coord array

		int num_vert;					// vertex num
		float radius;					// plane radius


	public:
		Sky (Texture_Manager *);
		~Sky();

		// function
		void render (int);										// render sky
		void init (float, float, float, float, float);	// init data
};

#endif
