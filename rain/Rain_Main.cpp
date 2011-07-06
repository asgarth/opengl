//-------------		file			Rain_Main.cpp		-------------------------------------------------------------------------------------

#include "Rain_Includes.h"
#include "OpenGL_Genesis.h"
#include "Texture_Manager.h"
#include "Frustum.h"
#include "Rain_Fire.h"
#include "Rain_Enemy.h"
#include "Rain_TNT.h"
#include "Rain_EXPL.h"
#include "Enemy_Fire.h"
#include "Rain_XPL_DISK.h"
#include "Rain_Pup.h"

#define HEIGHT				7.0f
#define SPEED_UP				13.0f
#define SPEED_LR				10.0f
#define FIRE_DETAIL			100
#define NUM_STAR				125
#define SPECIAL_STAR_OBJ	4						// numero di oggetti speciali sulllo sfondo
#define NUM_TER_OBJ			15						// numero oggetti sul terreno (quando verde)

int RenderScene (void);
int Setup_GL (void);
void Setup_Texture (void);
void Setup_World (void);
void Setup_Enemy (void);
void Setup_Space (void);
void Setup_Fire (void);
void Render_Fire (void);
void Render_Terrain  (void);
void Render_Space (void);
float Calcola_Distanza (float x1, float z1, float x2, float z2);
float Calc_Ship_Dist (float x2, float z2);
void Build_T_List (void);
void Build_Ship_List (void);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Calc_Frame_Rate (void);
void Render_Text (void);
void Genesis_Controller (void);

// variabili per la gestione della finestra
int screen_width = 800;
int screen_height = 600;
int bits = 16;
int game_dif = 0;							// difficoltà normale
bool dp;										// tasto D premuto?

char str_frame_rate[50] = {0};				// stringa per visualizzare gli fps
char str_ris[50] = {0};						// stringa per la risoluzione dello schermo
char str_energy[20] = {0};					// strigna per la visualizzazione dell'energia corrente
char str_punteggio[30] = {0};					// stringa per la visualizzazione del punteggio
float Frame_Interval = 0.0f;					// variabile in cui è memorizzato il tempo trascorso tra un frame e l'altro
float total_time;							// tempo totale trascorso dall'inizio del livello
bool spacep;									// tasto SPAZIO premuto?
bool start_game, game_over, pause = true, final_enemy = false;
int punteggio;

// setup luci
GLfloat light0_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light1_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
GLfloat light1_specular[] = { 0.9, 0.9, 0.9, 1.0 };
GLfloat light1_position[] = { 0.8, 0.5, 1.0, 0.0 };
GLfloat light1_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

// variabili per la gestione del dettaglio
bool kp, lp;									// tasto L, P premuto?
float show_detail = 1.0;						// variabile per stampare a schermo il cambiamento di dettaglio

// variabili per la base delle liste di visualizzazione
GLuint t_list, ship_list;

// puntatore per la gestione delle proprietà degli oggetti 3D
GLUquadricObj *quadratic;
// setup valori glMaterial
GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat high_shininess[] = { 80.0 };

// variabili per la gestione dell'oggetto principale
typedef struct {	float x, z, raggio;						// coordiante e raggio
				int shield_energie;
				float angle;								// angolo di inclinazione della nave
} SHIP;
SHIP ship;
typedef struct { float x, z, zi;							// posizione e velocità relativa della particella
				float life, fade;							// vita e velocità della morte
} FIRE_PARTICLE;
FIRE_PARTICLE ship_fire[FIRE_DETAIL];

// variabili per gestire il terreno
bool green_terrain = false;								// tipo di sfondo da visualizzare
bool tp;													// tasto T premuto?
typedef struct {	float texture_roll;						// variabile x lo scorrimento della texture
				float texture_speed;
} TERRAIN;
TERRAIN terrain;
typedef struct {	float x, z, height;						// oggetti disegnati sul terreno
				float base_dim;							// dimensioni alla base e alla cima
				int tipe;									// tipo di oggetto da disegnare
} TER_OBJ;
TER_OBJ ter_obj[NUM_TER_OBJ];
typedef struct {	float x_position, z_position;				// posizione della stella
				float deep, color, speed;					// profondità, colore e velocità della stella
} STAR;
STAR stars[125];
STAR special[SPECIAL_STAR_OBJ];

// variabili per i colpi
float fire_time = 0.0f;									// tempo tra un colpo e l'altro
float shield_time = 1.0f;									// tempo di visualizzazione dello scudo dopo la collisione
float enemy_time = 1.0f;									// tempo per la comparsa dei nemici (DA TOGLIERE POI)

// variabili per la gestione delle collisioni
bool in_frustum;
float dist_collisione;
bool collisione = false;

OpenGL_Genesis		OpenWork;							// oggetto globale per le funzioni opengl
Texture_Manager	Rain_Texture;						// oggetto per la gestione delle texture
Frustum				R_Frustum;							// oggetto per controllare cos'è nell'inquadratura
Rain_Fire			Fire (HEIGHT);						// oggetto per gestire i colpi sparati
Enemy_Fire			E_Fire (HEIGHT, 0.3);				// oggetto per la gestione dei colpi nemici
Rain_Enemy			Enemy (HEIGHT);					// oggetto per la gestione dei nemici su schermo
Rain_TNT			Explosion (HEIGHT);					// oggetto per la gestione delle esplosioni
Rain_EXPL			Expl (HEIGHT);						// oggetto per la gestione delle esplosioni dei pezzi
Rain_XPL_DISK		Expl_Disk (HEIGHT);				// oggetto per la gestione dei cerchi delle esplosioni
Rain_Pup			PowerUP (HEIGHT);					// oggetto per la gestione dei power_up

//-------------------------------------------------------------------------------------------------------------------------------------------------------

