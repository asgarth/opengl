//-------------         file              main.cpp          -----------------------------------------------------------------------------------

#include "Genesis_Includes.h"
#include "OpenGL_Genesis.h"
#include "Genesis_Camera.h"
#include "Frustum.h"
#include "Texture_Manager.h"
#include "Genesis_World.h"
#include "Genesis_Terrain.h"
#include "Particle_Engine.h"

#define kSpeed		9.0f					// velocità di movimento della camera
float frame_interval = 0.0f;				// variabile per il movimento basato sul tempo

// imposta 12 colori sfumati dal rosso al viola
static GLfloat colors [12][3] = {	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
								{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
								{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}	};
static char fire_colors [7][3] = {	{245,193,55},{235,167,33},{235,140,33},{235,81,33},{212, 50, 0},{200, 40, 0},{150, 20, 0} };


int RenderScene (GLvoid);
int InitGL (GLvoid);
void Setup_World ();
float Calcola_Distanza (CVector3 position, float x, float y, float z);
void Render_Snow ();
void Render_Rain ();
void Render_Particle ();
void Setup_Snow ();
void Setup_Rain ();
GLvoid Build_Snow_List (GLvoid);
GLvoid Build_Rain_List (GLvoid);
GLvoid Build_Sky_List (GLvoid);
GLvoid Build_Ground_List (GLvoid);
GLvoid Build_Tree_List (GLvoid);
void Genera_Alberi (void);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Calc_Frame_Rate ();
void Render_Text ();
void Genesis_Controller (void);

// variabili per la risoluzione dello schermo
int screen_width = 800;
int screen_height = 600;
int bits = 16;
float time_text;
bool mouse_mov = true;
bool disable_frustum = true;										// disabilita il frustum
bool xp;															// tasto X premuto?
bool pause_snow_rain = false;										// blocca la caduta della neve e della pioggia
bool jp;															// tasto J premuto?
CVector3 current_position;
char str_frame_rate[50] = {0};									// stringa per gli fps

// variabile per l'oscillazione durante il movimento
GLfloat walk_angle;
GLfloat walk;
bool oscillazione = false;											// oscillazione durante il movimento abilitata / disabilitata
bool op;															// tasto O premuto?

// variabili per la base della lista di visualizzazione
GLuint snow_list;
GLuint rain_list;
GLuint sky_list;
GLuint ground_list;
GLuint tree_list;

// puntatore per la gestione delle proprietà degli oggetti 3D
GLUquadricObj *quadratic;

// variabili per la gestione del terreno
bool draw_terrain = true;											// cambia il tipo di terreno
bool gp;

// variabili per il blending
bool blending = true;                                                               // blending on / off    -     default = ON
bool bp;                                                                                  // B premuto?

// variabili per la luce
bool light;                                           		                          // light on / off       -     default = ON
bool lp;                                                                                  // tasto L premuto?
GLfloat Light_0_Ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };                          // valori luce ambiente
GLfloat Light_0_Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };                    // valori luce diffusa
GLfloat Light_0_Position[] = { 0.0f, 29.5f, 0.0f, 1.0f };                         // posiziona la luce in alto

// variabili x la nebbia
bool fog = false;                                                                              // nebbia on / off      -     default = OFF
bool fp;                                                                                        // tasto F premuto?
GLfloat fog_color_light[4] = {0.7f, 0.7f, 0.7f, 1.0f};                                    // colore nebbia light ON
GLfloat fog_color_night[4] = {0.1f, 0.1f, 0.1f, 1.0f};                              // colore nebbia light OFF
GLfloat fog_start_dist = 3.0;                                                       // distanza a cui inizia la nebbia
GLfloat fog_end_dist = 12.0;                                                         // distanza a cui finisce la nebbia
GLfloat fog_density = 0.25;                                                         // densità della nebbia

// variabili x il cielo
bool sky_active = true;											// cielo attivo / disattivo
bool kp;														 	// tasto S premuto?

// varibili per la neve
bool draw_snow = false;											// disegna la neve
bool np;															// tasto N premuto?
typedef struct {	float x, y, z;
				float velocita;									// velocità di caduta della neve
} NEVE;
NEVE* snow;

// varibili per la pioggia
bool draw_rain = false;											// disegna la neve
bool pp;															// tasto N premuto?
typedef struct {	float x, y, z;
				float velocita;									// velocità di caduta della neve
} PIOGGIA;
PIOGGIA* rain;

// variabili x gli alberi
bool draw_tree = true;											// disegna gli alberi
bool tp;															// tasto T premuto?
int numero_alberi;				// numero di alberi da disegnare caricato dal file .TXT
GLfloat* x_tree;				// memoria per coordinate alberi
GLfloat* z_tree;				// memoria per coordinate alberi
int* tipo_albero;

GLfloat altezza_cyl_temp, distanza;
GLfloat x_roll_temp, y_roll_temp;
GLfloat x_temp[4], y_temp[4], z_temp[4], u_temp[4], v_temp[4];
GLfloat angolo_rot;											// angolo di rotazione delle sfere
GLfloat sky_angle;											// angolo di rotazione del cielo
int numero_divisioni;

OpenGL_Genesis		OpenWork;                                                            	 // oggetto globale per le funzioni opengl
Frustum				Genesis_Frustum;
Genesis_World		Genesis_World ("Data/Genesis_World.txt");    		// oggetto globale per la creazione del mondo
Genesis_Camera		Camera;
Texture_Manager	Genesis_Texture;									// oggetto per la gestione di 19 texture

Genesis_Terrain		Terreno_Piatto ("Data/terrain_1.raw");				// oggeto per il terreno piatto
Genesis_Terrain		Terreno_Mosso ("Data/terrain_2.raw");				// oggeto per il terreno con montagne

// variabili per i particle engine
Particle_Engine		Particle ("Data/Particle_Engine.txt");					// creazione di un oggeto per il particle engine
GLfloat x_temp_part, y_temp_part, z_temp_part;
bool enable_particle_engine = true;										// disegna i particle engine di default
bool zp;																	// tasto Z premuto?

//-------------------------------------------------------------------------------------------------------------------------------------------------------

