// Paint.h: interface for the Paint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAINT_H__A77F8A82_22EB_4496_84E7_1BFD6F914B26__INCLUDED_)
#define AFX_PAINT_H__A77F8A82_22EB_4496_84E7_1BFD6F914B26__INCLUDED_

#include "../pfc.h"
#include "World.h"
#include "Camera.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define COS_45 0.70710678118654752440084436210485

// Multiplicado de fuerza para el visionado
#define FORCE_VIEWER 3
#define WALL_HEIGHT 1

typedef struct {
	bool showCollisionForce;
	bool showCohesionForce;
	bool showTargetForce;
	bool showDirection;
	bool showPath;
	bool showGroup;
} SViewProperties;

class Paint  
{
public:

	Camera camera;

	bool updateOpenGL;		// Flag para pedir resetear parametros de iniciacion de openGL

	SViewProperties viewProperties;

private:

//	bool someGroupSelected;

	int windowWidth, windowHeight;		// Ancho y alto de ventana

	// Para la seleccion y la asignacion de nuevos targets
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];

public:

	Paint();
	virtual ~Paint();
	void Initialize();
	void InitializeOpenGL();
	void InitializeIllumination();

	void SetWindowSize(int wTMP, int hTMP);

	bool SelectGroup(CPoint, World* world);
	void SetNewTarget(CPoint, World* world);
	void SetNewFormDirection(CPoint, World* world);


	void Draw(World* world, bool sceneLoaded);
	void DrawCells(Scene *scene);

	void DrawModel(Character *ch);
	void DrawGroups(CList<Group, Group&>* groups);
	void DrawCharacter(Character* ch, Group* pGroup);
//	void DrawEnemies(CList<Group, Group&>* enemies);
	void DrawScene(Scene* scene);
	void DrawCircleArch(Vector center, float radius, float start, float end, float step);
	void DrawCircle(Vector center, float radius, float step);
};

#endif // !defined(AFX_PAINT_H__A77F8A82_22EB_4496_84E7_1BFD6F914B26__INCLUDED_)
