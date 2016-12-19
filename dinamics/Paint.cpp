// Paint.cpp: implementation of the Paint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Paint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Paint::Paint()
{
	this->Initialize();
}

Paint::~Paint()
{

}

void Paint::Initialize()
{
	camera.Initialize();
	camera.SetFocus( Vector(0, 0, 0) );
	
	windowWidth	= windowHeight = 0;

	updateOpenGL	= true;

	viewProperties.showCollisionForce = true;
	viewProperties.showCohesionForce = true;
	viewProperties.showTargetForce = true;
	viewProperties.showDirection = false;
	viewProperties.showPath = true;
	viewProperties.showGroup = true;
}

void Paint::InitializeOpenGL()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Habilita z-buffer
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);

	// Habilita transparencias
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// To avoid divide by 0
	if (windowHeight == 0)
		windowHeight = 1;

	glViewport(0,0,windowWidth,windowHeight);

	// Mejor renderizado
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Iluminacion
	InitializeIllumination();
}

void Paint::InitializeIllumination()
{
	// Inicializacion
	static GLfloat lightAmbient[]		= { 0.2f, 0.2f, 0.2f, 1.0f };
	static GLfloat lightDiffuse[]		= { 0.6f, 0.7f, 0.7f, 1.0f};
	
	static GLfloat lightPosition[]= { -2.0f, -2.0f, 1.0f, 0.0f }; // luz direccional
	// -----------------------

			// Turn on backface culling
			glFrontFace(GL_CCW);
 			glCullFace(GL_BACK);

			glPolygonMode	( GL_FRONT, GL_FILL );

			glShadeModel	( GL_SMOOTH );
			glEnable		( GL_NORMALIZE );

//			glLightModeli	( GL_LIGHT_MODEL_TWO_SIDE,	GL_FALSE );
			glEnable		( GL_LIGHTING );

			glLightModelfv	( GL_LIGHT_MODEL_AMBIENT,	lightAmbient );
			glLightfv		( GL_LIGHT0, GL_AMBIENT,	lightAmbient );
			glLightfv		( GL_LIGHT0, GL_DIFFUSE,	lightDiffuse );
			glLightfv		( GL_LIGHT0, GL_POSITION,	lightPosition );
			glEnable		( GL_LIGHT0 );

			// Enable de las propiedades del material para iluminacion
			glEnable		( GL_COLOR_MATERIAL );
			glColorMaterial	( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
}

void Paint::SetWindowSize(int wTMP, int hTMP)
{
	windowWidth = wTMP;
	windowHeight = hTMP;

	updateOpenGL = true;
}

void Paint::Draw(World* world, bool sceneLoaded)
{
	// - Setting the illumination & other stuff -------
	if (updateOpenGL)
	{
		InitializeOpenGL();
		updateOpenGL = false;
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0f * windowWidth / windowHeight, 0.1 ,5000);

	// - Setting the camera ------------
	if (camera.groupFollowed != NULL && camera.isFollowing)
		camera.FollowGroup();
	
	camera.Render();

	glGetIntegerv (GL_VIEWPORT, viewport);
	glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);

	if (sceneLoaded)
	{
		DrawScene(world->scene);
		DrawGroups(&world->enemies);
		DrawGroups(&world->groups);
	}
}

void Paint::DrawCells(Scene *scene)
{

	float cellWidth = scene->cellSize;
	int i;

	glColor4f(0.3f, 0.2f, 0.2f, 0.2f);
	glBegin(GL_LINES);

	for ( i=1; i<scene->width; i++ )
	{
		// Linias verticales
		glVertex3f( i*cellWidth + scene->maxLeft, scene->maxTop, 0 );
		glVertex3f( i*cellWidth + scene->maxLeft, scene->maxBottom, 0 );
	}

	for ( i=1; i<scene->height; i++ )
	{
		// Linias horitzontal
		glVertex3f( scene->maxLeft, -i*cellWidth + scene->maxTop, 0 );
		glVertex3f( scene->maxRight, -i*cellWidth + scene->maxTop, 0 );
	}
	glEnd();
}

