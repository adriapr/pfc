// Enemy.cpp: implementation of the Enemy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pfc.h"
#include "Enemy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Enemy::Enemy()
{
	Initialize();
}

Enemy::~Enemy()
{

}

void Enemy::Initialize()
{
	this->maxSpeed		= 5;
	this->maxTurnSpeed	= 0.02;
	this->stopSpeed		= 0.1;

	this->maxAccForce	= 1;
	this->maxBrakeForce	= 3;

	this->up			= Vector (0.0, 0.0, 1.0);

	this->position		= Vector (rand()%20, -rand()%20 - 100, 0.0);
}

Enemy& Enemy::operator =(const Enemy& myEnemy)
{

	if (&myEnemy == this) 
		return *this;
	
	this->position			= myEnemy.position;

	this->speed				= myEnemy.speed;
	this->forward			= myEnemy.forward;
	this->right				= myEnemy.right;
	this->up				= myEnemy.up;

	this->maxSpeed			= myEnemy.maxSpeed;
	this->maxTurnSpeed		= myEnemy.maxTurnSpeed;
	this->stopSpeed			= myEnemy.stopSpeed;

	this->maxAccForce		= myEnemy.maxAccForce;
	this->maxBrakeForce		= myEnemy.maxBrakeForce;

	return *this;
}

void Enemy::SetForward( Vector f )
{
	if (f.GetLength() > 0)
	{
		f = f.GetNormalized();
		this->forward = f;
		this->right = f.CrossProduct(&this->up);
	}
}