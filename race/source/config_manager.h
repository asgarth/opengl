/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

inline bool loadConfig (int *w, int *h, int *bpp, bool *f, bool *limit, int *server_port) {
	char readline[255];
	FILE *in;
	int temp_f, temp_limit;

	// open file
	if (!(in = fopen ("config.txt", "rt")))
		return false;

	// check file header
	fgets (readline, 256, in);
	if (strncmp (readline, "Project: R.A.C.E. - Config File", 30) != 0) {
		fclose (in);								// close
		return false;
	}

	// read display options
	fgets (readline, 256, in);
	fgets (readline, 256, in);	sscanf (readline, "screen_width = %d", w);
	fgets (readline, 256, in);	sscanf (readline, "screen_height = %d", h);
	fgets (readline, 256, in);	sscanf (readline, "bpp = %d", bpp);
	fgets (readline, 256, in);	sscanf (readline, "fullscreen = %d", &temp_f);
	fgets (readline, 256, in);	sscanf (readline, "limit_fps = %d", &temp_limit);
	fgets (readline, 256, in);	sscanf (readline, "limit_fps = %d", &temp_limit);
	fgets (readline, 256, in);
	fgets (readline, 256, in);	sscanf (readline, "server_port = %d", server_port);

	if (temp_f == 1)			*f = true;
	else							*f = false;

	if (temp_limit == 1)		*limit = true;
	else							*limit = false;

	fclose (in);
	return true;
}

inline bool writeConfig (int w, int h, int bpp, bool f, bool limit, int server_port) {
	FILE *out;
	int temp_f, temp_limit;

	// convert parameter
	if (f)		temp_f = 1;
	else			temp_f = 0;

	if (limit)	temp_limit = 1;
	else			temp_limit = 0;

	if (!(out = fopen ("config.txt", "wt")))
		return false;

	// write options
	fprintf (out, "Project: R.A.C.E. - Config File\n");
	fprintf (out, "\n");
   fprintf (out, "screen_width = %d\n", w);
   fprintf (out, "screen_height = %d\n", h);
   fprintf (out, "bpp = %d\n", bpp);
   fprintf (out, "fullscreen = %d\n", temp_f);
   fprintf (out, "limit_fps = %d\n", temp_limit);
   fprintf (out, "\n");
   fprintf (out, "server_port = %d\n", server_port);


   fclose (out);
	return true;
}

#endif