void Paint::DrawScene(Scene* scene)
{
	int wall;
	Vector center;

	// terreno interior
	glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_POLYGON);
		glNormal3f( 0.0, 0.0, 1.0f);
		glVertex3f( scene->maxLeft,		scene->maxTop,		0);
		glVertex3f( scene->maxRight,	scene->maxTop,		0);
		glVertex3f( scene->maxRight,	scene->maxBottom,	0);
		glVertex3f( scene->maxLeft,		scene->maxBottom,	0);
	glEnd();

	glClear( GL_DEPTH_BUFFER_BIT );

	// pintamos la rejilla
	DrawCells(scene);

	glClear( GL_DEPTH_BUFFER_BIT );

	// pintamos las paredes
	glColor3f(0.2, 0.2, 0.2);
	for (int row=0; row<scene->height; row++)
	{
		for (int col=0; col<scene->width; col++)
		{
			wall = scene->GetWallType(col, row);
			if ( wall == WALL_FULL )
			{
				glBegin(GL_QUADS);
					glNormal3f(0.0, 0.0, 1.0);
					glVertex3f( col * scene->cellSize,		-row * scene->cellSize,		WALL_HEIGHT);
					glVertex3f((col+1) * scene->cellSize,	-row * scene->cellSize,		WALL_HEIGHT);
					glVertex3f((col+1) * scene->cellSize,	-(row+1) * scene->cellSize, WALL_HEIGHT);
					glVertex3f( col * scene->cellSize,		-(row+1) * scene->cellSize, WALL_HEIGHT);
				glEnd();

				glBegin(GL_QUADS);
					glNormal3f( -1.0, 0.0, 0.0);
					glVertex3f( col * scene->cellSize,		-row * scene->cellSize,		0);
					glVertex3f( col * scene->cellSize,		-row * scene->cellSize,		WALL_HEIGHT);
					glVertex3f( col * scene->cellSize,		-(row+1) * scene->cellSize, WALL_HEIGHT);
					glVertex3f( col * scene->cellSize,		-(row+1) * scene->cellSize, 0);

					glNormal3f( 0.0, -1.0, 0.0);
					glVertex3f( col * scene->cellSize,		-(row+1) * scene->cellSize, 0);
					glVertex3f( col * scene->cellSize,		-(row+1) * scene->cellSize, WALL_HEIGHT);
					glVertex3f( (col+1) * scene->cellSize,	-(row+1) * scene->cellSize, WALL_HEIGHT);
					glVertex3f( (col+1) * scene->cellSize,	-(row+1) * scene->cellSize, 0);

					glNormal3f( 1.0, 0.0, 0.0);
					glVertex3f( (col+1) * scene->cellSize,	-(row+1) * scene->cellSize, 0);
					glVertex3f( (col+1) * scene->cellSize,	-(row+1) * scene->cellSize, WALL_HEIGHT);
					glVertex3f( (col+1) * scene->cellSize,	-row * scene->cellSize,		WALL_HEIGHT);
					glVertex3f( (col+1) * scene->cellSize,	-row * scene->cellSize,		0);

					glNormal3f( 0.0, 1.0, 0.0);
					glVertex3f( (col+1) * scene->cellSize,	-row * scene->cellSize,		0);
					glVertex3f( (col+1) * scene->cellSize,	-row * scene->cellSize,		WALL_HEIGHT);
					glVertex3f( col * scene->cellSize,		-row * scene->cellSize,		WALL_HEIGHT);
					glVertex3f( col * scene->cellSize,		-row * scene->cellSize,		0);
				glEnd();
			}
			else if ( wall == WALL_BOTTOM_RIGHT)
			{
				center.x = (col+1) * scene->cellSize;
				center.y = -(row+1) * scene->cellSize;
				center.z = 0;

				DrawCircleArch( center, scene->cellSize, PI/2, PI, 10 );
			}
			else if ( wall == WALL_BOTTOM_LEFT)
			{
				center.x = col * scene->cellSize;
				center.y = -(row+1) * scene->cellSize;
				center.z = 0;

				DrawCircleArch( center, scene->cellSize, 0, PI/2, 10 );
			}
			else if ( wall == WALL_TOP_RIGHT)
			{
				center.x = (col+1) * scene->cellSize;
				center.y = -row * scene->cellSize;
				center.z = 0;

				DrawCircleArch( center, scene->cellSize, PI, (3*PI)/2, 10 );
			}
			else if ( wall == WALL_TOP_LEFT)
			{
				center.x = col * scene->cellSize;
				center.y = -row * scene->cellSize;
				center.z = 0;

				DrawCircleArch( center, scene->cellSize, (3*PI)/2, 2*PI, 10 );
			}
		}
	} // Fin escenario
}

