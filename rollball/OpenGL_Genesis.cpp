//-------------		file			OpenGL_Genesis.cpp		-----------------------------------------------------------------------------

#include "OpenGL_Genesis.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

OpenGL_Genesis::OpenGL_Genesis()
{
	HWND		hWnd;								//Windows handle
	HDC		hDC;								//Device Context
	HGLRC		HRC;								//Rendering Context
	HINSTANCE	hInstance;

	bool done = false;								//indica se uscire dal programma
	bool active = true;								//indica se la finestra è attiva
	bool fullscreen = true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

OpenGL_Genesis::~OpenGL_Genesis() {	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

bool OpenGL_Genesis::Enable_OpenGL (WNDPROC WndProcTT, int width, int height, int bits)
{
	MyWndProc = WndProcTT;						// setta il puntatore alla funzione WndProc
	fullscreen = true;

	// Domanda all'utente se eseguire in modalità fullscreen o windowed
	if (MessageBox(NULL, " Eseguire in FullScreen ? ", " Rain - Fullscreen ", MB_YESNO | MB_ICONQUESTION) == IDNO)
		fullscreen = false;														// modalità finestra

	if (!CreateGLWindow (" - Rain (OpenGL) - ", width, height, 32, fullscreen))				// crea la finestra OpenGL
	{
		return 0;																// chiudi se il tentativo e' fallito
	}
	return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid OpenGL_Genesis::ReSizeGLScene(GLsizei width, GLsizei height)
{
	if (height == 0)									//se l'altezza è 0 diventa 1
		height = 1;

	glViewport (0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f , (GLfloat)width/(GLfloat)height , 0.1f, 250.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid OpenGL_Genesis::KillGLWindow (GLvoid)
{
	if (fullscreen) {
		ChangeDisplaySettings (NULL, 0);								//ripristina modalità di visualizzazione
		ShowCursor (TRUE);											//mostra mouse
	}

	if (hRC) {														//esiste un rendering context?
		if (!wglMakeCurrent (NULL, NULL))							//rilascio DC e RC?
			MessageBox (NULL, " Rilascio   DC e RC   fallito. ", "Genesis - SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		if (!wglDeleteContext (hRC))									//possiamo rilasciare l' RC?
			MessageBox (NULL, " Rilascio   RC   fallito. ", "Genesis - SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hRC = NULL;
	}

	if (hDC && !ReleaseDC (hWnd, hDC)) {								//possiamo rilasciare il DC?
		MessageBox (NULL, " Rilascio   DC   fallito. ", "Genesis - SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;
	}

	if (hWnd && !DestroyWindow (hWnd)) {							//possiamo distruggere la finestra?
		MessageBox (NULL, " Chiusura finestra fallita. ", "Genesis - SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;
	}

	if (!UnregisterClass ("OpenGL", hInstance)) {						//possiamo deregistrare la classe OpenGL?
		MessageBox (NULL, " Unregister class fallito. ", "Genesis - SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

// title = titolo della finestra
// width = larghezza finestra o risoluzione in fullscreen
// height = altezza finestra o risoluzione in fullscreen
// bits = numero di bits del colore
// fullscreenflag = lanciare in modalità fullscreen?
bool OpenGL_Genesis::CreateGLWindow (char* title, int width, int height, int bits, int fullscreenflag)
{
	GLuint			PixelFormat;				// contiene il risultato dopo aver cercato il formato dei pixel
	WNDCLASS		wc;						// struttura della Windows Class
	DWORD		dwExStyle;				// stile esteso finestra
	DWORD		dwStyle;				// stile finestra

	RECT			WindowRect;			// coordinate degli angoli alto/sx e basso/dx della finestra
	WindowRect.left = (long)0;
	WindowRect.right = (long)width;
	WindowRect.top = (long)0;
	WindowRect.bottom = (long)height;

	fullscreen = fullscreenflag;

	hInstance 			= GetModuleHandle(NULL);						// ottiene un'istanza per la finestra
	wc.style				= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// ottiene il DC e disegna nell'area
	wc.lpfnWndProc		= (WNDPROC) MyWndProc;						// indica che WndProc gestisce e msg di sistema
	wc.cbClsExtra		= 0;												// nessun dato extra
	wc.cbWndExtra		= 0;												// nessun dato extra
	wc.hInstance		= hInstance;										// setta l'istanza
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);				// carica l'icona di default
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);				// carica il puntatore di default
	wc.hbrBackground	= NULL;										// nessun background richiesto per l'OpenGL
	wc.lpszMenuName	= NULL;										// nessun menu
	wc.lpszClassName	= "OpenGL";										// nome della classe

	if (!RegisterClass(&wc)) {								// registrazione classe riuscita?
		MessageBox(NULL, " Registrazione Window Class fallita. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (fullscreen) {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// azzeramento memoria (pulisce lo schermo)
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// dimensioni del Device Mode
		dmScreenSettings.dmPelsWidth	= width;						// larghezza finestra
		dmScreenSettings.dmPelsHeight	= height;					// altezza finestra
		dmScreenSettings.dmBitsPerPel	= bits;						// seleziona i bits per pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// cambio modalità video riuscito?
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) {			//NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
			// se cambio modalità fallito entra in windowed mode
			if (MessageBox (NULL, " Modalità fullscreen non supportata.\nEntrare in window mode? ", "Genesis", MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
				fullscreen = FALSE;
			else {
				MessageBox(NULL, " Chiusura programma. ", "Genesis - ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	ShowCursor(FALSE);										// nascondi il puntatore del mouse
	if (fullscreen) {
		dwExStyle=WS_EX_APPWINDOW;				// stile esteso finestra
		dwStyle=WS_POPUP;								// stile finestra
	}
	else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// stile esteso finestra
		dwStyle=WS_OVERLAPPEDWINDOW;									// stile finestra
	}

	AdjustWindowRectEx (&WindowRect, dwStyle, FALSE, dwExStyle);				// settaggio dimensioni finestra

	// crea la finestra
	if (!(hWnd = CreateWindowEx(	dwExStyle,							// stile esteso finestra
									"OpenGL",							// nome della classe
									title,								// titolo finestra
									dwStyle |							// stile finestra
									WS_CLIPSIBLINGS |				// stile finestra rischiesto
									WS_CLIPCHILDREN,				// stile finestra rischiesto
									0, 0,								// posizione
									WindowRect.right - WindowRect.left,		// calcolo larghezza
									WindowRect.bottom - WindowRect.top,		// calcolo altezza
									NULL,								// nessuna finestra madre
									NULL,								// nessun menu
									hInstance,							// istanza
									NULL)))								// non passare niente a WM_CREATE
	{
		KillGLWindow();								// distrugge la finestra
		MessageBox(NULL," Errore nella creazione della finestra. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	static	PIXELFORMATDESCRIPTOR pfd =			// pfd descrive il formato pixel
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// dimensione del formato pixel
		1,												// numero versione
		PFD_DRAW_TO_WINDOW |					// il formato deve supportare Windows
		PFD_SUPPORT_OPENGL |						// il formato deve supportare le OpenGL
		PFD_DOUBLEBUFFER,							// il formato deve supportare il double buffering
		PFD_TYPE_RGBA,								// rischiesta RGBA
		bits,											// profondità colore
		0, 0, 0, 0, 0, 0,									// bit del colore ignorati
		0,												// no alpha buffer
		0,												// shift bit ignorati
		0,												// no accumulation buffer
		0, 0, 0, 0,										// accumulation bits ignorati
		16,												// 16bit Z-Buffer (Depth Buffer)
		0,												// no Stencil Buffer
		0,												// no Auxiliary Buffer
		PFD_MAIN_PLANE,								// livello di rendering primario
		0,												// RISERVATO
		0, 0, 0											// maschere non utilizzate
	};

	if (!(hDC = GetDC (hWnd))) {								// abbiamo un DC?
		KillGLWindow();										// distrugge la finestra
		MessageBox(NULL," Impossibile creare un GL Device Context. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC,&pfd))) {				// formato pixel adatto trovato?
		KillGLWindow();											// distrugge la finestra
		MessageBox(NULL," Impossibile trovarre un formato pixel. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!SetPixelFormat (hDC,PixelFormat, &pfd)) {					// formato pixel settato?
		KillGLWindow();											// distrugge la finestra
		MessageBox(NULL," Impossibile settare il formato pixel.", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC=wglCreateContext(hDC))) {				// Rendering Context settato?
		KillGLWindow();								// distrugge la finestra
		MessageBox(NULL," Impossibile creare un GL Rendering Context. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(!wglMakeCurrent (hDC, hRC)) {					// RC attivato?
		KillGLWindow();								// distrugge la finestra
		MessageBox(NULL," Impossibile attivare il Rendering Context. ", "Genesis - ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW);					// visualizza finestra
	SetForegroundWindow(hWnd);						// massimo livello priorità
	SetFocus(hWnd);									// da il controllo della tastiera all'applicazione
	ReSizeGLScene(width, height);					// attiva la modalità OpenGL

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid OpenGL_Genesis::Build_Font ()
{
	float cx, cy;
	base = glGenLists (256);

	for (int loop = 0; loop < 256; loop++) {
		cx = float(loop % 16) / 16.0f;
		cy = float(loop / 16) / 16.0f;

		glNewList (base + loop, GL_COMPILE);
			glBegin (GL_QUADS);
				glTexCoord2f(cx, 1-cy-0.0625f); 							// Texture Coord (Bottom Left)
				glVertex2i(0, 0); 										// Vertex Coord (Bottom Left)
				glTexCoord2f(cx+0.0625f, 1-cy-0.0625f); 					// Texture Coord (Bottom Right)
				glVertex2i(16, 0); 										// Vertex Coord (Bottom Right)
				glTexCoord2f(cx+0.0625f, 1-cy);							// Texture Coord (Top Right)
				glVertex2i(16, 16); 										// Vertex Coord (Top Right)
				glTexCoord2f(cx, 1-cy); 									// Texture Coord (Top Left)
				glVertex2i(0, 16); 										// Vertex Coord (Top Left)
			glEnd();
			glTranslated(10, 0, 0); 										// Move To The Right Of The Character
		glEndList();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid OpenGL_Genesis::Kill_Font ()
{
	glDeleteLists (base, 256);					// cancella le liste di visualizzazione
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

GLvoid OpenGL_Genesis::glPrint (GLint x, GLint y, char *string, int set, GLint width, GLint height)
{
	if (set > 1)
		set = 1;

	glDisable (GL_DEPTH_TEST);
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
	glLoadIdentity();
	glOrtho (0, width, 0, height, -100, 100);
	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
	glLoadIdentity ();

	glTranslated (x, y, 0);
	glListBase (base - 32 + (128 * set));
	glCallLists (strlen(string), GL_BYTE, string);

	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
	glEnable (GL_DEPTH_TEST);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------