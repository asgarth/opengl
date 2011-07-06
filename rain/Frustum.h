//-------------		file			Frustum.h		-----------------------------------------------------------------------------------
// CODICE PRESO DA GameTutorials.com

#ifndef Frustum_H
#define Frustum_H

#include "Rain_Includes.h"

class Frustum
{
	public:
		// funzione che calcola il frustum (chiamare ogni volta che la camera si muove)
		void CalculateFrustum();

		// funzione che calcola se il punto passato è nel frustum e quindi visibile
		bool PointInFrustum(float x, float y, float z);

		// funzione che riceve un punto e un raggio e calcola se la sfera risultante è nel frustum
		bool SphereInFrustum(float x, float y, float z, float raggio);

	private:
		// valori di A B C e D per ogni lato del frustum
		float m_Frustum[6][4];
};

#endif