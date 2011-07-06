//-------------		file			Genesis_Terrain.cpp		-----------------------------------------------------------------------------------

#include "Genesis_Terrain.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Genesis_Terrain::Genesis_Terrain (char *Filename)
{
	Load_Raw_File (Filename, MAP_SIZE * MAP_SIZE);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Genesis_Terrain::~Genesis_Terrain () {	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// Funzione che trova le altezze del nostro array

int Genesis_Terrain::Height(int X, int Y)
{
	int x = X % MAP_SIZE;					// controlla il valore x
	int y = Y % MAP_SIZE;					// controlla il valore x

	return (int)(HeightMap[x + (y * MAP_SIZE)] / 5);		// ritorna l'altezza trovata nell'array
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Terrain::Set_Texture_Coord(float x, float z)
{
	glTexCoord2f((float)x / ((float)MAP_SIZE / 10.0), - (float)z / ((float)MAP_SIZE / 10.0));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Terrain::RenderMap ()
{
	int X = 0, Y = 0;						// variabili per scorrere l'array
	int x, y, z;							// variabili temporanee
	bool render_sides = false;

	glBegin( GL_TRIANGLE_STRIP );
		for ( X = 0; X <= MAP_SIZE; X += STEP_SIZE ) {
			// controlla in che verso renderizzare la colonna
			if(render_sides)
			{
				// renderizza una colonna di terreno per la X corrente iniziando dalla fine
				for ( Y = MAP_SIZE; Y >= 0; Y -= STEP_SIZE )
				{
					// vertice basso - sinistra
					x = X;
					y = Height(X, Y );
					z = Y;
					Set_Texture_Coord( (float)x, (float)z );
					glVertex3i(x, y, z);

					// vertice basso - destra
					x = X + STEP_SIZE;
					y = Height(X + STEP_SIZE, Y );
					z = Y;
					Set_Texture_Coord( (float)x, (float)z );
					glVertex3i(x, y, z);
				}
			}
			else
			{
				// renderizza una colonna di terreno per la X corrente iniziando dall'inizio
				for ( Y = 0; Y <= MAP_SIZE; Y += STEP_SIZE ) {
					// vertice basso - destra
					x = X + STEP_SIZE;
					y = Height(X + STEP_SIZE, Y );
					z = Y;
					Set_Texture_Coord( (float)x, (float)z );
					glVertex3i(x, y, z);

					// certice basso - sinistra
					x = X;
					y = Height(X, Y );
					z = Y;
					Set_Texture_Coord( (float)x, (float)z );
					glVertex3i(x, y, z);
				}
			}

			// inverte la direzione in cui renderizzare le colonne
			render_sides = !render_sides;
		}
	glEnd();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Terrain::Load_Raw_File(LPSTR strName, int nSize)
{
	FILE *File = NULL;

	File = fopen( strName, "rb" );				// apre il file in modalità read/binary

	if (File == NULL)							// controlla se il file è stato dato ed esiste
	{
		// visualizza un messaggio di errore ed esce
		MessageBox(NULL, "Impossibile costruire il terreno", "Genesis - ERROR", MB_OK);
		return;
	}

	// HeighMap = array che contiene i risultati delle altezze
	// 1 = numero di byte da leggere per volta
	// nSize = numero di byte da leggere (nSize = altezza_mappa * larghezza_mappa)
	// File = file da cui leggere le informazioni
	fread (HeightMap, 1, nSize, File );			// legge i dati dal file e li mette nell'array delle altezze

	int result = ferror( File );					// controlla se tutto è andato bene

	// controlla errori
	if (result)
		MessageBox(NULL, "Impossibile trovare i dati per il terreno", "Genesis - ERROR", MB_OK);

	fclose(File);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
