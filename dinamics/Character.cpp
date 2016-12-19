// Character.cpp: implementation of the Character class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "../pfc.h"
#include "Character.h"

//#include "Enemy.h"
#include "Group.h"
#include "Scene.h"
#include "World.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Character::Character(float chMaxSpeed, float chMaxAccForce, float chMaxBrakeForce,
					 float chMaxTurnForce, float chStopSpeed, float chMaxFear)
{
	this->maxSpeed		= chMaxSpeed;
	this->stopSpeed		= chStopSpeed;

	this->maxAccForce	= chMaxAccForce;
	this->maxBrakeForce	= chMaxBrakeForce;
	this->maxTurnForce	= chMaxTurnForce;

	this->maxFear		= chMaxFear;

	Initialize();
}

Character::Character()
{
	Initialize();
}

// Constructor del enemigo
Character::Character(bool enemie)
{
	Initialize();

	this->maxSpeed		= 4;
	this->stopSpeed		= 0.1;

	this->maxAccForce	= 1.5;
	this->maxBrakeForce	= 2;
	this->maxTurnForce	= 0.1;

	this->maxFear		= 1;

	this->SetForward( Vector(1, 1, 0) );

	this->state			= CHARACTER_ENEMIE;
}

Character::~Character()
{

}

void Character::Initialize()
{
//	this->maxFear		= 10;
//	this->maxSpeed		= 5;
//	this->maxTurnSpeed	= 0.02;
//	this->stopSpeed		= 0.1;

//	this->maxAccForce	= 1;
//	this->maxBrakeForce	= 3;

	this->speed			= 0;
	this->fear			= 0;
	this->up			= Vector (0.0, 0.0, 1.0);
	this->state			= CHARACTER_FORM;

	this->checkPoint	= 0;
	this->regroupCP		= 0;
}

Character& Character::operator =(const Character& myCharacter)
{

	if (&myCharacter == this) 
		return *this;
	
	this->position			= myCharacter.position;

	this->checkPoint		= myCharacter.checkPoint;

	this->regroupCP			= myCharacter.regroupCP;
	for (int i=0; i< 100 ; i++)
		this->regroupCPs[i] = myCharacter.regroupCPs[i];

	this->regroupFinal		= myCharacter.regroupFinal;

	this->state				= myCharacter.state;
	this->fear				= myCharacter.fear;

	this->speed				= myCharacter.speed;
	this->forward			= myCharacter.forward;
	this->right				= myCharacter.right;
	this->up				= myCharacter.up;

	this->maxSpeed			= myCharacter.maxSpeed;
	this->maxTurnForce		= myCharacter.maxTurnForce;
	this->stopSpeed			= myCharacter.stopSpeed;

	this->maxAccForce		= myCharacter.maxAccForce;
	this->maxBrakeForce		= myCharacter.maxBrakeForce;

	this->maxFear			= myCharacter.maxFear;
	
	return *this;
}

void Character::SetForward( Vector f )
{
	if (f.GetLength() > 0)
	{
		f = f.GetNormalized();
		this->forward = f;
		this->right = f.CrossProduct(&this->up);
	}
}


void Character::Update(unsigned long elapsedTime, World* world, Group* pGroup)
{
	switch(state)
	{
		case CHARACTER_GO:
			RecalcFear(elapsedTime, &world->enemies, &world->groups);
			RecalcNewCharacterState( pGroup, world->scene );
			Go(elapsedTime, pGroup, world);
			break;

		case CHARACTER_FORM:
			RecalcFear(elapsedTime, &world->enemies, &world->groups);
			RecalcNewCharacterState( pGroup, world->scene );
			Form( elapsedTime, pGroup, world );
			break;

		case CHARACTER_REGROUP:
			RecalcFear(elapsedTime, &world->enemies, &world->groups);
			RecalcNewCharacterState( pGroup, world->scene );
			Regroup(elapsedTime, pGroup, world);
			break;

		case CHARACTER_FLEE:
			RecalcFear(elapsedTime, &world->enemies, &world->groups);
			RecalcNewCharacterState( pGroup, world->scene );
			Escape(elapsedTime, pGroup, world);
			break;

		case CHARACTER_ENEMIE:
			Go(elapsedTime, pGroup, world);
			break;
	}
}

