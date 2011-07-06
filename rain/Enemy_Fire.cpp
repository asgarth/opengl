//-------------		file			Enemy_Fire.cpp		-----------------------------------------------------------------------------------

#include "Enemy_Fire.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Enemy_Fire::Enemy_Fire (float height, float raggio)
{
	start_ef = NULL;
	ef_height = height;
	fire_ray = raggio;

	// inizializzazione proprietà oggetti 3D
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Enemy_Fire::~Enemy_Fire () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Enemy_Fire::Render_Enemy_Fire (float frame, bool pause)
{
	static float frame15;
	frame15 = 15.0 * frame;
	ef_list *ef_temp = start_ef;
      while (ef_temp != NULL) {
	      glPushMatrix ();
	      glTranslatef (ef_temp -> x, ef_height, ef_temp -> z);
	      glCallList(ef_fire_list_1);
	      glPopMatrix ();
		// calcola la posizione del colpo
		if (!pause) {
	      	ef_temp -> z += ef_temp -> angle_cos * frame15;
	      	if (ef_temp -> use_x_pos) {
				if (ef_temp -> x_pos)
	      			ef_temp -> x += ef_temp -> angle_sin * frame15;
				else
	      			ef_temp -> x -= ef_temp -> angle_sin * frame15;
      		}
      		else
      			ef_temp -> x += ef_temp -> angle_sin * frame15;
      	}
		// controlla se il colpo è fuori dai margini e se sì lo disrugge
		if (ef_temp -> destroy) {
	      	ef_list *ef_temp_canc = ef_temp;
	      	ef_temp = ef_temp -> next;
	      	Delete_Enemy_Fire (ef_temp_canc);
      	}
	      else
	      	ef_temp = ef_temp -> next;
      }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Enemy_Fire::Add_Enemy_Fire (float xs, float zs, float xe, float ze, float dist)
{
	static float angle;
	ef_list *e = new (ef_list);
	e -> x = xs;				e -> z = zs;
	// calcola angolo di inclinazione del colpo
	if (xe > xs)
		e -> x_pos = true;
	else
		e -> x_pos = false;
	e -> use_x_pos = true;
	angle = acos ((ze - zs) / dist);
	e -> angle_sin = sin (angle);
	e -> angle_cos = cos (angle);
	e -> destroy = e -> drawn = false;
	e -> prev = NULL;
	e -> next = start_ef;
	if (start_ef != NULL)
		start_ef -> prev = e;
	start_ef = e;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Enemy_Fire::Add_EF_M (float x, float z, float angle)
{
	ef_list *e = new (ef_list);
	e -> x = x;				e -> z = z;
	// calcola angolo di inclinazione del colpo
	e -> angle_sin = sin (angle * 0.0174532925f);
	e -> angle_cos = cos (angle * 0.0174532925f);
	e -> x_pos = e -> use_x_pos = false;
	e -> destroy = e -> drawn = false;
	e -> prev = NULL;
	e -> next = start_ef;
	if (start_ef != NULL)
		start_ef -> prev = e;
	start_ef = e;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Enemy_Fire::Delete_Enemy_Fire (ef_list *e)
{
	if (e -> prev != NULL)
		e -> prev -> next = e -> next;
	else
		start_ef = e -> next;
	if (e -> next != NULL)
		e -> next -> prev = e -> prev;
	delete (e);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Enemy_Fire::Build_EF_List (void)
{
	ef_fire_list_1 = glGenLists(1);
      glNewList(ef_fire_list_1, GL_COMPILE);
      	glEnable (GL_BLEND);
		glColor4f (1.0f, 0.4f, 0.2f, 1.0f);
		gluSphere (quadratic, fire_ray, 8, 8);
		glDisable (GL_BLEND);
	glEndList();
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Enemy_Fire::Delete_EF_List (void)
{
	glDeleteLists (ef_fire_list_1, 1);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------