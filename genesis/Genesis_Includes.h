//-------------		file			Includes.h		-------------------------------------------------------------------------------------

#ifndef _GENESIS_INCLUDES_H_
#define _GENESIS_INCLUDES_H_

#include <windows.h>				// Header File per le finestre
#include <math.h>				// Header File per librerie matematiche
#include <stdio.h>				// Header File per standard Input/Output
#include <stdarg.h>	   			// Header File per la gestione delle routines ad argomenti variabili
#include <string.h>	  			 // Header File per manipolazione stringhe
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

#include <gl\gl.h>				// Header File per la libreria OpenGL32
#include <gl\glu.h>				// Header File per la libreria GLu32
#include <gl\glaux.h>				// Header File per la libreria Glaux

#define PIOVER180			0.0174532925f                     // fattore di conversione tra gradi e radianti

// Classe base per la gestione dei vettori
struct CVector3
{
	public:

		CVector3() {	}									// costruttore

		CVector3(float X, float Y, float Z)				// costruttore che inizializza i campi con i parametri
		{
			x = X;
			y = Y;
			z = Z;
		}

		CVector3 operator+ (CVector3 vVector)				// overloading dell'operatore +
		{
			return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
		}

		CVector3 operator- (CVector3 vVector)				// overloading dell'operatore -
		{
			return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
		}

		CVector3 operator* (float num)						// overloading per moltiplicare per uno scalare
		{
			return CVector3(x * num, y * num, z * num);
		}

		CVector3 operator/ (float num)						// overloading per dividere per uno scalare
		{
			return CVector3(x / num, y / num, z / num);
		}


		float x, y, z;
};

#endif