/*--------------------------------------------------		Code_Includes.h		----------------------------------------------------------*/

#ifndef Code_Includes_H
#define Code_Includes_H

#include <windows.h>				// Header File per le finestre
#include <math.h>					// Header File per librerie matematiche
#include <stdio.h>					// Header File per standard Input/Output
#include <stdarg.h>					// Header File per la gestione delle routines ad argomenti variabili
#include <string.h>					// Header File per manipolazione stringhe
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <tchar.h>
#include <dos.h>
#include <basetsd.h>
#include <iostream.h>
#include <fstream.h>
#include <conio.h>
#include <assert.h>
#include <mmsystem.h>

#include <gl\gl.h>						// Header File per la libreria OpenGL32
#include <gl\glu.h>						// Header File per la libreria GLu32
#include <gl\glut.h>						// Header File per la libreria Glut

#define CODE_LENGTH		233	// lunghezza del file di testo da leggere

typedef struct {	char *string;
						float x_pos, y_pos;
						float x_vel;
						int size;
						float r, g, b;
} String_Code;

#endif