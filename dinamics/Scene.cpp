// Scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pfc.h"
#include "Scene.h"
#include "Group.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scene::Scene()
{
}

Scene::Scene( int width, int height)
{
	this->width = width;
	this->height = height;

	wall = (int*)malloc( (width*height) * sizeof(int) );
//	objectMassCenter = (Vector*)malloc( (width*height) * sizeof(Vector) );

//	for (int i=0; i<width*height; i++)
// 		objectMassCenter[i] = Vector(0,0,0);

	cellSize = CHARACTER_RADIUS * 4;

	maxLeft = 0;
	maxRight = maxLeft + (cellSize * width);
	maxTop = 0;
	maxBottom = maxTop - (cellSize * height);
}

Scene::~Scene()
{
	free(wall);
//	free(objectMassCenter);
}


void Scene::SetWallType(int col, int row, int type)
{
	wall[col + (row*width)] = type;
}

int Scene::GetWallType(int col, int row)
{
	if (col < 0 || col >= width || row < 0 || row >= height)
		return WALL_FULL;

	return wall[col + (row*width)];
}

Vector Scene::GetCellCenter(int col, int row)
{
	Vector cellCenter;

	cellCenter.x = (col * cellSize) + cellSize/2;
	cellCenter.y = (-row* cellSize) - cellSize/2;
	cellCenter.z = 0;

	return cellCenter;
}

// en que celda estam un punto
TCell Scene::GetCell(Vector* position)
{
	TCell c;
	c.col = ((*position).x - maxLeft) / cellSize;
	c.row = -((*position).y + maxTop) / cellSize;

	return c;
}

// orientation: orientacion respecto a la cnlda donde esta el monigote
// (1 - arriba, 2 - abajo, 3 - izquierda, 4 - derecha)  
Vector Scene::GetCellForce(int col, int row, float radius, int orientation, Character* ch, const unsigned long elapsedTime)
{
	int wallType;
	Vector cellCenter = Vector (0.0, 0.0, 0.0);
	float wallDistance, forceMagnitude;
	Vector wallForce = Vector (0.0, 0.0, 0.0);
	Vector vecAux, forceDirection;

	// Cojemos el tipo de celda
	if ( col<0 || col >= width || row < 0 || row >= height )
		wallType = WALL_FULL;
	else
		wallType = GetWallType(col, row);


	// calculamos la repulsion a partir del tipo de celda
	if (wallType == WALL_FULL)
	{
		cellCenter.x = (col*cellSize) + (cellSize/2);
		cellCenter.y = ((-row)*cellSize) - (cellSize/2);

		if (orientation == 2) // comparamos con celda superior
		{
			forceDirection = Vector (0.0, -1.0, 0.0);	// abajo
			vecAux = ch->position - cellCenter;
			wallDistance = (forceDirection * vecAux) - (cellSize/2);
		}

		else if (orientation == 4) // comparamos con celda inferior
		{
			forceDirection = Vector (1.0, 0.0, 0.0);	// derecha
			vecAux = ch->position - cellCenter;
			wallDistance = (forceDirection * vecAux) - (cellSize/2);
		}

		else if (orientation == 6) // comparamos con celda izquierda
		{
			forceDirection = Vector (-1.0, 0.0, 0.0);	// izquierda
			vecAux = ch->position - cellCenter;
			wallDistance = (forceDirection * vecAux) - (cellSize/2);
		}

		else if (orientation == 8) // comparamos con celda inferior
		{
			forceDirection = Vector (0.0, 1.0, 0.0);	// arriba
			vecAux = ch->position - cellCenter;
			wallDistance = (forceDirection * vecAux) - (cellSize/2);
		}

		else if (orientation != 5)
		{
			forceDirection = (ch->position - cellCenter);
			wallDistance = forceDirection.GetLength();// - (radius + cellSize/RAIZDE2);
		}
	}
	else if (wallType == WALL_TOP_LEFT)
	{
		cellCenter.x = (col*cellSize);
		cellCenter.y = ((-row)*cellSize);

		vecAux = ch->position - cellCenter;
		forceDirection = vecAux.GetNormalized();

		wallDistance = vecAux.GetLength() - (cellSize);
	}
	else if (wallType == WALL_TOP_RIGHT)
	{
		cellCenter.x = ((col+1)*cellSize);
		cellCenter.y = ((-row)*cellSize);

		vecAux = ch->position - cellCenter;
		forceDirection = vecAux.GetNormalized();

		wallDistance = vecAux.GetLength() - (cellSize);
	}
	else if (wallType == WALL_BOTTOM_LEFT)
	{
		cellCenter.x = (col*cellSize);
		cellCenter.y = ((-(row+1))*cellSize);

		vecAux = ch->position - cellCenter;
		forceDirection = vecAux.GetNormalized();

		wallDistance = vecAux.GetLength() - (cellSize);
	}
	else if (wallType == WALL_BOTTOM_RIGHT)
	{
		cellCenter.x = ((col+1)*cellSize);
		cellCenter.y = ((-(row+1))*cellSize);

		vecAux = ch->position - cellCenter;
		forceDirection = vecAux.GetNormalized();

		wallDistance = vecAux.GetLength() - (cellSize);
	}


	// Aplicamos la fuerza de repulsion
	if( wallType != WALL_EMPTY)
	{

		// Si hay colision (<CR) modifica directamente la posicion y velocidad
		if (wallDistance < CHARACTER_RADIUS )
		{
			ch->position -= (forceDirection * (wallDistance - CHARACTER_RADIUS));
			ch->SetForward( ch->forward + (ch->right * (ch->right * forceDirection) ) );

//			float newSpeed = (ch->forward * forceDirection) * ch->speed;
//			ch->speed = (newSpeed * DELTA_MOVEMENT * (double)elapsedTime);
		}

		if ( wallDistance < 3.8 )
//		else
		{
			// FORMULA DE REPULSION CON PAREDES
			forceMagnitude = (0.6 / (wallDistance*wallDistance)) - 0.0375;

			wallForce = forceDirection * forceMagnitude;
		}
	}

	return wallForce;
}