// Pinta un semicirculo en center de radio radius entre los angulos start y end con una resolucion step
void Paint::DrawCircleArch(Vector center, float radius, float start, float end, float step )
{
	float angle = end;
	float inc = (end - start) / step;
	float angleNext = angle - inc;

	// Tapa superior
	glBegin(GL_TRIANGLES);
		glNormal3f( 0.0, 0.0, 1.0);
		while (angle > start)
		{
			glVertex3d(center.x, center.y, WALL_HEIGHT);
			glVertex3d(center.x + (cos(angle)*radius), center.y + (sin(angle)*radius), WALL_HEIGHT);
			glVertex3d(center.x + (cos(angleNext)*radius), center.y + (sin(angleNext)*radius), WALL_HEIGHT);
			angle = angleNext;
			angleNext -= inc;
		}
	glEnd();

	// pared curvada
	angle = start;
	glBegin(GL_QUAD_STRIP);
		while (angle < end)
		{
			glNormal3f( (cos(angle)*radius), (sin(angle)*radius), 0.0f );
			glVertex3d(center.x + (cos(angle)*radius), center.y + (sin(angle)*radius), 0);
			glVertex3d(center.x + (cos(angle)*radius), center.y + (sin(angle)*radius), WALL_HEIGHT);
			angle += inc;
		}
		glVertex3d(center.x + (cos(end)*radius), center.y + (sin(end)*radius), 0);
		glVertex3d(center.x + (cos(end)*radius), center.y + (sin(end)*radius), WALL_HEIGHT);
 	glEnd();

	// paredes laterales
	glBegin(GL_QUADS);
		glNormal3f( sin(start)*radius, -cos(start)*radius, 0.0f );
		glVertex3d(center.x + (cos(start)*radius), center.y + (sin(start)*radius), WALL_HEIGHT);
		glVertex3d(center.x + (cos(start)*radius), center.y + (sin(start)*radius), 0);
		glVertex3d(center.x, center.y, 0);
		glVertex3d(center.x, center.y, WALL_HEIGHT);

		glNormal3f( -sin(end)*radius, cos(end)*radius, 0.0f );
		glVertex3d(center.x + (cos(end)*radius), center.y + (sin(end)*radius), WALL_HEIGHT);
		glVertex3d(center.x + (cos(end)*radius), center.y + (sin(end)*radius), 0);
		glVertex3d(center.x, center.y, 0);
		glVertex3d(center.x, center.y, WALL_HEIGHT);
 	glEnd();

}

void Paint::DrawCircle(Vector center, float radius, float step)
{
	float angle = 0;
	float inc = 2*PI / step;

	glBegin(GL_LINE_LOOP);

	while (angle < 2*PI)
	{
		glVertex3d(center.x + (cos(angle)*radius), center.y + (sin(angle)*radius), 0);
		angle += inc;
	}

	glEnd();
}

