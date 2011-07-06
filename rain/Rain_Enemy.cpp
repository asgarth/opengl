//-------------		file			Rain_Enemy.cpp		-----------------------------------------------------------------------------------

#include "Rain_Enemy.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_Enemy::Rain_Enemy (float height)
{
	start_enemy = NULL;
	enemy_height = height;
	detail_level = 1;

	// inizializzazione proprietà oggetti 3D
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_Enemy::~Rain_Enemy () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Enemy::Render_Enemy (float frame, bool pause)
{
	static int temp_det;
	switch (detail_level) {
		case 0:	temp_det = 8;
				break;
		case 1:	temp_det = 32;
				break;
		case 2:	temp_det = 64;
				break;
	}
	enemy_list *enemy_temp = start_enemy;
      while (enemy_temp != NULL) {
	      glPushMatrix ();
	      glTranslatef (enemy_temp -> x, enemy_height, enemy_temp -> z);
		switch (enemy_temp -> tipe) {
			case 1:	glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 1.0);
					gluSphere (quadratic, enemy_temp -> ray, temp_det, temp_det);
					if (!pause)
						enemy_temp -> z += enemy_temp -> speed * frame;
					break;
			case 2:	glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 1.0f);
					gluSphere (quadratic, enemy_temp -> ray, temp_det, temp_det);
					glPushMatrix ();
					glTranslatef (0.5, 0.0, -1.0);
					glRotatef (enemy_temp -> angle, 0.0, 0.0, 1.0);
					glColor4f (enemy_temp -> color_g, enemy_temp -> color_b, enemy_temp -> color_r, 1.0f);
					gluCylinder (quadratic , 0.7, 0.0, 3.0, 4, 4);
					glPopMatrix ();
					glPushMatrix ();
					glTranslatef (-0.5, 0.0, -1.0);
					glRotatef (-enemy_temp -> angle, 0.0, 0.0, 1.0);
					glColor4f (enemy_temp -> color_g, enemy_temp -> color_b, enemy_temp -> color_r, 1.0f);
					gluCylinder (quadratic , 0.7, 0.0, 3.0, 4, 4);
					glPopMatrix ();
					if (!pause) {
						enemy_temp -> angle += enemy_temp -> angle_speed * frame;
						enemy_temp -> z += enemy_temp -> speed * frame;
					}
					break;
			case 3:	glPushMatrix ();
					glColor4f (enemy_temp -> color_g, enemy_temp -> color_b, enemy_temp -> color_r, 1.0f);
					glRotatef (enemy_temp -> angle, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.6, 0.0, 2.0, 4, 4);					// punta 1
					glRotatef (90.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.6, 0.0, 2.0, 4, 4);					// punta 2
					glRotatef (90.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.6, 0.0, 2.0, 4, 4);					// punta 3
					glRotatef (90.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.6, 0.0, 2.0, 4, 4);					// punta 4
					glPopMatrix ();
					glEnable (GL_BLEND);
					glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 0.95f);
					gluSphere (quadratic, 0.8, temp_det >> 1, temp_det >> 1);
					glDisable (GL_BLEND);
					if (!pause) {
						enemy_temp -> angle += enemy_temp -> angle_speed * frame;
						enemy_temp -> z += enemy_temp -> speed * frame;
					}
					break;
			case 4:	glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 1.0f);
					gluSphere (quadratic, 1.0, temp_det, temp_det);
					glColor4f (enemy_temp -> color_g, enemy_temp -> color_b, enemy_temp -> color_r, 1.0f);
					glPushMatrix ();
					glTranslatef (-0.7, 0.0, -1.5);
					glRotatef (50.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.2, 0.2, 1.6, 8, 8);
					glPopMatrix ();
					glPushMatrix ();
					glTranslatef (0.7, 0.0, -1.5);
					glRotatef (-50.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.2, 0.2, 1.6, 8, 8);
					glPopMatrix ();
					glColor4f (enemy_temp -> color_b, enemy_temp -> color_g, enemy_temp -> color_r, 1.0f);
					glPushMatrix ();
					glTranslatef (0.7, 0.0, -2.0);
					glRotatef (35.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.7, 0.0, 2.5, 4, 4);
					glPopMatrix ();
					glPushMatrix ();
					glTranslatef (-0.7, 0.0, -2.0);
					glRotatef (-35.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.7, 0.0, 2.5, 4, 4);
					glPopMatrix ();
					if (!pause)
						enemy_temp -> z += enemy_temp -> speed * frame;
					break;
			case 5:	glTranslatef (0.0, 0.0, 0.8);
					glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 1.0f);
					gluSphere (quadratic, 0.8, temp_det >> 1, temp_det >> 1);
					glPushMatrix ();
					glTranslatef (-1.0, 0.0, -2.5);
					gluCylinder (quadratic , 0.5, 0.0, 2.0, 4, 4);
					glPopMatrix ();
					glPushMatrix ();
					glTranslatef (1.0, 0.0, -2.5);
					gluCylinder (quadratic , 0.5, 0.0, 2.0, 4, 4);
					glPopMatrix ();
					glColor4f (enemy_temp -> color_g, enemy_temp -> color_b, enemy_temp -> color_r, 1.0f);
					glPushMatrix ();
					glTranslatef (-1.0, 0.0, -1.5);
					glRotatef (40.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.2, 0.2, 1.6, 8, 8);
					glPopMatrix ();
					glPushMatrix ();
					glTranslatef (1.0, 0.0, -1.5);
					glRotatef (-40.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.2, 0.2, 1.6, 8, 8);
					glPopMatrix ();
					if (!pause)
						enemy_temp -> z += enemy_temp -> speed * frame;
					break;
			case 31:	glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 1.0f);
					gluSphere (quadratic, 3.0, temp_det, temp_det);
					glColor4f (enemy_temp -> color_b, enemy_temp -> color_g, enemy_temp -> color_r, 1.0f);
					glPushMatrix ();
					glTranslatef (-1.0, -0.5, 1.0);
					gluCylinder (quadratic , 0.8, 0.0, 4.0, 4, 4);
					glPopMatrix ();
					glPushMatrix ();
					glTranslatef (1.0, -0.5, 1.0);
					gluCylinder (quadratic , 0.8, 0.0, 4.0, 4, 4);
					glPopMatrix ();
					glPushMatrix ();
					glTranslatef (0.0, 0.5, 1.0);
					gluCylinder (quadratic , 0.8, 0.0, 5.0, 4, 4);
					glPopMatrix ();
					if (!pause) {
						if (enemy_temp -> z < -35.0f)
							enemy_temp -> z += enemy_temp -> speed * frame;
						else {
							enemy_temp -> x += sin (enemy_temp -> angle_speed * 0.0174532925f) * 0.25f;
							enemy_temp -> angle_speed += 2.0f;
							if (enemy_temp -> angle_speed > 360.0f)
								enemy_temp -> angle_speed -= 360.0f;
						}
					}
					break;
			case 32:	glColor4f (enemy_temp -> color_g, enemy_temp -> color_b, enemy_temp -> color_r, 1.0f);
					glTranslatef (-2.0, 0.0, 0.0);
					glPushMatrix ();
					glRotatef (90.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.35, 0.35, 4.0, 8, 8);
					glPopMatrix ();
					glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 1.0f);
					glTranslatef (3.8, 0.0, 0.0);
					glPushMatrix ();
					glRotatef (enemy_temp -> angle, 0.0, 0.0, 1.0);
					gluCylinder (quadratic , 0.8, 0.0, 4.0, 4, 4);
					glPopMatrix ();
					glPushMatrix ();
					glRotatef (180.0, 0.0, 1.0, 0.0);
					glRotatef (-enemy_temp -> angle, 0.0, 0.0, 1.0);
					gluCylinder (quadratic , 0.8, 0.0, 4.0, 4, 4);
					glPopMatrix ();
					glTranslatef (-1.7, 0.0, 0.0);
					glEnable (GL_BLEND);
					glColor4f (0.4f, 1.0f, 0.4f, 0.6f);
					glRotatef (90.0, 0.0, 0.0, 1.0);
					glRotatef (enemy_temp -> angle, 0.0, 1.0, 0.0);
					gluDisk(quadratic, 0.8, 1.2, 16, 1);
					glDisable (GL_BLEND);
					if (!pause) {
						enemy_temp -> angle += 60.0 * frame;
						if (enemy_temp -> z < -35.0f)
							enemy_temp -> z += enemy_temp -> speed * frame;
						else {
							enemy_temp -> x += sin (enemy_temp -> angle_speed * 0.0174532925f) * 0.25f;
							enemy_temp -> angle_speed += 2.0f;
							if (enemy_temp -> angle_speed > 360.0f)
								enemy_temp -> angle_speed -= 360.0f;
						}
					}
					break;
			case 33:	glColor4f (enemy_temp -> color_g, enemy_temp -> color_b, enemy_temp -> color_r, 1.0f);
					glTranslatef (2.0, 0.0, 0.0);
					glPushMatrix ();
					glRotatef (-90.0, 0.0, 1.0, 0.0);
					gluCylinder (quadratic , 0.35, 0.35, 4.0, 8, 8);
					glPopMatrix ();
					glColor4f (enemy_temp -> color_r, enemy_temp -> color_g, enemy_temp -> color_b, 1.0f);
					glTranslatef (-3.8, 0.0, 0.0);
					glPushMatrix ();
					glRotatef (enemy_temp -> angle, 0.0, 0.0, 1.0);
					gluCylinder (quadratic , 0.8, 0.0, 4.0, 4, 4);
					glPopMatrix ();
					glPushMatrix ();
					glRotatef (180.0, 0.0, 1.0, 0.0);
					glRotatef (-enemy_temp -> angle, 0.0, 0.0, 1.0);
					gluCylinder (quadratic , 0.8, 0.0, 4.0, 4, 4);
					glPopMatrix ();
					glTranslatef (1.7, 0.0, 0.0);
					glEnable (GL_BLEND);
					glColor4f (0.4f, 1.0f, 0.4f, 0.6f);
					glRotatef (90.0, 0.0, 0.0, 1.0);
					glRotatef (enemy_temp -> angle, 0.0, 1.0, 0.0);
					gluDisk(quadratic, 0.8, 1.2, 16, 1);
					glDisable (GL_BLEND);
					if (!pause) {
						enemy_temp -> angle += 60.0 * frame;
						if (enemy_temp -> z < -35.0f)
							enemy_temp -> z += enemy_temp -> speed * frame;
						else {
							enemy_temp -> x += sin (enemy_temp -> angle_speed * 0.0174532925f) * 0.25f;
							enemy_temp -> angle_speed += 2.0f;
							if (enemy_temp -> angle_speed > 360.0f)
								enemy_temp -> angle_speed -= 360.0f;
						}
					}
					break;
		}
		glPopMatrix ();
		if (enemy_temp -> destroy) {
			enemy_list *enemy_temp_canc = enemy_temp;
			enemy_temp = enemy_temp -> next;
			Delete_Enemy (enemy_temp_canc);
		}
		else
			enemy_temp = enemy_temp -> next;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Enemy::Add_Enemy (float x, float z, float raggio, int tipe, bool p_up)
{
	static float r_col, g_col, b_col;
	enemy_list *e = new (enemy_list);
	e -> x = x;				e -> z = z;
	e -> ray = raggio;			e -> tipe = tipe;
	e -> color_r = float(rand()%175) / 250.0f + 0.3f;
	e -> color_g = float(rand()%175) / 250.0f + 0.3f;
	e -> color_b = float(rand()%175) / 250.0f + 0.3f;
	e -> power_up = p_up;
	switch (tipe) {
		case 1:	e -> energie = 5;
				e -> angle = e -> angle_speed = 0.0f;
				e -> speed = float(rand()%20) * 0.1f + 8.0f;
				e -> enemy_fire_time = float(rand()%20) * 0.1f - 1.0f;
				break;
		case 2:	e -> energie = 15;
				e -> angle = float(rand()%360);
				e -> angle_speed = 45.0f;
				e -> speed = float(rand()%20) * 0.1f + 12.0f;
				e -> enemy_fire_time = float(rand()%20) * 0.1f - 1.0f;
				break;
		case 3:	e -> energie = 45;
				e -> angle = 0.0;
				e -> angle_speed = 50.0f;
				e -> speed = float(rand()%20) * 0.1f + 4.0f;
				e -> enemy_fire_time = float(rand()%20) * 0.1f - 1.0f;
				break;
		case 4:	e -> energie = 30;
				e -> angle = e -> angle_speed = 0.0;
				e -> speed = float(rand()%40) * 0.1f + 12.0f;
				e -> enemy_fire_time = float(rand()%15) * 0.1f;
				break;
		case 5:	e -> energie = 31;
				e -> angle = e -> angle_speed = 0.0;
				e -> speed = float(rand()%40) * 0.1f + 10.0f;
				e -> enemy_fire_time = float(rand()%20) * 0.1f - 1.0f;
				break;
		case 31:	e -> energie = 2000;
				e -> angle = e -> angle_speed = 0.0f;
				e -> speed = 15.0f;
				e -> enemy_fire_time = float(rand()%10) * 0.1f - 1.0f;
				break;
		case 32:	e -> energie = 800;
				e -> angle = 0.0f;
				e -> angle_speed = 0.0f;
				e -> speed = 15.0f;
				e -> enemy_fire_time = float(rand()%10) * 0.1f - 1.0f;
				r_col = e -> color_r;
				g_col = e -> color_g;
				b_col = e -> color_b;
				break;
		case 33:	e -> energie = 800;
				e -> angle = 0.0f;
				e -> angle_speed = 0.0f;
				e -> speed = 15.0f;
				e -> enemy_fire_time = float(rand()%10) * 0.1f - 1.0f;
				e -> color_r = r_col;
				e -> color_g = g_col;
				e -> color_b = b_col;
				break;
	}
	e -> destroy = e -> drawn = false;
	e -> prev = NULL;
	e -> next = start_enemy;
	if (start_enemy != NULL)
		start_enemy -> prev = e;
	start_enemy = e;

}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_Enemy::Delete_Enemy (enemy_list *e)
{
	if (e -> prev != NULL)
		e -> prev -> next = e -> next;
	else
		start_enemy = e -> next;
	if (e -> next != NULL)
		e -> next -> prev = e -> prev;
	delete (e);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------