Vector Scene::GetSceneForce(Character* ch, float radius, const unsigned long elapsedTime)
{
	Vector force = Vector (0.0, 0.0, 0.0);

	int col = (ch->position.x - maxLeft) / cellSize;
	int row = -(ch->position.y + maxTop) / cellSize;

	force += GetCellForce( col-1,	row-1,	CHARACTER_RADIUS, 1, ch, elapsedTime);	// arriba - izquierda
	force += GetCellForce( col,		row-1,	CHARACTER_RADIUS, 2, ch, elapsedTime);	// arriba
	force += GetCellForce( col+1,	row-1,	CHARACTER_RADIUS, 3, ch, elapsedTime);	// arriba - derecha
	force += GetCellForce( col-1,	row,	CHARACTER_RADIUS, 4, ch, elapsedTime);	// izquierda
	force += GetCellForce( col,		row,	CHARACTER_RADIUS, 5, ch, elapsedTime);	// centro (la misma celda)
	force += GetCellForce( col+1,	row,	CHARACTER_RADIUS, 6, ch, elapsedTime);	// derecha
	force += GetCellForce( col-1,	row+1,	CHARACTER_RADIUS, 7, ch, elapsedTime);	// abajo - izquierda
	force += GetCellForce( col,		row+1,	CHARACTER_RADIUS, 8, ch, elapsedTime);	// abajo
	force += GetCellForce( col+1,	row+1,	CHARACTER_RADIUS, 9, ch, elapsedTime);	// abajo - derecha

	return force;
}

