//-------------		file			Frustum.cpp		-----------------------------------------------------------------------------------
// CODICE PRESO DA GameTutorials.com

#include "Frustum.h"

// enum per i lati del frustum in modo da non doverli chiamare 0 o 1
enum FrustumSide
{
	RIGHT		= 0,		// RIGHT side
	LEFT		= 1,		// LEFT	 side
	BOTTOM	= 2,		// BOTTOM side
	TOP		= 3,		// TOP side
	BACK		= 4,		// BACK	 side
	FRONT		= 5		// FRONT side
};

// enum per i piani A B C e D
enum PlaneData
{
	A = 0,				// valore X della normale del piano
	B = 1,				// valore Y della normale del piano
	C = 2,				// valore Z della normale del piano
	D = 3				// distanza del piano dall'origine
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// funzione che normalizza un piano (lato A) dal frustum passato

void NormalizePlane(float frustum[6][4], int side)
{
	// calcolo magnitudine del piano (point A B C)
	// A, B, C = X, Y, Z
	// equazione della magnitudine:  magnitude = sqrt( x^2 + y^2 + z^2)
	float magnitude = (float)sqrt(	frustum[side][A] * frustum[side][A] +
								frustum[side][B] * frustum[side][B] +
								frustum[side][C] * frustum[side][C] );

	// valori del piano divisi per la magnitudine
	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// funzione che estrae il frustum dalla projection e dalla modelview matrix.

void Frustum::CalculateFrustum ()
{
	float   proj[16];								// array per projection matrix
	float   modl[16];								// array per modelview matrix
	float   clip[16];								// array per clipping planes

	glGetFloatv( GL_PROJECTION_MATRIX, proj );
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	// RIGHT side of the frustum
	m_Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
	m_Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
	m_Frustum[RIGHT][C] = clip[11] - clip[ 8];
	m_Frustum[RIGHT][D] = clip[15] - clip[12];

	// Normalize the RIGHT side
	NormalizePlane(m_Frustum, RIGHT);

	// LEFT side of the frustum
	m_Frustum[LEFT][A] = clip[ 3] + clip[ 0];
	m_Frustum[LEFT][B] = clip[ 7] + clip[ 4];
	m_Frustum[LEFT][C] = clip[11] + clip[ 8];
	m_Frustum[LEFT][D] = clip[15] + clip[12];

	// Normalize the LEFT side
	NormalizePlane(m_Frustum, LEFT);

	// BOTTOM side of the frustum
	m_Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
	m_Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
	m_Frustum[BOTTOM][C] = clip[11] + clip[ 9];
	m_Frustum[BOTTOM][D] = clip[15] + clip[13];

	// Normalize the BOTTOM side
	NormalizePlane(m_Frustum, BOTTOM);

	// TOP side of the frustum
	m_Frustum[TOP][A] = clip[ 3] - clip[ 1];
	m_Frustum[TOP][B] = clip[ 7] - clip[ 5];
	m_Frustum[TOP][C] = clip[11] - clip[ 9];
	m_Frustum[TOP][D] = clip[15] - clip[13];

	// Normalize the TOP side
	NormalizePlane(m_Frustum, TOP);

	// BACK side of the frustum
	m_Frustum[BACK][A] = clip[ 3] - clip[ 2];
	m_Frustum[BACK][B] = clip[ 7] - clip[ 6];
	m_Frustum[BACK][C] = clip[11] - clip[10];
	m_Frustum[BACK][D] = clip[15] - clip[14];

	// Normalize the BACK side
	NormalizePlane(m_Frustum, BACK);

	// FRONT side of the frustum
	m_Frustum[FRONT][A] = clip[ 3] + clip[ 2];
	m_Frustum[FRONT][B] = clip[ 7] + clip[ 6];
	m_Frustum[FRONT][C] = clip[11] + clip[10];
	m_Frustum[FRONT][D] = clip[15] + clip[14];

	// Normalize the FRONT side
	NormalizePlane(m_Frustum, FRONT);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// calcola se il punto passato è nel frustum

bool Frustum::PointInFrustum ( float x, float y, float z )
{
	// passa attraverso tutti i 6 lati del frustum
	for(int i = 0; i < 6; i++ )
		// calcola l'equazione del piano e vede se il punto è dietro uno dei piani
		if ( m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0 )
			return false;

	// il punto è nel frustum (davanti a tutti i piani del frustum)
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// calocla se una sfera è nel frustum

bool Frustum::SphereInFrustum ( float x, float y, float z, float raggio )
{
	for(int i = 0; i < 6; i++ )
		// calcola come per il punto ma aggiunge il raggio passato
		if ( m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= -raggio )
			return false;

	// la sfera è nel frustum
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------