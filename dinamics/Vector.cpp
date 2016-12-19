// Punto3D.cpp: implementation of the CPunt3D class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Vector.h"
#include "../pfc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Vector::Vector()
{

}


Vector::~Vector()
{

}


Vector::Vector(double x, double y, double z)
{
	this->x = x; 
	this->y = y;
	this->z = z;
}


// Assignment methods
Vector& Vector::operator =(const Vector& miCP)
{

	if (&miCP == this) 
		return *this;
	
	this->x = miCP.x; 
	this->y = miCP.y;
	this->z = miCP.z;
	
	return *this;
}

Vector& Vector::operator +=(const Vector& miCP)
{
	this->x += miCP.x; 
	this->y += miCP.y;
	this->z += miCP.z;
	
	return *this;
}

Vector& Vector::operator -=(const Vector& miCP)
{
	this->x -= miCP.x; 
	this->y -= miCP.y;
	this->z -= miCP.z;
	
	return *this;
}

Vector& Vector::operator *=(const double numero) 
{
	this->x *= numero; 
	this->y *= numero;
	this->z *= numero;
	
	return *this;
}

Vector& Vector::operator /=(const double numero)
{
	this->x /= numero; 
	this->y /= numero;
	this->z /= numero;
	
	return *this;
}


// Arithmetic methods
Vector Vector::operator +(const Vector& miCP)
{
	return Vector( this->x + miCP.x, 
	                 this->y + miCP.y,
	                 this->z + miCP.z
				   );
}


Vector Vector::operator -(const Vector& miCP)
{
	return Vector( this->x - miCP.x, 
	                 this->y - miCP.y,
	                 this->z - miCP.z
				   );
}


double Vector::operator *(const Vector& miCP)
{
	return ( this->x * miCP.x + 
	         this->y * miCP.y + 
	         this->z * miCP.z
		   );
}


Vector Vector::operator *(const double numero)
{
	return Vector( this->x * numero, 
	                 this->y * numero, 
	                 this->z * numero
				   );
}

Vector Vector::operator ^(const Vector& miCP)
{
	return Vector( this->y * miCP.z - this->z * miCP.y, 
	                 this->x * miCP.z - this->z * miCP.x, 
	                 this->x * miCP.y - this->y * miCP.x
				   );
}

Vector Vector::operator /(const double numero)
{
	return Vector( this->x / numero, 
	                 this->y / numero, 
	                 this->z / numero
				   );
}

bool Vector::operator ==(const Vector& miCP)
{

	if (&miCP == this) 
		return true;
	
	if (this->x == miCP.x && this->y == miCP.y && this->z == miCP.z)
		return true;
	
	return false;
}

double Vector::GetLength( )
{
	return (double)(sqrt(SQR(this->x)+SQR(this->y)+SQR(this->z)));
}

Vector Vector::GetNormalized( )
{
	Vector res;
	double l = this->GetLength();
	if (l == 0.0f) return NULL_VECTOR;
	res.x = this->x / l;
	res.y = this->y / l;
	res.z = this->z / l;
	return res;
}

double Vector::DotProduct( Vector * v)
{
	return (this->x + v->x) + (this->y + v->y) + (this->z + v->z);
}

Vector Vector::CrossProduct ( Vector * v )
{
	Vector resVector;
	resVector.x = this->y*v->z - this->z*v->y;
	resVector.y = this->z*v->x - this->x*v->z;
	resVector.z = this->x*v->y - this->y*v->x;

	return resVector;
}

Vector Vector::RotateOnFloor(double radians)
{
	this->x = ( this->x * cos( radians ) ) - ( this->y * sin(radians) );
	this->y = ( this->x * sin( radians ) ) + ( this->y * cos(radians) );

	return *this;
}