// encuentra un camino y se lo asigna al grupo
// devuelve true si es camino final | false si no lo es
bool Scene::CalculatePath( CList<Vector, Vector&> *cpList, Vector *vStart, Vector *vTarget )
{
	CList<TCell, TCell&> openList, closedList;

	TCell cell;

	TCell end = GetCell(vTarget);
	TCell start = GetCell(vStart);

	start.gCost = 0;
	start.fCost = GetHeuristic(&start, &end);
	openList.AddTail(start);

	cell.fCost = 20;
	cell.gCost = 0;

	cpList->RemoveAll();

	// mientras exista un camino a buscar, no sea la celda final ni hayamos explorado demasiados nodos
	while ( !openList.IsEmpty() && cell.fCost!=0 && closedList.GetCount() < 1500 )
	{
		cell = GetNextCell(&openList);

		AddAllValidAdjacents(&openList, &closedList, &cell, &end);

		closedList.AddTail(cell);
	}

	// Hemos encontrado el camino
	if ( !openList.IsEmpty() &&  cell.fCost==0)
	{
		// copiamos los chekPoints en cpList
		TracePath(&closedList, vTarget, cpList);
		return true;
	}

	else
	{
		// añadimos la celda mas lejana para ke sea el target temporal
//		closedList.AddTail( GetMaxCellCost(&closedList) );

		// copiamos los chekPoints en cpList
		TracePath(&closedList, NULL, cpList);
		return false;
	}
}

TCell Scene::GetMaxCellCost(CList<TCell, TCell&>* lst)
{
	POSITION pos = lst->GetHeadPosition();
	TCell cell, maxCell;
	int maxCost = 0;

	while (pos != NULL)
	{
		cell = lst->GetNext(pos);

		if (cell.gCost > maxCost)
		{
			maxCell = cell;
			maxCost = cell.gCost;
		}
	}

	return cell;
}

int Scene::GetHeuristic(TCell* cell, TCell* target)
{
	int H;
	int xDistance = abs(cell->col - target->col);
	int yDistance = abs(cell->row - target->row);

	if (xDistance > yDistance)
		 H = 14*yDistance + 10*(xDistance-yDistance);
	else
		 H = 14*xDistance + 10*(yDistance-xDistance);

	return H;
}

// Busca la celda con menor costo F = G + H
TCell Scene::GetNextCell(CList<TCell, TCell&>* lst)
{
	TCell *cell, minCell;
	int minCost = 1000000;

	POSITION pos, minPos, lastPos;

	pos = lst->GetHeadPosition();

	while (pos)
	{
		lastPos = pos;
		cell = &(lst->GetNext(pos));

		if (cell->fCost + cell->gCost < minCost)
		{
			minCost = cell->fCost + cell->gCost;
			minCell = *cell;
			minPos	= lastPos;
		}
	}

	lst->RemoveAt(minPos);

	return minCell;
}

void Scene::InsertIntoList(CList<TCell, TCell&>* lst, CList<TCell, TCell&>* closedList, TCell* cell)
{
	POSITION pos, lastPos;
	TCell iCell;

	pos = lst->GetHeadPosition();

	while (pos)
	{
		lastPos = pos;
		iCell = lst->GetNext(pos);

		if (iCell.col == cell->col && iCell.row == cell->row)
		{
			if (iCell.gCost > cell->gCost)
			{
				// reemplazamos la celda
				lst->RemoveAt(lastPos);
				lst->AddTail(*cell);
			}

			return;
		}
	}

	// miramos si esta en la closedList
	pos = closedList->GetHeadPosition();

	while (pos)
	{
		lastPos = pos;
		iCell = lst->GetNext(pos);

		if (iCell.col == cell->col && iCell.row == cell->row)
		{
			if (iCell.gCost > cell->gCost)
			{
				// la borramos de cerrados y la metemos en abiertos
				closedList->RemoveAt(lastPos);
				lst->AddTail(*cell);
			}

			return;
		}
	}


	// Si no esta en ninguna lista lo insertamos en opened
	lst->AddTail(*cell);
}