void Paint::DrawGroups(CList<Group, Group&>* groups)
{
	Group* gr;
	Character* ch;

	GLfloat colorGO[3]		= {0.0f, 1.0f, 0.0f};
	GLfloat colorFORM[3]	= {0.0f, 0.0f, 1.0f};
	GLfloat colorREGROUP[3]	= {1.0f, 0.0f, 1.0f};
	GLfloat colorFLEE[3]	= {1.0f, 0.0f, 0.0f};
	GLfloat colorENEMIE[3]	= {0.9f, 0.5f, 0.0f};
	GLfloat objectColor[4]	= {1.0f, 1.0f, 1.0f, 0.5f};

	POSITION posCH;
	POSITION pos = groups->GetHeadPosition();

	Vector target;

	while (pos != NULL)
	{
		gr = &groups->GetNext(pos);
		target = gr->GetTarget();

		if (gr->GetState() == GROUP_GO)
		{
			objectColor[0] = colorGO[0];
			objectColor[1] = colorGO[1];
			objectColor[2] = colorGO[2];
		}
		else if (gr->GetState() == GROUP_FORM)
		{
			objectColor[0] = colorFORM[0];
			objectColor[1] = colorFORM[1];
			objectColor[2] = colorFORM[2];
		}
		else if (gr->GetState() == GROUP_REGROUP)
		{
			objectColor[0] = colorREGROUP[0];
			objectColor[1] = colorREGROUP[1];
			objectColor[2] = colorREGROUP[2];
		}
		else if (gr->GetState() == GROUP_FLEE)
		{
			objectColor[0] = colorFLEE[0];
			objectColor[1] = colorFLEE[1];
			objectColor[2] = colorFLEE[2];
		}
		else if (gr->GetState() == GROUP_ENEMIE)
		{
			objectColor[0] = colorENEMIE[0];
			objectColor[1] = colorENEMIE[1];
			objectColor[2] = colorENEMIE[2];
		}
		objectColor[3] = 1.0f;

		// -- Cuerpo del objeto --
		glColor4fv(objectColor);

		// esfera
		if (viewProperties.showGroup)
		{
			glPushMatrix();
				glTranslatef( gr->compensedCenter.x, gr->compensedCenter.y, gr->dispersion );
				glutSolidSphere( 0.3, 10, 10 );
			glPopMatrix();
		}

		// Pintamos sin luces
		glDisable(GL_LIGHTING);

		// ruta
		if (viewProperties.showPath)
		{
			if (gr->GetState() == GROUP_ENEMIE)
				glColor4f(0.8f, 0.2f, 0.2f, 0.4f);
			else
				glColor4f(0.0f, 0.6f, 0.7f, 0.4f);

			glBegin(GL_LINE_STRIP);
				//glVertex3f( gr->center.x, gr->center.y, gr->center.z );
				for (int i=gr->nCheckPoints; i>=0; i--)
					glVertex3f( gr->checkPoints[i].x, gr->checkPoints[i].y, gr->checkPoints[i].z );
			glEnd();
		}

		// -- Objetivo --
		if (gr->GetState() != GROUP_ENEMIE)
		{
			glColor4f(1.0, 1.0, 1.0, 0.2);
			Vector front = (gr->formAligment.GetNormalized() * CHARACTER_RADIUS * 2);
			Vector auxRight = gr->formAligment.CrossProduct( &Vector(0,0,1) );

			Vector right = gr->waitingTarget - (front * COS_45) + (auxRight.GetNormalized() * CHARACTER_RADIUS *2 * COS_45);
			Vector left  = gr->waitingTarget - (front * COS_45) - (auxRight.GetNormalized() * CHARACTER_RADIUS *2 * COS_45);
			front += gr->waitingTarget;
		
			glBegin(GL_LINE_STRIP);
				glVertex3f( front.x, front.y, front.z );
				glVertex3f( right.x, right.y, right.z );
				glVertex3f( left.x,  left.y,  left.z );
				glVertex3f( front.x, front.y, front.z );
			glEnd();
			
//			glBegin(GL_LINES);
//				glVertex3f( gr->center.x, gr->center.y, 0 );			
//				glVertex3f( gr->waitingTarget.x, gr->waitingTarget.y, gr->waitingTarget.z );			
//			glEnd();
		}


		// Volvems a activar las luces
		glEnable(GL_LIGHTING);

		// Pintamos los individuos del grupo
		posCH = gr->characters.GetHeadPosition();
		while (posCH != NULL)
		{
			ch = &gr->characters.GetNext(posCH);

			objectColor[3] = 0.2f;
			glColor4fv(objectColor);

			// linias d union entre el centro y cada individuo
			if (viewProperties.showGroup)
			{
				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
					glVertex3f( gr->compensedCenter.x, gr->compensedCenter.y, gr->dispersion );			
					glVertex3f( ch->position.x, ch->position.y, ch->position.z );
				glEnd();
				glEnable(GL_LIGHTING);
			}

			DrawCharacter( ch , gr );
		}
	}

}