int RenderScene (GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();                                                                               // resetta la visuale

      // TRASFORMAZIONI
      Camera.Look();
      if (oscillazione)
      	glTranslatef (0.0, walk, 0.0);
      Genesis_Frustum.CalculateFrustum ();				// calcola il frustum
      current_position = Camera.Position ();

	// disabilita blending
	glDisable (GL_BLEND);
	glColor4f (1.0f, 1.0f, 1.0f, 1.0f);

	// controlla l'attivazione della nebbia
      if (fog)
	      glEnable(GL_FOG);
      else
            glDisable(GL_FOG);

      // disegna il cielo
      if (sky_active) {
	      glPushMatrix();
	      glTranslatef (0.0, -15.0, 0.0);
	      glRotatef (sky_angle, 0.0, 1.0, 0.0);
	      if (draw_snow || draw_rain)
	      	Genesis_Texture.Usa_Texture (17);
	      else
	      	Genesis_Texture.Usa_Texture (8);
		glCallList(sky_list);									// chiama la lista di visualizzazione del cielo
		// ripristina posizione
		glPopMatrix();
	}

	// disegna il terreno del tipo selezionato
	if (draw_snow)
      	Genesis_Texture.Usa_Texture (18);
      else
      	Genesis_Texture.Usa_Texture (9);
      glNormal3f (0.0f, 0.0f, 0.0f);
	if (!draw_terrain) {
		// disegna un terreno mosso
		glPushMatrix();
		glScalef(0.3, 0.3, 0.3);
 		glCallList(ground_list + 1);
		glPopMatrix();
	}
	else {
      	// disegna un terreno piatto
      	glPushMatrix();
      	glScalef(0.3, 0.3, 0.3);
      	glCallList(ground_list);
      	glPopMatrix();
      }

      // disegna i poligoni non trasparenti della scena
      for (int loop = 0; loop < Genesis_World.sector.num_poligoni; loop++)
      {
	      // scelta texture
            Genesis_Texture.Usa_Texture (Genesis_World.sector.poly[loop].texture);
	      switch (Genesis_World.sector.poly[loop].tipo_poligono) {
		      case 1:	x_temp[0] = Genesis_World.sector.poly[loop].vertex[0].x;
                        	y_temp[0] = Genesis_World.sector.poly[loop].vertex[0].y;
                       		z_temp[0] = Genesis_World.sector.poly[loop].vertex[0].z;
                       		u_temp[0] = Genesis_World.sector.poly[loop].vertex[0].u;
                       		v_temp[0] = Genesis_World.sector.poly[loop].vertex[0].v;

                       		x_temp[1] = Genesis_World.sector.poly[loop].vertex[1].x;
                       		y_temp[1] = Genesis_World.sector.poly[loop].vertex[1].y;
                       		z_temp[1] = Genesis_World.sector.poly[loop].vertex[1].z;
                       		u_temp[1] = Genesis_World.sector.poly[loop].vertex[1].u;
                       		v_temp[1] = Genesis_World.sector.poly[loop].vertex[1].v;

                       		x_temp[2] = Genesis_World.sector.poly[loop].vertex[2].x;
                       		y_temp[2] = Genesis_World.sector.poly[loop].vertex[2].y;
                       		z_temp[2] = Genesis_World.sector.poly[loop].vertex[2].z;
                       		u_temp[2] = Genesis_World.sector.poly[loop].vertex[2].u;
                       		v_temp[2] = Genesis_World.sector.poly[loop].vertex[2].v;

                       		if ( 	disable_frustum ||
                       			Genesis_Frustum.PointInFrustum(x_temp[0], y_temp[0], z_temp[0]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[1], y_temp[1], z_temp[1]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[2], y_temp[2], z_temp[2]) )
                       		{
	                       		glNormal3f (0.0, 0.0, 1.0);
                        		glBegin (GL_TRIANGLES);
                       				glTexCoord2f (u_temp[0], v_temp[0]);            glVertex3f (x_temp[0], y_temp[0], z_temp[0]);
                       				glTexCoord2f (u_temp[1], v_temp[1]);      		glVertex3f (x_temp[1], y_temp[1], z_temp[1]);
                       				glTexCoord2f (u_temp[2], v_temp[2]);            glVertex3f (x_temp[2], y_temp[2], z_temp[2]);
                 				glEnd();
                 			}
                  		break;

                  case 2:	x_temp[0] = Genesis_World.sector.poly[loop].vertex[0].x;
	                        y_temp[0] = Genesis_World.sector.poly[loop].vertex[0].y;
      	                 	z_temp[0] = Genesis_World.sector.poly[loop].vertex[0].z;
            	           	u_temp[0] = Genesis_World.sector.poly[loop].vertex[0].u;
                  	     	v_temp[0] = Genesis_World.sector.poly[loop].vertex[0].v;

                  	     	x_temp[1] = Genesis_World.sector.poly[loop].vertex[1].x;
                      		y_temp[1] = Genesis_World.sector.poly[loop].vertex[1].y;
      	                 	z_temp[1] = Genesis_World.sector.poly[loop].vertex[1].z;
      	                 	u_temp[1] = Genesis_World.sector.poly[loop].vertex[1].u;
      	                 	v_temp[1] = Genesis_World.sector.poly[loop].vertex[1].v;

       	                	x_temp[2] = Genesis_World.sector.poly[loop].vertex[2].x;
        	               	y_temp[2] = Genesis_World.sector.poly[loop].vertex[2].y;
            	           	z_temp[2] = Genesis_World.sector.poly[loop].vertex[2].z;
           	 	           	u_temp[2] = Genesis_World.sector.poly[loop].vertex[2].u;
           	            	v_temp[2] = Genesis_World.sector.poly[loop].vertex[2].v;

         	              	x_temp[3] = Genesis_World.sector.poly[loop].vertex[3].x;
         	              	y_temp[3] = Genesis_World.sector.poly[loop].vertex[3].y;
         	              	z_temp[3] = Genesis_World.sector.poly[loop].vertex[3].z;
          	             	u_temp[3] = Genesis_World.sector.poly[loop].vertex[3].u;
          	             	v_temp[3] = Genesis_World.sector.poly[loop].vertex[3].v;

          	             	if ( 	disable_frustum ||
          	             		Genesis_Frustum.PointInFrustum(x_temp[0], y_temp[0], z_temp[0]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[1], y_temp[1], z_temp[1]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[2], y_temp[2], z_temp[2]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[3], y_temp[3], z_temp[3]) )
                       		{
	                       		glNormal3f (0.0, 0.0, 1.0);
                 	      		glBegin (GL_TRIANGLE_STRIP);
                       				glTexCoord2f (u_temp[0], v_temp[0]);            glVertex3f (x_temp[0], y_temp[0], z_temp[0]);
                       				glTexCoord2f (u_temp[1], v_temp[1]);      		glVertex3f (x_temp[1], y_temp[1], z_temp[1]);
                       				glTexCoord2f (u_temp[2], v_temp[2]);            glVertex3f (x_temp[2], y_temp[2], z_temp[2]);
                       				glTexCoord2f (u_temp[3], v_temp[3]);            glVertex3f (x_temp[3], y_temp[3], z_temp[3]);
                 				glEnd();
                 			}
                 			break;

                 	case 3:	x_temp[0] = Genesis_World.sector.poly[loop].vertex[0].x;
	                        y_temp[0] = Genesis_World.sector.poly[loop].vertex[0].y;
      	                 	z_temp[0] = Genesis_World.sector.poly[loop].vertex[0].z;
            	           	u_temp[0] = Genesis_World.sector.poly[loop].vertex[0].u;
                  	     	v_temp[0] = Genesis_World.sector.poly[loop].vertex[0].v;

                  	     	x_temp[1] = Genesis_World.sector.poly[loop].vertex[1].x;
                      		y_temp[1] = Genesis_World.sector.poly[loop].vertex[1].y;
      	                 	z_temp[1] = Genesis_World.sector.poly[loop].vertex[1].z;
      	                 	u_temp[1] = Genesis_World.sector.poly[loop].vertex[1].u;
      	                 	v_temp[1] = Genesis_World.sector.poly[loop].vertex[1].v;

       	                	x_temp[2] = Genesis_World.sector.poly[loop].vertex[2].x;
        	               	y_temp[2] = Genesis_World.sector.poly[loop].vertex[2].y;
            	           	z_temp[2] = Genesis_World.sector.poly[loop].vertex[2].z;
           	 	           	u_temp[2] = Genesis_World.sector.poly[loop].vertex[2].u;
           	            	v_temp[2] = Genesis_World.sector.poly[loop].vertex[2].v;

         	              	x_temp[3] = Genesis_World.sector.poly[loop].vertex[3].x;
         	              	y_temp[3] = Genesis_World.sector.poly[loop].vertex[3].y;
         	              	z_temp[3] = Genesis_World.sector.poly[loop].vertex[3].z;
          	             	u_temp[3] = Genesis_World.sector.poly[loop].vertex[3].u;
          	             	v_temp[3] = Genesis_World.sector.poly[loop].vertex[3].v;

          	             	if ( 	disable_frustum ||
          	             		Genesis_Frustum.PointInFrustum(x_temp[0], y_temp[0], z_temp[0]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[1], y_temp[1], z_temp[1]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[2], y_temp[2], z_temp[2]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[3], y_temp[3], z_temp[3]) )
                       		{
	                       		if (Genesis_World.sector.poly[loop].text_roll_x == 1) {
		                       		Genesis_World.sector.poly[loop].x_roll += Genesis_World.sector.poly[loop].text_roll_vel * frame_interval;
		                       		if (Genesis_World.sector.poly[loop].x_roll >= (Genesis_World.sector.poly[loop].distanza_text_x))
		                       			Genesis_World.sector.poly[loop].x_roll = 0.0f;
	                       			x_roll_temp = Genesis_World.sector.poly[loop].x_roll;
                       			}
	                       		if (Genesis_World.sector.poly[loop].text_roll_y == 1) {
		                       		Genesis_World.sector.poly[loop].y_roll += Genesis_World.sector.poly[loop].text_roll_vel * frame_interval;
		                       		if (Genesis_World.sector.poly[loop].y_roll >= (Genesis_World.sector.poly[loop].distanza_text_y))
		                       			Genesis_World.sector.poly[loop].y_roll = 0.0f;
	                       			y_roll_temp = Genesis_World.sector.poly[loop].y_roll;
                       			}
	                       		glNormal3f (0.0, 0.0, 1.0);
                 	      		glBegin (GL_TRIANGLE_STRIP);
                       				glTexCoord2f (u_temp[0] + x_roll_temp, v_temp[0] + y_roll_temp);
                       				glVertex3f (x_temp[0], y_temp[0], z_temp[0]);
                       				glTexCoord2f (u_temp[1] + x_roll_temp, v_temp[1] + y_roll_temp);
                       				glVertex3f (x_temp[1], y_temp[1], z_temp[1]);
                       				glTexCoord2f (u_temp[2] + x_roll_temp, v_temp[2] + y_roll_temp);
                       				glVertex3f (x_temp[2], y_temp[2], z_temp[2]);
                       				glTexCoord2f (u_temp[3] + x_roll_temp, v_temp[3] + y_roll_temp);
                       				glVertex3f (x_temp[3], y_temp[3], z_temp[3]);
                 				glEnd();
                 			}
                 			break;

			case 4:	if ( Genesis_Frustum.SphereInFrustum(	Genesis_World.sector.poly[loop].vertex[0].x,
															Genesis_World.sector.poly[loop].vertex[0].y,
															Genesis_World.sector.poly[loop].vertex[0].z,
															Genesis_World.sector.poly[loop].raggio_x_frustum))
					{
						distanza = Calcola_Distanza (	current_position,
													Genesis_World.sector.poly[loop].vertex[0].x,
													Genesis_World.sector.poly[loop].vertex[0].y,
													Genesis_World.sector.poly[loop].vertex[0].z );
						if (distanza < 15)
							numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni;
						else
							if (distanza < 30)
								numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni >> 1;
							else
								if (distanza < 50)
									numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni >> 2;
								else
									numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni >> 3;
						if (numero_divisioni < 3)
							numero_divisioni = 3;
						glPushMatrix();
                 				glTranslatef (	Genesis_World.sector.poly[loop].vertex[0].x,
										Genesis_World.sector.poly[loop].vertex[0].y,
										Genesis_World.sector.poly[loop].vertex[0].z);
                 				if (	Genesis_World.sector.poly[loop].sphere_x_rot != 0.0 ||
	                 				Genesis_World.sector.poly[loop].sphere_y_rot != 0.0 ||
                 					Genesis_World.sector.poly[loop].sphere_z_rot != 0.0) {
								glRotatef (	angolo_rot,
                 									Genesis_World.sector.poly[loop].sphere_x_rot,
                 									Genesis_World.sector.poly[loop].sphere_y_rot,
                 									Genesis_World.sector.poly[loop].sphere_z_rot);
						}
						// disegna la sfera
						gluSphere(	quadratic , Genesis_World.sector.poly[loop].raggio,
									numero_divisioni, numero_divisioni);
                 				// ripristina posizioni
                 				glPopMatrix();
                 			}
                 			break;

			case 5:	altezza_cyl_temp = Genesis_World.sector.poly[loop].altezza;
					x_temp[0] = Genesis_World.sector.poly[loop].vertex[0].x;
                  		y_temp[0] = Genesis_World.sector.poly[loop].vertex[0].y;
                  		z_temp[0] = Genesis_World.sector.poly[loop].vertex[0].z;
                  		if ( Genesis_Frustum.SphereInFrustum(	Genesis_World.sector.poly[loop].vertex[0].x,
															Genesis_World.sector.poly[loop].vertex[0].y,
															Genesis_World.sector.poly[loop].vertex[0].z,
															Genesis_World.sector.poly[loop].raggio_x_frustum))
					{
						distanza = Calcola_Distanza (	current_position,
													Genesis_World.sector.poly[loop].vertex[0].x,
													Genesis_World.sector.poly[loop].vertex[0].y,
													Genesis_World.sector.poly[loop].vertex[0].z );
						if (distanza < 15)
							numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni;
						else
							if (distanza < 30)
								numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni >> 1;
							else
								if (distanza < 50)
									numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni >> 2;
								else
									numero_divisioni = Genesis_World.sector.poly[loop].numero_divisioni >> 3;
						if (numero_divisioni < 3)
							numero_divisioni = 3;
                  			glPushMatrix ();
                  			glTranslatef (x_temp[0], y_temp[0] + (altezza_cyl_temp / 2), z_temp[0]);
                  			glRotatef (90.0, 1.0, 0.0 ,0.0);
						// disegna il cilindro
                  			gluCylinder(	quadratic , Genesis_World.sector.poly[loop].raggio,
                  						Genesis_World.sector.poly[loop].raggio_cima, altezza_cyl_temp,
                  						numero_divisioni, numero_divisioni);
						glPopMatrix ();
						break;
					}
		}
	}

	// controlla se le trasparenza sono attivate
      if (blending) {
	      glEnable (GL_BLEND);										// abilita blending
	      glColor4f (1.0f, 1.0f, 1.0f, 0.5f);								// luminosità piena e 0.5 blending
     	}

     	// disegna i poligoni trasparenti
	for (int loop = 0; loop < Genesis_World.sector.num_poligoni_T; loop++)
      {
	      // scelta texture
            Genesis_Texture.Usa_Texture (Genesis_World.sector.poly_T[loop].texture);
	      switch (Genesis_World.sector.poly_T[loop].tipo_poligono)
	      {
		      case 1:	x_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].x;
                        	y_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].y;
                       		z_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].z;
                       		u_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].u;
                       		v_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].v;

                       		x_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].x;
                       		y_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].y;
                       		z_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].z;
                       		u_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].u;
                       		v_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].v;

                       		x_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].x;
                       		y_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].y;
                       		z_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].z;
                       		u_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].u;
                       		v_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].v;

                       		if ( 	disable_frustum ||
                       			Genesis_Frustum.PointInFrustum(x_temp[0], y_temp[0], z_temp[0]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[1], y_temp[1], z_temp[1]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[2], y_temp[2], z_temp[2]) )
                       		{
	                       		glNormal3f (0.0, 0.0, 1.0);
                        		glBegin (GL_TRIANGLES);
                       				glTexCoord2f (u_temp[0], v_temp[0]);			glVertex3f (x_temp[0], y_temp[0], z_temp[0]);
                       				glTexCoord2f (u_temp[1], v_temp[1]);			glVertex3f (x_temp[1], y_temp[1], z_temp[1]);
                       				glTexCoord2f (u_temp[2], v_temp[2]);			glVertex3f (x_temp[2], y_temp[2], z_temp[2]);
                 				glEnd();
                 			}
                  		break;

                  case 2:	x_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].x;
                        	y_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].y;
                       		z_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].z;
                       		u_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].u;
                    	   	v_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].v;

                   	    	x_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].x;
                   	    	y_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].y;
                    	   	z_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].z;
                    	   	u_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].u;
                     	  	v_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].v;

                     	  	x_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].x;
                     	  	y_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].y;
                     	  	z_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].z;
                     	  	u_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].u;
                     	  	v_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].v;

                   	    	x_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].x;
                  	     	y_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].y;
                  	     	z_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].z;
                  	     	u_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].u;
                  	     	v_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].v;

                       		if ( 	disable_frustum ||
                       			Genesis_Frustum.PointInFrustum(x_temp[0], y_temp[0], z_temp[0]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[1], y_temp[1], z_temp[1]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[2], y_temp[2], z_temp[2]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[3], y_temp[3], z_temp[3]) )
                       		{
	                       		glNormal3f (0.0, 0.0, 1.0);
                 	      		glBegin (GL_TRIANGLE_STRIP);
                       				glTexCoord2f (u_temp[0], v_temp[0]);            glVertex3f (x_temp[0], y_temp[0], z_temp[0]);
                       				glTexCoord2f (u_temp[1], v_temp[1]);      		glVertex3f (x_temp[1], y_temp[1], z_temp[1]);
                       				glTexCoord2f (u_temp[2], v_temp[2]);            glVertex3f (x_temp[2], y_temp[2], z_temp[2]);
                       				glTexCoord2f (u_temp[3], v_temp[3]);            glVertex3f (x_temp[3], y_temp[3], z_temp[3]);
                 				glEnd();
                 			}
                 			break;

                 	case 3:	x_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].x;
                        	y_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].y;
                       		z_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].z;
                       		u_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].u;
                    	   	v_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].v;

                   	    	x_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].x;
                   	    	y_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].y;
                    	   	z_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].z;
                    	   	u_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].u;
                     	  	v_temp[1] = Genesis_World.sector.poly_T[loop].vertex[1].v;

                     	  	x_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].x;
                     	  	y_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].y;
                     	  	z_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].z;
                     	  	u_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].u;
                     	  	v_temp[2] = Genesis_World.sector.poly_T[loop].vertex[2].v;

                   	    	x_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].x;
                  	     	y_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].y;
                  	     	z_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].z;
                  	     	u_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].u;
                  	     	v_temp[3] = Genesis_World.sector.poly_T[loop].vertex[3].v;

                       		if ( 	disable_frustum ||
                       			Genesis_Frustum.PointInFrustum(x_temp[0], y_temp[0], z_temp[0]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[1], y_temp[1], z_temp[1]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[2], y_temp[2], z_temp[2]) ||
                       			Genesis_Frustum.PointInFrustum(x_temp[3], y_temp[3], z_temp[3]) )
                       		{
	                       		if (Genesis_World.sector.poly_T[loop].text_roll_x == 1) {
		                       		Genesis_World.sector.poly_T[loop].x_roll += Genesis_World.sector.poly_T[loop].text_roll_vel;
		                       		if (Genesis_World.sector.poly_T[loop].x_roll >= (u_temp[0] - u_temp[1]))
		                       			Genesis_World.sector.poly_T[loop].x_roll = 0.0f;
	                       			x_roll_temp = Genesis_World.sector.poly_T[loop].x_roll;
                       			}
	                       		if (Genesis_World.sector.poly_T[loop].text_roll_y == 1) {
		                       		Genesis_World.sector.poly_T[loop].y_roll += Genesis_World.sector.poly_T[loop].text_roll_vel;
		                       		if (Genesis_World.sector.poly_T[loop].y_roll >= (v_temp[0] - v_temp[2]))
		                       			Genesis_World.sector.poly_T[loop].y_roll = 0.0f;
	                       			y_roll_temp = Genesis_World.sector.poly_T[loop].y_roll;
                       			}
	                       		glNormal3f (0.0, 0.0, 1.0);
                 	      		glBegin (GL_TRIANGLE_STRIP);
                       				glTexCoord2f (u_temp[0] + x_roll_temp, v_temp[0] + y_roll_temp);
                       				glVertex3f (x_temp[0], y_temp[0], z_temp[0]);
                       				glTexCoord2f (u_temp[1] + x_roll_temp, v_temp[1] + y_roll_temp);
                       				glVertex3f (x_temp[1], y_temp[1], z_temp[1]);
                       				glTexCoord2f (u_temp[2] + x_roll_temp, v_temp[2] + y_roll_temp);
                       				glVertex3f (x_temp[2], y_temp[2], z_temp[2]);
                       				glTexCoord2f (u_temp[3] + x_roll_temp, v_temp[3] + y_roll_temp);
                       				glVertex3f (x_temp[3], y_temp[3], z_temp[3]);
                 				glEnd();
                 			}
                 			break;

			case 4:	if ( Genesis_Frustum.SphereInFrustum(	Genesis_World.sector.poly_T[loop].vertex[0].x,
															Genesis_World.sector.poly_T[loop].vertex[0].y,
															Genesis_World.sector.poly_T[loop].vertex[0].z,
															Genesis_World.sector.poly_T[loop].raggio_x_frustum))
					{
						distanza = Calcola_Distanza (	current_position,
													Genesis_World.sector.poly_T[loop].vertex[0].x,
													Genesis_World.sector.poly_T[loop].vertex[0].y,
													Genesis_World.sector.poly_T[loop].vertex[0].z );
						if (distanza < 15)
							numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni;
						else
							if (distanza < 30)
								numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni >> 1;
							else
								if (distanza < 50)
									numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni >> 2;
								else
									numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni >> 3;
						if (numero_divisioni < 3)
							numero_divisioni = 3;
						glPushMatrix();
                 				glTranslatef (	Genesis_World.sector.poly_T[loop].vertex[0].x,
										Genesis_World.sector.poly_T[loop].vertex[0].y,
										Genesis_World.sector.poly_T[loop].vertex[0].z);
                 				if (	Genesis_World.sector.poly_T[loop].sphere_x_rot != 0.0 ||
	                 				Genesis_World.sector.poly_T[loop].sphere_y_rot != 0.0 ||
                 					Genesis_World.sector.poly_T[loop].sphere_z_rot != 0.0) {
							glRotatef (	angolo_rot,
                 								Genesis_World.sector.poly_T[loop].sphere_x_rot,
                 								Genesis_World.sector.poly_T[loop].sphere_y_rot,
                 								Genesis_World.sector.poly_T[loop].sphere_z_rot);
						}
						// disegna la sfera
						gluSphere(	quadratic , Genesis_World.sector.poly_T[loop].raggio,
									numero_divisioni, numero_divisioni);
                 				// ripristina posizioni
                 				glPopMatrix();
                 			}
                 			break;

			case 5:	altezza_cyl_temp = Genesis_World.sector.poly_T[loop].altezza;
					x_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].x;
                  		y_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].y;
                  		z_temp[0] = Genesis_World.sector.poly_T[loop].vertex[0].z;
                  		if ( Genesis_Frustum.SphereInFrustum(	Genesis_World.sector.poly_T[loop].vertex[0].x,
															Genesis_World.sector.poly_T[loop].vertex[0].y,
															Genesis_World.sector.poly_T[loop].vertex[0].z,
															Genesis_World.sector.poly_T[loop].raggio_x_frustum))
					{
						distanza = Calcola_Distanza (	current_position,
													Genesis_World.sector.poly_T[loop].vertex[0].x,
													Genesis_World.sector.poly_T[loop].vertex[0].y,
													Genesis_World.sector.poly_T[loop].vertex[0].z );
						if (distanza < 15)
							numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni;
						else
							if (distanza < 30)
								numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni >> 1;
							else
								if (distanza < 50)
									numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni >> 2;
								else
									numero_divisioni = Genesis_World.sector.poly_T[loop].numero_divisioni >> 3;
						if (numero_divisioni < 3)
							numero_divisioni = 3;
                  			glPushMatrix ();
                  			glTranslatef (x_temp[0], y_temp[0] + (altezza_cyl_temp / 2), z_temp[0]);
                  			glRotatef (90.0, 1.0, 0.0 ,0.0);
						// disegna il cilindro
                  			gluCylinder(	quadratic , Genesis_World.sector.poly_T[loop].raggio,
                  						Genesis_World.sector.poly_T[loop].raggio_cima, altezza_cyl_temp,
                  						numero_divisioni, numero_divisioni);
						glPopMatrix ();
						break;
					}
		}
	}

	// disabilita blending
	glDisable (GL_BLEND);
	glColor4f (1.0f, 1.0f, 1.0f, 1.0f);

	// controlla e disegna la neve
	if (draw_snow)
		Render_Snow ();

	// controlla e disegna la pioggia
	if (draw_rain) {
		glColor3f (0.7f, 0.7f, 0.9f);
	      Render_Rain ();
	      glColor3f (1.0f, 1.0f, 1.0f);
      }


	// disegna gli alberi generati casualmente
	if (draw_tree) {
		for (int loop = 0; loop < Genesis_World.sector.num_alberi; loop++) {
			if ( Genesis_Frustum.SphereInFrustum(x_tree[loop], 0.55, z_tree[loop], 0.7))
			{
				distanza = Calcola_Distanza (current_position, x_tree[loop], 0.65, z_tree[loop]);
				glPushMatrix ();
				glTranslatef(x_tree[loop], 0.65, z_tree[loop]);
				if (tipo_albero[loop] == 0)							// chioma a sfera
					if (distanza < 17)
						glCallList(tree_list);
					else
						glCallList(tree_list + 2);
				else
					if (distanza < 17)
						glCallList(tree_list + 1);
					else
						glCallList(tree_list + 3);
				glPopMatrix ();
			}
		}
	}

	// disegna i PARTICLE ENGINE
	if (enable_particle_engine)
		Render_Particle ();

	// gestione angoli di rotazione e posizione su y
      if (angolo_rot >= 360.0)
	     	angolo_rot = 0.0;
	angolo_rot += 20.0f * frame_interval;
	if (sky_angle >= 360.0)
      	sky_angle = 0.0;
      sky_angle += 1.5f * frame_interval;

      // renderizza il testo
      Render_Text ();

      glFlush ();
      return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

