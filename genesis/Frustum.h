//-------------		file			Frustum.h		-----------------------------------------------------------------------------------

#ifndef Frustum_H
#define Frustum_H

#include "Genesis_Includes.h"

class Frustum
{
	public:
		// funzione che calcola il frustum (chiamare ogni volta che la camera si muove)
		void CalculateFrustum();

		// funzione che calcola se il punto passato è nel frustum e quindi visibile
		bool PointInFrustum(float x, float y, float z);

		// funzione che riceve un punto e un raggio e calcola se la sfera risultante è nel frustum
		bool SphereInFrustum(float x, float y, float z, float radius);

		// funzione che riceve il centro e metà del lato di un cubo e calcola se è nel frustum
		bool CubeInFrustum( float x, float y, float z, float size );

	private:
		// valori di A B C e D per ogni lato del frustum
		float m_Frustum[6][4];
};

#endif
