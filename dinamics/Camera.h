// Camera.h: interface for the Camera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERA_H__25B7BE40_82ED_40D8_9A49_B9BB923C19B9__INCLUDED_)
#define AFX_CAMERA_H__25B7BE40_82ED_40D8_9A49_B9BB923C19B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../pfc.h"

class Group;

#define MIN_DISTANCE 1
#define V3D_PI 3.141592653589793238462643383279502884197169399375105820974944592308
#define PIdiv180 (V3D_PI/180.0)

class Camera
{
private:
	
	Vector viewDir;
	Vector rightVector;	
	Vector upVector;
	Vector position;

	double rotatedX, rotatedY, rotatedZ;	

	// distnace between the eye and the focus
	double distance;

public:
	Group *groupFollowed;
	bool isFollowing;

public:
	Camera(); 	//inits the values (Position: (0|0|0) Target: (0|0|-1) )
	~Camera();
	void Initialize(void);

	void SetFocus(Vector focus);
	Vector GetFocus();
	Vector GetEyePosition();
	Vector GetDirection();

	void Render ( void );

	void RotateX ( double Angle );
	void RotateY ( double Angle );
	void RotateZ ( double Angle );

	void Move ( Vector Direction );
	void MoveForward ( double Distance );
	void MoveUpward ( double Distance );
	void StrafeRight ( double Distance );

	void AddZoom(double delta);

	void FollowGroup();
};


#endif // !defined(AFX_CAMERA_H__25B7BE40_82ED_40D8_9A49_B9BB923C19B9__INCLUDED_)
