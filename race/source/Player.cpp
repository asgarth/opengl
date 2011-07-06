/************************************************************
		Project: R.A.C.E.

		License: GPL
************************************************************/

#include "Player.h"

/* ---------------------------------------------------------------------------------------------------------------------- */

Player::Player (void) : Object3D () {
	x_speed = 0.0;	z_speed = 0.0;

	angle.x = angle.y = angle.z = 0.0f;
	angle_dest.x = angle_dest.y = angle_dest.z = 0.0f;
}

/* ---------------------------------------------------------------------------------------------------------------------- */

Player::~Player () {
	// libera la memoria
	glDeleteTextures (1, shaderTexture);
	if (color)			delete color;
	if (mesh)			delete mesh;

	if (mutex)		SDL_DestroyMutex(mutex);			// close mutex
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// render
void Player::render (float frame_interval) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}

	// calculate angle
	calcAngle (frame_interval);
	glRotatef (angle.x, 1.0, 0.0, 0.0);
	glRotatef (angle.z + turn_angle, 0.0, 0.0, 1.0);
	// draw
	draw (OUTLINE_WIDTH);

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// aggiorna la posizione
void Player::updatePosition (float frame_interval, float height) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}

	// calculate speed
	if (x_speed > 0.0) {
		x_speed = x_speed / (1.0 + ATTRITO * frame_interval);
		if (x_speed < 0.005)		x_speed = 0.0;
	}
	if (x_speed < 0.0) {
		x_speed = x_speed / (1.0 + ATTRITO * frame_interval);
		if (x_speed > -0.005)		x_speed = 0.0;
	}
	if (z_speed > 0.0) {
		z_speed = z_speed / (1.0 + ATTRITO * frame_interval);
		if (z_speed < 0.005)		z_speed = 0.0;
	}
	if (z_speed < 0.0) {
		z_speed = z_speed / (1.0 + ATTRITO * frame_interval);
		if (z_speed > -0.005)		z_speed = 0.0;
	}

	// update position
	x += x_speed * frame_interval;
	z -= z_speed * frame_interval;

	// update speed value
	speed = sqrt(x_speed*x_speed+z_speed*z_speed);

	// update height
	dest_height = height + HEIGHT_OFFSET;
	if (y < dest_height) {
		y += (5.0+speed/10.0)*(dest_height-y)*frame_interval;
		if (y > dest_height)
			y = dest_height;
	}
	if (y > dest_height)  {
		y -= (5.0+speed/10.0)*(y-dest_height)*frame_interval;
		if (y < dest_height)
			y = dest_height;
	}

	// set Y angle between -180.0 and 180.0
	if (angle.y > 180.0)
		angle.y -= 360.0;
	if (angle.y < -180.0)
		angle.y += 360.0;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// calculate angle
void Player::calcAngle (float frame_interval) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}

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
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// inizializza l'oggetto
bool Player::init (const char *model, const char *shader, int m_num) {
	// init mutex
	mutex = SDL_CreateMutex ();

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

	return true;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// resetta l'oggetto
void Player::clear (void) {
	// libera la memoria
	glDeleteTextures (1, shaderTexture);
	if (color)			delete color;
	if (mesh)			delete mesh;

	if (mutex)		SDL_DestroyMutex(mutex);			// close mutex
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// posizione oggetto
void Player::getPosition (float *xp, float *yp, float *zp) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}

	*xp = x;
	*yp = y;
	*zp = z;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna l'angolo su y
float Player::getAngle (void) {
	float angle_temp;

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return 0;	}

	angle_temp = angle.y;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return 0;	}

	return angle_temp;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// setta l'angolo su y
void Player::setAngle (float a) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}

	angle.y = a;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna l'inclinazione corrente
float Player::getTurnAngle (void) {
	float angle_temp;

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return 0;	}

	angle_temp = turn_angle;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return 0;	}

	return angle_temp;
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// setta l'inclinazione
void Player::setTurnAngle (float turn) {
	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return;	}

	turn_angle = turn;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return;	}
}

/* ---------------------------------------------------------------------------------------------------------------------- */
// ritorna il numero del file del modello
int Player::getModelNumber (void) {
	int temp;

	// lock mutex
	if (SDL_mutexP(mutex) == -1) {printf("Errore LOCK mutex :(\n");	return 0;	}

	temp = model_number;

	// unlock mutex
	if (SDL_mutexV(mutex) == -1){	printf("Errore UNLOCK mutex :(\n");	return 0;	}

	return temp;
}
