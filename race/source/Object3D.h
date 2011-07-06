/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef OBJECT_3D_H
#define OBJECT_3D_H

#define ATTRITO			3.0f
#define OUTLINE_WIDTH	2.5f

#include "includes.h"
#include "race_math.h"

class Object3D {
	public:
		// variabili per i vertici
		Mesh *mesh;
		int n_mesh;
		Color *color;
		int num_colors;

		GLuint shaderTexture[1];				// variabile per texture 1D
		Vector lightAngle;						// light angle


		Object3D ();
		~Object3D();

		// funzioni
		void draw (float);						// render
		bool loadModel (const char *filename);
		bool loadShader (const char *filename);
};

#endif
