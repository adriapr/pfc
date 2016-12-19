// Group.h: interface for the Group class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_GROUP_H__843D5BB3_64C9_4F4F_8C6B_3B7469FFD4FC__INCLUDED_)
#define AFX_GROUP_H__843D5BB3_64C9_4F4F_8C6B_3B7469FFD4FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../pfc.h"
#include "Character.h"

class World;
class Scene;

// - Estados ----------------
 #define GROUP_GO			1
 #define GROUP_FORM			2
 #define GROUP_FLEE			3
 #define GROUP_REGROUP		4

 #define GROUP_ENEMIE		5

// - Limites ----------------
 #define FORM_DISTANCE		3
 #define MAX_DISPERSION		12
 #define FLEE_PROPORTION	0.7
 #define REGROUP_PROPORTION	0.5


class Group  
{
public:

	Vector center;
	Vector compensedCenter;			// Centro ponderado

	float dispersion;				// distnacia media entre el centro y los individuos
	Vector checkPoints[100];		// objetivos intermedios hasta el objetivo en [0]
	int nCheckPoints;

	bool finalTarget;				// objetivo final o hay que recalcular al llegar?
	Vector waitingTarget;			// Almacenamos objetivo final

	int leaderCheckPoint;			// checkpoint mas avanzado del grupo
	float minSpeed;					// velocidad dl individuo mas lento

	Vector forward;					// direccion media
	float speed;					// velocidad media

	bool selected;

	Vector formAligment;			// Direccion para la formacion

	CList<Character,Character&> characters;

private:

//	Vector target;
	int state;


public:
	Group();
	Group(bool enemie);

	virtual ~Group();
	Group& operator =(const Group& myGroup);

	void AddCharacter(Character* ch);
	void SetTarget(Vector newTarget, Scene* scene);
	void SetState(int newState);
	void SetDirection(Vector newTarget);
	void SetCheckPoints(CList <Vector, Vector&> *cp);

	 int GetState();
  Vector GetTarget();

	void Update(unsigned long elapsedTime, World* world);
	void UpdateStatus();
	void RecalcNewGroupState(Scene* scene);

// 	void PrepareRegroup();
//	void PrepareForm();

   float GetFleeingProportion();
   float GetRegroupingProportion();
	bool AllCharactersGrouped();

   // Metodos para los enemigos
   void SetNewEnemieTarget(Scene* scene);

};

#endif // !defined(AFX_GROUP_H__843D5BB3_64C9_4F4F_8C6B_3B7469FFD4FC__INCLUDED_)
