/*--------------------------------------------------		Code_String.cpp		----------------------------------------------------------*/

#include "Code_String.h"

/*---------------------------------------------------------------------------------------------------------------------------------------*/

Code_String::Code_String ()
{
	scr_w = 800;
	scr_h = 600;
	Setup_Code_Text ();
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

Code_String::~Code_String () {	}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Code_String::Get_String (String_Code *temp, int string_num, bool vert)
{
	if (string_num > CODE_LENGTH - 1)
		string_num = CODE_LENGTH - 1;
	delete (temp -> string);
	temp -> string = (char*) calloc (strlen (codez_text[string_num]) + 1, sizeof (char));
	copystr (temp -> string, codez_text[string_num]);
	if (vert) {
		temp -> y_pos = float(rand()%35) + scr_h;
		temp -> x_pos = float(rand()%(scr_w << 1)) / 2.0f;
	}
	else {
		temp -> x_pos = float(rand()%35) + scr_w;
		temp -> y_pos = float(rand()%(scr_h << 1)) / 2.0f;
	}
	temp -> size = rand()%4 + 1;
	switch (temp -> size) {
		case 1:	temp -> x_vel = float(rand()%50) - 120.0f;
					break;
		case 2:	temp -> x_vel = float(rand()%70) - 180.0f;
					break;
		case 3:	temp -> x_vel = float(rand()%75) - 240.0f;
					break;
		case 4:	temp -> x_vel = float(rand()%80) - 300.0f;
					break;
		default:	temp -> x_vel = float(rand()%70) - 210.0f;
					break;
	}
	temp -> r = float(rand()%70) / 70.0f + 0.3f;
	temp -> g = float(rand()%70) / 70.0f + 0.3f;
	temp -> b = float(rand()%70) / 70.0f + 0.3f;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Code_String::Setup_Code_Text (void)
{
	char oneline[255] = "";			// variabile temporanea per la memorizzazione delle stringhe
	int length;
	FILE *filein = fopen ("CodeX_Saver.txt", "rt");
	// legge e memorizza le righe di codice
	for (int loop = 0; loop < CODE_LENGTH - 1; loop++) {
		readstr (filein, oneline);
		length = strlen (oneline);
		codez_text[loop] = (char*) calloc (length + 1, sizeof (char));
		copystr (codez_text[loop], oneline);
	}
	fclose (filein);				// chiude il file
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Code_String::Dealloc_Text (void)
{
	for (int loop = 0; loop < CODE_LENGTH - 1; loop++)
		delete (codez_text[loop]);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Code_String::readstr (FILE *filename, char *string)
{
	fgets (string, 255, filename);
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Code_String::copystr (char* dest, char* source)
{
	while ( *source != '\n' && *source != '\0') {
		if (*source != '\t')
			*dest = *source;
		dest ++;
		source ++;
	}
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/

void Code_String::Set_Resolution (int sw, int sh)
{
	scr_w = sw;
	scr_h = sh;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
