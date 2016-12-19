// Enemy.h: interface for the Enemy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENEMY_H__C9CF7167_C3A8_4E26_8B5E_70F8F18F1B55__INCLUDED_)
#define AFX_ENEMY_H__C9CF7167_C3A8_4E26_8B5E_70F8F18F1B55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../pfc.h"

class Enemy  
{

public:

	Vector position;

	double speed;					// velocidad instantanea
	Vector forward;					// 3 vectores de orientacion
	Vector right;
	Vector up;

private:

	// - Propiedades -
	double maxAccForce;				// acceleracion
	double maxBrakeForce;			// frenada
	double maxSpeed;				// velocidad maxima

	double maxTurnSpeed;			// velocidad de rotacion maxima
	double stopSpeed;				// velocidad q se considera parado

public:

	Enemy();
	virtual ~Enemy();

	void Initialize();
	void SetForward( Vector f );


	Enemy& operator =(const Enemy& myEnemy);
};

#endif // !defined(AFX_ENEMY_H__C9CF7167_C3A8_4E26_8B5E_70F8F18F1B55__INCLUDED_)
