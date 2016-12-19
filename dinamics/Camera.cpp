// camera.cpp: implementation of the Camera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../pfc.h"
#include "camera.h"

#include "Group.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Camera::Camera()
{
	this->Initialize();
}

Camera::~Camera() {
}

void Camera::Initialize()
{
	position = Vector (0.0, 0.0, 0.0);
	viewDir = Vector( 0.0, 1.0, 0.0);
	rightVector = Vector (1.0, 0.0, 0.0);
	upVector = Vector (0.0, 0.0, 1.0);

	rotatedX = rotatedY = rotatedZ = 0.0;

	distance = 100;

	// 45º Down
	RotateX(-60);

	groupFollowed = NULL;
	isFollowing = false;
}

// -- Render ------------------------------------------------------------------
void Camera::Render( void )
{
	// The point where the camera (eye) is located:
	Vector eyePosition = position - (viewDir * distance);

	gluLookAt(	eyePosition.x,  eyePosition.y,  eyePosition.z,
				position.x,		position.y,		position.z,
				upVector.x,		upVector.y,		upVector.z);
}


// -- Rotation Methods ---------------------------------------------------

// Rotation up - down
void Camera::RotateX (double angle)
{
	rotatedX += angle;

//	if (rotatedX > 0)
//		rotatedX = 0;
//
//	else
	//Rotate viewdir around the right vector:
	viewDir = (viewDir*cos(angle*PIdiv180)	+ upVector*sin(angle*PIdiv180)).GetNormalized();

	//now compute the new UpVector (by cross product)
//	upVector = upVector.CrossProduct(&viewDir, &rightVector) * -1;
}

// Rotation left - right
void Camera::RotateY (double angle)
{
	rotatedY += angle;
	
	//Rotate viewdir around the up vector:
	viewDir = (viewDir*cos(angle*PIdiv180)	- rightVector*sin(angle*PIdiv180)).GetNormalized();

	//now compute the new RightVector (by cross product)
	rightVector = viewDir.CrossProduct( &upVector );
}

// Rotation of the normal vector
void Camera::RotateZ (double angle)
{
	rotatedZ += angle;
	
	//Rotate viewdir around the right vector:
	rightVector = (rightVector*cos(angle*PIdiv180)	+ upVector*sin(angle*PIdiv180)).GetNormalized();

	//now compute the new UpVector (by cross product)
//	upVector = upVector.CrossProduct(&viewDir, &rightVector)*-1;
}


// -- Movement methods -----------------------------------

void Camera::Move (Vector direction) {
	position = position + direction;
}

void Camera::MoveForward( double delta )
{
	float tmpZ = position.z;
	position = position + (viewDir * delta);
	position.z = tmpZ;
}

void Camera::StrafeRight ( double delta )
{
	float tmpZ = position.z;
	position = position + (rightVector * delta);
	position.z = tmpZ;
}

void Camera::MoveUpward( double delta )
{
	position = position + (upVector * delta);
}

// Get the poit where the camera is looking at
Vector Camera::GetFocus()
{
	return position;
}

// Set the poit where the camera is looking at
void Camera::SetFocus(Vector focus)
{
	position = focus;
}

// Set new Zoom
void Camera::AddZoom(double delta)
{
	distance += delta;

	if (distance < MIN_DISTANCE)
		distance = MIN_DISTANCE;
}

Vector Camera::GetEyePosition()
{
	return position - (viewDir * distance);
}

Vector Camera::GetDirection()
{
	return this->viewDir;
}

void Camera::FollowGroup()
{
	if (groupFollowed != NULL)
		this->SetFocus( groupFollowed->center );
}
