/************************************************************
		Sistemi Intelligenti - Flock

		Sergio Sartorelli
************************************************************/

#include "includes.h"
#include "ParticleEngine.h"
#include "TextureManager.h"

void renderFrame (void);
bool init (void);
void calcFrameRate (void);
void update (void);
void eventManager (void);

using namespace std;

// GLOBAL VAR
bool done = false;
// window
int screen_width = 640, screen_height = 480, screen_bpp = 32;	// risoluzione di default
bool fullscreen = false;
// frame rate
int fps = 30;									// FPS (init value)
bool limit_fps = true;					// limita gli fps ad un valore massimo
bool show_fps = true;				// visualizza gli fps durante il gioco
float frame_interval = 0.0;			// intervallo tra un frame e l'altro
bool pause = false;						// pause rendering
// camera
Vector cam;									// camera position
float angle = 0.0f;						// camera angle
float dist = 30.0f;						// camera distance

// gravity
Vector grav;									// gravity

// opengl light setup
float LightPosition[] = {0.0f, 10.0f, 0.0f, 1.0f};		// position
float LightAmbient[] = {0.8f, 0.8f, 0.8f, 1.0f};			// ambient
float LightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};			// diffuse
float LightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};		// specular

// global object
SDL_GL_Init 			SDL_Window;
TextureManager	textureManager;
ParticleEngine 		engine( 2000 );


