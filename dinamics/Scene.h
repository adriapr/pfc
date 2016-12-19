// Scene.h: interface for the Scene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENE_H__0487ACE8_DEC7_4305_8423_2102347AE9AC__INCLUDED_)
#define AFX_SCENE_H__0487ACE8_DEC7_4305_8423_2102347AE9AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Character;
class Group;

// - Tipos de paredes
#define WALL_FULL			0
#define WALL_BOTTOM_RIGHT	1
#define WALL_BOTTOM_LEFT	2
#define WALL_TOP_RIGHT		3
#define WALL_TOP_LEFT		4
#define WALL_EMPTY			5

#define RAIZDE2 0.70710678118654752440084436210485

// Constantes en al formual de repulsion
//#define A_CONST 49.05
//#define B_CONST -146.25
// #define C_CONST 97.3

#define A_CONST 49.6
#define B_CONST 347.75
#define C_CONST 98.6

typedef struct {
	int col;
	int row;

	int direction;		// direccion de entrada (teclado num.)
	int gCost;			// Coste acumulado
	int fCost;			// Heuristica
} TCell;

class Scene  
{
public:

	float cellSize;

	int width;
	int height;

	float maxLeft;
	float maxRight;
	float maxTop;
	float maxBottom;

	int* wall;
//	Vector* objectMassCenter;

public:
	Scene();
	Scene(int width, int height);
	virtual ~Scene();

	void SetWallType(int col, int row, int type);
	int  GetWallType(int col, int row);
	Vector GetCellCenter(int col, int row);

	TCell GetCell(Vector* position);

	Vector GetSceneForce(Character* ch, float radius, const unsigned long elapsedTime);
	Vector GetCellForce(int col, int row, float radius, int orientation, Character* ch, const unsigned long elapsedTime);

	bool DetectCollision(Vector* start, Vector* end);
	bool DetectCollisionWithCell(Vector forward, Vector pointIN, bool right, bool up, int col, int row, TCell* tEnd);

	bool CalculatePath( CList<Vector, Vector&> *cpList, Vector *start, Vector *target );
	int TracePath(CList<TCell, TCell&>* lst, Vector* target, CList<Vector, Vector&> *cpSortedList);

	TCell GetParentCellInPath(CList<TCell, TCell&>* lst, TCell* parentCell);

	TCell GetMaxCellCost(CList<TCell, TCell&>* lst);

	TCell GetNextCell(CList<TCell, TCell&>* lst);
	void InsertIntoList(CList<TCell, TCell&>* lst, CList<TCell, TCell&>* closedList, TCell* cell);
	
	int GetHeuristic(TCell* cell, TCell* target);
	void AddAllValidAdjacents(CList<TCell, TCell&>* lst, CList<TCell, TCell&>* closedList, TCell* cell, TCell* target);
};

#endif // !defined(AFX_SCENE_H__0487ACE8_DEC7_4305_8423_2102347AE9AC__INCLUDED_)
