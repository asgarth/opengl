//-------------		file			Rol_Main.cpp		-------------------------------------------------------------------------------------

#include "Rain_Includes.h"
#include "OpenGL_Genesis.h"
#include "Texture_Manager.h"
#include "Load_World.h"

#define SNOW_NUM		300

bool RenderScene (void);
void CalcolaMovimenti (void);
void Next_Level (void);
bool Setup_GL (void);
void Setup_Texture (void);
void Setup_World (void);
void Render_Snow (void);
void Render_Rain (void);
void Setup_Snow (void);
void Build_End_List (void);
void Build_Snow_List (void);
void Build_Rain_List (void);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Calc_Frame_Rate (void);
void Render_Text (void);
void Mouse_Controller (void);
void Genesis_Controller (void);

// variabili per la gestione della finestra
int screen_width = 800;
int screen_height = 600;
int bits = 16;

char str_frame_rate[50] = {0};				// stringa per visualizzare gli fps
char str_ris[50] = {0};						// stringa per la risoluzione dello schermo
char str_level_design[30] = {0};			// stringa per il nome del creatore del livello
char str_salti[20] = {0};						// stringa per visualizzare i salti ancora disponibili
float Frame_Interval = 0.0f;				// variabile in cui è memorizzato il tempo trascorso tra un frame e l'altro
float x_rot, z_rot, y_position = 15.0f;		// angoli di inclinazione della visuale e anltezza
float total_time;								// tempo totale trascorso dall'inizio del livello
int num_salti;									// numero di salti ancora disponibili
bool pause = true, spacep;					// gioco in pausa?
bool lp;										// salta al livello successivo
bool mouse_mov;								// indica se il mouse è stato mosso

// setup luci
GLfloat light0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light1_diffuse[] = { 0.9, 0.9, 0.9, 1.0 };
GLfloat light1_specular[] = { 0.9, 0.9, 0.9, 1.0 };
GLfloat light1_position[] = { 0.8, 0.5, 1.0, 0.0 };
GLfloat light1_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

// puntatore per la gestione delle proprietà degli oggetti 3D
GLUquadricObj *quadratic;
// setup valori glMaterial
GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat high_shininess[] = { 80.0 };

// variabili per la gestione dell'oggetto principale
typedef struct {	float x, y, z, ray;							// coordiante e raggio
					float x_sp, z_sp;							// velocità
					float col_r, col_g, col_b;				// colore della sfera
					float x_angle, z_angle;					// angolo di inclinazione della sfera
} BALL;
BALL ball;

// variabili per le liste di visualizzazione
GLuint end_list, snow_list, rain_list;

// variabili varie
bool salto, ctrlp;												// salto?
float jump_angle, current_height;							// attuale altezza del salto
int texture_wall;

// variabili per pioggia e neve
typedef struct {	float x, y, z, vel;							// dati della neve o della pioggia
} SNOW;
SNOW snow[SNOW_NUM];
SNOW rain;
float snow_angle;												// angolo per il movimento della neve

// OGGETTI
OpenGL_Genesis		OpenWork;							// oggetto globale per le funzioni opengl
Texture_Manager	Rain_Texture;						// oggetto per la gestione delle texture
Load_World			World ("Data/Level_0.txt");			// oggetto per creare lo scenario

//-------------------------------------------------------------------------------------------------------------------------------------------------------