/* ---------------------------------------------------------------------------------------------------------------------- */
// MAIN
int main( int argc, char *argv[] ) {
	// init graphic
	if ( ! init() )
		return -1;

	// main loop
	while ( ! done ) {
		eventManager ();						// input and event handle
		renderFrame ();							// render scene
		calcFrameRate ();						// calculate fps (and limit fps)
		if ( !pause)
			update ();										// update position
	}

	// release memory
	SDL_Window.release_memory ();	// free memory for sdl window
	return 0;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// funzione di renderizzazione del frame
void renderFrame (void) {
	SDL_Window.render_mode ();				// enable 3D rendering

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity ();									// reset camera

	//gluLookAt (0.0, 1.0, 35.0,		0.0, 0.0, 0.0,		0.0, 1.0, 0.0);
	cam.x = dist * sin( ( angle - 180 ) * PIOVER180 );
	cam.y = 0.0f;
	cam.z = dist * cos( ( angle - 180 ) * PIOVER180 );
	gluLookAt( cam.x, cam.y, cam.z,		0.0, 0.0, 0.0,		0.0, 1.0, 0.0 );

	textureManager.setTexture (0);
	engine.render( angle );
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// funzione di renderizzazione del frame
void update (void) {
	engine.update( frame_interval, &grav );
}
/* ---------------------------------------------------------------------------------------------------------------------- */
// init
bool init( void ) {
	cout << "---------- INIT SDL LIBRARY ----------" << endl;
	// init SDL
	if ( !SDL_Window.sdl_init( "Particle Engine", screen_width, screen_height, screen_bpp, fullscreen ) ) {
		cerr << "SDL_Init error: " << SDL_GetError() << endl;
		return false;
	}
	cout << "SDL init... OK" << endl;

	// setup open gl
	cout << "---------- INIT OPENGL LIBRARY ----------" << endl;
	SDL_Window.opengl_init();						// init opengl
	SDL_Window.resize_scene( 400.0f );		// resize window
	// setup light
	glLightfv (GL_LIGHT1, GL_AMBIENT, LightAmbient);		// setup for ambient light
	glLightfv (GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// setup for difuse light
	glLightfv (GL_LIGHT1, GL_SPECULAR, LightSpecular);	// setup for specular light
	glLightfv (GL_LIGHT1, GL_POSITION, LightPosition);	// light position
	glEnable (GL_LIGHT1);
	glEnable (GL_LIGHTING);
	cout << "OpenGl init... OK" << endl;

	// setup texture
	if (!textureManager.loadTexture ("texture/particle.png", 0)) {
		cerr << "Error loading texture :(" << endl;
		exit (-1);
	}
	cout << "Loading textures... OK" << endl;

	// init grav
	grav.x = 0.0f;		grav.y = 0.0f;		grav.z = 0.0f;

	// setup particle or flock engine
	engine.setup();

	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// calcola il frame rate ed imposta le relative variabili
void calcFrameRate( void ) {
	static Uint32 last_second, last_frame, this_frame;			// variabili per contenere i tempi tra un frame e l'altro
	static float wait_time = 16.50;												// tempo di attesa con fps limitati
	static int render_num;															// mantiene il numero di frame renderizzati in un sec

	this_frame = SDL_GetTicks();												// memorizza il tempo di questo frame
	frame_interval = ( this_frame - last_frame ) * 0.001f;	// salva l'intervallo in secondi tra i 2 frame ( < 0 )
	// FPS LIMITATI CHIAMANDO SDL_Delay ()
	if ( limit_fps && ( frame_interval < wait_time ) )				// 16,66 = 1000 / 60
		SDL_Delay( int( wait_time - frame_interval ) );

	last_frame = this_frame;							// imposta il tempo dell'ultimo frame al tempo corrente
	render_num++;											// incrementa il numero di frame renderizzati in questo secondo

	// se sono passati 1000 millisecondi (= 1 sec) imposta gli fps e resetta
	if ( ( this_frame - last_second ) > 1000 ) {
		fps = render_num;									// salva il frame rate
		render_num = 0;									// riazzera il conteggio
		last_second = this_frame;						// memorizza l'ultimo reset

		if ( limit_fps ) {									// modifica il valore di wait per avere 60 fps
			if ( fps > 62 )		wait_time += 0.5f;
			if ( fps > 70 )		wait_time += 0.8f;
			if ( fps < 58 )	   wait_time -= 0.5f;
			if ( fps < 50 )	   wait_time -= 1.0f;
		}
	}

	// update screen
	glFlush();
	SDL_GL_SwapBuffers();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// funzione di gestione degli eventi
void eventManager( void ) {
	SDL_Event event;					// variabile SDL per gestione eventi
	Uint8 *keys;								// variabile controllo tastiera

	static bool space = false;

	//	EVENTS HANDLE
	while ( SDL_PollEvent( &event ) ) {
		if ( event.type == SDL_QUIT ) {
			done = true;
			return;
		}
	}


	//	KEYBOARD HANDLE
	keys = SDL_GetKeyState( NULL );

	// camera
	if (keys[SDLK_UP]) {									// UP
		dist -= 10.0f * frame_interval;
	}
	if (keys[SDLK_DOWN]) {							// DOWN
		dist += 10.0f * frame_interval;
	}
	if (keys[SDLK_LEFT]) {								// LEFT
		angle -= 25.0f * frame_interval;
	}
	if (keys[SDLK_RIGHT]) {							// RIGHT
		angle += 25.0f * frame_interval;
	}

	// gravity
	if (keys[SDLK_w]) {									// UP
		grav.y += 20.0f * frame_interval;
	}
	if (keys[SDLK_s]) {										// DOWN
		grav.y -= 20.0f * frame_interval;
	}
	if (keys[SDLK_d]) {										// LEFT
		grav.x += 20.0f * frame_interval;
	}
	if (keys[SDLK_a]) {										// RIGHT
		grav.x -= 20.0f * frame_interval;
	}
	if (keys[SDLK_q]) {										// FRONT
		grav.z += 20.0f * frame_interval;
	}
	if (keys[SDLK_z]) {										// BACK
		grav.z -= 20.0f * frame_interval;
	}
	if (keys[SDLK_x] || keys[SDLK_r]) {		// RESET
		grav.x = 0.0f;		grav.y = 0.0f;		grav.z = 0.0f;
	}

	// HELP
	if (keys[SDLK_h]) {										// HELP
		ShellExecute( NULL, "open", "help.txt", 0, 0, 1 );
	}

	// pause
	if (keys[SDLK_SPACE]) {							// SPACE
		if ( !space ) {
			space = true;
			pause = !pause;
		}
	}
	if (!keys[SDLK_SPACE]) {								// NOT SPACE
		space = false;
	}

	// quit
	if ( keys[ SDLK_ESCAPE ] )								// ESC
		done = true;
}
