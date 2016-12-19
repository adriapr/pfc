// Group.cpp: implementation of the Group class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Group.h"
#include "Scene.h"
#include "World.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Group::Group()
{
	characters.RemoveAll();
	state = GROUP_FORM;
	selected = false;
	formAligment = Vector(1,0,0);
	finalTarget = true;
}

Group::Group(bool enemie)
{
	characters.RemoveAll();
	state = GROUP_ENEMIE;
	selected = false;
	formAligment = Vector(1,0,0);
	finalTarget = true;
}

Group::~Group()
{
	characters.~CList();
}

Group& Group::operator =(const Group& myGroup)
{
	if (&myGroup == this) 
		return *this;

	this->compensedCenter	= myGroup.compensedCenter;
	this->minSpeed			= myGroup.minSpeed;
	this->center			= myGroup.center;
	this->waitingTarget		= myGroup.waitingTarget;
	this->finalTarget		= myGroup.finalTarget;
	this->speed				= myGroup.speed;
	this->forward			= myGroup.forward;
	this->dispersion		= myGroup.dispersion;
	this->state				= myGroup.state;

	this->selected			= myGroup.selected;
	this->formAligment		= myGroup.formAligment;

	this->nCheckPoints		= myGroup.nCheckPoints;
	this->leaderCheckPoint	= myGroup.leaderCheckPoint;

	for (int i=0; i<=myGroup.nCheckPoints; i++)
		this->checkPoints[i] = myGroup.checkPoints[i];


	this->characters.RemoveAll();
	POSITION pos = myGroup.characters.GetHeadPosition();
	while (pos != NULL)
		this->characters.AddTail( myGroup.characters.GetNext(pos) );

	return *this;
}

void Group::AddCharacter(Character* ch)
{
	characters.AddTail(*ch);
}

void Group::SetTarget(Vector newTarget, Scene* scene )
{
	TCell cell;
	
	if (scene != NULL )
		cell = scene->GetCell( &newTarget );

	// solo asignamos si la celda esta vacia
	if ( scene == NULL || scene->GetWallType(cell.col, cell.row) == WALL_EMPTY )
	{
		this->finalTarget = true;
		this->waitingTarget = newTarget;
		this->nCheckPoints = 0;

		// Comprobamos que el centro del grupo este en una celda valida
		if (scene != NULL)
		{
			cell = scene->GetCell( &center );
			if (scene->GetWallType(cell.col, cell.row) != WALL_EMPTY)
			{
				this->center = characters.GetHead().position;
				this->SetState( GROUP_REGROUP );

				return;
			}
 		}


		CList <Vector, Vector&> cp;


		if ( scene != NULL && scene->DetectCollision(&center, &newTarget) )
		{
			this->finalTarget = scene->CalculatePath(&cp, &center, &newTarget);
			SetCheckPoints(&cp);
		}

		else
			checkPoints[0] = newTarget;

		// Actualiza los objetivos de los individuos del grupo
		POSITION pos = characters.GetHeadPosition();
		while (pos != NULL)
			characters.GetNext(pos).SetNewTarget(this->nCheckPoints);
	}
}

void Group::SetNewEnemieTarget(Scene* scene)
{
	CList <Vector, Vector&> cp;
	int col=-1, row=-1;

    while (scene->GetWallType(col, row) != WALL_EMPTY)
	{
		srand((unsigned)time(0)); 
		col = (rand() % scene->width);
		row = (rand() % scene->height);
	}

	// Con A*
	if ( scene != NULL && scene->DetectCollision(&center, &(scene->GetCellCenter(col, row))) )
	{
		this->finalTarget = scene->CalculatePath( &cp, &center, &(scene->GetCellCenter(col, row)) );
		SetCheckPoints(&cp);
		this->waitingTarget = this->checkPoints[0];
	}

	// Camino directo
	else
	{
		this->checkPoints[0] = scene->GetCellCenter(col, row);
		this->waitingTarget = this->checkPoints[0];
	}
	

	POSITION pos = characters.GetHeadPosition();
	while (pos != NULL)
		characters.GetNext(pos).SetNewTarget(this->nCheckPoints);
}

void Group::SetCheckPoints(CList <Vector, Vector&> *cp)
{
	int c = 0;
	this->nCheckPoints = cp->GetCount() - 1;

	POSITION pos = cp->GetHeadPosition();
	while (pos != NULL)
		this->checkPoints[c++] = cp->GetNext(pos);
}

void Group::SetDirection(Vector newTarget)
{
	Vector direction = newTarget - checkPoints[0];

	formAligment = direction.GetNormalized();
}

void Group::SetState(int newState)
{
	state = newState;
}

int Group::GetState()
{
	return state;
}

Vector Group::GetTarget()
{
	return waitingTarget;
//	return checkPoints[0];
}


//void Group::Update(unsigned long elapsedTime, CList<Enemy, Enemy&>* enemies, Scene* scene)
void Group::Update(unsigned long elapsedTime, World* world)
{
	POSITION pos = characters.GetHeadPosition();

	// Actualiza los individuos
	while (pos != NULL)
		characters.GetNext(pos).Update(elapsedTime, world, this);

	// recalcula el estado
	RecalcNewGroupState( world->scene );

	// Recalcula los datos dependientes de los individuos que lo componen
	UpdateStatus();


}

