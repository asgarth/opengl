//-------------		file			Genesis_Camera.cpp		-----------------------------------------------------------------------------------

#include "Genesis_Camera.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// ritorna il vettore perpendicolare ai due vettori passati

CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;

	// cross product (equazione non commutativa)
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	return vNormal;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// ritorna la magnitudine di un vettore

float Magnitude(CVector3 vNormal)
{
	// magnitude = sqrt(V.x^2 + V.y^2 + V.z^2)
	return (float) sqrt(	(vNormal.x * vNormal.x) +
						(vNormal.y * vNormal.y) +
						(vNormal.z * vNormal.z) );
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// ritorna un vettore normalizzato (esattamente di lunghezza 1)

CVector3 Normalize(CVector3 vVector)
{
	float magnitude = Magnitude(vVector);
	vVector = vVector / magnitude;

	return vVector;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
// costruttore

Genesis_Camera::Genesis_Camera()
{
	m_vPosition		= CVector3(0.0, 0.0, 0.0);		// positione iniziale
	m_vView 		= CVector3(0.0, 1.0, 0.5);		// visuale iniziale
	m_vUpVector   	= CVector3(0.0, 0.0, 1.0);		// standard up vector
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

Genesis_Camera::~Genesis_Camera() {	}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::PositionCamera (	float positionX, float positionY, float positionZ,
				  		     			float viewX,     float viewY,     float viewZ,
									float upVectorX, float upVectorY, float upVectorZ)
{
	m_vPosition		= CVector3(positionX, positionY, positionZ);
	m_vView		= CVector3(viewX, viewY, viewZ);
	m_vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::Mouse_Controller(int screen_width, int screen_height)
{
	POINT mousePos;							// struttura window per X eY
	int middleX = screen_width  >> 1;				// metà larghezza
	int middleY = screen_height >> 1;				// metà altezza
	float angleY = 0.0f;							// direction per su e giù
	float angleZ = 0.0f;							// valore da rotare sull'asse Y
	static float currentRotX = 0.0f;

	// corrente posizione X eY del mouse
	GetCursorPos(&mousePos);

	// se il cursore è nel centro non fare niente
	if ((mousePos.x == middleX) && (mousePos.y == middleY))
		return;

	// setta la posizione al centro dello schermo
	SetCursorPos(middleX, middleY);

	// controlla la direzione del movimento del mouse e la riduce dividendola
	angleY = (float)( (middleX - mousePos.x) ) / 1000.0f;
	angleZ = (float)( (middleY - mousePos.y) ) / 1000.0f;

	// controlla se l'angolo non sia troppo grande per evitare che faccia un giro di 360 gradi
	currentRotX -= angleZ;
	if(currentRotX > 1.0f)
		currentRotX = 1.0f;
	else
		if(currentRotX < -1.0f)
			currentRotX = -1.0f;
		else
		{
			// usa l'asse perpendicolare al vettore view e al vettore up per la rotazione su e giù
			CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);

			// Rotate around our perpendicular axis and along the y-axis
			RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
			RotateView(angleY, 0, 1, 0);
		}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	// vettore della visuale
	CVector3 vView = m_vView - m_vPosition;

	// calcola sen e cos dell'angolo per non doverlo calcolare ogni volta
	float cosTheta = (float) cos (angle);
	float senTheta = (float) sin (angle);

	// nuova posizione x per il punto ruotato
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * senTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * senTheta)	* vView.z;

	// nuova posizione y per il punto ruotato
	vNewView.y  = ((1 - cosTheta) * x * y + z * senTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * senTheta)	* vView.z;

	// nuova posizione z per il punto ruotato
	vNewView.z  = ((1 - cosTheta) * x * z - y * senTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * senTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	// aggiunge alla nuova posizione ruotata la posizione in cui ci troviamo
	m_vView = m_vPosition + vNewView;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::StrafeCamera(float speed)
{
	// aggiunge il vettore strafe alla nostra posizione
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	// aggiunge il vettore strafe alla visuale
	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::MoveCamera(float speed)
{
	// vettore visuale corrente normalizzato
	CVector3 vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	// spostamento in quella direzione
	m_vPosition.x += vVector.x * speed;
	m_vPosition.z += vVector.z * speed;
	m_vView.x += vVector.x * speed;
	m_vView.z += vVector.z * speed;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::MoveUp(float speed)
{
	// spostamento in alto o in basso della posizione e della visuale
	m_vPosition.y += speed;
	m_vView.y += speed;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::Update(int screen_width, int screen_height)
{
	// calcola il vettore strafe per non doverlo calcolare ogni volta nelle altre funzioni
	m_vStrafe = Cross (m_vView - m_vPosition, m_vUpVector);
	m_vStrafe = Normalize (m_vStrafe);

	Mouse_Controller (screen_width, screen_height);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

void Genesis_Camera::Look()
{
	gluLookAt (	m_vPosition.x, m_vPosition.y, m_vPosition.z,
				m_vView.x,	 m_vView.y,     m_vView.z,
				m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------
