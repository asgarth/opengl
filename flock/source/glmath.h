#ifndef GL_MATH_H
#define GL_MATH_H

// librerie di sistema
#include <math.h>

#define PIOVER180			0.0174532925f			// fattore di conversione tra gradi e radianti
#define PI 						3.1415926535897f	// pi


typedef struct { float x, y, z;				// ANGLE
} Angle;

typedef struct { float x, y, z;				// VECTOR
} Vector;

typedef struct { float u, v;					// TEXTURE COORD
} TexCoord;

typedef struct { float Data[16];			// MATRIX (opengl format for 4x4 matrix)
} Matrix;

typedef struct { Vector Nor;				// Vertex Normal
					  Vector Pos;						// Vertex Position
} Vertex;

typedef struct{ float a, b, c, d;			// Plane
} Plane;

typedef struct { Vertex Verts[3];		// POLYGON
} POLYGON;												// non cambiare nome (incompatibilità win)

typedef struct { POLYGON *poly;		// POLYGON
					  int n_poly;							// numero poligoni
					  int color_index;					// indice nell'array dei colori
} Mesh;

typedef struct { float rgb[3];				// array di componenti
} Color;

// Math Functions
inline float dotProduct (Vector &v1, Vector &v2) {		// calculate angle between 2 vectors
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float magnitude (Vector &v) {								// vector's lenght
	return sqrtf (v.x * v.x + v.y * v.y + v.z * v.z);
}

inline float vectorAngle(Vector a, Vector b){
  float angle;
  float product;
  float maga, magb;

  product = dotProduct (a, b);
  maga    = magnitude (a);
  magb    = magnitude (b);

  angle   = acos(product / (maga * magb) );

  return 57.295779513 * angle;
}

inline void crossProduct (Vector &v1, Vector &v2, Vector &cross) {
	cross.x = v1.y*v2.z - v1.z*v2.y;
	cross.y = v1.z*v2.x - v1.x*v2.z;
	cross.z = v1.x*v2.y - v1.y*v2.x;
}

inline void normalize (Vector &v) {									// normalize a vector
	float m = magnitude (v);

	if (m != 0.0f) {
		v.x /= m;
		v.y /= m;
		v.z /= m;
	}
}

inline void rotateVector (Matrix &m, Vector &v1, Vector &v2) {						// rotate a vector with the matrix
	v2.x = (m.Data[0] * v1.x) + (m.Data[4] * v1.y) + (m.Data[8]  * v1.z);		// x axis
	v2.y = (m.Data[1] * v1.x) + (m.Data[5] * v1.y) + (m.Data[9]  * v1.z);	// y axis
	v2.z = (m.Data[2] * v1.x) + (m.Data[6] * v1.y) + (m.Data[10] * v1.z);	// z axis
}

inline void vectorSubtract (Vector &v1, Vector &v2, Vector &out) {
	out.x = v1.x - v2.x;
	out.y = v1.y - v2.y;
	out.z = v1.z - v2.z;
}

inline void vectorScale (Vector &v1, float scale, Vector &out) {
	out.x = v1.x * scale;
	out.y = v1.y * scale;
	out.z = v1.z * scale;
}

inline void makePlane (Vector &a, Vector &normal, Plane &result){
  result.a = normal.x;
  result.b = normal.y;
  result.c = normal.z;
  result.d = (normal.x * a.x) + (normal.y * a.y) + (normal.z * a.z);
}

inline float calculateDistance (float x1, float z1, float x2, float z2){
	float x = fabs (x1 - x2);
	float z = fabs (z1 - z2);
	return sqrt (x*x + z*z);
}

#endif
