/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef INCLUDES_H
#define INCLUDES_H

// librerie di sistema
#include <stdio.h>			// Header File per standard Input/Output
#include <stdlib.h>
#include <string.h>
#include <time.h>

// librerie grafiche
#include <SDL/SDL.h>			// header per Simple DirectMedia Layer
#include <SDL/SDL_image.h>	// header file per lib immagini delle SDL
#include <gl/gl.h>			// Header File per la libreria OpenGL32
#include <gl/glu.h>			// Header File per la libreria GLu32
#include <gl/glext.h>		// Header File per le estensioni OpenGL
#include "SDL_GL_Init.h"

#define VERSION							0.1f

#define PKT_SIZE							1024
#define MAX_CLIENT						7
#define SEND_TIMEOUT						22
#define SEND_INFO_TIMEOUT				7000
#define SERVER_SLEEP_TIMEOUT			10
#define CLIENT_SLEEP_TIMEOUT			17

#define TERRAIN_RES						16.0
#define HEIGHT_OFFSET					3.0f

#endif
