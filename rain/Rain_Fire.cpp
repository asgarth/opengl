//-------------		file			Rain_Fire.cpp		-----------------------------------------------------------------------------------

#include "Rain_Fire.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_Fire::Rain_Fire (float height)
{
	fire_start = NULL;
	fire_left = true;
	fire_height = height;
	fire_type = 1;
	fire_power = 6;

	cos_15 = cos (15.0 * 0.0174532925f);
	sin_15 = sin (15.0 * 0.0174532925f);
	cos_25 = cos (25.0 * 0.0174532925f);
	sin_25 = sin (25.0 * 0.0174532925f);

	// inizializzazione proprietà oggetti 3D
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_Fire::~Rain_Fire () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Fire::Aggiorna_Fire (float frame, bool pause)
{
	glEnable (GL_BLEND);
	fire_list *fire_temp = fire_start;
      while (fire_temp != NULL) {
	      glPushMatrix ();
	      glTranslatef (fire_temp -> x, fire_height, fire_temp -> z);
	      glRotatef (-fire_temp -> angle, 0.0, 1.0, 0.0);
	      if (fire_temp -> power > 15)
	      	glColor4f (1.0f, 0.4f, 0.4f, 0.9f);
	      else
	      	if (fire_temp -> power > 10)
	      		glColor4f (0.8f, 0.8f, 0.2f, 0.9f);
	      	else
	      		glColor4f (0.4f, 0.4f, 1.0f, 0.9f);
	      glCallList(fire_gl_list);
	      glPopMatrix ();
	      if (!pause) {
		      fire_temp -> z -= fire_temp -> z_sp * frame;
			fire_temp -> x += fire_temp -> x_sp * frame;
		}
	      if (fire_temp -> destroy) {
	      	fire_list *fire_temp_canc = fire_temp;
	      	fire_temp = fire_temp -> next;
	      	Delete_Fire (fire_temp_canc);
      	}
	      else
	      	fire_temp = fire_temp -> next;
      }
      glDisable (GL_BLEND);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Fire::Add_Fire (float shipx, float shipz)
{
	switch (fire_type) {
		case 1:	if (fire_left)
					Add_F_Angle (shipx - 0.30f, shipz - 1.0f, 25.0, 0.0);
				else
					Add_F_Angle (shipx + 0.30f, shipz - 1.0f, 25.0, 0.0);
				// inverte da destra a sinistra il prossimo colpo
				fire_left = !fire_left;
				break;
		case 2:	Add_F_Angle (shipx, shipz - 1.0f, 25.0, 0.0);
				Add_F_Angle (shipx + 0.40f, shipz - 0.7f, 25.0, 15.0);
				Add_F_Angle (shipx - 0.40f, shipz - 0.7f, 25.0, -15.0);
				break;
		case 3:	Add_F_Angle (shipx, shipz - 1.0f, 25.0, 0.0);
				Add_F_Angle (shipx + 0.25f, shipz - 0.7f, 23.0, 15.0);
				Add_F_Angle (shipx - 0.25f, shipz - 0.7f, 23.0, -15.0);
				Add_F_Angle (shipx + 0.40f, shipz - 0.7f, 21.0, 25.0);
				Add_F_Angle (shipx - 0.40f, shipz - 0.7f, 21.0, -25.0);
				break;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Fire::Add_F_Angle (float x, float z, float vel, int angle)
{
	fire_list *f = new (fire_list);
	f -> x = x;
	f -> z = z;
	f -> power = fire_power;
	f -> angle = angle;
	switch (angle) {
		case 0:		f -> z_sp = vel;
					f -> x_sp = 0.0;
					break;
		case 15:		f -> z_sp = vel * cos_15;
					f -> x_sp = vel * sin_15;
					break;
		case -15:	f -> z_sp = vel * cos_15;
					f -> x_sp = -(vel * sin_15);
					break;
		case 25:		f -> z_sp = vel * cos_25;
					f -> x_sp = vel * sin_25;
					break;
		case -25:	f -> z_sp = vel * cos_25;
					f -> x_sp = -(vel * sin_25);
					break;
	}
	f -> destroy = false;
	f -> prev = NULL;
	f -> next = fire_start;
	if (fire_start != NULL)
		fire_start -> prev = f;
	fire_start = f;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Fire::Delete_Fire (fire_list *f)
{
	if (f -> prev != NULL)
		f -> prev -> next = f -> next;
	else
		fire_start = f -> next;
	if (f -> next != NULL)
		f -> next -> prev = f -> prev;
	delete (f);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Fire::Build_Fire_List (void)
{
	fire_gl_list = glGenLists(1);
      glNewList(fire_gl_list, GL_COMPILE);
		gluCylinder (quadratic , 0.0, 0.17, 0.5, 4, 4);
	glEndList();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Fire::Delete_Fire_List (void)
{
	glDeleteLists (fire_gl_list, 1);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------