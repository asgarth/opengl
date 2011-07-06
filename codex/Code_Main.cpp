/*--------------------------------------------------		Code_Main.cpp		----------------------------------------------------------*/

#include "Code_Includes.h"
#include "Texture_Manager.h"
#include "Code_String.h"
#include "Render_Text.h"

#define NUM_STRING		120

void Render_Scene (void);
void Setup_GL (void);
void Setup_Texture (void);
void Setup_String (void);
void Resize_GL (int, int);
void Genesis_Mouse (int, int, int, int);
void Genesis_Keyboard (unsigned char, int, int);
void Genesis_Special_K (int, int, int);
void Kill_All (void);
void Calc_Frame_Rate (void);

// variabili globali della finestra
int screen_width = 800, screen_height = 600;
bool fullscreen = true;
float Frame_Interval = 0.0f;				// variabile in cui è memorizzato il tempo trascorso tra un frame e l'altro
char str_frame_rate[50] = {0};			// stringa per visualizzare gli fps
bool vertical = true;

// variabili per il programma
String_Code string_mem[NUM_STRING];

// oggetti globali
Texture_Manager	TM;										// oggetto per la gestione delle texture
Code_String			CodeS;									// oggetto per la gestione del testo
Render_Text			Rend_Text;							// oggetto per il rendering del testo

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Render_Scene (void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();										// resetta la visuale

	glEnable (GL_BLEND);
	TM.Usa_Texture (0);								// texture per il testo

	// disegna il testo
	if (vertical) {
		for (int loop = 0; loop < NUM_STRING; loop++) {
			Rend_Text.Render_Vertical_String (&string_mem[loop], Frame_Interval);
			int leng = strlen (string_mem[loop].string) + 3;
			if (string_mem[loop].y_pos < -leng * 10 * string_mem[loop].size)
				CodeS.Get_String (&string_mem[loop], rand()%(CODE_LENGTH - 1), vertical);
		}
	}
	else {
		for (int loop = 0; loop < NUM_STRING; loop++) {
			Rend_Text.Render_String (&string_mem[loop], Frame_Interval);
			int leng = strlen (string_mem[loop].string) + 3;
			if (string_mem[loop].x_pos < -leng * 10 * string_mem[loop].size)
				CodeS.Get_String (&string_mem[loop], rand()%(CODE_LENGTH - 1), vertical);
		}
	}

	// calcola il frame rate e aggiorna lo schermo
	Calc_Frame_Rate ();
	glFlush ();
	glutSwapBuffers ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Setup_GL (void)
{
	// setup finestra
	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);											// sfondo nero
	glClearDepth (1.0f);																	// setup depth buffer
	glDepthFunc (GL_LEQUAL);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);									// funzione per il blending
	glShadeModel (GL_SMOOTH);													// enable smooth shading
	glEnable (GL_TEXTURE_2D);													// enable texture mapping

	Resize_GL (screen_width, screen_height);
	CodeS.Set_Resolution (screen_width, screen_height);

	srand(time(NULL));								// inizializza il generatore di numeri casuali
	Setup_Texture ();								// carica le texture
	Rend_Text.Build_Font ();						// costruisce le liste di visualizzazione del testo
	Setup_String ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Setup_Texture (void)
{
	if (!(TM.LoadTGA ("CodeX_Saver_texture.tga")))
		MessageBox (NULL, "Errore Caricamento Texture", "ERROR", MB_OK | MB_ICONERROR);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Setup_String (void)
{
	for (int loop = 0; loop < NUM_STRING; loop++) {
		CodeS.Get_String (&string_mem[loop], rand()%(CODE_LENGTH - 1), vertical);
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

int main (int argc, char** argv)
{
	// inizializzazione della finestra
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
	// prende le informazioni sulla risoluzione dello schermo
	screen_width = glutGet (GLUT_SCREEN_WIDTH);
	screen_height = glutGet (GLUT_SCREEN_HEIGHT);
	if (screen_width == 0 || screen_height == 0) {
		screen_width = 800;
		screen_height = 600;
	}
	// crea la finestra
	glutInitWindowSize (screen_width, screen_height);
	glutCreateWindow ("CodeX Saver 0.5");
	// setup finestra
	Setup_GL ();
	glutFullScreen ();
	ShowCursor (FALSE);
	glutReshapeFunc (Resize_GL);
	// funzioni per la gestione della tastiera e del mouse
	glutKeyboardFunc (Genesis_Keyboard);
	glutSpecialFunc (Genesis_Special_K);
	glutMouseFunc (Genesis_Mouse);
	// funzioni di visualizzazione
	glutDisplayFunc (Render_Scene);
	glutIdleFunc (Render_Scene);

	glutMainLoop ();

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Resize_GL (int width, int height)
{
	if (height == 0)									//se l'altezza è 0 diventa 1
		height = 1;

	glViewport (0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0.0, (GLdouble) screen_width, 0.0, (GLdouble) screen_height, -50.0, 50.0);
	//gluPerspective(45.0f , (GLfloat) width / (GLfloat) height , 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	screen_width = width;
	screen_height = height;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Genesis_Mouse (int btn, int state, int x, int y)
{
	Kill_All ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Genesis_Keyboard (unsigned char c, int x, int y)
{
	if (c == 'v')
		vertical = !vertical;
	else
		Kill_All ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Genesis_Special_K (int i, int x, int y)
{
	if (i == GLUT_KEY_F1) {
		if (fullscreen) {
			glutReshapeWindow (800, 600);
			glutPositionWindow (20, 30);
			ShowCursor (TRUE);
		}
		else {
			glutFullScreen ();
			ShowCursor (FALSE);
		}
		fullscreen = !fullscreen;
	}
	else
		Kill_All ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Kill_All (void)
{
	Rend_Text.Kill_Font ();
	CodeS.Dealloc_Text ();
	int winID = glutGetWindow ();
	glutDestroyWindow (winID);
	exit (0);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Calc_Frame_Rate (void)
{
	static float frame_per_second = 0.0f;				// variabile per gli fps
	static float last_time = 0.0f;							// variabile per il tempo dall'ultimo frame

	static float frame_time = 0.0f;						// tempo dell'ultimo frame
	float current_time = timeGetTime() * 0.001f;		// tempo del frame corrente

	Frame_Interval = current_time - frame_time;	// intervallo tra un frame e l'altro
	frame_time = current_time;

	++frame_per_second;

	if (current_time - last_time > 1.0f) {
		last_time = current_time;
		sprintf (str_frame_rate, "FPS: %d", int (frame_per_second));

		frame_per_second = 0;
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