void Character::Go(const unsigned long elapsedTime, Group* pGroup, World* world)
{
	POSITION pos = pGroup->characters.GetHeadPosition();
	Vector auxVector;
	float turn, acc;

	bool groupIsBack = false;

	if (pGroup->forward * (pGroup->center - position) < 2)
		groupIsBack = true;

	// Hacia el objetivo (constante)
	toTargetForce = (pGroup->checkPoints[checkPoint] - position);
	toTargetForce = toTargetForce.GetNormalized();

	// Cohesion (constante)
	toGroupCenterForce = (pGroup->compensedCenter - position);

//	if (toTargetForce * toGroupCenterForce < 0)
// 		groupIsBack = true;

//	if (groupIsBack)
//		toGroupCenterForce = forward * (forward * toGroupCenterForce);

	collisionForce = GetRepulsion(elapsedTime, world);

	// PONDERACION
	toGroupCenterForce	= toGroupCenterForce	* 0.15;
	toTargetForce		= toTargetForce			* 0.8;
	collisionForce		= collisionForce		* 1.5;

	// Si estamos retrasados vamos directos al checkPpoint sin usar la fuerza de cohesion
	if (pGroup->leaderCheckPoint < checkPoint)
		toGroupCenterForce	= Vector(0,0,0);


	// SUMA TOTAL
	Vector desiredDirection = toTargetForce + toGroupCenterForce + collisionForce;// + pGroup->forward;
	turn = this->right * desiredDirection.GetNormalized() * 0.2;
	acc = this->forward * desiredDirection.GetNormalized() * 2; //* maxSpeed;

	if (groupIsBack && speed > pGroup->minSpeed*1.02 && acc > 0)
		acc = 0;

	// hacemos el movimiento
	DoMove(turn, acc, elapsedTime, &pGroup->checkPoints[0], world->scene, pGroup);
}

void Character::Form(const unsigned long elapsedTime, Group* pGroup, World* world)
{
	POSITION pos = pGroup->characters.GetHeadPosition();
	Vector auxVector;
	float turn, acc;

	// Hacia el objetivo (constante)
	toTargetForce = (pGroup->checkPoints[checkPoint] - position);
	toTargetForce = toTargetForce.GetNormalized();

	// Cohesion (constante)
	toGroupCenterForce = (pGroup->compensedCenter - position);
	toGroupCenterForce = toGroupCenterForce.GetNormalized();
	
	// Repulsion de paredes y otros individuos
	collisionForce = GetRepulsion(elapsedTime, world);

	// PONDERACION
	toGroupCenterForce	= toGroupCenterForce	* 0.5;
	toTargetForce		= toTargetForce			* 0.2;
	collisionForce		= collisionForce		* 1;


	// SUMA TOTAL
	Vector desiredDirection; // = toTargetForce + collisionForce;
	turn = right * desiredDirection.GetNormalized() * 0.2;
	acc = forward * desiredDirection.GetNormalized() * 2;

	// si estamos relativamente cerca
	if ( (pGroup->center - position).GetLength() < pGroup->dispersion * 1.6 + 0.1)
	{
		// estamos suficentemente separados (y cerca) -> nos aliniamos mientras esperamos
		if ( collisionForce.GetLength() < 0.6)
		{
			// Una vez parados nos aliniamos
			if (speed < stopSpeed)
				FormAlign(elapsedTime, pGroup);
			// Sino frenamos
			else
			{
//				desiredDirection = toGroupCenterForce;
//				turn = right * desiredDirection.GetNormalized();
				DoMove(0, -maxBrakeForce, elapsedTime, NULL, world->scene, pGroup);
			}
		}
		// estamos apretados (y cerca) -> salimos
		else
		{
			desiredDirection = toTargetForce + collisionForce;
			turn = right * desiredDirection.GetNormalized() * 0.2;
			acc = forward * desiredDirection.GetNormalized() * 2;
			DoMove(turn, acc, elapsedTime, &pGroup->checkPoints[0], world->scene, pGroup);
		}
	}
	// Estamos muy lejos -> hacia el centro
	else
	{
		desiredDirection = toGroupCenterForce /* + collisionForce*/;
		turn = right * desiredDirection.GetNormalized() * 0.2;
		acc = forward * desiredDirection.GetNormalized() * 2;
		DoMove(turn, acc, elapsedTime, &pGroup->checkPoints[0], world->scene, pGroup);
	}
}