void Paint::DrawCharacter(Character* ch, Group* pGroup)
{
	GLfloat colorGO[3]		= {0.0f, 1.0f, 0.0f};
	GLfloat colorFORM[3]	= {0.0f, 0.0f, 1.0f};
	GLfloat colorREGROUP[3]	= {1.0f, 0.0f, 1.0f};
	GLfloat colorFLEE[3]	= {1.0f, 0.0f, 0.0f};
	GLfloat colorENEMIE[3]	= {0.9f, 0.5f, 0.0f};
	GLfloat objectColor[4]	= {1.0f, 1.0f, 1.0f, 0.5f};


	// Pintamos sin luces
	glDisable(GL_LIGHTING);

 	// vector de direccion
	if (viewProperties.showDirection)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
		glBegin(GL_LINES);
			glVertex3f( ch->position.x, ch->position.y, ch->position.z );
			glVertex3f( ch->position.x + ch->forward.x * ch->speed,
						ch->position.y + ch->forward.y * ch->speed,
						ch->position.z + ch->forward.z * ch->speed);
		glEnd();
	}


	// chekpoint
	if (viewProperties.showPath)
	{
		glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
		glBegin(GL_LINES);
			glVertex3f( ch->position.x, ch->position.y, ch->position.z );
			glVertex3f( pGroup->checkPoints[ch->checkPoint].x, 
						pGroup->checkPoints[ch->checkPoint].y,
						pGroup->checkPoints[ch->checkPoint].z );
		glEnd();

		if (ch->state == CHARACTER_REGROUP)
		{
			glColor4f(1.0f, 0.2f, 1.0f, 0.4f);
			glBegin(GL_LINE_STRIP);
				glVertex3f( ch->position.x, ch->position.y, ch->position.z );
				for (int i=ch->regroupCP; i>=0; i--)
					glVertex3f( ch->regroupCPs[i].x, ch->regroupCPs[i].y, ch->regroupCPs[i].z );
			glEnd();
		}
	}

	// fuerza de cohesion
 	if (viewProperties.showCohesionForce)
	{
		glColor4f(1.0f, 0.2f, 1.0f, 0.4f);
		glBegin(GL_LINES);
			glVertex3f( ch->position.x, ch->position.y, ch->position.z );
			glVertex3f( ch->position.x + ch->toGroupCenterForce.x * FORCE_VIEWER,
						ch->position.y + ch->toGroupCenterForce.y * FORCE_VIEWER,
						ch->position.z + ch->toGroupCenterForce.z * FORCE_VIEWER );
		glEnd();
	}

	// fuerza de repulsion con paredes y otros individuos
 	if (viewProperties.showCollisionForce)
	{
		glColor4f(1.0f, 0.2f, 0.0f, 0.4f);
		glBegin(GL_LINES);
			glVertex3f( ch->position.x, ch->position.y, ch->position.z );
			glVertex3f( ch->position.x + ch->collisionForce.x * FORCE_VIEWER,
						ch->position.y + ch->collisionForce.y * FORCE_VIEWER,
						ch->position.z + ch->collisionForce.z * FORCE_VIEWER );
		glEnd();
	}

	// fuerza hacia el objetivo
 	if (viewProperties.showTargetForce)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
		glBegin(GL_LINES);
			glVertex3f( ch->position.x, ch->position.y, ch->position.z );
			glVertex3f( ch->position.x + ch->toTargetForce.x * FORCE_VIEWER,
						ch->position.y + ch->toTargetForce.y * FORCE_VIEWER,
						ch->position.z + ch->toTargetForce.z * FORCE_VIEWER );
		glEnd();
	}

	// miedo (linia de doble anchura)
	glColor4f(0.0f, 0.6f, 0.7f, 1.0f);
	glLineWidth(2.0);
	glBegin(GL_LINES);
		glVertex3f( ch->position.x, ch->position.y, ch->position.z );
		glVertex3f( ch->position.x + (ch->up.x * ch->fear * 0.1),
					ch->position.y + (ch->up.y * ch->fear * 0.1),
					ch->position.z + (ch->up.z * ch->fear * 0.1) );
	glEnd();
	glLineWidth(1.0);

	// circulo de seleccion si esta seleccionado
	if (pGroup->selected)
	{
		glColor4f(1.0, 1.0, 0.0f, 0.7f);
		DrawCircle(ch->position, CHARACTER_RADIUS, 20);
	}

	// Volvems a activar las luces
	glEnable(GL_LIGHTING);

	// color del cuerpo
	if (ch->state == CHARACTER_GO)
		glColor3fv(colorGO);
	else if (ch->state == CHARACTER_FORM)
		glColor3fv(colorFORM);
	else if (ch->state == CHARACTER_REGROUP)
		glColor3fv(colorREGROUP);
	else if (ch->state == CHARACTER_FLEE)
		glColor3fv(colorFLEE);
	else if (ch->state == CHARACTER_ENEMIE)
		glColor3fv(colorENEMIE);

	DrawModel(ch);
}


