/************************************************************
		Project: R.A.C.E.

		License: GPL
		www.gametutorials.com
************************************************************/


#ifndef Frustum_H
#define Frustum_H

#include "includes.h"
#include "race_math.h"

class Frustum
{
	private:
		// valori di A B C e D per ogni lato del frustum
		float m_Frustum[6][4];


	public:
		// funzione che calcola il frustum (chiamare ogni volta che la camera si muove)
		void calculateFrustum();

		// funzione che calcola se il punto passato è nel frustum e quindi visibile
		bool pointInFrustum(float x, float y, float z);

		// funzione che riceve un punto e un raggio e calcola se la sfera risultante è nel frustum
		bool sphereInFrustum(float x, float y, float z, float raggio);
};

#endif