int RenderScene (GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();										// resetta la visuale

	// TRASFORMAZIONI
	glCallList(t_list);

	// disegna il terreno
	glDisable (GL_LIGHTING);
	if (green_terrain)
		Render_Terrain ();
	else
		Render_Space ();

	// d	isegna ship
	glPushMatrix();
	glTranslatef (ship.x, HEIGHT, ship.z);
	glRotatef (ship.angle, 0.0, 0.0, 1.0);
	glCallList(ship_list);
	Render_Fire ();									// disegna il fuoco della nave
	// disegna lo scudo intorno alla nave quando si viene colpiti
      if (shield_time < 1.0f && ship.shield_energie >= 0) {
		glEnable (GL_BLEND);
		Rain_Texture.Usa_Texture (7);
		if (shield_time < 0.5f)
			glColor4f (1.0f, 1.0f, 1.0f, shield_time + 0.2);
		else
			glColor4f (1.0f, 1.0f, 1.0f, 1.2 - shield_time);
		glTranslatef (0.0, 0.0, 0.3);
		gluSphere (quadratic , ship.raggio, 16, 16);
		glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
		glDisable (GL_BLEND);
	}
	glPopMatrix();

	glEnable (GL_LIGHTING);
      // disegna colpi
      Rain_Texture.Usa_Texture (10);
      Fire.Aggiorna_Fire (Frame_Interval, pause);

      // disegna oggetti
	Rain_Texture.Usa_Texture (3);
	Enemy.Render_Enemy (Frame_Interval, pause);
	Rain_Texture.Usa_Texture (8);
	E_Fire.Render_Enemy_Fire (Frame_Interval, pause);

	// calcola le collisioni tra i nemici e la nave
	if (!pause) {
		for (Enemy.loop_enemy = Enemy.start_enemy; Enemy.loop_enemy != NULL; Enemy.loop_enemy = Enemy.loop_enemy -> next) {
			if (R_Frustum.SphereInFrustum (Enemy.loop_enemy -> x, HEIGHT, Enemy.loop_enemy -> z, Enemy.loop_enemy -> ray)) {
				Enemy.loop_enemy -> drawn = true;
				// calcola la distanza del nemico dalla nave
				dist_collisione = Calc_Ship_Dist (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z);
				// aggiunge un colpo se il tempo è giusto
				Enemy.loop_enemy -> enemy_fire_time += Frame_Interval;
				if (Enemy.loop_enemy -> enemy_fire_time > 1.0 && Enemy.loop_enemy -> z < -12.0) {
					switch (Enemy.loop_enemy -> tipe) {
						case 1:	E_Fire.Add_Enemy_Fire (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, ship.x, ship.z, dist_collisione);
								Enemy.loop_enemy -> enemy_fire_time = -7.0;
								break;
						case 2:	E_Fire.Add_Enemy_Fire (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, ship.x, ship.z, dist_collisione);
								Enemy.loop_enemy -> enemy_fire_time = -5.0;
								break;
						case 3:	E_Fire.Add_EF_M (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, 0.0);
								E_Fire.Add_EF_M (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, 20.0);
								E_Fire.Add_EF_M (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, -20.0);
								Enemy.loop_enemy -> enemy_fire_time = -4.0;
								break;
						case 4:	E_Fire.Add_Enemy_Fire (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, ship.x, ship.z, dist_collisione);
								Enemy.loop_enemy -> enemy_fire_time = -1.0;
								break;
						case 5:	E_Fire.Add_EF_M (Enemy.loop_enemy -> x + 0.3f, Enemy.loop_enemy -> z - 0.3f, 10.0);
								E_Fire.Add_EF_M (Enemy.loop_enemy -> x - 0.3f, Enemy.loop_enemy -> z - 0.3f, -10.0);
								Enemy.loop_enemy -> enemy_fire_time = -2.0;
								break;
						case 31:	E_Fire.Add_EF_M (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, 0.0);
								E_Fire.Add_EF_M (Enemy.loop_enemy -> x + 0.2, Enemy.loop_enemy -> z - 0.1, 10.0);
								E_Fire.Add_EF_M (Enemy.loop_enemy -> x + 0.2, Enemy.loop_enemy -> z - 0.1, -10.0);
								E_Fire.Add_EF_M (Enemy.loop_enemy -> x + 0.4, Enemy.loop_enemy -> z - 0.3, 20.0);
								E_Fire.Add_EF_M (Enemy.loop_enemy -> x + 0.4, Enemy.loop_enemy -> z - 0.3, -20.0);
								Enemy.loop_enemy -> enemy_fire_time = -1.5;
								break;
						case 32:	E_Fire.Add_Enemy_Fire (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, ship.x, ship.z, dist_collisione);
								Enemy.loop_enemy -> enemy_fire_time = -0.5;
								break;
						case 33:	E_Fire.Add_Enemy_Fire (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z, ship.x, ship.z, dist_collisione);
								Enemy.loop_enemy -> enemy_fire_time = -0.5;
								break;
					}
				}
				// controlla le collisioni
				if (dist_collisione < ship.raggio + Enemy.loop_enemy -> ray) {
					ship.shield_energie -= 25;
					shield_time = 0.0f;
					punteggio += 5 * Enemy.loop_enemy -> tipe;
					// controlla se il nemico deve rilasciare un power_up
					if (Enemy.loop_enemy -> power_up)
						PowerUP.Add_Power_Up (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z);
					Explosion.Add_Xpl (Enemy.loop_enemy, 1);
					Expl.Add_Xpl (Enemy.loop_enemy);
					Expl_Disk.Add_Xpl (Enemy.loop_enemy);
					Enemy.loop_enemy -> destroy = true;
					if (ship.shield_energie < 0.0) {
						game_over = true;
						pause = true;
						break;
					}
				}
			}
			else
				if (Enemy.loop_enemy -> drawn)
					Enemy.loop_enemy -> destroy = true;
		}
	}
	// calcola se è stato preso un power_up
	if (!pause) {
		for (PowerUP.loop_p_up = PowerUP.xpl_start; PowerUP.loop_p_up != NULL; PowerUP.loop_p_up = PowerUP.loop_p_up -> next) {
			if (R_Frustum.SphereInFrustum (PowerUP.loop_p_up -> x, HEIGHT, PowerUP.loop_p_up -> z, PowerUP.loop_p_up -> ray)) {
				// calcola la distanza dalla nave
				dist_collisione = Calc_Ship_Dist (PowerUP.loop_p_up -> x, PowerUP.loop_p_up -> z);
				if (dist_collisione < ship.raggio + PowerUP.loop_p_up -> ray) {
					PlaySound("Data/Hourglass.wav", NULL, SND_ASYNC);
					switch (PowerUP.loop_p_up -> type) {
						case 0:	Fire.fire_type ++;
								if (Fire.fire_type > 3) {
									Fire.fire_type = 3;
									punteggio += 200;
								}
								break;
						case 1:	Fire.fire_power += 5;
								if (Fire.fire_power > 16) {
									Fire.fire_power = 16;
									punteggio += 200;
								}
								break;
						case 2:	punteggio += 1000;
								break;
						case 3:	ship.shield_energie += 50;
								if (ship.shield_energie == 150)
									punteggio += 200;
								if (ship.shield_energie > 100)
									ship.shield_energie = 100;
								break;
					}
					PowerUP.loop_p_up -> destroy = true;
				}
			}
			else
				PowerUP.loop_p_up -> destroy = true;
		}
	}
	// calcola i le collisioni con i colpi nemici
	if (!pause) {
		for (E_Fire.loop_ef = E_Fire.start_ef; E_Fire.loop_ef != NULL; E_Fire.loop_ef = E_Fire.loop_ef -> next) {
			if (R_Frustum.SphereInFrustum (E_Fire.loop_ef -> x, HEIGHT, E_Fire.loop_ef -> z, 0.2)) {
				E_Fire.loop_ef -> drawn = true;
				dist_collisione = Calc_Ship_Dist (E_Fire.loop_ef -> x, E_Fire.loop_ef -> z);
				if (dist_collisione < ship.raggio) {
					ship.shield_energie -= 10;
					shield_time = 0.0f;
					E_Fire.loop_ef -> destroy = true;
					if (ship.shield_energie < 0.0) {
						game_over = true;
						pause = true;
						break;
					}
				}
			}
			else
				if (E_Fire.loop_ef -> drawn)
					E_Fire.loop_ef -> destroy = true;
		}
	}
      // calcola le collisioni tra i colpi e i colpi nemici e tra i colpi e i nemici
      if (!pause) {
      	for (Fire.loop_fire = Fire.fire_start;  Fire.loop_fire != NULL; Fire.loop_fire = Fire.loop_fire -> next) {
	      	// distrugge il colpo se è fuori schermo
	      	if (!R_Frustum.SphereInFrustum (Fire.loop_fire -> x, HEIGHT, Fire.loop_fire -> z, 0.3))
				Fire.loop_fire -> destroy = true;
			else {
				// controlla i colpi nemici
				for (E_Fire.loop_ef = E_Fire.start_ef; E_Fire.loop_ef != NULL; E_Fire.loop_ef = E_Fire.loop_ef -> next) {
					dist_collisione = Calcola_Distanza (Fire.loop_fire -> x, Fire.loop_fire -> z, E_Fire.loop_ef -> x, E_Fire.loop_ef -> z);
					if (dist_collisione < 0.3) {
						Fire.loop_fire -> destroy = E_Fire.loop_ef -> destroy = true;
						Explosion.Add_Xpl (Fire.loop_fire, 1);
					}
				}
				// controlla i nemici
				for (Enemy.loop_enemy = Enemy.start_enemy; Enemy.loop_enemy != NULL; Enemy.loop_enemy = Enemy.loop_enemy -> next) {
					dist_collisione = Calcola_Distanza (Fire.loop_fire -> x, Fire.loop_fire -> z, Enemy.loop_enemy -> x, Enemy.loop_enemy -> z);
					if (dist_collisione < Enemy.loop_enemy -> ray) {
						Enemy.loop_enemy -> energie -= Fire.loop_fire -> power;
						if (Enemy.loop_enemy -> energie < 0) {
							// controlla se il nemico deve rilasciare un power_up
							if (Enemy.loop_enemy -> power_up)
								PowerUP.Add_Power_Up (Enemy.loop_enemy -> x, Enemy.loop_enemy -> z);
							Explosion.Add_Xpl (Enemy.loop_enemy, 1);
							Expl.Add_Xpl (Enemy.loop_enemy);
							Expl_Disk.Add_Xpl (Enemy.loop_enemy);
							Fire.loop_fire -> destroy = Enemy.loop_enemy -> destroy = true;
							punteggio += 5 * Enemy.loop_enemy -> tipe;
						}
						else {
							Explosion.Add_Xpl (Fire.loop_fire, 0);
							Fire.loop_fire -> destroy = true;
						}
					}
				}
			}
		}
	}

	glDisable (GL_LIGHTING);
	// disegna le esplosioni in atto
	Rain_Texture.Usa_Texture (11);
	Explosion.Render_Xpl (Frame_Interval, pause);
	Rain_Texture.Usa_Texture (3);
	Expl.Render_Xpl (Frame_Interval, pause);
	Expl_Disk.Render_Xpl (Frame_Interval, pause);
	//disegna i power_up
	Rain_Texture.Usa_Texture (3);
	PowerUP.Render_Power_Up (Frame_Interval, pause);

	// aggiorna i tempi
	if (!pause) {
		total_time += Frame_Interval;
		fire_time += Frame_Interval;
		enemy_time += Frame_Interval;
		shield_time += Frame_Interval;
	}
	show_detail += Frame_Interval;

	// renderizza il testo
	Render_Text ();
	// genera casualmente i nemici
	if (!pause)
		Setup_Enemy ();

	glFlush ();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

int Setup_GL (void)
{
	// inizializzazione proprietà oggetti 3D
	quadratic = gluNewQuadric (); 						// crea un puntatore all'oggetto 3D
	gluQuadricNormals (quadratic, GLU_SMOOTH); 		// imposta l'illuminazione a SMOOTH
	gluQuadricTexture (quadratic, GL_TRUE);				// crea le coordinate per la texture

	// setup finestra
	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);								// sfondo nero
	glClearDepth (1.0f);												// setup depth buffer
	glDepthFunc (GL_LEQUAL);										// tipo di depth test
	glEnable (GL_DEPTH_TEST);										// abilita il depth test
	glEnable (GL_TEXTURE_2D);										// enable texture mapping
	glEnable (GL_COLOR_MATERIAL);								// possibilità di colorare le texture attivata
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);							// funzione per il blending
	glShadeModel (GL_SMOOTH);									// enable smooth shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// calcolo prospettiva

      // setup proprietà dei materiali (DA MIGLIORARE)
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	// setup luci (DA MIGLIORARE)
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glDisable(GL_LIGHT0);

      // inizializza il generatore di numeri casuali
	srand(time(NULL));
      // carica le texture
      Setup_Texture ();
      return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Texture (void)
{
	Rain_Texture.Load_BMP_Textures ("Texture/text.bmp");		Rain_Texture.Load_BMP_Textures ("Texture/ground_1.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/floor.bmp");		Rain_Texture.Load_BMP_Textures ("Texture/glass_white.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/star.bmp");		Rain_Texture.Load_BMP_Textures ("Texture/nebula.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/nebula2.bmp");	Rain_Texture.Load_BMP_Textures ("Texture/glass_blue.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/fire_particle.bmp");Rain_Texture.Load_BMP_Textures ("Texture/sand.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/shoot.bmp");		Rain_Texture.Load_BMP_Textures ("Texture/particle.bmp");

	// setup del frustum della scena (non serve nella funzione render visto che la camera non si sposta :) )
	// TRASFORMAZIONI
	glTranslatef (-15.0, -13.0, -40.0);
	glRotatef (60.0, 1.0, 0.0, 0.0);
	glRotatef (-50.0, 0.0, 1.0, 0.0);
	R_Frustum.CalculateFrustum ();
	glLoadIdentity ();

	OpenWork.Build_Font ();				// prepara le liste di visualizzazione dei font
	Build_T_List ();						// setup lista x trasformazione camera
	Build_Ship_List ();					// setup ship
	Fire.Build_Fire_List ();				// setup lista di visualizzazione dei colpi
	E_Fire.Build_EF_List ();				// setup lista di visualizzazione dei colpi nemici
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_World (void)
{
	// setup dello sfondo nello spazio
	Setup_Space ();
	Setup_Fire ();

	// setup variabili di gioco
	pause = true;
	start_game = false;
	game_over = false;
	total_time = 0.0f;
	punteggio = 0;
	ship.shield_energie = 100;

	// setup dati del terreno
	terrain.texture_roll = 0.0f;
	terrain.texture_speed = 0.5f;
	for (int loop = 0; loop < NUM_TER_OBJ; loop++) {
		ter_obj[loop].x = float(rand()%1200) / 10.0f - 70.0f;
		ter_obj[loop].z = float(rand()%1000) / 10.0f - 100.0f;
		ter_obj[loop].height = float(rand()%30) / 10.0f + 2.5f;
		ter_obj[loop].base_dim = float(rand()%20) / 10.0f + 1.5f;
		ter_obj[loop].tipe = rand()%4;
	}

	// setup dati ship
      ship.x = 0.0f;
      ship.z = -3.0f;
      ship.angle = 0.0f;
      ship.raggio = 1.3f;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Enemy (void)
{
	static int temp_p_up;
	static bool power_up1, power_up2, power_up3;
	// funzione Add_Enemy: float x, float z, float raggio, int tipo
	power_up1 = power_up2 = power_up3 = false;
	temp_p_up = rand()%20;
	if (temp_p_up == 0)
		power_up1 = true;
	if (temp_p_up == 1)
		power_up2 = true;
	if (temp_p_up == 2 || temp_p_up == 3)
		power_up3 = true;
	if ((game_dif == 0 && enemy_time > 2.3) || (game_dif != 0 && enemy_time > 1.2)) {
		if (total_time < 90) {
			Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, float(rand()%10) * 0.1f + 1.0f, 1, power_up1);
			Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.1f, 2, power_up2);
		}
		else {
			if (total_time < 180) {
				Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, float(rand()%10) * 0.1f + 1.0f, 1, power_up1);
				Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.1f, 2, power_up2);
				Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.3f, 3, power_up3);
			}
			else {
				if (total_time < 270) {
					Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.3f, 3, power_up3);
					Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.8f, 4, power_up1);
					Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.6f, 5, power_up2);
				}
				else {
					if (total_time < 360) {
						Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.3f, 3, power_up3);
						Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.8f, 4, power_up1);
						Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.6f, 5, power_up2);
						power_up2 = false;
						Enemy.Add_Enemy (float(rand()%48) / 2.0f - 12.0f, -70.0f, 1.8f, 4, power_up2);
					}
					else {
						if (!final_enemy && total_time > 365 && Enemy.start_enemy == NULL) {
							Enemy.Add_Enemy (-3.0f, -60.0f, 3.0f, 31, false);
							Enemy.Add_Enemy (1.0f, -60.0f, 2.2f, 32, false);
							Enemy.Add_Enemy (-7.0f, -60.0f, 2.2f, 33, false);
							final_enemy = true;
						}
					}
				}
			}
		}
		enemy_time = 0.0f;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Space (void)
{
	for (int loop = 0; loop < 125; loop++) {
		stars[loop].x_position = float(rand()%800) / 10.0f - 50.0f;
		stars[loop].z_position = float(rand()%800) / 10.0f - 70.0f;
		stars[loop].deep = float(rand()%400) / 400.0f + 0.2f;
		stars[loop].speed = stars[loop].deep * float(rand()%100) / 50.0f + 0.2f;
		stars[loop].color = float(rand()%100) / 110.0f + 0.1;
	}
	for (int loop = 0; loop < SPECIAL_STAR_OBJ; loop++) {
		special[loop].x_position = float(rand()%800) / 10.0f - 50.0f;
		special[loop].z_position = float(rand()%800) / 10.0f - 70.0f;
		special[loop].deep = float(rand()%200) / 50.0f + 7.0f;
		special[loop].speed = float(rand()%100) / 50.0f + 0.2f;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Fire (void)
{
	for (int loop = 0; loop < 130; loop++) {
		ship_fire[loop].x = float(rand()%21) / 100.0f;
		ship_fire[loop].zi = float(rand()%100) / 250.0f;
		ship_fire[loop].life = float(rand()%100) / 100.0f;
		ship_fire[loop].fade = float(rand()%100) / 400.0 + 0.30 + ship_fire[loop].x;
		ship_fire[loop].x -= 0.1;
		ship_fire[loop].z = float(rand()%51) / 200.0f + ship_fire[loop].x * 2.0f;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Fire (void)
{
	glEnable (GL_BLEND);
	Rain_Texture.Usa_Texture (8);
	static int half_fire = FIRE_DETAIL >> 1;
	for (int loop = 0; loop < FIRE_DETAIL; loop++) {
		glPushMatrix ();
		if (loop < half_fire)
			glTranslatef (0.4 + ship_fire[loop].x, 0.0, 1.0 + ship_fire[loop].z);
		else
			glTranslatef (-0.4 + ship_fire[loop].x, 0.0, 1.0 + ship_fire[loop].z);
		glRotatef (-ship.angle, 0.0, 0.0, 1.0);
		glRotatef (-60.0, 1.0, 0.0, 0.0);
		glColor4f (1.0, 0.4, 0.2, ship_fire[loop].life);
		glBegin (GL_TRIANGLE_STRIP);
			glTexCoord2f (1.0, 1.0);		glVertex3f (0.05, 0.05, 0.0);
			glTexCoord2f (0.0, 1.0);		glVertex3f (-0.05, 0.05, 0.0);
			glTexCoord2f (1.0, 0.0);		glVertex3f (0.05, -0.05, 0.0);
			glTexCoord2f (0.0, 0.0);		glVertex3f (-0.05, -0.05, 0.0);
		glEnd();
		if (!pause) {
			ship_fire[loop].z += ship_fire[loop].zi * Frame_Interval;
			ship_fire[loop].life -= ship_fire[loop].fade * Frame_Interval;
			if (ship_fire[loop].life < 0.0) {
				ship_fire[loop].x = float(rand()%21) / 100.0f;
				ship_fire[loop].z = 0.0f;
				ship_fire[loop].zi = float(rand()%100) / 250.0f;
				ship_fire[loop].life = 1.0;
				ship_fire[loop].fade = float(rand()%100) / 400.0 + 0.30 + ship_fire[loop].x;
				ship_fire[loop].x -= 0.1;
			}
		}
		glPopMatrix ();
	}
	glDisable (GL_BLEND);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Terrain  (void)
{
	Rain_Texture.Usa_Texture (1);
	glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (2.0, 2.0 + terrain.texture_roll);		glVertex3f (30.0, 0.0, -70.0);
		glTexCoord2f (0.0, 2.0 + terrain.texture_roll);		glVertex3f (-50.0, 0.0, -70.0);
		glTexCoord2f (2.0, 0.0 + terrain.texture_roll);		glVertex3f (30.0, 0.0, 10.0);
		glTexCoord2f (0.0, 0.0 + terrain.texture_roll);		glVertex3f (-50.0, 0.0, 10.0);
	glEnd();
	if (!pause) {
		// scorrimento texture del terreno
		terrain.texture_roll += terrain.texture_speed * Frame_Interval;
		if (terrain.texture_roll > 40.0f)
			terrain.texture_roll = 0.0f;
	}
	Rain_Texture.Usa_Texture (9);
	for (int loop = 0; loop < NUM_TER_OBJ; loop++) {
		glPushMatrix ();
		glTranslatef (ter_obj[loop].x, ter_obj[loop].height, ter_obj[loop].z);
		glRotatef (90.0, 1.0, 0.0, 0.0);
		if (ter_obj[loop].tipe > 0)
			gluCylinder (quadratic , 0.0, ter_obj[loop].base_dim, ter_obj[loop].height, 4, 4);
		else
			gluCylinder (quadratic , 0.0, ter_obj[loop].base_dim, ter_obj[loop].height, 32, 32);
		glPopMatrix ();
		if (!pause) {
			// aggiornamento oggetti sul terreno
			ter_obj[loop].z += 20.0 * Frame_Interval;
			if (ter_obj[loop].z > 15.0) {
				ter_obj[loop].x = float(rand()%1200) / 10.0f - 70.0f;
				ter_obj[loop].z = float(rand()%1000) / 10.0f - 190.0f;
				ter_obj[loop].height = float(rand()%30) / 10.0f + 2.5f;
				ter_obj[loop].base_dim = float(rand()%20) / 10.0f + 1.5f;
				ter_obj[loop].tipe = rand()%4;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Space (void)
{
	glEnable (GL_BLEND);
	Rain_Texture.Usa_Texture (4);
	for (int loop = 0; loop < NUM_STAR; loop++) {
		glPushMatrix ();
		glTranslatef (stars[loop].x_position, 0.0, stars[loop].z_position);
		glRotatef (-60.0, 1.0, 0.0, 0.0);
		// scegli in modo simpatico le sfumature del colore
		if (loop % 15 == 0)
			glColor4f (0.1, stars[loop].color, stars[loop].color / 3, 1.0);
		else
			if (loop % 10 == 0)
				glColor4f (0.1, stars[loop].color / 3, stars[loop].color, 1.0);
			else
				if (loop % 5 == 0)
					glColor4f (stars[loop].color, stars[loop].color / 3, stars[loop].color / 3, 1.0);
				else
					glColor4f (1.0, 1.0, stars[loop].color * 3.0, 1.0);
		glBegin (GL_TRIANGLE_STRIP);
			glTexCoord2f (1.0, 1.0);		glVertex3f (stars[loop].deep, 0.0, 0.0);
			glTexCoord2f (0.0, 1.0);		glVertex3f (0.0, 0.0, 0.0);
			glTexCoord2f (1.0, 0.0);		glVertex3f (stars[loop].deep, stars[loop].deep, 0.0);
			glTexCoord2f (0.0, 0.0);		glVertex3f (0.0, stars[loop].deep, 0.0);
		glEnd();
		glPopMatrix ();
		if (!pause) {
			stars[loop].z_position += 8.0 * stars[loop].speed * Frame_Interval;
			if (stars[loop].z_position > 10.0) {
				stars[loop].x_position = float(rand()%800) / 10.0f - 50.0f;
				stars[loop].z_position = -75.0f;
				stars[loop].deep = float(rand()%400) / 400.0f + 0.2f;
				stars[loop].speed = stars[loop].deep * float(rand()%100) / 50.0f + 0.2f;
				stars[loop].color = float(rand()%100) / 110.0f + 0.1;
			}
		}
	}
	glColor4f (1.0, 1.0, 1.0, 0.9);
	for (int loop = 0; loop < SPECIAL_STAR_OBJ; loop++) {
		glPushMatrix ();
		glTranslatef (special[loop].x_position, 0.0, special[loop].z_position);
		glRotatef (50.0, 0.0, 1.0, 0.0);
		glRotatef (-60.0, 1.0, 0.0, 0.0);
		if (special[loop].speed > 1)
			Rain_Texture.Usa_Texture (5);
		else
			Rain_Texture.Usa_Texture (6);
		glBegin (GL_TRIANGLE_STRIP);
			glTexCoord2f (1.0, 1.0);		glVertex3f (special[loop].deep, 0.0, 0.0);
			glTexCoord2f (0.0, 1.0);		glVertex3f (0.0, 0.0, 0.0);
			glTexCoord2f (1.0, 0.0);		glVertex3f (special[loop].deep, special[loop].deep, 0.0);
			glTexCoord2f (0.0, 0.0);		glVertex3f (0.0, special[loop].deep, 0.0);
		glEnd();
		glPopMatrix ();
		if (!pause) {
			special[loop].z_position += 5.0 * special[loop].speed * Frame_Interval;
			if (special[loop].z_position > 40.0) {
				special[loop].x_position = float(rand()%800) / 10.0f - 50.0f;
				special[loop].z_position = float(rand()%800) / 10.0f - 150.0f;
				special[loop].deep = float(rand()%200) / 50.0f + 7.0f;
				special[loop].speed = float(rand()%100) / 50.0f + 0.2f;
			}
		}
	}
	glDisable (GL_BLEND);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

float Calcola_Distanza (float x1, float z1, float x2, float z2)
{
	float x_dist, z_dist;

	if (x1 > x2)
		x_dist = x1 - x2;
	else
		x_dist = x2 - x1;
	if (z1 > z2)
		z_dist = z1 - z2;
	else
		z_dist = z2 - z1;

	return sqrt(x_dist*x_dist + z_dist*z_dist);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

float Calc_Ship_Dist (float x2, float z2)
{
	float x_dist, z_dist;

	if (ship.x > x2)
		x_dist = ship.x - x2;
	else
		x_dist = x2 - ship.x;
	if (ship.z > z2)
		z_dist = ship.z - z2;
	else
		z_dist = z2 - ship.z;

	return sqrt(x_dist*x_dist + z_dist*z_dist);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Build_T_List (void)
{
	t_list = glGenLists(1);
      glNewList(t_list, GL_COMPILE);
      	glDisable (GL_BLEND);
      	glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
      	// TRASFORMAZIONI
		glTranslatef (-15.0, -13.0, -40.0);
		glRotatef (60.0, 1.0, 0.0, 0.0);
		glRotatef (-50.0, 0.0, 1.0, 0.0);
	glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Build_Ship_List (void)
{
	ship_list = glGenLists(1);
      glNewList(ship_list, GL_COMPILE);
      	glDisable (GL_TEXTURE_2D);
           	glColor3f (0.1f, 0.1f, 0.25f);
      	// ala SX
		glBegin (GL_TRIANGLE_STRIP);
			glVertex3f (-0.18, 0.1, -0.6);
			glVertex3f (-0.9, -0.1, 0.6);
			glVertex3f (-0.18, -0.1, 0.6);
			glVertex3f (-0.65, -0.12, 0.8);
		glEnd ();
		glBegin (GL_TRIANGLES);
			glVertex3f (-0.65, -0.12, 0.8);
			glVertex3f (-0.6, -0.2, 1.4);
			glVertex3f (-0.9, -0.1, 0.6);
		glEnd ();
		// ala DX
		glBegin (GL_TRIANGLE_STRIP);
			glVertex3f (0.18, 0.1, -0.6);
			glVertex3f (0.9, -0.1, 0.6);
			glVertex3f (0.18, -0.1, 0.6);
			glVertex3f (0.65, -0.12, 0.8);
		glEnd ();
		glBegin (GL_TRIANGLES);
			glVertex3f (0.65, -0.12, 0.8);
			glVertex3f (0.6, -0.2, 1.4);
			glVertex3f (0.9, -0.1, 0.6);
		glEnd ();
		// SFERA E CILINDRI LATERALI
		glEnable (GL_TEXTURE_2D);
		// sfera centrale
		glPushMatrix ();
		glTranslatef (0.0, 0.0, 0.25);
		Rain_Texture.Usa_Texture (2);
		glColor4f (1.0f, 0.5f, 0.5f, 1.0f);
		gluSphere (quadratic , 0.2, 16, 16);
		glPopMatrix ();
		// parti trasparenti
		glEnable (GL_BLEND);
		Rain_Texture.Usa_Texture (7);
		glColor4f (0.8f, 0.8f, 1.0f, 0.9f);
		// Cilindro SX
		glPushMatrix ();
		glTranslatef (-0.4, 0.0, -1.0);
		gluCylinder (quadratic , 0.0, 0.15, 2.0, 16, 16);
		glPopMatrix ();
		// Cilindro DX
		glPushMatrix ();
		glTranslatef (0.4, 0.0, -1.0);
		gluCylinder (quadratic , 0.0, 0.15, 2.0, 16, 16);
		glPopMatrix ();

		glDisable (GL_BLEND);
		glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
	glEndList();
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
      if (!Setup_GL ()) {                                                 // richiama Setup_GL x inizializzare la finestra
            OpenWork.KillGLWindow();                                                // distrugge la finestra
            MessageBox (NULL," Inizializzazione fallita. ", "ERROR", MB_OK | MB_ICONEXCLAMATION);
            return 0;
      }

      // inizializza la posizione degli oggetti
      Setup_World ();

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
	                        // Disegna la scena
                              RenderScene();
                              SwapBuffers (OpenWork.hDC);					// scambia le superfici di disegno (double buffering)
                              Calc_Frame_Rate ();								// calcola il frame rate
                              Genesis_Controller ();							// gestione comandi tastiera
                        }
                  }

                  // F1 = passaggio da window mode a fullscreen
                  if (OpenWork.keys[VK_F1]) {
                        OpenWork.keys[VK_F1] = false;
                        OpenWork.KillGLWindow();
                        OpenWork.fullscreen  = !OpenWork.fullscreen;

                        if (!OpenWork.CreateGLWindow (" - Rain (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!Setup_GL ()) {                                                       // richiama Setup_GL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Rain - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }

                  // F2 = modalità 640 x 480    -     32 bits
                  if (OpenWork.keys[VK_F2]) {
                        screen_width = 640;
                        screen_height = 480;
	                  OpenWork.keys[VK_F2] = false;
				OpenWork.KillGLWindow();

                        if (!OpenWork.CreateGLWindow (" - Rain (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!Setup_GL ()) {                                                       // richiama Setup_GL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Rain - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }

                  // F3 = modalità 800 x 600    -     32 bits
                  if (OpenWork.keys[VK_F3]) {
	                  screen_width = 800;
                        screen_height = 600;
                        OpenWork.keys[VK_F3] = false;
                        OpenWork.KillGLWindow();

                        if (!OpenWork.CreateGLWindow (" - Rain (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!Setup_GL ()) {                                                       // richiama Setup_GL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Rain - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }

                  // F4 = modalità 1024 x 768   -     32 bits
                  if (OpenWork.keys[VK_F4]) {
	                  screen_width = 1024;
                        screen_height = 768;
                        OpenWork.keys[VK_F4] = false;
                        OpenWork.KillGLWindow();

                        if (!OpenWork.CreateGLWindow (" - Rain (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!Setup_GL ()) {                                                       // richiama Setup_GL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Rain - ERROR", MB_OK | MB_ICONEXCLAMATION);
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

                        if (!OpenWork.CreateGLWindow (" - Rain (OpenGL) - ", screen_width, screen_height, bits, OpenWork.fullscreen))
                              return 0;
                        if (!Setup_GL ()) {                                                       // richiama Setup_GL x inizializzare la finestra
                              OpenWork.KillGLWindow();                                    // distrugge la finestra
                              MessageBox (NULL," Inizializzazione fallita. ", "Rain - ERROR", MB_OK | MB_ICONEXCLAMATION);
                              return 0;
                        }
                  }
            }
      }

	glDeleteLists (ship_list, 1);
	Fire.Delete_Fire_List ();
	E_Fire.Delete_EF_List ();
      OpenWork.Kill_Font ();
      // chiude la finestra
      OpenWork.KillGLWindow ();                                                            // SHUTDOWN
      return OpenWork.msg.wParam;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Calc_Frame_Rate (void)
{
	static float frame_per_second = 0.0f;			// variabile per gli fps
      static float last_time = 0.0f;					// variabile per il tempo dall'ultimo frame

	static float frame_time = 0.0f;				// tempo dell'ultimo frame
	float current_time = timeGetTime() * 0.001f;	// tempo del frame corrente

	Frame_Interval = current_time - frame_time;	// intervallo tra un frame e l'altro
	frame_time = current_time;

 	++frame_per_second;

	if (current_time - last_time > 1.0f) {
		last_time = current_time;
		sprintf (str_frame_rate, "FPS: %d - Total Time %d", int (frame_per_second), int (total_time));

		frame_per_second = 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Text (void)
{
	// disegna il testo con blending attivo
	glEnable (GL_BLEND);
	Rain_Texture.Usa_Texture (0);
      if (pause) {
		glColor4f (0.2f, 0.4f, 1.0f, 1.0f);
	      if (!start_game) {
      		OpenWork.glPrint ((screen_width >> 1) - 165, (screen_height >> 1), "- PREMERE  SPAZIO  PER INIZIARE -", 1, screen_width, screen_height);
      		glColor4f (0.5f, 0.8f, 0.5f, 1.0f);
      		OpenWork.glPrint ((screen_width >> 1) - 270, (screen_height >> 1) + 51, "Nemico colpito = 5 punti  -  Colpo a vuoto = 0 punti  :)", 0, screen_width, screen_height);
      		OpenWork.glPrint ((screen_width >> 1) - 270, (screen_height >> 1) + 34, "        I power up possono dare punti o energia", 0, screen_width, screen_height);
      	}
      	else {
	      	glColor4f (0.2f, 0.4f, 1.0f, 1.0f);
      		OpenWork.glPrint ((screen_width >> 1) - 140, (screen_height >> 1), "PAUSA - PREMERE   SPAZIO", 1, screen_width, screen_height);
      	}
      	if (game_over) {
      		OpenWork.glPrint ((screen_width >> 1) - 80, (screen_height >> 1) + 24, "- GAME OVER -", 1, screen_width, screen_height);
      		glColor4f (0.4f, 0.4f, 1.0f, 1.0f);
      		OpenWork.glPrint (0, (screen_height >> 1) - 124, "Si ringraziano i beta-tester :)", 0, screen_width, screen_height);
      		glColor4f (1.0f, 0.7f, 0.2f, 1.0f);
      		OpenWork.glPrint (0, (screen_height >> 1) - 141, "          Manfredo / i", 1, screen_width, screen_height);
      		OpenWork.glPrint (0, (screen_height >> 1) - 158, "          Gabriele Marchetti", 1, screen_width, screen_height);
      		OpenWork.glPrint (0, (screen_height >> 1) - 175, "          Luigi Quarenghi", 1, screen_width, screen_height);
      		OpenWork.glPrint (0, (screen_height >> 1) - 192, "          Enrico Toso", 1, screen_width, screen_height);
      	}
      }

      glColor4f (0.2f, 0.4f, 1.0f, 1.0f);
      OpenWork.glPrint (screen_width - 267, screen_height - 17, "R. A. I. N.   -   ver 0.20", 0, screen_width, screen_height);
      glColor4f (1.0f, 0.0f, 0.0f, 1.0f);
      // stampa l'energia rimasta
      sprintf (str_energy, "Shield Energy = %d %", ship.shield_energie);
      OpenWork.glPrint (0, screen_height - 17, str_energy, 0, screen_width, screen_height);
      sprintf (str_punteggio, "Punti = %d", punteggio);
      OpenWork.glPrint (0, screen_height - 34, str_punteggio, 0, screen_width, screen_height);
      glColor4f (0.2f, 0.4f, 1.0f, 1.0f);
      // stampa il frame rate
      OpenWork.glPrint (screen_width - 254, screen_height - 34, str_frame_rate, 1, screen_width, screen_height);
      // stampa la risoluzione coorente sullo schermo
	sprintf (str_ris, "%dx%dx%d", screen_width, screen_height, bits);
      if (screen_width != 1024)
      	OpenWork.glPrint (screen_width - 106, screen_height - 51, str_ris, 0, screen_width, screen_height);
      else
      	OpenWork.glPrint (screen_width - 120, screen_height - 51, str_ris, 0, screen_width, screen_height);
      // stampa il livello di dettaglio dopo un cambiamento
      if (show_detail < 1.0) {
      	switch (Enemy.detail_level) {
	      	case 0:	OpenWork.glPrint ((screen_width >> 1) - 115, screen_height - 100, "Livello dettaglio: BASSO", 1, screen_width, screen_height);
	      			break;
	      	case 1:	OpenWork.glPrint ((screen_width >> 1) - 115, screen_height - 100, "Livello dettaglio: MEDIO", 1, screen_width, screen_height);
	      			break;
	      	case 2:	OpenWork.glPrint ((screen_width >> 1) - 115, screen_height - 100, "Livello dettaglio: ALTO", 1, screen_width, screen_height);
	      			break;
      	}
      	glColor4f (0.6f, 0.4f, 1.0f, 1.0f);
      	if (game_dif == 0)
      		OpenWork.glPrint ((screen_width >> 1) - 45, screen_height - 120, "NORMALE", 1, screen_width, screen_height);
      	else
      		OpenWork.glPrint ((screen_width >> 1) - 45, screen_height - 120, "DIFFICILE", 1, screen_width, screen_height);
	}
	glDisable (GL_BLEND);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE CHE CONTROLLA LA PRESSIONE DEI TASTI

void Genesis_Controller (void)
{
	static float temp, x_off, z_off;

	// SPAZIO pausa il gioco
	if (OpenWork.keys[' '] && !spacep) {
		spacep = true;
		start_game = true;
		pause = !pause;
	}
	if (!OpenWork.keys[' '])
		spacep = false;

	// T cambia lo sfondo
	if (OpenWork.keys['T'] && !tp) {
		tp = true;
		green_terrain = !green_terrain;
	}
	if (!OpenWork.keys['T'])
		tp = false;

	// K e L diminuiscono e aumentano il dettaglio della grafica
	if (OpenWork.keys['K'] && !kp) {
		kp = true;
		show_detail = 0.0;
		if (Enemy.detail_level > 0)
			Enemy.detail_level --;
		switch (Enemy.detail_level) {
			case 0:	// disabilita Anti-Aliasing
					glDisable (GL_LINE_SMOOTH);
					break;
			case 1:	// disabilita Anti-Aliasing
					glDisable (GL_LINE_SMOOTH);
					break;
		}
	}
	if (!OpenWork.keys['K'])
		kp = false;

	if (OpenWork.keys['L'] && !lp) {
		lp = true;
		show_detail = 0.0;
		if (Enemy.detail_level < 2)
			Enemy.detail_level ++;
		switch (Enemy.detail_level) {
			case 1:	// disabilita Anti-Aliasing
					glDisable (GL_LINE_SMOOTH);
					break;
			case 2:	// abilita Anti-Aliasing
					glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
					glEnable (GL_LINE_SMOOTH);
					break;
		}
	}
	if (!OpenWork.keys['L'])
		lp = false;

	// D = cambia la difficoltà di gioco
	if (OpenWork.keys['D'] && !dp) {
		dp = true;
		show_detail = 0.0;
		game_dif ++;
		if (game_dif > 1)
			game_dif = 0;
	}
	if (!OpenWork.keys['D'])
		dp = false;

	if (!pause && start_game)
	{
		// UP o DOWN = spostamento asse Z
		if (OpenWork.keys[VK_UP]) {
			if (ship.z > -36.0f)
				ship.z -= SPEED_UP * Frame_Interval;
		}

		if (OpenWork.keys[VK_DOWN]) {
			if (ship.z < -3.5f) {
				temp = (ship.x - 6.85) * (-1.192) - 3.55;
				if (ship.z < temp)
					ship.z += SPEED_UP * Frame_Interval;
			}
		}

		// RIGHT o LEFT = rotazione asse Y
		if (OpenWork.keys[VK_RIGHT]) {
			if (ship.x < 13.0f) {
				temp = (ship.z + 3.55) * (-0.839) + 6.80;
				if (ship.x < temp) {
					ship.x += SPEED_LR * Frame_Interval;
				}
			}
			if (ship.angle > -20.0f)
				ship.angle -= 60 * Frame_Interval;
		}
		else
			if (ship.angle < 0.0f)
				ship.angle += 60 * Frame_Interval;

		if (OpenWork.keys[VK_LEFT]) {
			if (ship.x > -13.0f) {
				ship.x -= SPEED_LR * Frame_Interval;
			}
			if (ship.angle < 20.0f)
				ship.angle += 60 * Frame_Interval;
		}
		else
			if (ship.angle > 0.0f)
				ship.angle -= 60 * Frame_Interval;

		// CTRL = spara
		if (OpenWork.keys[VK_CONTROL] && fire_time > 0.25f) {
			Fire.Add_Fire (ship.x, ship.z);
			if (fire_time > 0.25f)
				fire_time = 0.0f;
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------