void Paint::DrawModel(Character *ch)
{
	Vector front = (ch->forward.GetNormalized() * CHARACTER_RADIUS);
	Vector right = ch->position - (front * COS_45) + (ch->right.GetNormalized() * CHARACTER_RADIUS * COS_45);
	Vector left  = ch->position - (front * COS_45) - (ch->right.GetNormalized() * CHARACTER_RADIUS * COS_45);
	front += ch->position;
	
	glBegin(GL_TRIANGLES);
		glNormal3f( 0.0f, 0.0f, -1.0f );
		glVertex3f( front.x, front.y, 0 );
		glVertex3f( left.x,  left.y,  0 );
		glVertex3f( right.x, right.y, 0 );

		glNormal3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( front.x, front.y, CHARACTER_HEIGHT );
		glVertex3f( right.x, right.y, CHARACTER_HEIGHT );
		glVertex3f( left.x,  left.y,  CHARACTER_HEIGHT );
	glEnd();

	Vector n;
	n.z = 0;

	glBegin(GL_QUADS);
		// normal de la cara front - right
		n.x = - (right.y - front.y);
		n.y = (right.x - front.x);
		glNormal3f( n.x, n.y, n.z);

		glVertex3f( front.x, front.y, CHARACTER_HEIGHT );
		glVertex3f( front.x, front.y, 0 );
		glVertex3f( right.x, right.y, 0 );
		glVertex3f( right.x, right.y, CHARACTER_HEIGHT );

		// normal de la cara right left
		n.x = - (left.y - right.y);
		n.y = (left.x - right.x);
		glNormal3f( n.x, n.y, n.z);

		glVertex3f( right.x, right.y, CHARACTER_HEIGHT );
		glVertex3f( right.x, right.y, 0 );
		glVertex3f( left.x, left.y, 0 );
		glVertex3f( left.x, left.y, CHARACTER_HEIGHT );

		// normal de la cara front - left
		n.x = (left.y - front.y);
		n.y = - (left.x - front.x);
		glNormal3f( n.x, n.y, n.z);

		glVertex3f( left.x, left.y, CHARACTER_HEIGHT );
		glVertex3f( left.x, left.y, 0 );
		glVertex3f( front.x, front.y, 0 );
		glVertex3f( front.x, front.y, CHARACTER_HEIGHT );

	glEnd();
}


