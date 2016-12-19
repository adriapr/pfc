// world.cpp: implementation of the World class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "world.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

World::World()
{
	this->Initialize();
}

World::~World()
{
	groups.~CList();
}

void World::Initialize()
{
	groups.RemoveAll();
	enemies.RemoveAll();

//	Group* gr;
//	Character* en;
//	
//	for (int j=0; j<1; j++)
//	{
//		gr = new Group(true);
//
//		for (int i=0; i<2; i++)
//		{
//			en = new Character(true);
//
//			en->position	= Vector( i+4, -(j*4 + 2), 0);
//
//			gr->AddCharacter(en);
//		}
//
//		enemies.AddTail(*gr);
//	}

	stats.nGroups = 0;
	stats.nCharacters = 0;
}

void World::UnselectAllGroups()
{
	POSITION pos = groups.GetHeadPosition();

	while (pos != NULL)
		groups.GetNext(pos).selected = false;
}

void World::Update(unsigned long elapsedTime)
{
	POSITION pos = groups.GetHeadPosition();

	while (pos != NULL)
		groups.GetNext(pos).Update(elapsedTime, this);

	pos = enemies.GetHeadPosition();
	while (pos != NULL)
		enemies.GetNext(pos).Update(elapsedTime, this);

//	unsigned long t = timeGetTime() - stats.timeStart;
//	stats.nUpdates++;
}

void World::RegroupSelectedGroup()
{
	Group *gr;
	Character *ch;
	POSITION pos = this->groups.GetHeadPosition();
	POSITION posCh;

	while (pos != NULL)
	{
		gr = &groups.GetNext(pos);

		if (gr->selected)
		{
			gr->SetState(GROUP_REGROUP);

			posCh = gr->characters.GetHeadPosition();
			while (posCh != NULL)
			{
				ch = &gr->characters.GetNext(posCh);
				ch->state = CHARACTER_REGROUP;
				ch->PrepareRegroup(gr, scene);
			}

		}
	}
}


