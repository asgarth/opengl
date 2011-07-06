//-------------		file			Rain_EXPL.cpp		-----------------------------------------------------------------------------------

#include "Rain_EXPL.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_EXPL::Rain_EXPL (float height)
{
	xpl_start = NULL;
	xpl_height = height;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Rain_EXPL::~Rain_EXPL () {	}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_EXPL::Render_Xpl (float frame, bool pause)
{
	static float life_per_frame;
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
		      	glPushMatrix ();
				glColor4f (xpl_temp -> color_r, xpl_temp -> color_g, xpl_temp -> color_b, xpl_temp -> partX[loop].life);
				glRotatef (xpl_temp -> partX[loop].angle, xpl_temp -> partX[loop].ax, xpl_temp -> partX[loop].ay, xpl_temp -> partX[loop].az);
				glBegin (GL_TRIANGLES);
					glTexCoord2f (0.4, 0.0);		glVertex3f (xpl_temp -> partX[loop].x + EXPL_SIDE, xpl_temp -> partX[loop].y, xpl_temp -> partX[loop].z);
					glTexCoord2f (0.0, 0.0);		glVertex3f (xpl_temp -> partX[loop].x, xpl_temp -> partX[loop].y, xpl_temp -> partX[loop].z);
					glTexCoord2f (0.2, 0.4);		glVertex3f (xpl_temp -> partX[loop].x + (EXPL_SIDE * 0.5f), xpl_temp -> partX[loop].y + (EXPL_SIDE * 0.5f), xpl_temp -> partX[loop].z);
				glEnd();
				glPopMatrix ();
				if (!pause) {
					life_per_frame = xpl_temp -> partX[loop].life * frame;
					xpl_temp -> partX[loop].angle += 60.0f * frame;
					xpl_temp -> partX[loop].x += xpl_temp -> partX[loop].xs * life_per_frame;
					xpl_temp -> partX[loop].y += xpl_temp -> partX[loop].ys * life_per_frame;
					xpl_temp -> partX[loop].z += xpl_temp -> partX[loop].zs * life_per_frame;
					xpl_temp -> partX[loop].life -= xpl_temp -> partX[loop].fade * frame;
				}
				xpl_temp -> destroy = false;
			}
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

void Rain_EXPL::Add_Xpl (const enemy_list *e)
{
	part_list *p = new (part_list);
	p -> x = e -> x;
	p -> z = e -> z;
	p -> speed = e -> speed * 0.5f;
	p -> color_r = e -> color_r;
	p -> color_g = e -> color_g;
	p -> color_b = e -> color_b;
	p -> num_part = int(e -> ray * 10);
	p -> partX = new particleXPL[p -> num_part];
	p -> destroy = false;
	for (int loop = 0; loop < p -> num_part; loop++) {
		p -> partX[loop].x = p -> partX[loop].y = p -> partX[loop].z = 0.0f;
		p -> partX[loop].xs = float(rand()%15) * 0.5f - 3.5f;
		p -> partX[loop].ys = float(rand()%15) * 0.5f - 3.5f;
		p -> partX[loop].zs = float(rand()%15) * 0.5f - 3.5f;
		p -> partX[loop].ax = float(rand()%21) * 0.1f - 1.0f;
		p -> partX[loop].ay = float(rand()%21) * 0.1f - 1.0f;
		p -> partX[loop].az = float(rand()%21) * 0.1f - 1.0f;
		p -> partX[loop].angle = float(rand()%360);
		p -> partX[loop].life = 1.0f;
		p -> partX[loop].fade = float(rand()%100) * 0.01f + 0.4f;
	}
	p -> prev = NULL;
	p -> next = xpl_start;
	if (xpl_start != NULL)
		xpl_start -> prev = p;
	xpl_start = p;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void Rain_EXPL::Delete_Xpl (part_list *p)
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