void Scene::AddAllValidAdjacents(CList<TCell, TCell&> *lst, CList<TCell, TCell&> *closedList, TCell* cell, TCell* target)
{
	TCell insertCell;

	if ( GetWallType(cell->col-1, cell->row-1) == WALL_EMPTY )
	{
		insertCell.col = cell->col-1;
		insertCell.row = cell->row-1;
		insertCell.direction = 7;
		insertCell.gCost = cell->gCost + 14;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
	if ( GetWallType(cell->col, cell->row-1) == WALL_EMPTY )
	{
		insertCell.col = cell->col;
		insertCell.row = cell->row-1;
		insertCell.direction = 8;
		insertCell.gCost = cell->gCost + 10;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
	if ( GetWallType(cell->col+1, cell->row-1) == WALL_EMPTY )
	{
		insertCell.col = cell->col+1;
		insertCell.row = cell->row-1;
		insertCell.direction = 9;
		insertCell.gCost = cell->gCost + 14;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
	
	if ( GetWallType(cell->col-1, cell->row) == WALL_EMPTY )
	{
		insertCell.col = cell->col-1;
		insertCell.row = cell->row;
		insertCell.direction = 4;
		insertCell.gCost = cell->gCost + 10;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
	if ( GetWallType(cell->col+1, cell->row) == WALL_EMPTY )
	{
		insertCell.col = cell->col+1;
		insertCell.row = cell->row;
		insertCell.direction = 6;
		insertCell.gCost = cell->gCost + 10;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
	
	if ( GetWallType(cell->col-1, cell->row+1) == WALL_EMPTY )
	{
		insertCell.col = cell->col-1;
		insertCell.row = cell->row+1;
		insertCell.direction = 1;
		insertCell.gCost = cell->gCost + 14;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
	if ( GetWallType(cell->col, cell->row+1) == WALL_EMPTY )
	{
		insertCell.col = cell->col;
		insertCell.row = cell->row+1;
		insertCell.direction = 2;
		insertCell.gCost = cell->gCost + 10;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
	if ( GetWallType(cell->col+1, cell->row+1) == WALL_EMPTY )
	{
		insertCell.col = cell->col+1;
		insertCell.row = cell->row+1;
		insertCell.direction = 3;
		insertCell.gCost = cell->gCost + 14;
		insertCell.fCost = GetHeuristic(&insertCell, target);
		InsertIntoList(lst, closedList, &insertCell);
	}
}

int Scene::TracePath(CList<TCell, TCell&>* lst, Vector* target, CList<Vector, Vector&> *cpSortedList)
{
	int lastDirection;
	TCell cell;
	Vector v;
	CList<Vector, Vector&> cpList;


	// Añadimos el target
	if (target != NULL)
		cpList.AddHead(*target);

	// Cogemos la celda del target
	cell = lst->GetTail();

	lastDirection = cell.direction;

	// hasta llegar al inicio
	while (cell.gCost > 0)
	{
		cell = GetParentCellInPath(lst, &cell);
		
		if (cell.direction != lastDirection)
		{
			cpList.AddTail( GetCellCenter(cell.col, cell.row) );
			lastDirection = cell.direction;
		}
	}

	// copiamos los checkpoints a la lista ordenada
	while( !cpList.IsEmpty() )
	{
		cpSortedList->AddTail( cpList.RemoveHead() );
	}

	// quitamos el ultimo (es la propia celda donde esta)
	if (target != NULL)
		cpSortedList->RemoveTail();

	return cpSortedList->GetCount();
}

TCell Scene::GetParentCellInPath(CList<TCell, TCell&>* lst, TCell* parentCell)
{
	int fRow, fCol;
	TCell cell;

	if (parentCell->direction == 1)
	{
		fCol = parentCell->col+1;
		fRow = parentCell->row-1;
	}
	else if (parentCell->direction == 2)
	{
		fCol = parentCell->col;
		fRow = parentCell->row-1;
	}
	else if (parentCell->direction == 3)
	{
		fCol = parentCell->col-1;
		fRow = parentCell->row-1;
	}

	if (parentCell->direction == 4)
	{
		fCol = parentCell->col+1;
		fRow = parentCell->row;
	}
	else if (parentCell->direction == 6)
	{
		fCol = parentCell->col-1;
		fRow = parentCell->row;
	}

	if (parentCell->direction == 7)
	{
		fCol = parentCell->col+1;
		fRow = parentCell->row+1;
	}
	else if (parentCell->direction == 8)
	{
		fCol = parentCell->col;
		fRow = parentCell->row+1;
	}
	else if (parentCell->direction == 9)
	{
		fCol = parentCell->col-1;
		fRow = parentCell->row+1;
	}

	POSITION pos = lst->GetHeadPosition();

	while (pos!=NULL)
	{
		cell = lst->GetNext(pos);

		if (cell.col == fCol && cell.row == fRow)
			return cell;
	}

	// SI LLEGA AQUI MAL VAMOS
	return *parentCell;
}


// ----- DETECCION DE COLISIONES (para mirar si hay que usar A* o no)
// ------------------------------------------------------------------

// true -> hay colision || false -> camino libre
bool Scene::DetectCollision(Vector* start, Vector* end)
{
	// en que celda estamos
	TCell tStart = GetCell(start);
	TCell tEnd = GetCell(end);

	// linia entre ambos puntos
	Vector direction = (*end - *start).GetNormalized();

	// direccion
	bool up, right;
	
	// posicion dentro de la celda
	Vector localPosition;
	localPosition.x = (*start).x - cellSize*(float)tStart.col;
	localPosition.y = (*start).y + cellSize*(float)tStart.row;
	localPosition.z = 0;

	// Singularidad vertical
	if (direction.x == 0)
	{
		for (int i=tStart.row; i<=tEnd.row; i++)
		{
			if ( GetWallType(tStart.col, i) != WALL_EMPTY )
				return true;
		}

		return false;
	}

	// Direccion
	if (direction.x > 0)
		right = true;
	else
		right = false;

	if (direction.y > 0)
		up = true;
	else
		up = false;

	return DetectCollisionWithCell(direction, localPosition, right, up, tStart.col, tStart.row, &tEnd);
}

bool Scene::DetectCollisionWithCell(Vector forward, Vector pointIN, bool right, bool up, int col, int row, TCell* tEnd)
{
	float incHorizontal, incVertical;
	Vector pointOUT;

	// Si chocamos
	if ( GetWallType(col, row) != WALL_EMPTY)
		return true;

	// Si ya hemos hecho suficientes tests
	if (tEnd->col == col && tEnd->row == row)
		return false;

	// Calculo de deltas
	if (right)
		incHorizontal = (cellSize - pointIN.x) / forward.x;
	else
		incHorizontal = (pointIN.x) / (-forward.x);

	if (up)
		incVertical = (pointIN.y) / (-forward.y);
	else
		incVertical = (-cellSize - pointIN.y) / forward.y;

	// Salimos por los laterales
	if (abs(incHorizontal) < abs(incVertical))
	{
		if (right)
		{
			pointOUT = pointIN + (forward * incHorizontal);
			pointOUT.x -= cellSize;
			return DetectCollisionWithCell(forward, pointOUT, right, up, col+1, row, tEnd);
		}
		else
		{
			pointOUT = pointIN - (forward * incHorizontal);
			pointOUT.x += cellSize;
			return DetectCollisionWithCell(forward, pointOUT, right, up, col-1, row, tEnd);
		}
	}

	// Salimos por arriba o abajo
	else
	{
		if (up)
		{
			pointOUT = pointIN + (forward * incVertical);
			pointOUT.y -= cellSize;
			return DetectCollisionWithCell(forward, pointOUT, right, up, col, row-1, tEnd);
		}

		else
			pointOUT = pointIN + (forward * incVertical);
			pointOUT.y += cellSize;
			return DetectCollisionWithCell(forward, pointOUT, right, up, col, row+1, tEnd);
	}
}