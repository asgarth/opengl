//-------------		file			Genesis_Terrain.h		-----------------------------------------------------------------------------------

#ifndef Genesis_Terrain_H
#define Genesis_Terrain_H

#include "Genesis_Includes.h"

#define MAP_SIZE		1024				// dimensione del file .RAW della displacement map
#define STEP_SIZE		16					// lato di ogni quadrato disegnato

class Genesis_Terrain
{
	private:
		BYTE HeightMap[MAP_SIZE * MAP_SIZE];				// contiene le altezze con cui costruire il terreno

		void Set_Texture_Coord(float, float);
		void Load_Raw_File(LPSTR , int);

	public:
		Genesis_Terrain (char *);
		~Genesis_Terrain ();
		int Height(int, int);
		void RenderMap();
};

#endif