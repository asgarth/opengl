//-------------		file			OpenGL_Genesis.h		-----------------------------------------------------------------------------

#ifndef OpenGL_Genesis_H
#define OpenGL_Genesis_H

#include "Rain_Includes.h"

class OpenGL_Genesis
{
	public:
		HDC				hDC;
		HGLRC				hRC;
		HWND				hWnd;
		HINSTANCE		hInstance;
		MSG				msg;
		WNDPROC 			MyWndProc;					// variabile di appoggio per puntare a WndProc

		bool			done;
		bool			keys[256];
		bool			active;
		bool			fullscreen;

		GLuint base;										// variabile per liste di viusalizzazione dei font

		OpenGL_Genesis();				// costruttore
		virtual ~OpenGL_Genesis();		// destructor

		bool Enable_OpenGL (WNDPROC WndProcTT, int width, int height, int bits);
		GLvoid ReSizeGLScene (GLsizei width, GLsizei hight);
		GLvoid KillGLWindow (GLvoid);
		bool CreateGLWindow (char* title, int width, int height, int bits, int fullscreenflag);

		// funzione per visualizzazione font
		GLvoid Build_Font (GLvoid);
		GLvoid Kill_Font (GLvoid);
		GLvoid glPrint (GLint, GLint, char *, int, GLint, GLint);
};

#endif