bool RenderScene (void)
{
	static int loop;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();										// resetta la visuale

	// disegna lo sfondo
	Rain_Texture.Usa_Texture (texture_wall);
	glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (1.0f, 1.0f);
		glVertex3f 	(85.0f, 	85.0f,		-150.0f);
		glTexCoord2f (0.0f, 1.0f);
		glVertex3f	 (-85.0f, 	85.0f, 		-150.0f);
		glTexCoord2f (1.0f, 0.0f);
		glVertex3f 	(85.0f, 	-85.0f, 	-150.0f);
		glTexCoord2f (0.0f, 0.0f);
		glVertex3f 	(-85.0f, 	-85.0f, 	-150.0f);
	glEnd ();

	// disegna pioggia o neve
	if (texture_wall == 2)
		Render_Snow ();
	if (texture_wall == 1)
		Render_Rain ();

	// TRASFORMAZIONI
	gluLookAt (	0.0 + ball.x,	y_position + ball.y,	-15.0 - y_position / 5.0f + ball.z,
					0.0 + ball.x,	ball.y, 					0.0 + ball.z,
					0.0, 			0.0, 					1.0);

	// disegna la sfera
	Rain_Texture.Usa_Texture (8);
	glColor4f (ball.col_r, ball.col_g, ball.col_b, 1.0f);
	glPushMatrix();
		glTranslatef (0.0f, ball.y, 0.0f);
		glRotatef (-x_rot, 1.0f, 0.0f, 0.0f);
		glRotatef (-z_rot, 0.0f, 0.0f, 1.0f);
		glTranslatef (ball.x, ball.ray, ball.z);
		glRotatef (ball.x_angle, 1.0f, 0.0f, 0.0f);
		glRotatef (ball.z_angle, 0.0f, 0.0f, 1.0f);
		gluSphere (quadratic , ball.ray, 32, 32);
	glPopMatrix();
	glColor4f (1.0f, 1.0f, 1.0f, 1.0f);

	// disegna il percorso
	for (loop = 0; loop < World.num_poligoni; loop ++) {
		glPushMatrix ();
			glTranslatef (0.0f, World.pol[loop].height, 0.0f);
			glRotatef (-x_rot, 1.0f, 0.0f, 0.0f);
			glRotatef (-z_rot, 0.0f, 0.0f, 1.0f);
			if (World.pol[loop].blend) {
				glDisable (GL_LIGHTING);
				glEnable (GL_BLEND);
				glColor4f (1.0f, 1.0f, 1.0f, 0.85f);
			}
			if (ball.y < World.pol[loop].height - 0.5f) {
				glDisable (GL_LIGHTING);
				glEnable (GL_BLEND);
				glColor4f (1.0f, 1.0f, 1.0f, 0.7f);
			}
			// controlla che la texture sia nell'intervallo altrimenti ne sceglie una a caso
			if ( World.pol[loop].texture < 1 || World.pol[loop].texture > 9)
				World.pol[loop].texture = rand()%8 + 1;
			Rain_Texture.Usa_Texture (World.pol[loop].texture);
			glBegin (GL_TRIANGLE_STRIP);
				glTexCoord2f (World.pol[loop].vertex[0].u,		World.pol[loop].vertex[0].v);
				glVertex3f 	(World.pol[loop].vertex[0].x, 	0.0f, 		World.pol[loop].vertex[0].z);
				glTexCoord2f (World.pol[loop].vertex[1].u, 	World.pol[loop].vertex[1].v);
				glVertex3f	 (World.pol[loop].vertex[1].x, 	0.0f, 		World.pol[loop].vertex[1].z);
				glTexCoord2f (World.pol[loop].vertex[2].u, 	World.pol[loop].vertex[2].v);
				glVertex3f 	(World.pol[loop].vertex[2].x, 	0.0f, 		World.pol[loop].vertex[2].z);
				glTexCoord2f (World.pol[loop].vertex[3].u, 	World.pol[loop].vertex[3].v);
				glVertex3f 	(World.pol[loop].vertex[3].x, 	0.0f, 		World.pol[loop].vertex[3].z);
			glEnd ();
			glDisable (GL_BLEND);
			glColor4f (1.0, 1.0f, 1.0f, 1.0f);
			glEnable (GL_LIGHTING);
		glPopMatrix ();
	}

	// disegna il traguardo
	Rain_Texture.Usa_Texture (8);
	glPushMatrix();
		glTranslatef (0.0f, World.y_f, 0.0f);
		glRotatef (-x_rot, 1.0f, 0.0f, 0.0f);
		glRotatef (-z_rot, 0.0f, 0.0f, 1.0f);
		glCallList (end_list);
	glPopMatrix ();

	// controlla la posizione della sfera e la sua altezza
	CalcolaMovimenti ();

	glFlush ();
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void CalcolaMovimenti (void)
{
	static int loop;
	static float max_height;
	static float x0, x1, x2, x3, z0, z1, z2, z3, x_tempDX, x_tempSX, z_tempUP, z_tempDW;

	// controlla se è sul traguardo e il quadro è finito
	if (	(ball.x < World.x_f1 && ball.x > World.x_f2) &&
		(ball.z > World.z_f - 0.3 && ball.z < World.z_f + 0.3) &&
		(ball.y > World.y_f - 0.2 && ball.y < World.y_f + 2.0) ) {
			Next_Level ();
	}

	//  controlla la posizione della sfera
	current_height = 0.0f;
	for (loop = 0; loop < World.num_poligoni; loop ++) {
		// memorizza i vertici in variabili temporanee
		x0 = World.pol[loop].vertex[0].x;		z0 = World.pol[loop].vertex[0].z;
		x1 = World.pol[loop].vertex[1].x;		z1 = World.pol[loop].vertex[1].z;
		x2 = World.pol[loop].vertex[2].x;		z2 = World.pol[loop].vertex[2].z;
		x3 = World.pol[loop].vertex[3].x;		z3 = World.pol[loop].vertex[3].z;
		// controlla i margini del poligono sulla X
		x_tempDX = ((ball.z - z2) / (z0 - z2)) * (x0 - x2) + x2;
		x_tempSX = ((ball.z - z3) / (z1 - z3)) * (x1 - x3) + x3;
		// controlla i margini del poligono sulla Z
		z_tempUP = ((ball.x - x0) / (x1 - x0)) * (z1 - z0) + z0;
		z_tempDW = ((ball.x - x2) / (x3 - x2)) * (z3 - z2) + z2;
		// controlla che la sfera sia dentro i margini
		if (	(ball.x < x_tempDX + 0.1f) && (ball.x > x_tempSX - 0.1f) &&
			(ball.z < z_tempUP + 0.1f) && (ball.z > z_tempDW - 0.1f) &&
			(ball.y > World.pol[loop].height - 0.2f)) {
			if (World.pol[loop].height > current_height)
				current_height = World.pol[loop].height;
		}
	}

	// se la sfera è + alta del poligono spostamento verso il basso
	if (!pause) {
		if (ball.y > current_height && !salto) {
			ball.y -= 4.0f * Frame_Interval;
			if (ball.y < current_height)
				ball.y = current_height;
			if (ball.y < 3.0f)							// caduta e si ricomincia
				Setup_World ();
		}
	}

	// aggiorna il movimento e la velocità
	if (!pause) {
		total_time += Frame_Interval;
		snow_angle += 30 * Frame_Interval;
		if (snow_angle > 360.0f)
			snow_angle -= 360.0f;
		if (!salto) {
			ball.x_sp += sin (z_rot * 0.0174532925f) * 0.3f * Frame_Interval;
			ball.z_sp -= sin (x_rot * 0.0174532925f) * 0.3f * Frame_Interval;
		}
		// spostamento della sfera
		ball.x += ball.x_sp - 0.5f * ball.x_sp;
		ball.z += ball.z_sp - 0.5f * ball.z_sp;
		// rotazione della sfera (se durante un salto la rotazione rallenta)
		if (ball.y > current_height + 0.1f) {
			ball.x_angle += ball.z_sp * 400.0f * Frame_Interval;
			ball.z_angle -= ball.x_sp * 400.0f * Frame_Interval;
		}
		else {
			ball.x_angle += ball.z_sp * 1000.0f * Frame_Interval;
			ball.z_angle -= ball.x_sp * 1000.0f * Frame_Interval;
		}
	}

	// gestione dei salti
	if (!pause && salto) {
			ball.y += cos (jump_angle * 0.0174532925f) * 0.15f;
			jump_angle += 130.0f * Frame_Interval;
			if (jump_angle > 90.0f)
				salto = false;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Next_Level (void)
{
	//PlaySound ("Data/Hourglass.wav", NULL, SND_SYNC);
	PlaySound ("Data/Hourglass.wav", NULL, SND_ASYNC);
	delete (World.pol);
	World.file_level_name = World.file_next_level;
	World.Setup_World ();
	Build_End_List ();						// setup del traguardo
	sprintf (str_level_design, "Level Design: %s", World.level_creator);
	Setup_World ();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

bool Setup_GL (void)
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
	glMaterialfv (GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv (GL_FRONT, GL_SHININESS, high_shininess);

	// setup luci (DA MIGLIORARE)
	glLightfv (GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv (GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv (GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv (GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv (GL_LIGHT1, GL_POSITION, light1_position);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT1);
	glDisable (GL_LIGHT0);

      // inizializza il generatore di numeri casuali
	srand (time (NULL));
      // carica le texture
      Setup_Texture ();
      return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Texture (void)
{
	Rain_Texture.Load_BMP_Textures ("Texture/text.bmp");		Rain_Texture.Load_BMP_Textures ("Texture/sky_2.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/sky_3.bmp");		Rain_Texture.Load_BMP_Textures ("Texture/sky.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/floor_black.bmp");Rain_Texture.Load_BMP_Textures ("Texture/floor_red.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/floor_green.bmp");Rain_Texture.Load_BMP_Textures ("Texture/floor_yel.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/glass_white.bmp");Rain_Texture.Load_BMP_Textures ("Texture/glass_blue.bmp");
	Rain_Texture.Load_BMP_Textures ("Texture/snow.bmp");

	OpenWork.Build_Font ();					// prepara le liste di visualizzazione dei font

	World.Setup_World ();
	Build_End_List ();						// setup del traguardo
	Build_Snow_List ();						// setup neve
	Build_Rain_List ();						// setup pioggia
	sprintf (str_level_design, "Level Design: %s", World.level_creator);
	Setup_Snow ();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_World (void)
{
	num_salti = World.num_salti;
	x_rot = z_rot = 0.0f;
	total_time = 0.0f;
	jump_angle = 0.0f;
	texture_wall = rand()%4 + 1;
	if (texture_wall > 3)
		texture_wall = 3;

	// setup dati della sfera
	ball.ray = 0.5f;
	ball.x = ball.z = 0.0f;
	ball.y = World.pol[0].height;
	ball.x_sp = ball.z_sp = 0.0f;
	ball.x_angle = ball.z_angle = 0.0f;
	ball.col_r = float(rand()%16) * 0.05f + 0.2f;
	ball.col_g = float(rand()%16) * 0.05f + 0.2f;
	ball.col_b = float(rand()%16) * 0.05f + 0.2f;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Snow (void)
{
	static int loop;
	Rain_Texture.Usa_Texture (10);
	for (loop = 0; loop < SNOW_NUM; loop ++) {
		if (!pause) {
			snow[loop].y -= snow[loop].vel * Frame_Interval;
			snow[loop].x += sin (snow_angle * 0.0174532925f) * Frame_Interval;
			snow[loop].z += sin (snow_angle * 0.0174532925f) * 0.5f * Frame_Interval;
		}
		glPushMatrix ();
		glTranslatef (snow[loop].x, snow[loop].y, snow[loop].z);
		glCallList (snow_list);
		if (snow[loop].y < -25.0f) {
			snow[loop].y = 33.0f;
			snow[loop].x = float(rand()%120) * 0.5f - 30.0f;
			snow[loop].z = float(rand()%80) * (-0.5f) - 5.0f;
			snow[loop].vel = float(rand()%10) * 0.5f +10.0f;
		}
		glPopMatrix ();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Render_Rain (void)
{
	static int loop;
	glDisable (GL_TEXTURE_2D);
	glColor3f (0.6f, 0.7f, 0.9f);
	for (loop = 0; loop < SNOW_NUM; loop ++) {
		if (!pause)
			snow[loop].y -= snow[loop].vel * snow[loop].vel * snow[loop].vel * Frame_Interval;
		glPushMatrix ();
		glTranslatef (snow[loop].x, snow[loop].y, snow[loop].z);
		glCallList (rain_list);
      	glTranslatef (snow[loop].x - snow[loop].x * 0.5f, snow[loop].y, snow[loop].z + 5.0f);
		glCallList (rain_list);
		if (snow[loop].y < -25.0f) {
			snow[loop].y = 33.0f;
			snow[loop].x = float(rand()%120) * 0.5f - 30.0f;
			snow[loop].z = float(rand()%80) * (-0.5f) - 5.0f;
			snow[loop].vel = float(rand()%10) * 0.5f +10.0f;
		}
		glPopMatrix ();
	}
	glEnable (GL_TEXTURE_2D);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Setup_Snow (void)
{
	for (int loop = 0; loop < SNOW_NUM; loop ++) {
		snow[loop].y = float(rand()%300) * 0.2f - 20.0f;
		snow[loop].x = float(rand()%120) * 0.5f - 30.0f;
		snow[loop].z = float(rand()%80) * 0.5f + 5.0f;
		snow[loop].vel = float(rand()%10) * 0.5f +10.0f;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Build_End_List (void)
{
	end_list = glGenLists(1);
	glNewList (end_list, GL_COMPILE);
		glColor4f (0.1f, 0.0f, 0.5f, 1.0f);
		glPushMatrix ();
			glTranslatef (World.x_f1, 2.0, World.z_f);
			glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
			gluCylinder (quadratic , 0.0, 0.3, 2.0, 4, 4);
		glPopMatrix ();
		glPushMatrix ();
			glTranslatef (World.x_f2, 2.0, World.z_f);
			glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
			gluCylinder (quadratic , 0.0, 0.3, 2.0, 4, 4);
		glPopMatrix ();
		glDisable (GL_TEXTURE_2D);
		glDisable (GL_LIGHTING);
		glEnable (GL_BLEND);
		glColor4f (1.0f, 0.2f, 0.4f, 0.6f);
		glBegin (GL_TRIANGLE_STRIP);
			glVertex3f (World.x_f2,	1.5,	World.z_f);
			glVertex3f (World.x_f1,	1.5,	World.z_f);
			glVertex3f (World.x_f2,	0.0, World.z_f);
			glVertex3f (World.x_f1,	0.0, World.z_f);
		glEnd ();
		glDisable (GL_BLEND);
		glEnable (GL_TEXTURE_2D);
		glEnable (GL_LIGHTING);
		glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
	glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Build_Snow_List (void)
{
	snow_list = glGenLists(1);
	glNewList (snow_list, GL_COMPILE);
		gluSphere (quadratic, 0.08, 8, 8);
	glEndList();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Build_Rain_List (void)
{
	rain_list = glGenLists(1);
	glNewList (rain_list, GL_COMPILE);
		glBegin (GL_LINES);
      		glVertex3f (0.0, 0.15, 0.0);
      		glVertex3f (0.0, 0.0, 0.0);
      	glEnd ();
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
		case WM_LBUTTONDOWN: 					// premuto tasto sinistro del mouse?
		{
			if (!salto && current_height != 0.0f && ball.y == current_height && num_salti > 0) {
				salto = true;
				jump_angle = 0.0f;
				num_salti --;
			}
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

      // setup dati
      Setup_World ();

      // loop principale
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
                              RenderScene();									// disegna la scena
                              Render_Text ();									// renderizza il testo sullo schermo
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

      // SHUTDOWN
	glDeleteLists (end_list, 1);
	glDeleteLists (snow_list, 1);
	glDeleteLists (rain_list, 1);
	OpenWork.Kill_Font ();
	OpenWork.KillGLWindow ();
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
	glDisable(GL_LIGHTING);
	Rain_Texture.Usa_Texture (0);

	glColor4f (0.5f, 0.5f, 1.0f, 1.0f);
	OpenWork.glPrint (0, screen_height - 17, "RollerBall  -   ver 0.9", 0, screen_width, screen_height);
	glColor4f (0.2f, 0.4f, 1.0f, 1.0f);
	// stampa il frame rate
	OpenWork.glPrint (screen_width - 254, screen_height - 17, str_frame_rate, 1, screen_width, screen_height);
	// stampa la risoluzione coorente sullo schermo
	sprintf (str_ris, "%dx%dx%d", screen_width, screen_height, bits);
	if (screen_width != 1024)
		OpenWork.glPrint (screen_width - 106, screen_height - 34, str_ris, 0, screen_width, screen_height);
	else
		OpenWork.glPrint (screen_width - 120, screen_height - 34, str_ris, 0, screen_width, screen_height);

	// stampa i salti ancora disponibili
	glColor4f (1.0f, 0.5f, 0.0f, 1.0f);
	sprintf (str_salti, "Salti: %d", num_salti);
	OpenWork.glPrint (0, screen_height - 34, str_salti, 0, screen_width, screen_height);
	// stampa il nome del creatore del livello
	glColor4f (0.0f, 1.0f, 0.0f, 1.0f);
	OpenWork.glPrint (0, 17, str_level_design, 1, screen_width, screen_height);

	// stampa una stringa se il gioco è in pausa
	if (pause) {
		glColor4f (0.2f, 0.4f, 1.0f, 1.0f);
		OpenWork.glPrint ((screen_width >> 1) - 140, (screen_height >> 1), "PAUSA - PREMERE   SPAZIO", 1, screen_width, screen_height);
	}
	glDisable (GL_BLEND);
	glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_LIGHTING);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Mouse_Controller(void)
{
	POINT mousePos;								// struttura window per X eY
	int middleX = screen_width  >> 1;				// metà larghezza
	int middleY = screen_height >> 1;				// metà altezza
	mouse_mov = true;

	// corrente posizione X eY del mouse
	GetCursorPos(&mousePos);

	// se il cursore è nel centro non fare niente
	if ((mousePos.x == middleX) && (mousePos.y == middleY)) {
		mouse_mov = false;
		return;
	}

	// setta la posizione al centro dello schermo
	SetCursorPos(middleX, middleY);

	// fa le rotazioni in base alla posizione del mouse ma prima riduce il valore
	x_rot -= (float)( (middleY - mousePos.y) ) * 0.8f * Frame_Interval;
	z_rot += (float)( (middleX - mousePos.x) ) * 0.5f * Frame_Interval;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNZIONE CHE CONTROLLA LA PRESSIONE DEI TASTI E IL MOVIMENTO DEL MOUSE

void Genesis_Controller (void)
{
	// SPAZIO pausa il gioco
	if (OpenWork.keys[' '] && !spacep) {
		spacep = true;
		pause = !pause;
	}
	if (!OpenWork.keys[' '])
		spacep = false;

	if (OpenWork.keys['L'] && !lp) {
		lp = true;
		Next_Level ();
	}
	if (!OpenWork.keys['L'])
		lp = false;

	// modifica l'altezza della visuale
	if (OpenWork.keys['Q'])
		y_position += 10.0f * Frame_Interval;

	if (OpenWork.keys['A'])
		y_position -= 10.0f * Frame_Interval;

	if (!pause) {
		Mouse_Controller ();

		if (!mouse_mov) {
			// UP o DOWN = spostamento asse Z
			if (OpenWork.keys[VK_UP]) {
				x_rot -= 20.0 * Frame_Interval;
			}
			if (OpenWork.keys[VK_DOWN]) {
				x_rot += 20.0 * Frame_Interval;
			}

			// RIGHT o LEFT = rotazione asse Y
			if (OpenWork.keys[VK_RIGHT]) {
				z_rot -= 20.0 * Frame_Interval;
			}
			if (OpenWork.keys[VK_LEFT]) {
				z_rot += 20.0 * Frame_Interval;
			}
		}
		// CTRL = salto
		if (OpenWork.keys[VK_CONTROL] && !ctrlp) {
			ctrlp = true;
			if (!salto && current_height != 0.0f && ball.y == current_height && num_salti > 0) {
				salto = true;
				jump_angle = 0.0f;
				num_salti --;
			}
		}
		if (!OpenWork.keys[VK_CONTROL])
			ctrlp = false;
	}
	// controlla che gli angoli di inclinazione non siano troppo grandi
	if (x_rot > 25.0f)
		x_rot = 25.0f;
	if (x_rot < -25.0f)
		x_rot = -25.0f;
	if (z_rot > 30.0f)
		z_rot = 30.0f;
	if (z_rot < -30.0f)
		z_rot = -30.0f;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------