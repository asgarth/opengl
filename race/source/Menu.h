/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef MENU_H
#define MENU_H

#include "includes.h"
#include "Texture_Manager.h"
#include "Text.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Sky.h"

class Menu {
	private:
		Texture_Manager *tm;							// texture manager
		Text *font;										// font manager
		Terrain *ground;								// ground
		Frustum *frustum;
		Sky *sky;

		int index, join_index, opt_index;
		int opt_menu_model;
		int sub_menu;
		int width, height;
		bool close_menu;
		float frame_interval;
		char *ip_string, *port_string;
		bool ip_stop_writing, port_stop_writing;

		void draw_black_screen (void);			// draw display black background
		void draw_main (void);						// draw main screen
		void draw_join (void);						// draw join screen option
		void draw_select_map (void);				// draw map selection screen
		void draw_option (void);					// draw option screen
		void render_terrain (void);				// render background terrain
		void eventManager (void);					// gestione input
		void calcFrameRate (void);					// calc frame rate and limit fps

	public:
		Menu (Texture_Manager *, Text *, Terrain *, Frustum *, Sky *);
		~Menu(void);

		void init (int, int);						// init
		void clear (void);							// clear
		void show (void);								// show menu
		int getSelection (void);					// return selection
		char* getAddress (void);					// return ip address
		int getPort (void);							// return server port
		int getModel (void);							// return selected model
};

#endif