void Character::Regroup(const unsigned long elapsedTime, Group* pGroup, World* world)
{
	POSITION pos = pGroup->characters.GetHeadPosition();
	Vector auxVector;
	float turn, acc;

	// Hacia el objetivo (NULO)
	toTargetForce = Vector (0.0, 0.0, 0.0);

	// Cohesion (linial)
	toGroupCenterForce = (regroupCPs[regroupCP] - position);
	toGroupCenterForce = toGroupCenterForce.GetNormalized();

	// Repulsion de paredes
	collisionForce = GetRepulsion(elapsedTime, world);

	// PONDERACION
//	toGroupCenterForce	= toGroupCenterForce	* 1;
	collisionForce		= collisionForce		* 2;

	// SUMA TOTAL
	Vector desiredDirection = toGroupCenterForce + collisionForce; // + pGroup->forward;
	turn = right * desiredDirection.GetNormalized() * 0.2;
	acc = forward * desiredDirection.GetNormalized() * 2;

	// hacemos el movimiento
	DoMove(turn, acc, elapsedTime, &pGroup->center, world->scene, pGroup);
}

void Character::Escape(const unsigned long elapsedTime, Group* pGroup, World* world)
{
	POSITION pos = pGroup->characters.GetHeadPosition();
	POSITION posEnemieGroup = world->enemies.GetHeadPosition();
	POSITION posEnemie;
	Group* enGroup;
	Character* en;

	float turn, acc;
	Vector auxVector;
	bool someEnemies = false;
	toTargetForce = Vector (0.0, 0.0, 0.0);

	while (posEnemieGroup != NULL)
	{
		enGroup = &world->enemies.GetNext(posEnemieGroup);
		posEnemie = enGroup->characters.GetHeadPosition();

		while (posEnemie != NULL)
		{
			en = &enGroup->characters.GetNext(posEnemie);
		
			auxVector = (this->position - en->position);
			if (auxVector.GetLength() < 30 * CHARACTER_RADIUS)
			{
				someEnemies = true;
				auxVector = ( auxVector.GetNormalized() ) / ( auxVector.GetLength() * auxVector.GetLength() );
				toTargetForce += auxVector;
			}
		}
	}
	
	// Direccion de huida
	if (someEnemies)
		toTargetForce = toTargetForce.GetNormalized();
	else
		toTargetForce = Vector (-forward.x, -forward.y, 0.0) * speed * 0.1;

	// Cohesion (NINGUNA)
	toGroupCenterForce = Vector (0.0, 0.0, 0.0);

	// Repulsion de paredes
	collisionForce = GetRepulsion(elapsedTime, world);

	// PONDERACION
//	toGroupCenterForce	= toGroupCenterForce	* 2;
//	toTargetForce		= toTargetForce;
	collisionForce		= collisionForce		* 2;
	
	// SUMA TOTAL
	Vector desiredDirection = toTargetForce +  collisionForce; // + pGroup->forward;
	turn = right * desiredDirection.GetNormalized() * 0.2;
	acc = forward * desiredDirection.GetNormalized() * 2;

	// hacemos el movimiento
	DoMove( turn, acc, elapsedTime, NULL, world->scene, pGroup );
}

