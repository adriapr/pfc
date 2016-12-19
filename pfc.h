// pfc.h : main header file for the PFC application
//

#if !defined(AFX_PFC_H__DF3C9344_D142_41E3_A20F_57BA419F8E78__INCLUDED_)
#define AFX_PFC_H__DF3C9344_D142_41E3_A20F_57BA419F8E78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "afxtempl.h"		// CList
#include "dinamics/Vector.h"
#include "resource.h"
#include <mmsystem.h>		// control de tiempo
#include <math.h>			// funciones matematicas
#include <gl/glu.h>			// OpenGL
#include <gl/glut.h>		// Toolkit de OpenGL
#include <cstdlib>			// para los randoms

#define CHARACTER_RADIUS 1.0f
#define CHARACTER_HEIGHT 0.2f

#define WAITING_TIME_UNTIL_RECALC 20	// Tiempo entre 2 updates
#define DELTA_MOVEMENT 0.001f			// segundos por unidad de tiempo (1 ms = 0.001 sg)
#define PI 3.14159268f

/////////////////////////////////////////////////////////////////////////////
// CPfcApp:
// See pfc.cpp for the implementation of this class
//

class CPfcApp : public CWinApp
{
public:
	CPfcApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPfcApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CPfcApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PFC_H__DF3C9344_D142_41E3_A20F_57BA419F8E78__INCLUDED_)
