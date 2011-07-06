/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Object3D.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Object3D::Object3D () {
	lightAngle.x = 0.0f;					// Set The X Direction
	lightAngle.y = 1.0f;					// Set The Y Direction
	lightAngle.z = 1.0f;					// Set The Z Direction

	normalize (lightAngle);
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Object3D::~Object3D () {

}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Object3D::draw (float line_width) {
	// check value
	if (line_width > OUTLINE_WIDTH)		line_width = OUTLINE_WIDTH;
	if (line_width < 0.5f)					line_width = 0.5f;

	glDepthFunc (GL_LESS);
	glEnable (GL_CULL_FACE);							// Enable OpenGL Face Culling

	float TmpShade;										// Temporary Shader Value
	Matrix TmpMatrix;										// Temporary MATRIX Structure
	Vector TmpVector, TmpNormal;						// Temporary VECTOR Structures

	glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable (GL_LINE_SMOOTH);

	glGetFloatv (GL_MODELVIEW_MATRIX, TmpMatrix.Data);	// Get The Generated Matrix

	/** Cel-Shading Code */
	glDisable (GL_TEXTURE_2D);									// disable 2D texturing
	glEnable (GL_TEXTURE_1D);									// enable 1D texturing
	glBindTexture (GL_TEXTURE_1D, shaderTexture[0]);	// select texture

	glBegin (GL_TRIANGLES);
		for (int k = 0; k < n_mesh; k++) {
			int temp_index = mesh[k].color_index;
			if (temp_index < 0)
				temp_index = 0;
			glColor3fv (&color[temp_index].rgb[0]);			// seleziona il colore
			for (int i = 0; i < mesh[k].n_poly; i++) {		// polygon loop
				for (int j = 0; j < 3; j++) {						// vertex loop
					TmpNormal.x = mesh[k].poly[i].Verts[j].Nor.x;			// Fill Up The TmpNormal Structure With
					TmpNormal.y = mesh[k].poly[i].Verts[j].Nor.y;			// The Current Vertices' Normal Values
					TmpNormal.z = mesh[k].poly[i].Verts[j].Nor.z;

					rotateVector (TmpMatrix, TmpNormal, TmpVector);	// Rotate This By The Matrix

					normalize (TmpVector);									// Normalize The New Normal

					TmpShade = dotProduct (TmpVector, lightAngle);	// Calculate The Shade Value

					if (TmpShade < 0.0f)
						TmpShade = 0.0f;							// Clamp The Value to 0 If Negative

					glTexCoord1f (TmpShade);					// Set The Texture Co-ordinate As The Shade Value
					glVertex3fv (&mesh[k].poly[i].Verts[j].Pos.x);	// Send The Vertex Position
		   	 }
			}
		}
	glEnd ();													// Tell OpenGL To Finish Drawing

	glDisable (GL_TEXTURE_1D);

	/** Outline Code */
	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);	// blend func
	glPolygonMode (GL_BACK, GL_LINE);	// Draw Backfacing Polygons As Wireframes
	glLineWidth (line_width);			// Set The Line Width
	glCullFace (GL_FRONT);					// Don't Draw Any Front-Facing Polygons
	glDepthFunc (GL_LEQUAL);				// Change The Depth Mode

	glBegin (GL_TRIANGLES);
		for (int k = 0; k < n_mesh; k++) {
			glColor3f (0.0f, 0.0f, 0.0f);							// seleziona il colore
			for (int i = 0; i < mesh[k].n_poly; i++) {		// polygon loop
				for (int j = 0; j < 3; j++) {						// vertex loop
					glVertex3fv (&mesh[k].poly[i].Verts[j].Pos.x);
				}
			}
		}
	glEnd ();									// Tell OpenGL We've Finished
	glCullFace (GL_BACK);					// Reset The Face To Be Culled
	glPolygonMode (GL_BACK, GL_FILL);	// Reset Back-Facing Polygon Drawing Mode
	glDisable (GL_BLEND);

	glDisable (GL_CULL_FACE);
	glEnable (GL_TEXTURE_2D);
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// load shader file
bool Object3D::loadShader (const char *filename) {
	char readline[255];
	float shaderData[32][3];				// array for shader value (96)
	// open file
	FILE *in	= fopen (filename, "r");
	if (!in) {				// check error
		printf ("Errore caricamento shader file\n");
		return false;
	}
	for (int i = 0; i < 32; i++) {		// loop for 32 Greyscale Values
		if (feof (in))
			break;

		fgets (readline, 255, in);			// get line
		shaderData[i][0] = shaderData[i][1] = shaderData[i][2] = float(atof (readline));
	}
	fclose (in);								// close

	// SETUP OPENGL TEXTURE
	glGenTextures (1, &shaderTexture[0]);					// new texture
	glBindTexture (GL_TEXTURE_1D, shaderTexture[0]);	// bind texture; 1D
	// DON'T USE OpenGL Bi/Trilinear Filtering!
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage1D (GL_TEXTURE_1D, 0, GL_RGB, 32, 0, GL_RGB , GL_FLOAT, shaderData);

	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// load model
bool Object3D::loadModel (const char *filename) {
	char readline[255];

	// open file
	FILE *in = fopen (filename, "r");
	if (!in)					// check error
		return false;

	while (fgets (readline, 256, in) != NULL) {
		// salta la riga se è un commento
		if (!strncmp (readline, "//", 2))
			continue;

		int n_frame, n_index, n_flags;
		char mesh_name[50];
		// legge il numero di frame
		sscanf (readline, "Frames: %d", &n_frame);
		sscanf (readline, "Frame: %d", &n_frame);
		// legge il numero di mesh
		if (sscanf (readline, "Meshes: %d", &n_mesh) == 1) {
			mesh = new Mesh [n_mesh];			// alloca lo spazio x le mesh
			for (int i = 0; i < n_mesh; i++) {
				if (!fgets (readline, 256, in))			return false;
				// mesh: name, flags, material index
				if (sscanf (readline, "\"%[^\"]\" %d %d",mesh_name, &n_flags, &mesh[i].color_index) != 3)
					return false;

				// vertices
            if (!fgets (readline, 256, in))			return false;

            int n_vertices = 0;
            Vertex *verts;
				if (sscanf (readline, "%d", &n_vertices) != 1)
					return false;
				verts = new Vertex [n_vertices];

				for (int j = 0; j < n_vertices; j++) {
					if (!fgets (readline, 256, in))			return false;

					float texture_1, texture_2;
					if (sscanf (	readline, "%d %f %f %f %f %f %d",
                        	&n_flags,
                        	&verts[j].Pos.x, &verts[j].Pos.y, &verts[j].Pos.z,
                        	&texture_1, &texture_2,
                        	&n_index) != 7)
						return false;
				}

				// normals
            if (!fgets (readline, 256, in))			return false;

				int n_normals = 0;
				Vertex *normals;
				if (sscanf (readline, "%d", &n_normals) != 1)
					return false;
				normals = new Vertex [n_normals];

				for (int j = 0; j < n_normals; j++) {
					if (!fgets (readline, 256, in))			return false;

					if (sscanf (readline, "%f %f %f",&normals[j].Nor.x,
																&normals[j].Nor.y,
																&normals[j].Nor.z) != 3)
						return false;
				}

				// triangles
				if (!fgets (readline, 256, in))			return false;

				if (sscanf (readline, "%d", &mesh[i].n_poly) != 1)
					return false;
				mesh[i].poly = new POLYGON [mesh[i].n_poly];

				for (int j = 0; j < mesh[i].n_poly; j++) {
					if (!fgets (readline, 256, in))			return false;

					int v1, v2, v3, n1, n2, n3;
					if (sscanf (readline, "%d %d %d %d %d %d %d %d",
                        	&n_flags,
                        	&v1, &v2, &v3,
                        	&n1, &n2, &n3,
                        	&n_index) != 8)
						return false;

					// copio i vertici
					mesh[i].poly[j].Verts[0].Pos.x = verts[v1].Pos.x;
					mesh[i].poly[j].Verts[0].Pos.y = verts[v1].Pos.y;
					mesh[i].poly[j].Verts[0].Pos.z = verts[v1].Pos.z;
					mesh[i].poly[j].Verts[1].Pos.x = verts[v2].Pos.x;
					mesh[i].poly[j].Verts[1].Pos.y = verts[v2].Pos.y;
					mesh[i].poly[j].Verts[1].Pos.z = verts[v2].Pos.z;
					mesh[i].poly[j].Verts[2].Pos.x = verts[v3].Pos.x;
					mesh[i].poly[j].Verts[2].Pos.y = verts[v3].Pos.y;
					mesh[i].poly[j].Verts[2].Pos.z = verts[v3].Pos.z;
					// copio le normali
					mesh[i].poly[j].Verts[0].Nor.x = normals[n1].Nor.x;
					mesh[i].poly[j].Verts[0].Nor.y = normals[n1].Nor.y;
					mesh[i].poly[j].Verts[0].Nor.z = normals[n1].Nor.z;
					mesh[i].poly[j].Verts[1].Nor.x = normals[n2].Nor.x;
					mesh[i].poly[j].Verts[1].Nor.y = normals[n2].Nor.y;
					mesh[i].poly[j].Verts[1].Nor.z = normals[n2].Nor.z;
					mesh[i].poly[j].Verts[2].Nor.x = normals[n3].Nor.x;
					mesh[i].poly[j].Verts[2].Nor.y = normals[n3].Nor.y;
					mesh[i].poly[j].Verts[2].Nor.z = normals[n3].Nor.z;
				}

				// CLEAR - fatto da me
				delete verts;
				delete normals;
			}
		}

		// lettura materiali (COLORI)
		if (sscanf (readline, "Materials: %d", &num_colors) == 1) {
			color = new Color [num_colors];
			char szName[50];
			float temp;

			for (int i = 0; i < num_colors; i++) {
				// name
				if (!fgets (readline, 256, in))			return false;
				if (sscanf (readline, "\"%[^\"]\"", szName) != 1)
					return false;

				// ambient
				if (!fgets (readline, 256, in))			return false;

				// diffuse
				if (!fgets (readline, 256, in))			return false;
				if (sscanf (readline, "%f %f %f %f", &color[i].rgb[0], &color[i].rgb[1], &color[i].rgb[2], &temp) != 4)
					return false;

				// specular
				if (!fgets (readline, 256, in))			return false;

				// emissive
				if (!fgets (readline, 256, in))			return false;

				// shininess
				if (!fgets (readline, 256, in))			return false;

				// transparency
				if (!fgets (readline, 256, in))			return false;

				// diffuse texture
				if (!fgets (readline, 256, in))			return false;

				// alpha texture
				if (!fgets (readline, 256, in))			return false;
			}
		}
	}
	fclose (in);												// close
	return true;												// ok :)
}