// Hace el movimiento y si se le pasa un targetPoint frenara para parar en el
void Character::DoMove(float turn, float acc, const unsigned long elapsedTime, Vector* targetPoint, Scene* scene, Group* pGroup)
{
	bool turning = false;

	// Truncamos la fuerza de giro
	if (turn > maxTurnForce)
	{
		turning = true;
		turn = maxTurnForce;
	}
	if (turn < -maxTurnForce)
	{
		turning = true;
		turn = -maxTurnForce;
	}

	// si estamos justo al reves giramos completamente
//	if (turn == 0 && acc < 0)
//		turn = maxTurnForce;

	SetForward( forward + (right * turn) );

	// Truncamos la aceleracion
	if (acc > maxAccForce)
		acc = maxAccForce;
	if (acc < -maxBrakeForce)
		acc = -maxBrakeForce;

	// Si gira a tope no acelera
	if (turning && acc > 0)
		acc = 0;

	// Si estamos cerca del objetivo paramos
	// tipler pg. 29
	if ( targetPoint != NULL )
	{
		double brakespace = (speed*speed) / (2 * maxBrakeForce);
		double dist = (*targetPoint - position).GetLength();

		// Estamos cerca o por delante del objetivo -> Frenamos
		if ( dist < brakespace + CHARACTER_RADIUS/5 && speed > 0)
			acc = -maxBrakeForce;
	}
	
	speed += (acc * DELTA_MOVEMENT * (double)elapsedTime);

	// Truncamos la velocidad
	if (speed < stopSpeed && acc < 0)
		speed = 0;
	if (speed > maxSpeed)
		speed = maxSpeed;

	// actualizamos posicion
	position += (forward*speed) * DELTA_MOVEMENT * (double)elapsedTime;

	// miramos si hemos llegado a un checkpoint
	CheckPointControl(pGroup, scene);
}

void Character::CheckPointControl(Group* pGroup, Scene* scene)
{
	double dist = (pGroup->checkPoints[checkPoint] - position).GetLength();

	if ( dist < CHARACTER_RADIUS*5 )
		checkPoint--;

	if (checkPoint < 0)
	{
		// si es un enemigo obtenemos un nuevo objetivo aleatorio
		if (state == CHARACTER_ENEMIE)
			pGroup->SetNewEnemieTarget(scene);

		// si se trata d un individuo normal
		else
		{
			// Si no es un objetivo final buscamos otro cacho de camino
			if (!pGroup->finalTarget)
				pGroup->SetTarget(pGroup->waitingTarget, scene);

			// Si ya hemos llegado al objetivo final
			else
				checkPoint = 0;
		}
	}


	if (state == CHARACTER_REGROUP)
	{
		dist = (regroupCPs[regroupCP] - position).GetLength();

		if ( dist < CHARACTER_RADIUS*4 )
			regroupCP--;

		if (regroupCP < 0)
		{
			// Si no es un objetivo final buscamos otro cacho de camino
			if (!regroupFinal)
				PrepareRegroup(pGroup, scene);

			// Si ya hemos llegado al objetivo final
			else
				regroupCP = 0;
		}
	}
}

