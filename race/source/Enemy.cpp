/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Enemy.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Enemy::Enemy (void) : Object3D () {
	x_speed = 0.0;	z_speed = 0.0;

	angle.x = angle.y = angle.z = 0.0f;
	angle_dest.x = angle_dest.y = angle_dest.z = 0.0f;

	active = false;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Enemy::~Enemy () {
	// disattivo il giocatore
	active = false;

	// libera la memoria
	glDeleteTextures (1, shaderTexture);
	if (color)			delete color;
	if (mesh)			delete mesh;

	if (mutex)		SDL_DestroyMutex(mutex);			// close mutex
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Enemy::render (float frame_interval, float line_width) {
	glPushMatrix ();

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return;	}

	// calcola l'angolo
	calcAngle (frame_interval);
	// translate
	glTranslatef (x, y, z);
	glRotatef (-angle.y, 0.0, 1.0, 0.0);
	// calculate angle
	calcAngle (frame_interval);
	glRotatef (angle.x, 1.0, 0.0, 0.0);
	glRotatef (angle.z + turn_angle, 0.0, 0.0, 1.0);
	// draw
	draw (line_width);

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return;	}

	glPopMatrix();
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// calculate angle
void Enemy::calcAngle (float frame_interval) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return;	}

	// calcolo angoli di inclinazione
	// ANGLE X
	float x_angle_1 = 0.0f, z_angle_1 = 0.0f;
	float x_angle_2 = 0.0f, z_angle_2 = 0.0f;
	if (angle.y > 0.0)
		x_angle_1 = ((90.0-(fabs(90.0-angle.y)))/90.0)*((angle_dest.x));
	else
		x_angle_1 = -((90.0-(fabs(-90.0-angle.y)))/90.0)*((angle_dest.x));
	if (angle.y > 0.0 && angle.y < 90.0)
		z_angle_1 = angle_dest.x - x_angle_1;
	else
		if (angle.y > 90.0)
			z_angle_1 = -(angle_dest.x - x_angle_1);
		else
			if (angle.y > -90.0)
				z_angle_1 = angle_dest.x + x_angle_1;
			else
				z_angle_1 = -(angle_dest.x + x_angle_1);
	// ANGLE Z
	float y_temp = angle.y - 90.0;
	if (y_temp > 0.0)
		x_angle_2 = ((90.0-(fabs(90.0-y_temp)))/90.0)*((angle_dest.z));
	else
		x_angle_2 = -((90.0-(fabs(-90.0-y_temp)))/90.0)*((angle_dest.z));
	if (angle.y > 0.0 && angle.y < 90.0)
		z_angle_2 = angle_dest.z + x_angle_2;
	else
		if (angle.y > 90.0)
			z_angle_2 = angle_dest.z - x_angle_2;
		else
			if (angle.y > -90.0)
				z_angle_2 = -angle_dest.z - x_angle_2;
			else
				z_angle_2 = -angle_dest.z + x_angle_2;

	float x_dist = angle.x - (x_angle_1+x_angle_2);
	float z_dist = angle.z - (z_angle_1+z_angle_2);
	if (x_dist < -1.0)
		angle.x += (((fabs(x_angle_1)+fabs(x_angle_2)) * 2.0f) - ROT_SPEED * x_dist) * frame_interval;
	if (x_dist > 1.0)
		angle.x -= (((fabs(x_angle_1)+fabs(x_angle_2)) * 2.0f) + ROT_SPEED * x_dist) * frame_interval;
	if (z_dist < -1.0)
		angle.z += (((fabs(z_angle_1)+fabs(z_angle_2)) * 2.0f) - ROT_SPEED * z_dist) * frame_interval;
	if (z_dist > 1.0)
		angle.z -= (((fabs(z_angle_1)+fabs(z_angle_2)) * 2.0f) + ROT_SPEED * z_dist) * frame_interval;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// inizializza l'oggetto
bool Enemy::init (const char *model, const char *shader, int m_num) {
	// init mutex
	mutex = SDL_CreateMutex ();

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return false;	}

	// load shader
	printf ("\tLoading shader file (%s)...", shader);
	if (!loadShader (shader))
		return false;
	printf ("OK\n");
	// load model
	printf ("\tLoading model file (%s)...", model);
	if (!loadModel (model))
		return false;
	printf ("OK\n");

	// salva il numero del modello
	model_number = m_num;

	// reset position
	x = 0.0;
	y = 1.0;
	z = 0.0;

	x_speed = 0.0;	z_speed = 0.0;

	angle.x = angle.y = angle.z = 0.0f;
	angle_dest.x = angle_dest.y = angle_dest.z = 0.0f;

	// setto il giocatore come attivo
	active = true;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return false;	}

	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// resetta l'oggetto
void Enemy::clear (void) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return;	}

	// disattivo il giocatore
	active = false;

	// libera la memoria
	glDeleteTextures (1, shaderTexture);
	if (color)			delete color;
	if (mesh)			delete mesh;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return;	}

	if (mutex)		SDL_DestroyMutex(mutex);			// close mutex
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// posizione oggetto
void Enemy::getPosition (float *xp, float *yp, float *zp) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return;	}

	*xp = x;
	*yp = y;
	*zp = z;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// posizione oggetto
void Enemy::setPosition (float xp, float yp, float zp) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return;	}

	x = xp;
	y = yp;
	z = zp;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna l'angolo su y
float Enemy::getAngle (void) {
	float angle_temp;

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return 0;	}

	angle_temp = angle.y;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return 0;	}

	return angle_temp;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// setta l'angolo su y
void Enemy::setAngle (float a) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return;	}

	angle.y = a;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna l'inclinazione corrente
float Enemy::getTurnAngle (void) {
	float angle_temp;

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return 0;	}

	angle_temp = turn_angle;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return 0;	}

	return angle_temp;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// setta l'inclinazione
void Enemy::setTurnAngle (float turn) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("ENEMY: Errore LOCK mutex :(\n");	return;	}

	turn_angle = turn;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("ENEMY: Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna il numero del file del modello
int Enemy::getModelNumber (void) {
	int temp;

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return 0;	}

	temp = model_number;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return 0;	}

	return temp;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna se l'oggeto è attivo
bool Enemy::isActive (void) {
	return active;
}