bool Paint::SelectGroup(CPoint point, World* world)
{
	GLuint selectionBuffer[2048];
	int hits, id_group = 0;

	Group* gr;
	Character* ch;

	POSITION posCH;
	POSITION pos = world->groups.GetHeadPosition();

//	glGetIntegerv(GL_VIEWPORT, viewport);

	// Setting the buffer for selection
	glSelectBuffer(2048, selectionBuffer);

	// We change de render mode (no drawing into the screen, just for selection)
	glRenderMode(GL_SELECT);
	
	// Initialize the names stack and put something
	glInitNames();
	glPushName(0);

	// Select the projection matrix, push it & reset it.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	
	glLoadIdentity();

	// Acotamos la matriz con un margen de 5 pixels
	gluPickMatrix(point.x, (windowHeight-point.y), 5, 5, viewport);

	// Apply perspective matrix - same in Paint() -
	gluPerspective(60.0, 1.0f * windowWidth / windowHeight, 1.0, 5000.0);

	// Select the Modelview matrix
	glMatrixMode(GL_MODELVIEW);

	// - A normal render for all the points ----------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// - Setting the camera ------------
	glLoadIdentity();
	camera.Render();

	while (pos != NULL)
	{
		gr = &world->groups.GetNext(pos);

		// Pintamos los individuos del grupo
		posCH = gr->characters.GetHeadPosition();
		while (posCH != NULL)
		{
			ch = &gr->characters.GetNext(posCH);

			glLoadName(id_group);

			glPushMatrix();
				glTranslatef( ch->position.x, ch->position.y, ch->position.z );
				// COn pintar una esfera sobra para la seleccion
				glutWireSphere( CHARACTER_RADIUS, 10, 10 );
			glPopMatrix();
		}
		id_group++;
	}

	// ------------------------------------------------------------------

	// Get back to the modelview matrix mode
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	// Going back to normal render mode
	hits = glRenderMode(GL_RENDER);

	// Seleccion succeeded
	if (hits > 0)
	{
		// Se deseleciona todo
		world->UnselectAllGroups();		

		pos = world->groups.FindIndex(selectionBuffer[3]);
		gr = &world->groups.GetAt(pos);
		
		gr->selected = true;
		camera.groupFollowed = gr;

		return true;
	}

	return false;
}

void Paint::SetNewTarget(CPoint point, World* world)
{
	Group* gr;
	POSITION pos = world->groups.GetHeadPosition();

	GLdouble wx, wy, wz;
	Vector direction; // = camera.GetDirection();

	gluUnProject ( (GLdouble)point.x, (GLdouble)(windowHeight - point.y),
					0.5, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );

	direction.x = wx;
	direction.y = wy;
	direction.z = wz;

	Vector position = camera.GetEyePosition();
	direction = position - direction;

	float incZ = position.z/direction.z;

	// Nos aseguramos que el target este en la parte frontal de la camara
	if (incZ > 0)
	{
		Vector target = position - (direction * incZ);
		while (pos != NULL)
		{
			gr = &world->groups.GetNext(pos);

			if (gr->selected)
				gr->SetTarget( target, world->scene);
		}
	}
}

void Paint::SetNewFormDirection(CPoint point, World* world)
{
	Group* gr;
	POSITION pos = world->groups.GetHeadPosition();

	GLdouble wx, wy, wz;
	Vector direction; // = camera.GetDirection();

	gluUnProject ( (GLdouble)point.x, (GLdouble)(windowHeight - point.y),
					0.5, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );

	direction.x = wx;
	direction.y = wy;
	direction.z = wz;

	Vector position = camera.GetEyePosition();
	direction = position - direction;

	float incZ = position.z/direction.z;

	// Nos aseguramos que el target este en la parte frontal de la camara
	if (incZ > 0)
	{
		Vector target = position - (direction * incZ);
		while (pos != NULL)
		{
			gr = &world->groups.GetNext(pos);

			if (gr->selected)
				gr->SetDirection( target );
		}
	}
}