void Character::RecalcNewCharacterState( Group *gr, Scene* scene )
{
	float dist;

	// Mapeo del diagrama de estados
	switch(state)
	{
	// AVANZAR
	case CHARACTER_GO:

		// Si nivel de panico es muy alto -> HUIR
		if (fear > maxFear)
		{
			PropagateFear(gr);
			state = CHARACTER_FLEE;
		}

		// Si el grupo esta Reagrupandose -> REAGRUPAR
		if ( gr->GetState() == GROUP_REGROUP )
		{
			state = CHARACTER_REGROUP;
			PrepareRegroup(gr, scene);
		}

		// Si el grupo esta formando -> FORMANDO
		else if ( gr->GetState() == GROUP_FORM )
			state = CHARACTER_FORM;

		break;

	// FORMAR
	case CHARACTER_FORM:

		// distancia al centro del grupo
		dist = (gr->center - position).GetLength();

		// Si nivel de panico es muy alto -> HUIR
		if (fear > maxFear)
		{
			PropagateFear(gr);
			state = CHARACTER_FLEE;
		}

		else if ( gr->GetState() == GROUP_REGROUP && dist >= CHARACTER_RADIUS*6)
		{
			state = CHARACTER_REGROUP;
			PrepareRegroup(gr, scene);
		}

		// Si el grupo esta Avanzando -> AVANZANDO
		else if ( gr->GetState() == GROUP_GO )
			state = CHARACTER_GO;


		break;


	case CHARACTER_REGROUP:

		// distancia al centro del grupo
		dist = (gr->center - position).GetLength();

		// Si nivel de panico es muy alto -> HUIR
		if (fear > maxFear)
		{
			PropagateFear(gr);
			state = CHARACTER_FLEE;
		}


		// Si el grupo esta formando -> FORMANDO
		else if ( gr->GetState() == GROUP_FORM || dist < CHARACTER_RADIUS*6 )
			state = CHARACTER_FORM;

		// Si el grupo esta avanzando -> REAGRUPAR
		else if ( gr->GetState() == GROUP_GO )
			state = CHARACTER_GO;


		break;


	case CHARACTER_FLEE:

		// Si nivel de panico ha descendido -> Reagruparse
		if (fear < maxFear*0.8)
		{
			state = CHARACTER_REGROUP;
			PrepareRegroup(gr, scene);
		}

	default:
		break;
	}
}

// Si el centro del grupo no es visible
// Busca los chekpoints individuales
void Character::PrepareRegroup(Group *gr, Scene *scene)
{
	// si no hay camino directo
	if ( scene->DetectCollision(&position, &gr->center) )
	{
		// Miramos que el centro del grupo este en una celda vacia,
		// sino el centro se desplaza a la posicion del primer ch q pase por aki
		TCell cell = scene->GetCell( &gr->center );
		if ( scene->GetWallType( cell.col, cell.row ) != WALL_EMPTY )
			gr->center = this->position;

		// Usamos el A* para econtrar el camino al centro del grupo
		CList <Vector, Vector&> cp;
		this->regroupFinal = scene->CalculatePath(&cp, &position, &gr->center);
		SetRegroupCheckPoints(&cp);
		
		// si el grupo no esta reagrupandose habra q actualizar el camino hacia el centro
		// ya que el centro se puede desplazar
		if (gr->GetState() != GROUP_REGROUP)
			this->regroupFinal = false;
	}

	// si hay camino directo
	else
	{
		// Un solo CP q sera el objetivo final
		regroupCPs[0] = gr->center;
		regroupCP = 0;
	}

}

void Character::SetRegroupCheckPoints(CList <Vector, Vector&> *cp)
{
	int c = 0;
	this->regroupCP = cp->GetCount() - 1;

	POSITION pos = cp->GetHeadPosition();
	while (pos != NULL)
		this->regroupCPs[c++] = cp->GetNext(pos);
}

void Character::FormAlign(const unsigned long elapsedTime, Group* pGroup)
{
	double side = right * pGroup->formAligment;		// Positivo (objetivo queda a la izquierda)
													// Negativo (objetivo queda a la derecha)

	if (side > maxTurnForce)
		side = maxTurnForce;
	if (side < -maxTurnForce)
		side = -maxTurnForce;

	SetForward( forward + right * side );
}


Vector Character::GetRepulsion(const unsigned long elapsedTime, World* world)
{
	Vector force = Vector(0.0, 0.0, 0.0);

	force += world->scene->GetSceneForce(this, CHARACTER_RADIUS, elapsedTime);
	force += GetCharactersSeparationForce(elapsedTime, world);

	if (force.GetLength() > 0.02)
	{
//		float turn = this->right * force.GetNormalized();
//		float acc = this->forward * force.GetNormalized() * maxSpeed;

		return force;
	}

	return Vector(0.0, 0.0, 0.0);
}

