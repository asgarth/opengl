//-------------		file			Genesis_Camera.h		-----------------------------------------------------------------------------------

#ifndef Genesis_Camera_H
#define Genesis_Camera_H

#include "Genesis_Includes.h"

class Genesis_Camera
{
	private:
		CVector3 m_vPosition;						// posizione camera
		CVector3 m_vView;							// visuale della camera
		CVector3 m_vUpVector;						// camera up vector
		CVector3 m_vStrafe;						// camera strafe vector

	public:
		float total_x_rot;							// variabile per controllare l'angolo sull'asse x
		char* frame_rate;

		Genesis_Camera ();
		~Genesis_Camera ();
		// cambia i parametri della camera (usata per inizializzazione)
		void PositionCamera (	float positionX,	float positionY, 	float positionZ,
			 		    		float viewX,     	float viewY,     	float viewZ,
							float upVectorX,float upVectorY, float upVectorZ);
		// ruota la telecamera intorno la posizione
		void RotateView (float angle, float X, float Y, float Z);
		// controlla il movimento del mouse
		void Mouse_Controller (int screen_width, int screen_height);
		// ruota la camera intorno ad un punto
		void RotateAroundPoint (CVector3 vCenter, float X, float Y, float Z);
		// funzione per lo strafe della camera left e right secondo speed (+/-)
		void StrafeCamera (float speed);
		// muove la camera
		void MoveCamera (float speed);
		// muove la camera in alto o in basso
		void MoveUp (float speed);

		// aggiorna la visuale della camera e gli altri dati (chiamare ogni frame)
		void Update(int screen_width, int screen_height);
		// richiama gluLookAt
		void Look ();

		// funzioni per accedere ai dati privati della classe
		CVector3 Position() 	{	return m_vPosition;		}
		CVector3 View()		{	return m_vView;			}
		CVector3 UpVector() {	return m_vUpVector;		}
		CVector3 Strafe()	{	return m_vStrafe;		}
};

#endif