int InitGL (GLvoid)
{
	// inizializzazione proprietà oggetti 3D
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture

	// setup world
      Setup_World ();

	// setup finestra
      glClearColor (0.0f, 0.0f, 0.0f, 0.5f);                                                          // sfondo nero
      glClearDepth (1.0f);                                                                                  // setup depth buffer
      glDepthFunc (GL_LEQUAL);                                                                        // tipo di depth test
      glEnable (GL_DEPTH_TEST);                                                                 // abilita il depth test
      glEnable (GL_TEXTURE_2D);                                                                 // enable texture mapping
      glBlendFunc (GL_SRC_ALPHA, GL_ONE);                                                 // funzione per il blending
      glShadeModel (GL_SMOOTH);                                                                 // enable smooth shading
      glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);               // calcolo prospettiva

      // setup della luce
      glLightfv(GL_LIGHT0, GL_AMBIENT, Light_0_Ambient);                               // setup luce ambiente
      glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_0_Diffuse);                                     // setup luce diffusa
      glLightfv(GL_LIGHT0, GL_POSITION, Light_0_Position);                             // posizione della luce
      glEnable(GL_LIGHT0);                                                                            // enable LIGHT0

      // setup nebbia
      glFogi (GL_FOG_MODE, GL_LINEAR);                                                    // tipo di nebbia
      glFogfv (GL_FOG_COLOR, fog_color_light);								// colore nebbia
      glFogf (GL_FOG_DENSITY, fog_density);                                                     // densità nebbia
      glFogf (GL_FOG_START, fog_start_dist);                                              // distanza di inizio della nebbia
      glFogf (GL_FOG_END, fog_end_dist);                                                  // distanza finale della nebbia
      glHint (GL_FOG_HINT, GL_NICEST);                                                    // valore hint della nebbia

      // setup camera
      Camera.PositionCamera(0, 0.2f, 0,   0, 0.2f, -1,   0, 1, 0);
      return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_World ()
{
	// caricamento texture
	Genesis_Texture.Load_BMP_Textures ("Texture/floor.bmp");		Genesis_Texture.Load_BMP_Textures ("Texture/wall.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/up.bmp");			Genesis_Texture.Load_BMP_Textures ("Texture/glass.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/floor_2.bmp");		Genesis_Texture.Load_BMP_Textures ("Texture/glass_2.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/sphere.bmp");		Genesis_Texture.Load_BMP_Textures ("Texture/legno_2.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/sky_1.bmp");		Genesis_Texture.Load_BMP_Textures ("Texture/ground_1.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/wall_of_end.bmp");	Genesis_Texture.Load_BMP_Textures ("Texture/cylinder_2.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/legno_1.bmp");		Genesis_Texture.Load_BMP_Textures ("Texture/ametista.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/text.bmp");		Genesis_Texture.Load_BMP_Textures ("Texture/foglie.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/snow.bmp");		Genesis_Texture.Load_BMP_Textures ("Texture/sky_snow.bmp");
    	Genesis_Texture.Load_BMP_Textures ("Texture/ground_snow.bmp");Genesis_Texture.Load_BMP_Textures ("Texture/particle.bmp");

    	// setup mondo 3D
	Genesis_World.Setup_World ();		// setup del mondo 3D da un file TXT
      OpenWork.Build_Font ();				// prepara le liste di visualizzazione dei font
      Build_Sky_List ();					// setup del cielo
      Build_Ground_List ();				// setup del terreno
      Genera_Alberi ();					// genera casualmente le coordinate per gli alberi e costriusce la liste di visualizzazione
      Setup_Snow ();						// genera casualmente la posizione della neve
      Setup_Rain ();						// genera casualmente la posizione della pioggia

      Particle.Setup_Particle_Engine ();	// imposta i valori iniziali del particle engine
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

float Calcola_Distanza (CVector3 position, float x, float y, float z)
{
	distanza = 0.0f;
	if (x > position.x)
		distanza += x - position.x;
	else
		distanza += position.x - x;
	if (y > position.y)
		distanza += y - position.y;
	else
		distanza += position.y - y;
	if (z > position.z)
		distanza += z - position.z;
	else
		distanza += position.z - z;

	return distanza;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Snow ()
{
	for (int loop = 0; loop < Genesis_World.sector.num_neve; loop++) {
	      if ( Genesis_Frustum.SphereInFrustum(snow[loop].x, snow[loop].y, snow[loop].z, 0.03) )
		{
			glPushMatrix();
			glTranslatef (snow[loop].x, snow[loop].y, snow[loop].z);
			glCallList (snow_list);
			glPopMatrix();
		}
		if (!pause_snow_rain) {
			snow[loop].x += (float)sin(angolo_rot * PIOVER180) * 0.1f;
			snow[loop].y -= snow[loop].velocita;
			snow[loop].z += (float)cos(angolo_rot * PIOVER180) * 0.1f;
			if (snow[loop].y < 0.0) {
				snow[loop].x = float(rand()%600) / 10.0f - 30.0f;
				snow[loop].y = float(rand()%4) + 17.0f;
				snow[loop].z = float(rand()%600) / 10.0f - 30.0f;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Rain ()
{
	for (int loop = 0; loop < Genesis_World.sector.num_pioggia; loop++) {
	      if ( Genesis_Frustum.PointInFrustum(rain[loop].x, rain[loop].y, rain[loop].z) )
		{
			glPushMatrix();
			glTranslatef (rain[loop].x, rain[loop].y, rain[loop].z);
			glCallList (rain_list);
			glPopMatrix();
		}
		if (!pause_snow_rain) {
			rain[loop].y -= rain[loop].velocita;
			if (rain[loop].y < -1.0) {
				rain[loop].x = float(rand()%600) / 10.0f - 30.0f;
				rain[loop].y = float(rand()%4) + 17.0f;
				rain[loop].z = float(rand()%600) / 10.0f - 30.0f;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Particle ()
{
	Genesis_Texture.Usa_Texture (19);
	glEnable (GL_BLEND);

	for (int loop_engine = 0; loop_engine < Particle.num_particle_engine; loop_engine++)
	{
		switch (Particle.engine[loop_engine].tipo)
		{
			case 1:	for (int loop = 0; loop < Particle.engine[loop_engine].num_particle; loop++) {
						if (Particle.engine[loop_engine].particle[loop].active)
						{
							x_temp_part = Particle.engine[loop_engine].x_coord + Particle.engine[loop_engine].particle[loop].x;
							y_temp_part = Particle.engine[loop_engine].y_coord + Particle.engine[loop_engine].particle[loop].y;
							z_temp_part = Particle.engine[loop_engine].z_coord + Particle.engine[loop_engine].particle[loop].z;
							if (Genesis_Frustum.SphereInFrustum (x_temp_part, y_temp_part, z_temp_part, Particle.engine[loop_engine].lato))
							{
								// imposta il colore della particella ed assegna il valore alfa in base a life per farla sfumare mentre muore
								glColor4f (	Particle.engine[loop_engine].particle[loop].r, Particle.engine[loop_engine].particle[loop].g,
											Particle.engine[loop_engine].particle[loop].b, Particle.engine[loop_engine].particle[loop].life);
								glPushMatrix ();
								glTranslatef (x_temp_part, y_temp_part, z_temp_part);
								gluSphere( quadratic, Particle.engine[loop_engine].lato,4 , 4);
								glPopMatrix ();
							}
							// movimento delle particelle
							Particle.engine[loop_engine].particle[loop].x += Particle.engine[loop_engine].particle[loop].xi / (Particle.engine[loop_engine].slowdown);
							Particle.engine[loop_engine].particle[loop].y += Particle.engine[loop_engine].particle[loop].yi / (Particle.engine[loop_engine].slowdown);
							Particle.engine[loop_engine].particle[loop].z +=Particle. engine[loop_engine].particle[loop].zi / (Particle.engine[loop_engine].slowdown);
							// movimento dovuto alla forza di gravità
							Particle.engine[loop_engine].particle[loop].xi += Particle.engine[loop_engine].xgrav;
							Particle.engine[loop_engine].particle[loop].yi += Particle.engine[loop_engine].ygrav;
							Particle.engine[loop_engine].particle[loop].zi += Particle.engine[loop_engine].zgrav;
							// decrementa la vita della particella
							Particle.engine[loop_engine].particle[loop].life -= Particle.engine[loop_engine].particle[loop].fade;
							// se life minore di 1 la particella viene ricreata
							if (	Particle.engine[loop_engine].particle[loop].life < 0.0f) {
								Particle.engine[loop_engine].particle[loop].life = 1.0f;
								// imposta il valore con cui la particella muore
								Particle.engine[loop_engine].particle[loop].fade = (rand()%Particle.engine[loop_engine].fade_MAX) / 1000.0f + Particle.engine[loop_engine].fade_MIN;
								// posizioni iniziali
								Particle.engine[loop_engine].particle[loop].x = 0.0f;
								Particle.engine[loop_engine].particle[loop].y = 0.0f;
								Particle.engine[loop_engine].particle[loop].z = 0.0f;
								// nuova velocità
								Particle.engine[loop_engine].particle[loop].xi = Particle.engine[loop_engine].xspeed + float ( (rand()%200 - 100) / 50.0f);
								Particle.engine[loop_engine].particle[loop].yi = Particle.engine[loop_engine].yspeed + float ( (rand()%200 - 100) / 50.0f);
								Particle.engine[loop_engine].particle[loop].zi = Particle.engine[loop_engine].zspeed + float ( (rand()%200 - 100) / 50.0f);
								// impostazione del colore che cambia ogni volta
								Particle.engine[loop_engine].particle[loop].r = colors[Particle.color][0];
								Particle.engine[loop_engine].particle[loop].g = colors[Particle.color][1];
								Particle.engine[loop_engine].particle[loop].b = colors[Particle.color][2];
							}
						}
					}
				break;
			case 2:	for (int loop = 0; loop < Particle.engine[loop_engine].num_particle; loop++) {
						if (Particle.engine[loop_engine].particle[loop].active)
						{
							x_temp_part = Particle.engine[loop_engine].x_coord + Particle.engine[loop_engine].particle[loop].x;
							y_temp_part = Particle.engine[loop_engine].y_coord + Particle.engine[loop_engine].particle[loop].y;
							z_temp_part = Particle.engine[loop_engine].z_coord + Particle.engine[loop_engine].particle[loop].z;
							if (Genesis_Frustum.SphereInFrustum (x_temp_part, y_temp_part, z_temp_part, Particle.engine[loop_engine].lato))
							{
								glColor3ub (	fire_colors[ char ( (Particle.engine[loop_engine].particle[loop].life + 0.5) * 4) ] [0],
											fire_colors[ char ( (Particle.engine[loop_engine].particle[loop].life + 0.5) * 4) ] [1],
											fire_colors[ char ( (Particle.engine[loop_engine].particle[loop].life + 0.5) * 4) ] [2]);
								glPushMatrix ();
								glTranslatef (x_temp_part, y_temp_part, z_temp_part);
								gluSphere( quadratic, Particle.engine[loop_engine].lato,4 , 4);
								glPopMatrix ();
							}
							// movimento delle particelle
							Particle.engine[loop_engine].particle[loop].x += Particle.engine[loop_engine].particle[loop].xi / (Particle.engine[loop_engine].slowdown);
							Particle.engine[loop_engine].particle[loop].y += Particle.engine[loop_engine].particle[loop].yi / (Particle.engine[loop_engine].slowdown);
							Particle.engine[loop_engine].particle[loop].z += Particle. engine[loop_engine].particle[loop].zi / (Particle.engine[loop_engine].slowdown);
							// movimento dovuto alla forza di gravità
							Particle.engine[loop_engine].particle[loop].xi += Particle.engine[loop_engine].xgrav;
							Particle.engine[loop_engine].particle[loop].yi += Particle.engine[loop_engine].ygrav;
							Particle.engine[loop_engine].particle[loop].zi += Particle.engine[loop_engine].zgrav;
							// decrementa la vita della particella
							Particle.engine[loop_engine].particle[loop].life -= Particle.engine[loop_engine].particle[loop].fade;
							// se life minore di 1 la particella viene ricreata
							if (	Particle.engine[loop_engine].particle[loop].life < 0.0f) {
								Particle.engine[loop_engine].particle[loop].life = 1.0f;
								// imposta il valore con cui la particella muore
								Particle.engine[loop_engine].particle[loop].fade = (rand()%Particle.engine[loop_engine].fade_MAX) / 1000.0f + Particle.engine[loop_engine].fade_MIN;
								// posizioni iniziali
								Particle.engine[loop_engine].particle[loop].x = 0.0f;
								Particle.engine[loop_engine].particle[loop].y = 0.0f;
								Particle.engine[loop_engine].particle[loop].z = 0.0f;
								// nuova velocità
								Particle.engine[loop_engine].particle[loop].xi = Particle.engine[loop_engine].xspeed + float ( (rand()%200 - 100) / 50.0f);
								Particle.engine[loop_engine].particle[loop].yi = Particle.engine[loop_engine].yspeed + float ( (rand()%200 - 100) / 50.0f);
								Particle.engine[loop_engine].particle[loop].zi = Particle.engine[loop_engine].zspeed + float ( (rand()%200 - 100) / 50.0f);
							}
						}
					}
				break;
		}
	}
	glDisable (GL_BLEND);

	if (Particle.delay > 25) {
		Particle.delay = 0;
		Particle.color++;
		if (Particle.color > 11)
			Particle.color = 0;
	}
	Particle.delay++;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Snow (void)
{
	snow = new NEVE[Genesis_World.sector.num_neve];
	for (int loop = 0; loop < Genesis_World.sector.num_neve; loop++) {
		snow[loop].x = float (rand() % 600) / 10.0f - 30.0f;
		snow[loop].y = float (rand() % 5) + 15.0f;
		snow[loop].z = float (rand() % 600) / 10.0f - 30.0f;
		snow[loop].velocita = float(rand()%50) / 500.0f + 0.1f;
	}
      Build_Snow_List ();												// setup della neve
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Rain (void)
{
	rain = new PIOGGIA[Genesis_World.sector.num_pioggia];
	for (int loop = 0; loop < Genesis_World.sector.num_pioggia; loop++) {
		rain[loop].x = float (rand() % 600) / 10.0f - 30.0f;
		rain[loop].y = float (rand() % 20) + 1.0f;
		rain[loop].z = float (rand() % 600) / 10.0f - 30.0f;
		rain[loop].velocita = float(rand()%50) / 500.0f + 1.5f;
	}
      Build_Rain_List ();												// setup della pioggia
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid Build_Snow_List (GLvoid)
{
	snow_list = glGenLists(1);
      glNewList(snow_list, GL_COMPILE);
		// scelta texture
		Genesis_Texture.Usa_Texture (16);
		quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
		gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
		gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
		gluSphere(quadratic , 0.03, 8, 8);
	glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid Build_Rain_List (GLvoid)
{
	rain_list = glGenLists(1);
      glNewList(rain_list, GL_COMPILE);
      	glDisable (GL_TEXTURE_2D);						// disabilita temporaneamente le texture
      	glBegin (GL_LINES);
      		glVertex3f (0.0, 0.15, 0.0);
      		glVertex3f (0.0, 0.0, 0.0);
      	glEnd ();
		glEnable (GL_TEXTURE_2D);							// enable texture mapping
	glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid Build_Ground_List (GLvoid)
{
	ground_list = glGenLists(2);
      glNewList(ground_list, GL_COMPILE);
		// carica la texture per disegnare il terreno
		glPushMatrix ();
      	glTranslatef (-512.0, -((float)Terreno_Piatto.Height(512, 512) + 0.02), -512.0);
      	Terreno_Piatto.RenderMap();								// chiama la funzione che renderizza il terreno
		glPopMatrix ();
	glEndList();
	glNewList(ground_list + 1, GL_COMPILE);
		// carica la texture per disegnare il terreno
		glPushMatrix ();
      	glTranslatef (-512.0, -((float)Terreno_Mosso.Height(512, 512) + 0.02), -512.0);
      	Terreno_Mosso.RenderMap();								// chiama la funzione che renderizza il terreno
      	glPopMatrix ();
	glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid Build_Sky_List (GLvoid)
{
	sky_list = glGenLists(1);
      glNewList(sky_list, GL_COMPILE);
		// scelta texture
		quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
		gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
		gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture
		gluSphere(quadratic , 70.0, 32, 32);
	glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid Build_Tree_List (GLvoid)
{
	tree_list = glGenLists(4);
	glNewList (tree_list, GL_COMPILE);
		// scelta texture
		Genesis_Texture.Usa_Texture (7);
		glPushMatrix ();
		glRotatef (90.0, 1.0, 0.0 ,0.0);
		gluCylinder(quadratic , 0.06, 0.06, 0.65, 16, 16);
		glRotatef (-90.0, 1.0, 0.0 ,0.0);
		// carica la texture per la chioma
		Genesis_Texture.Usa_Texture (15);
		gluSphere(quadratic , 0.2, 32, 32);
		glPopMatrix ();
	glEndList ();
	glNewList (tree_list + 1, GL_COMPILE);
		// scelta texture
		Genesis_Texture.Usa_Texture (7);
		glPushMatrix ();
		glRotatef (90.0, 1.0, 0.0 ,0.0);
		gluCylinder(quadratic , 0.06, 0.06, 0.65, 16, 16);
		glRotatef (-90.0, 1.0, 0.0 ,0.0);
		// carica la texture per la chioma
		Genesis_Texture.Usa_Texture (15);
		glTranslatef(0.0, 0.35, 0.0);
		glRotatef (90.0, 1.0, 0.0 ,0.0);
		gluCylinder(quadratic , 0.0, 0.25, 0.60, 32, 32);
		glPopMatrix ();
      glEndList();
      glNewList (tree_list + 2, GL_COMPILE);
		// scelta texture
		Genesis_Texture.Usa_Texture (7);
		glPushMatrix ();
		glRotatef (90.0, 1.0, 0.0 ,0.0);
		gluCylinder(quadratic , 0.06, 0.06, 0.65, 8, 8);
		glRotatef (-90.0, 1.0, 0.0 ,0.0);
		// carica la texture per la chioma
		Genesis_Texture.Usa_Texture (15);
		gluSphere(quadratic , 0.2, 16, 16);
		glPopMatrix ();
	glEndList ();
	glNewList (tree_list + 3, GL_COMPILE);
		// scelta texture
		Genesis_Texture.Usa_Texture (7);
		glPushMatrix ();
		glRotatef (90.0, 1.0, 0.0 ,0.0);
		gluCylinder(quadratic , 0.06, 0.06, 0.65, 8, 8);
		glRotatef (-90.0, 1.0, 0.0 ,0.0);
		// carica la texture per la chioma
		Genesis_Texture.Usa_Texture (15);
		glTranslatef(0.0, 0.35, 0.0);
		glRotatef (90.0, 1.0, 0.0 ,0.0);
		gluCylinder(quadratic , 0.0, 0.25, 0.60, 16, 16);
		glPopMatrix ();
      glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid Genera_Alberi (GLvoid)
{
	numero_alberi = Genesis_World.sector.num_alberi;
	x_tree = new float[numero_alberi];
	z_tree = new float[numero_alberi];
	tipo_albero = new int[numero_alberi];
	for (int loop = 0; loop < numero_alberi / 3; loop++) {
		tipo_albero[loop] = rand() % 2;
		x_tree[loop] = float(rand()%560) / 40.0f + 15.0f;
		z_tree[loop] = float(rand()%580) / 10.0f - 29.0f;
	}
	for (int loop = numero_alberi / 3; loop < numero_alberi - (numero_alberi / 3); loop++) {
		tipo_albero[loop] = rand() % 2;
		x_tree[loop] = -(float(rand()%560) / 40.0f + 15.0f);
		z_tree[loop] = float(rand()%580) / 10.0f - 29.0f;
	}
	for (int loop = numero_alberi - (numero_alberi / 3); loop < numero_alberi - (numero_alberi / 6); loop++) {
		tipo_albero[loop] = rand() % 2;
		x_tree[loop] = float(rand()%280) / 10.0f - 14.5f;
		z_tree[loop] = -(float(rand()%560) / 40.0f + 15.0f);
	}
	for (int loop = numero_alberi - (numero_alberi / 6); loop < numero_alberi; loop++) {
		tipo_albero[loop] = rand() % 2;
		x_tree[loop] = float(rand()%280) / 10.0f - 14.5f;
		z_tree[loop] = float(rand()%560) / 40.0f + 15.0f;
	}
	Build_Tree_List ();								// costruisce le liste di visualizzazione
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

// FUNZIONE DI GESTIONE DEI MESSAGGI DI SISTEMA
LRESULT CALLBACK WndProc(     HWND  hWnd,                   // handle per questa finestra
                                                UINT  uMsg,                   // messaggio per questa finestra
                                                WPARAM      wParam,                 // informazioni addizionali
                                                LPARAM      lParam)                       // informazioni addizionali
{
      switch (uMsg) {                                                    // Controlla i messaggi di Windows
            case WM_ACTIVATE:                               // Controlla se il sistema ha lanciato un messaggio di attivazione finestra
            {
                  if (!HIWORD (wParam))                           // Controlla lo stato di riduzione a icona
                  {
                        OpenWork.active = TRUE;             // Il programma e' attivo
                  }
                  else
                  {
                        OpenWork.active = FALSE;                  // Il programma deve chiudere
                  }
                  return 0;                                             // Ritorna al loop principale
            }
            case WM_SYSCOMMAND:                             // Intercetta i comandi del sistema
            {
                  switch (wParam)                                       // Controlla la chiamate del sistema
                  {
                        case SC_SCREENSAVE:                       // Il sistema sta cercando di avviare lo screen saver ?
                        case SC_MONITORPOWER:               // Il sistema cerca di entrare in modalità powersave ?
                        return 0;                                       // Impedisci che questo accada
                  }
                  break;                                                      // Esci
            }
            case WM_CLOSE:                                        // L'applicazione ha ricevuto un messaggio di chiusura ?
            {
                  PostQuitMessage(0);                                   // Spedisci un messaggio di chiusura
                  return 0;
            }
            case WM_LBUTTONDOWN: 				// è stato premuto il tasto sinistro del mouse?
		{
			mouse_mov = !mouse_mov;
			ShowCursor(mouse_mov);
			return 0; // Jump Back
		}


            case WM_KEYDOWN:                                // Un bottone e' stato premuto ?
            {
                  OpenWork.keys[wParam] = true;             // Se si, settane il flag a true
                  return 0;
            }
            case WM_KEYUP:                                        // Un bottone e' stato rilasciato ?
            {
                  OpenWork.keys[wParam] = false;                  // Se si, settare il flag a false
                  return 0;
            }
            case WM_SIZE:                                               // Ridimensiona la finestra
            {
                  OpenWork.ReSizeGLScene (LOWORD (lParam), HIWORD(lParam));         // LoWord = Width, HiWord = Height
                  return 0;
            }
      }
      // Passa tutti i messaggi non gestiti a DefWindowProc
      return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

// FUNZIONE PRINCIPALE DEL PROGRAMMA
int WINAPI WinMain(     HINSTANCE   hInstance,                    // Istanza
                                    HINSTANCE   hPrevInstance,                // Precedente istanza
                                    LPSTR             lpCmdLine,                    // Parametri della linea di comando
                                    int                  nCmdShow)                     // Stato di visualizzazione della finestra
{

      // crezione finestra di default 800 x 600 -     32 bits
      OpenWork.Enable_OpenGL ((WNDPROC) WndProc, screen_width, screen_height, bits);

      // inizializzazione proprietà finestra
      if (!InitGL ()) {                                                 // richiama InitGL x inizializzare la finestra
            OpenWork.KillGLWindow();                                                // distrugge la finestra
            MessageBox (NULL," Inizializzazione fallita. ", "ERROR", MB_OK | MB_ICONEXCLAMATION);
            return 0;
      }

      // Loop principale dell'applicazione
      while (!OpenWork.done)
      {
            if (PeekMessage(&OpenWork.msg, NULL, 0, 0, PM_REMOVE))            // è stato mandato un messaggio all'applicazione ?
            {
                  if (OpenWork.msg.message == WM_QUIT)                                    // ricevuto msg di chiusura?
                        OpenWork.done = true;
                  else                                                                                      // se non è così, analizza il messaggio + nel dettaglio
                  {
                        TranslateMessage (&OpenWork.msg);                                 // Traduzione messaggio
                        DispatchMessage  (&OpenWork.msg);                                 // Spedizione messaggio
                  }
            }
            else                                                                                            // se non ci sono messaggi
            {
                  if (OpenWork.active)                                                    // finestra e' attiva?
                  {
                        if (OpenWork.keys[VK_ESCAPE])                               // tasto ESC premuto?
                              OpenWork.done = true;
                        else                                                                          // aggiorna lo schermo
                        {
	                        // Disegna la scena.
                              RenderScene();
                              SwapBuffers (OpenWork.hDC);					// scambia le superfici di disegno (double buffering)
                              Camera.Update(screen_width, screen_height);		// aggiorna la camera
                              Genesis_Controller ();							// gestione comandi tastiera
                              Calc_Frame_Rate ();
                        }
                  }

                  // F1 = passaggio da window mode a fullscreen
                  if (OpenWork.keys[VK_F1]) {
                        OpenWork.keys[VK_F1] = false;
                        OpenWork.KillGLWindow();
                        OpenWork.fullscreen  = !OpenWork.fullscreen;

                        if (!OpenWork.CreateGLWindow (" - Genesis (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!InitGL ()) {                                                       // richiama InitGL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }

                  // F2 = modalità 640 x 480    -     32 bits
                  if (OpenWork.keys[VK_F2]) {
                        screen_width = 640;
                        screen_height = 480;
	                  OpenWork.keys[VK_F2] = false;
				OpenWork.KillGLWindow();

                        if (!OpenWork.CreateGLWindow (" - Genesis (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!InitGL ()) {                                                       // richiama InitGL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }

                  // F3 = modalità 800 x 600    -     32 bits
                  if (OpenWork.keys[VK_F3]) {
	                  screen_width = 800;
                        screen_height = 600;
                        OpenWork.keys[VK_F3] = false;
                        OpenWork.KillGLWindow();

                        if (!OpenWork.CreateGLWindow (" - Genesis (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!InitGL ()) {                                                       // richiama InitGL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }

                  // F4 = modalità 1024 x 768   -     32 bits
                  if (OpenWork.keys[VK_F4]) {
	                  screen_width = 1024;
                        screen_height = 768;
                        OpenWork.keys[VK_F4] = false;
                        OpenWork.KillGLWindow();

                        if (!OpenWork.CreateGLWindow (" - Genesis (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!InitGL ()) {                                                       // richiama InitGL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }
                  // F = passa da 16 a 32 bits
                  if (OpenWork.keys[VK_F5]) {
	                  if (bits == 16)
	                  	bits = 32;
	                  else
	                  	bits = 16;
                        OpenWork.keys[VK_F5] = false;
                        OpenWork.KillGLWindow();

                        if (!OpenWork.CreateGLWindow (" - Genesis (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!InitGL ()) {                                                       // richiama InitGL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }
            }
      }

	glDeleteLists (sky_list, 1);
      glDeleteLists (snow_list, 1);
      glDeleteLists (rain_list, 1);
      glDeleteLists (ground_list, 2);
      glDeleteLists (tree_list, 4);
      OpenWork.Kill_Font ();
      // chiude la finestra
      OpenWork.KillGLWindow ();                                                            // SHUTDOWN
      return OpenWork.msg.wParam;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Calc_Frame_Rate ()
{
	static float frame_per_second = 0.0f;			// variabile per gli fps
      static float last_time = 0.0f;					// variabile per il tempo dall'ultimo frame

	static float frameTime = 0.0f;				// tempo dell'ultimo frame
	float current_time = timeGetTime() * 0.001f;	// tempo del frame corrente

	frame_interval = current_time - frameTime;	// intervallo tra un frame e l'altro
	frameTime = current_time;

 	++frame_per_second;

	if (current_time - last_time > 1.0f) {
		last_time = current_time;
		sprintf (str_frame_rate, "FPS: %d", int (frame_per_second));

		frame_per_second = 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Text ()
{
	// disegna il testo con blending attivo
	glEnable (GL_BLEND);
      Genesis_Texture.Usa_Texture (14);
      glColor4f (colors[Particle.color][0], colors[Particle.color][1], colors[Particle.color][2], 1.0f);
      OpenWork.glPrint (0, screen_height - 34, str_frame_rate, 0, screen_width, screen_height);
      glColor4f (1.0f, 0.0f, 0.0f, 1.0f);
      OpenWork.glPrint (0, screen_height - 17, "Genesis - ver 0.32", 0, screen_width, screen_height);
	switch (screen_width)
	{
		case 640:	if (bits == 16)
		     				OpenWork.glPrint (screen_width - 106, screen_height - 17, "640x480x16", 0, screen_width, screen_height);
		     			else
		     				OpenWork.glPrint (screen_width - 106, screen_height - 17, "640x480x32", 0, screen_width, screen_height);
		     		break;
      	case 800:	if (bits == 16)
		     				OpenWork.glPrint (screen_width - 106, screen_height - 17, "800x600x16", 0, screen_width, screen_height);
		     			else
		     				OpenWork.glPrint (screen_width - 106, screen_height - 17, "800x600x32", 0, screen_width, screen_height);
		     		break;
		case 1024:	if (bits == 16)
		     				OpenWork.glPrint (screen_width - 120, screen_height - 17, "1024x768x16", 0, screen_width, screen_height);
		     			else
		     				OpenWork.glPrint (screen_width - 120, screen_height - 17, "1024x768x16", 0, screen_width, screen_height);
		     		break;
	}
	if (time_text > 0)
	{
		glColor4f (0.5f, 0.5f, 1.0f, 1.0f);
		if (disable_frustum)
      		OpenWork.glPrint (0, screen_height - 55, "Frustum Disabilitato", 0, screen_width, screen_height);
      	else
      		OpenWork.glPrint (0, screen_height - 55, "Frustum Abilitato", 0, screen_width, screen_height);
      	if (oscillazione)
      		OpenWork.glPrint (0, screen_height - 72, "Oscillazione Attivata", 0, screen_width, screen_height);
      	else
      		OpenWork.glPrint (0, screen_height - 72, "Oscillazione Disattivata", 0, screen_width, screen_height);
      	if (enable_particle_engine)
      		OpenWork.glPrint (0, screen_height - 89, "Particle Engine Attivati", 0, screen_width, screen_height);
      	else
      		OpenWork.glPrint (0, screen_height - 89, "Particle Engine Disattivati", 0, screen_width, screen_height);
      	if (draw_tree)
      		OpenWork.glPrint (0, screen_height - 106, "Alberi Abilitati", 0, screen_width, screen_height);
      	else
      		OpenWork.glPrint (0, screen_height - 106, "Alberi Disabilitati", 0, screen_width, screen_height);

      	time_text -= 6.0 * frame_interval;
      }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE CHE CONTROLLA LA PRESSIONE DEI TASTI

void Genesis_Controller ()
{
      // L = abilita / disabilita luce
      if (OpenWork.keys['L'] && !lp) {                                        // L premuto e rilasciato?
            lp = true;
            light = !light;                                                         // inverti il valore di light
            if (!light) {
                  glDisable (GL_LIGHTING);                                    // disable light
                  glFogfv (GL_FOG_COLOR, fog_color_light);			// cambia il colore nebbia
            }
		else {
                  glEnable (GL_LIGHTING);                               	// enable light
                  glFogfv (GL_FOG_COLOR, fog_color_night);		// cambia il colore nebbia
		}
	}
      if (!OpenWork.keys['L'])
            lp = false;

	// X = abilita / disabilita il FRUSTUM
	if (OpenWork.keys['X'] && !xp) {
            xp = true;
            disable_frustum = !disable_frustum;
            time_text = 30.0f;
      }
      if (!OpenWork.keys['X'])
            xp = false;

      // B = abilita / disabilita effetti blending
      if (OpenWork.keys['B'] && !bp) {                                        // B premuto e rilasciato?
            bp = true;
            blending = !blending;                                                   // inverti il valore di blending
      }
      if (!OpenWork.keys['B'])
            bp = false;

      // F = attiva / disattiva nebbia
      if (OpenWork.keys['F'] && !fp) {
            fp = true;
            fog = !fog;
      }
      if (!OpenWork.keys['F'])
            fp = false;

	// O = abilita / disabilita oscillazione durante il movimento
      if (OpenWork.keys['O'] && !op) {                                        // O premuto e rilasciato?
            op = true;
            oscillazione = !oscillazione;                                           // inverti il valore di oscillazione
            time_text = 30.0f;
      }
      if (!OpenWork.keys['O'])
            op = false;

	// Z = abilita / disabilita i particle engine
      if (OpenWork.keys['Z'] && !zp) {                                        // Z premuto e rilasciato?
            zp = true;
            enable_particle_engine = !enable_particle_engine;
            time_text = 30.0f;
      }
      if (!OpenWork.keys['Z'])
            zp = false;

	// K = attiva / disattiva cielo
      if (OpenWork.keys['K'] && !kp) {
            kp = true;
            sky_active = !sky_active;
      }
      if (!OpenWork.keys['K'])
            kp = false;

	// J = attiva / disattiva la caduta della neve e della pioggia
      if (OpenWork.keys['J'] && !jp) {
            jp = true;
            pause_snow_rain = !pause_snow_rain;
      }
      if (!OpenWork.keys['J'])
            jp = false;

	// G = cambia il tipo di terreno
	if (OpenWork.keys['G'] && !gp) {
            gp = true;
            draw_terrain = !draw_terrain;
      }
      if (!OpenWork.keys['G'])
            gp = false;

	// N = attiva / disattiva la neve
	if (OpenWork.keys['N'] && !np) {
            np = true;
            draw_snow = !draw_snow;
		if (draw_snow)
			draw_rain = false;
      }
      if (!OpenWork.keys['N'])
		np = false;

	// P = attiva / disattiva la pioggia
	if (OpenWork.keys['P'] && !pp) {
            pp = true;
            draw_rain = !draw_rain;
            if (draw_rain)
            	draw_snow = false;
      }
      if (!OpenWork.keys['P'])
            pp = false;

	// T = attiva / disattiva gli alberi
	if (OpenWork.keys['T'] && !tp) {
            tp = true;
            draw_tree = !draw_tree;
            time_text = 30.0f;
      }
      if (!OpenWork.keys['T'])
            tp = false;

	// UP o DOWN = spostamento asse x
      if (OpenWork.keys[VK_UP]) {
            Camera.MoveCamera (kSpeed * frame_interval);
            // oscillazione durante spostamento
            if (oscillazione) {
            	if (walk_angle >= 359.0f)
                 		walk_angle = 0.0f;
            	else
                  	walk_angle += 15;
            	walk = (float)sin(walk_angle * PIOVER180) / 20.0f;
            }
      }
      if (OpenWork.keys[VK_DOWN]) {
            Camera.MoveCamera (-kSpeed * frame_interval);
            // oscillazione durante spostamento
            if (oscillazione) {
            	if (walk_angle <= 1.0f)
                  	walk_angle = 359.0f;
            	else
                  	walk_angle -= 15;
            	walk = (float)sin(walk_angle * PIOVER180) / 20.0f;
            }
      }

      // RIGHT o LEFT = spostamento sull'asse x
      if (OpenWork.keys[VK_RIGHT]) {
            Camera.StrafeCamera (kSpeed * frame_interval / 2);
            // oscillazione durante spostamento
            if (oscillazione) {
            	if (walk_angle >= 359.0f)
                 		walk_angle = 0.0f;
            	else
                  	walk_angle += 15;
            	walk = (float)sin(walk_angle * PIOVER180) / 20.0f;
            }
      }
      if (OpenWork.keys[VK_LEFT]) {
            Camera.StrafeCamera (-kSpeed * frame_interval / 2);
            // oscillazione durante spostamento
            if (oscillazione) {
            	if (walk_angle <= 1.0f)
                  	walk_angle = 359.0f;
            	else
                  	walk_angle -= 15;
            	walk = (float)sin(walk_angle * PIOVER180) / 20.0f;
            }
      }

      // W o S = spostamento sull'asse y
      if (OpenWork.keys['W'])
            Camera.MoveUp (0.1);
	if (OpenWork.keys['S'])
		if (current_position.y > 0.2f)
            	Camera.MoveUp (-0.1);

	// A o D = rotazione sull'asse y
	if (OpenWork.keys['A'])
            Camera.RotateView (0.05, 0, 1, 0);
	if (OpenWork.keys['D'])
            Camera.RotateView (-0.05, 0, 1, 0);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