void World::ReadScene(CString fName)
{
	Group* gr;
	Character* ch;

	CStdioFile fScene; 
	char *buffer = (char*)malloc(2000*sizeof(char));
	CString aux;

	int iGR, iCH, pos, posAux, nGroups, nCharacters;
	Vector vecAux, grTarget;
	char wall;

	// Propiedades de character
	float chMaxSpeed, chMaxAccForce, chMaxBrakeForce, chMaxTurnForce, chMaxFear;

	// Propiedades de la escena
	int sceneWidth, sceneHeight;

	this->Initialize();

	// Abrimos el archivo para lectura
	if (fScene.Open(fName, CFile::modeRead) != NULL)
	{
		// - nGroups ------------------------------------
		fScene.ReadString(buffer,50);

		aux = CString(buffer);
		posAux = aux.Find(": ",0)+1;

		nGroups = atoi( aux.Mid(posAux, aux.GetLength() ) );
		stats.nGroups = nGroups;

		// Leemos Cada grupo
		for (iGR=0; iGR<nGroups; iGR++)
		{
			// Linia en blanco
			fScene.ReadString(buffer,100);

			gr = new Group();

			gr->SetState( GROUP_GO );

			// - nCharacters ------------------------------
			fScene.ReadString(buffer,50);
			aux = CString(buffer);
			posAux = aux.Find(": ",0)+1;

			nCharacters = atoi( aux.Mid(posAux, aux.GetLength() ) );
			stats.nCharacters += nCharacters;

			// LEEMOS UN CHARACTER
			for (iCH=0; iCH<nCharacters; iCH++)
			{
				// Linia en blanco
				fScene.ReadString(buffer,50);

				// - Character maxSpeed -------------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxSpeed = atof( aux.Mid(posAux, aux.GetLength() ) );

				// - Character maxAccForce ----------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxAccForce = atof( aux.Mid(posAux, aux.GetLength() ) );

				// - Character maxBrakeForce --------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxBrakeForce = atof( aux.Mid(posAux, aux.GetLength() ) );

				// - Character maxTurnSpeed ---------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxTurnForce = atof( aux.Mid(posAux, aux.GetLength() ) );

//				// - Character stopSpeed -------------------
//				fScene.ReadString(buffer,50);
//				aux = CString(buffer);
//				posAux = aux.Find(": ",0)+1;
// 				chStopSpeed = atof( aux.Mid(posAux, aux.GetLength() ) );

				// - Character maxFear --------------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxFear = atof( aux.Mid(posAux, aux.GetLength() ) );

				ch = new Character(chMaxSpeed, chMaxAccForce, chMaxBrakeForce, chMaxTurnForce, 0.1f, chMaxFear);

				// - Character Position -------------------
				fScene.ReadString(buffer,100);
				aux = CString(buffer);

				// -- X 
				posAux = aux.Find(": ",0)+1;
				pos = aux.Find(' ', posAux+2);
				vecAux.x = atof( aux.Mid(posAux, pos ) );
				
				// -- Y 
				posAux = pos+1;
				pos = aux.Find(' ', posAux);
				vecAux.y = atof( aux.Mid(posAux, pos ) );

				// -- Z 
				posAux = pos+1;
				vecAux.z = atof( aux.Mid(posAux, aux.GetLength() ) );

				ch->position = vecAux;
				ch->regroupCPs[0] = vecAux;

				// - Character Forward -------------------
				fScene.ReadString(buffer,100);
				aux = CString(buffer);

				// -- X 
				posAux = aux.Find(": ",0)+1;
				pos = aux.Find(' ', posAux+2);
				vecAux.x = atof( aux.Mid(posAux, pos ) );
				
				// -- Y 
				posAux = pos+1;
				pos = aux.Find(' ', posAux);
				vecAux.y = atof( aux.Mid(posAux, pos ) );

				// -- Z 
				posAux = pos+1;
				vecAux.z = atof( aux.Mid(posAux, aux.GetLength() ) );

				ch->SetForward( vecAux );

				gr->AddCharacter(ch);
			}	// FIN CHARACTER

			gr->UpdateStatus();
			gr->SetTarget(gr->center, NULL);
			groups.AddTail(*gr);
		}	// FIN GRUPO

		// Linia en blanco
		fScene.ReadString(buffer,100);

		// ENEMIGOS

		// - nGroups ------------------------------------
		fScene.ReadString(buffer,50);

		aux = CString(buffer);
		posAux = aux.Find(": ",0)+1;

		nGroups = atoi( aux.Mid(posAux, aux.GetLength() ) );
		stats.nGroups += nGroups;

		// Leemos Cada grupo de enemigos
		for (iGR=0; iGR<nGroups; iGR++)
		{
			// Linia en blanco
			fScene.ReadString(buffer,100);

			gr = new Group(true);
//			gr->SetState( GROUP_ENEMIE );

			// - nCharacters ------------------------------
			fScene.ReadString(buffer,50);
			aux = CString(buffer);
			posAux = aux.Find(": ",0)+1;

			nCharacters = atoi( aux.Mid(posAux, aux.GetLength() ) );
			stats.nCharacters += nCharacters;

			// LEEMOS UN CHARACTER
			for (iCH=0; iCH<nCharacters; iCH++)
			{
				// Linia en blanco
				fScene.ReadString(buffer,50);

				// - Character maxSpeed -------------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxSpeed = atof( aux.Mid(posAux, aux.GetLength() ) );

				// - Character maxAccForce ----------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxAccForce = atof( aux.Mid(posAux, aux.GetLength() ) );

				// - Character maxBrakeForce --------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxBrakeForce = atof( aux.Mid(posAux, aux.GetLength() ) );

				// - Character maxTurnSpeed ---------------
				fScene.ReadString(buffer,50);
				aux = CString(buffer);
				posAux = aux.Find(": ",0)+1;
				chMaxTurnForce = atof( aux.Mid(posAux, aux.GetLength() ) );

//				// - Character stopSpeed -------------------
//				fScene.ReadString(buffer,50);
//				aux = CString(buffer);
//				posAux = aux.Find(": ",0)+1;
// 				chStopSpeed = atof( aux.Mid(posAux, aux.GetLength() ) );

				ch = new Character(chMaxSpeed, chMaxAccForce, chMaxBrakeForce, chMaxTurnForce, 0.1f, 0);
				ch->state = CHARACTER_ENEMIE;

				// - Character Position -------------------
				fScene.ReadString(buffer,100);
				aux = CString(buffer);

				// -- X 
				posAux = aux.Find(": ",0)+1;
				pos = aux.Find(' ', posAux+2);
				vecAux.x = atof( aux.Mid(posAux, pos ) );
				
				// -- Y 
				posAux = pos+1;
				pos = aux.Find(' ', posAux);
				vecAux.y = atof( aux.Mid(posAux, pos ) );

				// -- Z 
				posAux = pos+1;
				vecAux.z = atof( aux.Mid(posAux, aux.GetLength() ) );

				ch->position = vecAux;
				ch->regroupCPs[0] = vecAux;

				// - Character Forward -------------------
				fScene.ReadString(buffer,100);
				aux = CString(buffer);

				// -- X 
				posAux = aux.Find(": ",0)+1;
				pos = aux.Find(' ', posAux+2);
				vecAux.x = atof( aux.Mid(posAux, pos ) );
				
				// -- Y 
				posAux = pos+1;
				pos = aux.Find(' ', posAux);
				vecAux.y = atof( aux.Mid(posAux, pos ) );

				// -- Z 
				posAux = pos+1;
				vecAux.z = atof( aux.Mid(posAux, aux.GetLength() ) );

				ch->SetForward( vecAux );
				gr->AddCharacter(ch);
			}	// FIN CHARACTER

			gr->UpdateStatus();
			gr->SetTarget(gr->center, NULL);
			enemies.AddTail(*gr);
		}	// FIN GRUPO DE ENEMIGOS


		// - Leemos Terreno ---------------------------------
		
		// Linia en blanco
		fScene.ReadString(buffer,50);

		// - Ancho escena ------------------------------------
		fScene.ReadString(buffer,50);
		aux = CString(buffer);
		posAux = aux.Find(": ",0)+1;
		sceneWidth = atoi( aux.Mid(posAux, aux.GetLength() ) );

		// - Alto escena ------------------------------------
		fScene.ReadString(buffer,50);
		aux = CString(buffer);
		posAux = aux.Find(": ",0)+1;
		sceneHeight = atoi( aux.Mid(posAux, aux.GetLength() ) );

		scene = new Scene(sceneWidth, sceneHeight);

		for (int row=0; row<sceneHeight; row++)
		{
			fScene.ReadString(buffer,sceneWidth+10);
			aux = CString(buffer);
			for (int col=0; col<sceneWidth; col++)
			{
				wall = aux.GetAt(col);
				if (wall == '0')
					scene->SetWallType(col, row, WALL_FULL);
				else if (wall == '1')
					scene->SetWallType(col, row, WALL_BOTTOM_RIGHT);
				else if (wall == '2')
					scene->SetWallType(col, row, WALL_BOTTOM_LEFT);
				else if (wall == '3')
					scene->SetWallType(col, row, WALL_TOP_RIGHT);
				else if (wall == '4')
					scene->SetWallType(col, row, WALL_TOP_LEFT);
				else
					scene->SetWallType(col, row, WALL_EMPTY);
			}
		} // Fin escenario

		// Calculamos los centros de masa de lso objetos por propagacion
//		scene->CalculateObjectsMassCenter();

	fScene.Close();
	}
}
