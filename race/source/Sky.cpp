/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Sky.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Sky::Sky (Texture_Manager *main_tm) {
	tm = main_tm;							// set texture manager
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// clear all
Sky::~Sky () {
	if (vertex)		delete vertex;
	if (texcoord)	delete texcoord;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// rendere sky
void Sky::render (int texture) {
	glPushMatrix ();
		glEnable (GL_TEXTURE_2D);
		glTranslatef (0.0f, -100.0f, 0.0f);
		glRotatef (SDL_GetTicks()/1000.0f,0.0f, 1.0f, 0.0f);
		glRotatef(270, 1.0f, 0.0f, 0.0f);

		tm -> setTexture (texture);
		glBegin(GL_TRIANGLE_STRIP);
		for (int i=0; i < num_vert; i++) {
			glColor3f (1.0f, 1.0f, 1.0f);
			glTexCoord2f (texcoord[i].u, texcoord[i].v);
			glVertex3f (vertex[i].x, vertex[i].y, vertex[i].z);
		}
		glEnd();
	glPopMatrix();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// init sky vertices
void Sky::init (float ray, float dtheta, float dphi, float hTile, float vTile) {
	radius = ray;

	int theta, phi;

	// initialize vertex and texture arrays
	num_vert = (int)((360/dtheta)*(90/dphi)*4);
	vertex = new Vector [num_vert];
	texcoord = new TexCoord [num_vert];
	printf ("\nGENERATING SKY\n");
	printf ("\tVertex num: %d\n", num_vert);

	// Used to calculate the UV coordinates
	float vx, vy, vz, mag;

	// Generate the dome
	int n = 0;
	for (phi=0; phi <= 90 - dphi; phi += (int)dphi) {
		for (theta=0; theta <= 360 - dtheta; theta += (int)dtheta) {
			// Calculate the vertex at phi, theta
			vertex[n].x = radius * sinf(phi*PIOVER180) * cosf(PIOVER180*theta);
			vertex[n].y = radius * sinf(phi*PIOVER180) * sinf(PIOVER180*theta);
			vertex[n].z = radius * cosf(phi*PIOVER180);

			// Create a vector from the origin to this vertex
			Vector v;
			v.x = vertex[n].x;
			v.y = vertex[n].y;
			v.z = vertex[n].z;

			// Normalize the vector
			normalize (v);

			// Calculate the spherical texture coordinates
			texcoord[n].u = hTile * (float)(atan2(v.x, v.z)/(PI*2)) + 0.5f;
			texcoord[n].v = vTile * (float)(asinf(v.y) / PI) + 0.5f;
			n++;

			// Calculate the vertex at phi+dphi, theta
			vertex[n].x = radius * sinf((phi+dphi)*PIOVER180) * cosf(theta*PIOVER180);
			vertex[n].y = radius * sinf((phi+dphi)*PIOVER180) * sinf(theta*PIOVER180);
			vertex[n].z = radius * cosf((phi+dphi)*PIOVER180);

			// Calculate the texture coordinates
			v.x = vertex[n].x;
			v.y = vertex[n].y;
			v.z = vertex[n].z;

			normalize (v);

			texcoord[n].u = hTile * (float)(atan2(v.x, v.z)/(PI*2)) + 0.5f;
			texcoord[n].v = vTile * (float)(asinf(v.y) / PI) + 0.5f;
			n++;

			// Calculate the vertex at phi, theta+dtheta
			vertex[n].x = radius * sinf(PIOVER180*phi) * cosf(PIOVER180*(theta+dtheta));
			vertex[n].y = radius * sinf(PIOVER180*phi) * sinf(PIOVER180*(theta+dtheta));
			vertex[n].z = radius * cosf(PIOVER180*phi);

			// Calculate the texture coordinates
			v.x = vertex[n].x;
			v.y = vertex[n].y;
			v.z = vertex[n].z;

			normalize (v);

			texcoord[n].u = hTile * (float)(atan2(v.x, v.z)/(PI*2)) + 0.5f;
			texcoord[n].v = vTile * (float)(asinf(v.y) / PI) + 0.5f;
			n++;

			if (phi > -90 && phi < 90)	{
				// Calculate the vertex at phi+dphi, theta+dtheta
				vertex[n].x = radius * sinf((phi+dphi)*PIOVER180) * cosf(PIOVER180*(theta+dtheta));
				vertex[n].y = radius * sinf((phi+dphi)*PIOVER180) * sinf(PIOVER180*(theta+dtheta));
				vertex[n].z = radius * cosf((phi+dphi)*PIOVER180);

				// Calculate the texture coordinates
				v.x = vertex[n].x;
				v.y = vertex[n].y;
				v.z = vertex[n].z;

				normalize (v);

				texcoord[n].u = hTile * (float)(atan2(v.x, v.z)/(PI*2)) + 0.5f;
				texcoord[n].v = vTile * (float)(asinf(v.y) / PI) + 0.5f;
				n++;
			}
		}
	}

	// Fix the problem at the seam
	for (int i = 0; i < num_vert-3; i++) {
		if (texcoord[i].u - texcoord[i+1].u > 0.9f)
			texcoord[i+1].u += 1.0f;

		if (texcoord[i+1].u - texcoord[i].u > 0.9f)
			texcoord[i].u += 1.0f;

		if (texcoord[i].u - texcoord[i+2].u > 0.9f)
			texcoord[i+2].u += 1.0f;

		if (texcoord[i+2].u - texcoord[i].u > 0.9f)
			texcoord[i].u += 1.0f;

		if (texcoord[i+1].u - texcoord[i+2].u > 0.9f)
			texcoord[i+2].u += 1.0f;

		if (texcoord[i+2].u - texcoord[i+1].u > 0.9f)
			texcoord[i+1].u += 1.0f;

		if (texcoord[i].v - texcoord[i+1].v > 0.8f)
			texcoord[i+1].v += 1.0f;

		if (texcoord[i+1].v - texcoord[i].v > 0.8f)
			texcoord[i].v += 1.0f;

		if (texcoord[i].v - texcoord[i+2].v > 0.8f)
			texcoord[i+2].v += 1.0f;

		if (texcoord[i+2].v - texcoord[i].v > 0.8f)
			texcoord[i].v += 1.0f;

		if (texcoord[i+1].v - texcoord[i+2].v > 0.8f)
			texcoord[i+2].v += 1.0f;

		if (texcoord[i+2].v - texcoord[i+1].v > 0.8f)
			texcoord[i+1].v += 1.0f;
	}
}
