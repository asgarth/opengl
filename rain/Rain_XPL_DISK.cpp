//-------------		file			Rain_XPL_DISK.cpp		-----------------------------------------------------------------------------------

#include "Rain_XPL_DISK.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_XPL_DISK::Rain_XPL_DISK (float height)
{
	xpl_start = NULL;
	xpl_height = height;

	// inizializzazione proprietà oggetti 3D
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_XPL_DISK::~Rain_XPL_DISK () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_XPL_DISK::Render_Xpl (float frame, bool pause)
{
	static float ray_per_frame, fade_per_frame;
	glEnable (GL_BLEND);
	glDisable (GL_TEXTURE_2D);
	part_list *xpl_temp = xpl_start;
      while (xpl_temp != NULL) {
	      glPushMatrix ();
	      glTranslatef (xpl_temp -> x, xpl_height, xpl_temp -> z);
	      glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
	      // cerchio 1
		glColor4f (xpl_temp -> color_r, xpl_temp -> color_g, xpl_temp -> color_b, xpl_temp -> life - 0.45f);
		gluDisk(quadratic, xpl_temp -> ray_1, xpl_temp -> ray_2, 16, 1);
		// cerchio 2
		glColor4f (xpl_temp -> color_r, xpl_temp -> color_g, xpl_temp -> color_b, xpl_temp -> life - 0.3f);
		gluDisk(quadratic, xpl_temp -> ray_2, xpl_temp -> ray_3, 16, 1);
		// cerchio 3
		glColor4f (xpl_temp -> color_r, xpl_temp -> color_g, xpl_temp -> color_b, xpl_temp -> life - 0.15f);
		gluDisk(quadratic, xpl_temp -> ray_3, xpl_temp -> ray_4, 16, 1);
		// cerchio 4
		glColor4f (xpl_temp -> color_r, xpl_temp -> color_g, xpl_temp -> color_b, xpl_temp -> life);
		gluDisk(quadratic, xpl_temp -> ray_4, xpl_temp -> ray_5, 32, 1);
		glPopMatrix ();
		// aggiornamento
		if (!pause) {
			xpl_temp -> z += xpl_temp -> speed * frame;
			ray_per_frame = 6.0 * frame;
			xpl_temp -> ray_1 += ray_per_frame;
			xpl_temp -> ray_2 += ray_per_frame;
			xpl_temp -> ray_3 += ray_per_frame;
			xpl_temp -> ray_4 += ray_per_frame;
			xpl_temp -> ray_5 += ray_per_frame;
			fade_per_frame = xpl_temp -> fade * frame * 0.5;
			xpl_temp -> color_r -= fade_per_frame;
			xpl_temp -> color_g -= fade_per_frame;
			xpl_temp -> life -= xpl_temp -> fade * frame;
		}
		if (xpl_temp -> life < 0.0f) {
	      	part_list *xpl_temp_canc = xpl_temp;
	      	xpl_temp = xpl_temp -> next;
	      	Delete_Xpl (xpl_temp_canc);
      	}
	      else
	      	xpl_temp = xpl_temp -> next;
      }
      glEnable (GL_TEXTURE_2D);
      glDisable (GL_BLEND);
      glColor4f (1.0, 1.0, 1.0, 1.0);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_XPL_DISK::Add_Xpl (const enemy_list *e)
{
	part_list *p = new (part_list);
	p -> x = e -> x;
	p -> z = e -> z;
	p -> ray_1 = 0.0f;
	p -> ray_2 = p -> ray_1 + 0.4f;
	p -> ray_3 = p -> ray_2 + 0.3f;
	p -> ray_4 = p -> ray_3 + 0.2f;
	p -> ray_5 = p -> ray_4 + 0.2f;
	p -> speed = e -> speed * 0.5f;
	p -> color_r = e -> color_r;
	p -> color_g = e -> color_g;
	p -> color_b = e -> color_b;
	p -> life = 1.0f;
	p -> fade = 3.0;
	p -> prev = NULL;
	p -> next = xpl_start;
	if (xpl_start != NULL)
		xpl_start -> prev = p;
	xpl_start = p;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_XPL_DISK::Delete_Xpl (part_list *p)
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