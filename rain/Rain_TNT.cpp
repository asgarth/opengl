//-------------		file			Rain_TNT.cpp		-----------------------------------------------------------------------------------

#include "Rain_TNT.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_TNT::Rain_TNT (float height)
{
	xpl_start = NULL;
	xpl_height = height;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_TNT::~Rain_TNT () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_TNT::Render_Xpl (float frame, bool pause)
{
	static float life_per_frame, frame05, frame06, frame07;
	frame05 = -0.5 * frame;
	frame06 = -0.6 * frame;
	frame07 = -0.7 * frame;
	glEnable (GL_BLEND);
	part_list *xpl_temp = xpl_start;
      while (xpl_temp != NULL) {
	      glPushMatrix ();
	      glTranslatef (xpl_temp -> x, xpl_height, xpl_temp -> z);
	      glRotatef (-60.0, 1.0, 0.0, 0.0);
	      if (!pause)
	      	xpl_temp -> z += xpl_temp -> speed * frame;
	      xpl_temp -> destroy = true;
	      for (int loop = 0; loop < xpl_temp -> num_part; loop++) {
		      if (xpl_temp -> partX[loop].life > 0.0) {
				glColor4f (xpl_temp -> color_r, xpl_temp -> color_g, xpl_temp -> color_b, xpl_temp -> partX[loop].life);
				glBegin (GL_TRIANGLE_STRIP);
					glTexCoord2f (1.0, 1.0);		glVertex3f (xpl_temp -> partX[loop].x + TNT_SIDE, xpl_temp -> partX[loop].y + TNT_SIDE, xpl_temp -> partX[loop].z);
					glTexCoord2f (0.0, 1.0);		glVertex3f (xpl_temp -> partX[loop].x, xpl_temp -> partX[loop].y + TNT_SIDE, xpl_temp -> partX[loop].z);
					glTexCoord2f (1.0, 0.0);		glVertex3f (xpl_temp -> partX[loop].x + TNT_SIDE, xpl_temp -> partX[loop].y, xpl_temp -> partX[loop].z);
					glTexCoord2f (0.0, 0.0);		glVertex3f (xpl_temp -> partX[loop].x, xpl_temp -> partX[loop].y, xpl_temp -> partX[loop].z);
				glEnd();
				if (!pause) {
					life_per_frame = xpl_temp -> partX[loop].life * frame;
		    		  	xpl_temp -> partX[loop].x += xpl_temp -> partX[loop].xs * life_per_frame;
		     		 	xpl_temp -> partX[loop].y += xpl_temp -> partX[loop].ys * life_per_frame;
		      		xpl_temp -> partX[loop].z += xpl_temp -> partX[loop].zs * life_per_frame;
		      		xpl_temp -> partX[loop].life -= xpl_temp -> partX[loop].fade * frame;
	      		}
		      	xpl_temp -> destroy = false;
	      	}
	      }
	      if (!pause) {
			xpl_temp -> color_r += frame05;
			xpl_temp -> color_g += frame06;
			xpl_temp -> color_b += frame07;
		}

	      glPopMatrix ();
	      if (xpl_temp -> destroy) {
	      	part_list *xpl_temp_canc = xpl_temp;
	      	xpl_temp = xpl_temp -> next;
	      	Delete_Xpl (xpl_temp_canc);
      	}
	      else
	      	xpl_temp = xpl_temp -> next;
      }
      glColor4f (1.0, 1.0, 1.0, 1.0);
      glDisable (GL_BLEND);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_TNT::Add_Xpl (const enemy_list *e, int tipo)
{
	float fade_temp = 0.9 - e -> ray * 0.1f;
	part_list *p = new (part_list);
	p -> x = e -> x;
	p -> z = e -> z;
	p -> speed = e -> speed * 0.5f;
	p -> color_r = 1.0;
	p -> color_g = e -> color_g * 0.5f;
	p -> color_b = e -> color_b * 0.3f;
	p -> num_part = int(e -> ray * 30.0f);
	p -> partX = new particleXPL[p -> num_part];
	p -> destroy = false;
	for (int loop = 0; loop < p -> num_part; loop++) {
		p -> partX[loop].x = p -> partX[loop].y = p -> partX[loop].z = 0.0f;
		p -> partX[loop].xs = float(rand()%28) * 0.5f - 7.0f;
		p -> partX[loop].ys = float(rand()%28) * 0.5f - 7.0f;
		switch (tipo) {
			case 0:	p -> partX[loop].zs = float(rand()%28) * 0.5f + 3.0f;
					break;
			case 1:	p -> partX[loop].zs = float(rand()%28) * 0.5f - 7.0f;
					break;
		}
		p -> partX[loop].life = 1.0f;
		p -> partX[loop].fade = float(rand()%100) * 0.01f + fade_temp;
	}
	p -> prev = NULL;
	p -> next = xpl_start;
	if (xpl_start != NULL)
		xpl_start -> prev = p;
	xpl_start = p;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_TNT::Add_Xpl (const fire_list *f, int tipo)
{
	part_list *p = new (part_list);
	p -> x = f -> x;
	p -> z = f -> z;
	p -> speed = 0.0f;
	if (tipo != 0) {
		p -> color_r = 0.5f;
		p -> color_g = 0.7f;
		p -> color_b = 1.0f;
	}
	else {
		p -> color_r = 0.4f;
		p -> color_g = 1.0f;
		p -> color_b = 0.4f;
	}
	p -> num_part = 20;
	p -> partX = new particleXPL[p -> num_part];
	p -> destroy = false;
	for (int loop = 0; loop < p -> num_part; loop++) {
		p -> partX[loop].x = p -> partX[loop].y = p -> partX[loop].z = 0.0f;
		p -> partX[loop].xs = float(rand()%28) * 0.5f - 7.0f;
		p -> partX[loop].ys = float(rand()%28) * 0.5f - 7.0f;
		switch (tipo) {
			case 0:	p -> partX[loop].zs = float(rand()%28) * 0.5f + 3.0f;
					break;
			case 1:	p -> partX[loop].zs = float(rand()%28) * 0.5f - 7.0f;
					break;
		}
		p -> partX[loop].life = 1.0f;
		p -> partX[loop].fade = float(rand()%100) * 0.01f + 2.0;
	}
	p -> prev = NULL;
	p -> next = xpl_start;
	if (xpl_start != NULL)
		xpl_start -> prev = p;
	xpl_start = p;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_TNT::Delete_Xpl (part_list *p)
{
	if (p -> prev != NULL)
		p -> prev -> next = p -> next;
	else
		xpl_start = p -> next;
	if (p -> next != NULL)
		p -> next -> prev = p -> prev;
	delete (p -> partX);
	delete (p);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------