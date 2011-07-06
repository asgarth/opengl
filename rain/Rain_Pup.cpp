//-------------		file			Rain_Pup.cpp		-----------------------------------------------------------------------------------

#include "Rain_Pup.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_Pup::Rain_Pup (float height)
{
	xpl_start = NULL;
	xpl_height = height;

	// inizializzazione proprietà oggetti 3D
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_Pup::~Rain_Pup () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Pup::Render_Power_Up (float frame, bool pause)
{
	glEnable (GL_BLEND);
	part_list *xpl_temp = xpl_start;
      while (xpl_temp != NULL) {
	      glPushMatrix ();
	      glTranslatef (xpl_temp -> x, xpl_height, xpl_temp -> z);
		switch (xpl_temp -> type) {
			case 0:	glColor4f (0.3f, 1.0f, 0.3f, 1.0f);			// VERDE + BLU
					gluSphere (quadratic, xpl_temp -> ray * 0.5f, 8, 8);
					glColor4f (0.7f, 0.7f, 1.0f, 0.5f);
					gluSphere (quadratic, xpl_temp -> ray, 16, 16);
					glColor4f (0.3f, 1.0f, 0.3f, 0.6f);
					break;
			case 1:	glColor4f (1.0f, 0.3f, 0.3f, 1.0f);			// ROSSO + BLU
					gluSphere (quadratic, xpl_temp -> ray * 0.5f, 8, 8);
					glColor4f (0.5f, 0.5f, 1.0f, 0.5f);
					gluSphere (quadratic, xpl_temp -> ray, 16, 16);
					glColor4f (1.0f, 0.3f, 0.3f, 0.6f);
					break;
			case 2:	glColor4f (0.3f, 0.3f, 1.0f, 1.0f);			// BLU + VERDE
					gluSphere (quadratic, xpl_temp -> ray * 0.5f, 8, 8);
					glColor4f (0.5f, 1.0f, 0.5f, 0.5f);
					gluSphere (quadratic, xpl_temp -> ray, 16, 16);
					glColor4f (0.3f, 0.3f, 1.0f, 0.6f);
					break;
			case 3:	glColor4f (0.3f, 0.3f, 1.0f, 1.0f);			// BLU + ROSSO
					gluSphere (quadratic, xpl_temp -> ray * 0.5f, 8, 8);
					glColor4f (1.0f, 0.7f, 0.7f, 0.5f);
					gluSphere (quadratic, xpl_temp -> ray, 16, 16);
					glColor4f (0.3f, 0.3f, 1.0f, 1.0f);
					break;
		}
		// disegna i cerchi che girano
		glPushMatrix ();
		glRotatef (xpl_temp -> angle, 1.0f, 0.0f, 0.0f);
		gluDisk(quadratic, xpl_temp -> ray, xpl_temp -> ray + 0.25f, 16, 1);
		glPopMatrix ();
		glRotatef (-xpl_temp -> angle, 0.0f, 1.0f, 0.0f);
		gluDisk(quadratic, xpl_temp -> ray, xpl_temp -> ray + 0.25f, 16, 1);
	      glPopMatrix ();
	      if (!pause) {
	      	xpl_temp -> z += 10.0f * frame;
	      	xpl_temp -> angle += 100.0f * frame;
      	}
	      if (xpl_temp -> destroy) {
	      	part_list *xpl_temp_canc = xpl_temp;
	      	xpl_temp = xpl_temp -> next;
	      	Delete_Power_Up (xpl_temp_canc);
      	}
	      else
	      	xpl_temp = xpl_temp -> next;
      }
      glDisable (GL_BLEND);
      glColor4f (1.0, 1.0, 1.0, 1.0);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Pup::Add_Power_Up (float x, float z)
{
	part_list *p = new (part_list);
	p -> x = x;
	p -> z = z;
	p -> ray = 0.4f;
	p -> angle = rand()%360;
	p -> type = rand()%4;
	p -> destroy = false;
	p -> prev = NULL;
	p -> next = xpl_start;
	if (xpl_start != NULL)
		xpl_start -> prev = p;
	xpl_start = p;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Pup::Delete_Power_Up (part_list *p)
{
	if (p -> prev != NULL)
		p -> prev -> next = p -> next;
	else
		xpl_start = p -> next;
	if (p -> next != NULL)
		p -> next -> prev = p -> prev;
	delete (p);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------