Vector Character::GetCharactersSeparationForce(const unsigned long elapsedTime, World* world)
{
	Group* gr;
	Character* ch;

	POSITION posCH;
	POSITION posGR = world->groups.GetHeadPosition();

	Vector separationForce = Vector(0.0, 0.0, 0.0);
	Vector auxVector;
	float distance;

	// Con otros individuos
	while (posGR != NULL)
	{
		gr = &world->groups.GetNext(posGR);
		posCH = gr->characters.GetHeadPosition();

		while (posCH != NULL)
		{
			ch = &gr->characters.GetNext(posCH);

			if (ch != this)
			{
				auxVector = (this->position - ch->position);
				distance = auxVector.GetLength();
				if ( distance < 10 )
				{
					//auxVector = ( auxVector.GetNormalized() ) / ( distance * distance );
					auxVector = auxVector.GetNormalized() * (3 / (distance*distance));
					separationForce += auxVector;
 				}
				
				// Colision
				if (distance < 2)
				{
					distance -= CHARACTER_RADIUS*2;
					auxVector = (this->position - ch->position);

					position -= auxVector.GetNormalized() * distance;
					ch->position += auxVector.GetNormalized() * distance;
				}
			}
		}
	}

	// Con enemigos
	posGR = world->enemies.GetHeadPosition();
	while (posGR != NULL)
	{
		gr = &world->groups.GetNext(posGR);
		posCH = gr->characters.GetHeadPosition();

		while (posCH != NULL)
		{
			ch = &gr->characters.GetNext(posCH);

			if (ch != this)
			{
				auxVector = (this->position - ch->position);
				distance = auxVector.GetLength();
				if (distance >= 2 && distance < 4 )
				{
					//auxVector = ( auxVector.GetNormalized() ) / ( distance * distance );
					auxVector = auxVector.GetNormalized() * (4 / (distance*distance));
					separationForce += (auxVector/2);	// Los enemigos van mas juntos
				}
				
				// Colision
				else if (distance < 2 && distance >0)
				{
					distance -= CHARACTER_RADIUS*2;
					auxVector = (this->position - ch->position);

					position -= auxVector.GetNormalized() * distance;
					ch->position += auxVector.GetNormalized() * distance;
				}
			}
		}
	}


	return separationForce;
}


void Character::RecalcFear(const unsigned long elapsedTime, CList<Group, Group&>* enemies, CList<Group, Group&>* groups )
{	// Recorremos todas las fuentes d miedo y las vamos sumando de forma podnerada con la distancia

	float addFear = 0;

	POSITION posGroup = enemies->GetHeadPosition();
	POSITION pos;

	Group* gr;
//	Character* ch;

	// Todos los enemigos
	while (posGroup != NULL)
	{
		gr = &enemies->GetNext(posGroup);
		pos = gr->characters.GetHeadPosition();

		while (pos != NULL)
		{
			Vector dist = position - gr->characters.GetNext(pos).position;
			if (dist.GetLength() < 30 * CHARACTER_RADIUS )
				addFear += (1 / dist.GetLength() ) * 20;
		}
	}

	// añadimos miedo
	this->fear += addFear * DELTA_MOVEMENT * (double)elapsedTime;

	// Quitmaos miedo devido al tiempo transcurrido
	this->fear -= (maxFear/10) * DELTA_MOVEMENT * (double)elapsedTime;

	if (fear > maxFear*3)
		fear = maxFear*3;

	else if (fear < 0)
		fear = 0;
}

void Character::PropagateFear(Group *gr)
{
	Character *ch;
	POSITION pos = gr->characters.GetHeadPosition();

	while (pos != NULL)
	{
		ch = &gr->characters.GetNext(pos);
		ch->fear *= 1.1;
	}
}

void Character::SetNewTarget(int nCP)
{
	this->checkPoint = nCP;
}
