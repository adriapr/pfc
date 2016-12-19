// Punto3D.h: interface for the CPunt3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUNTO3D_H__BCF97A06_1C10_45F7_B262_FB9E2AA96262__INCLUDED_)
#define AFX_PUNTO3D_H__BCF97A06_1C10_45F7_B262_FB9E2AA96262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SQR(x) (x*x)
#define NULL_VECTOR Vector(0.0f,0.0f,0.0f)

class Vector  
{
public:

	double x;
    double y;
	double z;

public: 
	Vector();
	Vector(double x,double y,double z);
	virtual ~Vector();

	// Operadores de asignacion
	Vector& operator =(const Vector& miCP);
	Vector& operator +=(const Vector& miCP);
	Vector& operator -=(const Vector& miCP);
	Vector& operator *=(const double numero); // Producto de Vector por un escalar
	Vector& operator /=(const double numero); // Division de Vector por un escalar

	// Operadores aritméticos
	Vector operator +(const Vector& miCP);
	Vector operator -(const Vector& miCP);
	double operator *(const Vector& miCP);	// Producto escalar de dos Vector
	Vector operator *(const double numero);	// Producto de un Vector por un escalar
	Vector operator ^(const Vector& miCP);	// Producto vectorial de dos Vector
	Vector operator /(const double numero);	// Producto de un Vector por un escalar

	bool operator ==(const Vector& miCP);	// Comparacion


	Vector Vector::GetNormalized( );
	double Vector::GetLength( );
	double Vector::DotProduct( Vector * v);
	Vector Vector::CrossProduct(Vector * v);
	Vector Vector::RotateOnFloor(double radians);
};



#endif // !defined(AFX_PUNTO3D_H__BCF97A06_1C10_45F7_B262_FB9E2AA96262__INCLUDED_)