void Group::UpdateStatus()
{
	int notFleeing = 0;
	int notRegrouping = 0;

	POSITION pos = characters.GetHeadPosition();
	Character* ch;

	float sumSpeed = 0;
	Vector newCompensedCenter = Vector( 0.0, 0.0, 0.0 );

	Vector newCenter = Vector( 0.0, 0.0, 0.0 );
	Vector newForward = Vector( 0.0, 0.0, 0.0 );
	float newDispersion = 0;
	float newSpeed = 0;
	float newMinSpeed = 1000;
	int maxCP = 0;
	int lCheckPoint = MAX_CHECKPOINTS;

	while (pos != NULL)
	{
		ch = &characters.GetNext(pos);

		// Los individuos q huyen no los contamos
		if (ch->state != CHARACTER_FLEE)
		{
			notFleeing++;
			newDispersion += (ch->position - this->center).GetLength();
			newSpeed += ch->speed;

			if (ch->maxSpeed < newMinSpeed)
				newMinSpeed = ch->maxSpeed;

			newForward += ch->forward * ch->speed;
			if (lCheckPoint > ch->checkPoint)
				lCheckPoint = ch->checkPoint;

			if (ch->state != CHARACTER_REGROUP)
			{
				notRegrouping++;
				newCenter += ch->position;
				newCompensedCenter += ch->position / ch->maxSpeed;
				sumSpeed += (1 / ch->maxSpeed);
			}

		}

		if (ch->checkPoint > maxCP)

			maxCP = ch->checkPoint;
	}

	if (notFleeing >0)
	{
		if (notRegrouping > 0 && state != GROUP_REGROUP)
		{
			this->center = newCenter / (float)notRegrouping;
			this->compensedCenter = newCompensedCenter / (float)sumSpeed;
		}

		this->dispersion = newDispersion / (float)notFleeing;
		this->speed = newSpeed / (float)notFleeing;
		this->forward = newForward.GetNormalized();
	}
	else
		this->dispersion = MAX_DISPERSION*2;

	nCheckPoints = maxCP;
	leaderCheckPoint = lCheckPoint;
	minSpeed = newMinSpeed;
}

void Group::RecalcNewGroupState(Scene *scene)
{
	float distance;
	distance = (GetTarget() - center).GetLength();

	// Mapeo del diagrama de estados
	switch(state)
	{
	// AVANZAR
	case GROUP_GO:

		if (GetFleeingProportion() >= FLEE_PROPORTION)
			state = GROUP_FLEE;

		// Si hay mucha dispersion -> REAGRUPAR
		else if ( dispersion > MAX_DISPERSION )
//		else if (GetRegroupingProportion() >= REGROUP_PROPORTION)
			state = GROUP_REGROUP;

		// Si estamos cerca del objetivo -> FORMAR
		else if ( distance < FORM_DISTANCE )
			state = GROUP_FORM;

		break;

	// FORMAR
	case GROUP_FORM:

		if (GetFleeingProportion() >= FLEE_PROPORTION)
			state = GROUP_FLEE;

		// Si hay mucha dispersion -> REAGRUPAR
		else if ( dispersion > MAX_DISPERSION )
			state = GROUP_REGROUP;

		// Si estamos lejos del objetivo -> AVANZAR
		else if ( distance > FORM_DISTANCE*2 )
			state = GROUP_GO;

		break;

	// REAGRUPAR
	case GROUP_REGROUP:

		if (GetFleeingProportion() >= FLEE_PROPORTION)
			state = GROUP_FLEE;

		// Si ha disminuido la dispersion -> AVANZAR
		else if ( dispersion < MAX_DISPERSION * 0.7 && AllCharactersGrouped() )
		{
			SetTarget( this->checkPoints[0], scene);
			state = GROUP_GO;
		}

		break;

	case GROUP_FLEE:

		// Si no hay demasiado individuos huyendo -> REAGRUPAR
		if (GetFleeingProportion() < FLEE_PROPORTION*0.8)
			state = GROUP_REGROUP;

	default:
		break;
	}
}

// TRUE: Si todo tienen regroupCP == 0 (todos agrupados)
// FALSE si no es el caso
bool Group::AllCharactersGrouped()
{
	Character *ch;
	POSITION pos = characters.GetHeadPosition();

	while (pos != NULL)
	{
		ch = &characters.GetNext(pos);

		if ( ch->regroupCP > 0)
			return false;
	}

	return true;
}


float Group::GetFleeingProportion()
{
	int nFlee = 0;

	POSITION pos = characters.GetHeadPosition();

	while (pos != NULL)
	{
		if ( characters.GetNext(pos).state == CHARACTER_FLEE )
			nFlee++;
	}

	return (float)nFlee / characters.GetCount();
}


float Group::GetRegroupingProportion()
{
	int nRegroup = 0;

	POSITION pos = characters.GetHeadPosition();

	while (pos != NULL)
	{
		if ( characters.GetNext(pos).state == CHARACTER_REGROUP )
			nRegroup++;
	}

	return (float)nRegroup / characters.GetCount();
}

