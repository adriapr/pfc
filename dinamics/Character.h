// Character.h: interface for the Character class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARACTER_H__D5263196_AE20_4EA9_9AC7_7A342DC9576B__INCLUDED_)
#define AFX_CHARACTER_H__D5263196_AE20_4EA9_9AC7_7A342DC9576B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../pfc.h"
#include "Enemy.h"


class Group;	// forward declaration
class Scene;
class World;

// - Estados -------------
 #define CHARACTER_GO		1
 #define CHARACTER_FORM		2
 #define CHARACTER_FLEE		3
 #define CHARACTER_REGROUP	4

 #define CHARACTER_ENEMIE	5
// -----------------------

#define MAX_CHECKPOINTS 100;

//#define CHARACTER_TIME_WAITING_AFTER_COLLISION 80 // milisegundos despues de deteccion

class Character  
{
public:

//	Vector target;

	int state;

	Vector position;

	Vector regroupCPs[100];			// objetivos intermedios para la reagrupacion
	int regroupCP;					// indice de CP
	bool regroupFinal;				// True: es objetivo final || False: habra q recalcular camino final

	int checkPoint;					// indide para el camino del grupo

	float speed;					// velocidad instantanea
	Vector forward;					// 3 vectores de orientacion
	Vector right;
	Vector up;

	Vector toTargetForce;			// Fuerza hacia el objetivo
	Vector toGroupCenterForce;		// Fuerza hacia el centro del grupo
	Vector collisionForce;			// Fuerza de repusion de las paredes y otros objetos

	float fear;						// miedo actual

	// - Propiedades -
	float maxSpeed;					// velocidad maxima
	float stopSpeed;				// velocidad q se considera parado

	float maxAccForce;				// acceleracion
	float maxBrakeForce;			// frenada
	float maxTurnForce;				// velocidad de rotacion maxima

	float maxFear;					// Nivel maximo de miedo

public:

	Character();
	Character(float chMaxSpeed, float chMaxAccForce, float chMacBrakeForce,
			  float chMaxTurnForce, float chStopSpeed, float chMaxFear);
	
	Character(bool enemie);	// Constructor para el enemigo

	virtual ~Character();
	Character& operator =(const Character& myCharacter);

	void Initialize();
	void SetForward( Vector f );

	void Update(unsigned long elapsedTime, World* world, Group* pGroup);
	void RecalcNewCharacterState( Group *gr, Scene* scene );
	void RecalcFear(const unsigned long elapsedTime, CList<Group, Group&>* enemies, CList<Group, Group&>* groups);
	void PropagateFear(Group * gr);

	Vector GetCharactersSeparationForce(const unsigned long elapsedTime, World* world);
	Vector GetRepulsion(const unsigned long elapsedTime, World* world);

	void SetNewTarget(int nCP);

	void Go(const unsigned long elapsedTime, Group* pGroup, World* world);
	void Form(const unsigned long elapsedTime, Group* pGroup, World* world);
	void Regroup(const unsigned long elapsedTime, Group* pGroup, World* world);
	void Escape(const unsigned long elapsedTime, Group* pGroup, World* world);

	void PrepareRegroup(Group *gr, Scene *scene);
	void SetRegroupCheckPoints(CList <Vector, Vector&> *cp);

	void FormAlign(const unsigned long elapsedTime, Group* pGroup);
	void DoMove(float turn, float acc, const unsigned long elapsedTime, Vector* targetPoint, Scene* scene, Group* pGroup);
	void CheckPointControl(Group* pGroup, Scene* scene);
};

#endif // !defined(AFX_CHARACTER_H__D5263196_AE20_4EA9_9AC7_7A342DC9576B__INCLUDED_)
