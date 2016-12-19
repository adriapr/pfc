// pfcDoc.h : interface of the CPfcDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PFCDOC_H__36B9013E_45BD_48B2_B3FA_35187EF2D6FB__INCLUDED_)
#define AFX_PFCDOC_H__36B9013E_45BD_48B2_B3FA_35187EF2D6FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pfc.h"
#include "dinamics/World.h"

class CPfcDoc : public CDocument
{
protected: // create from serialization only
	CPfcDoc();
	DECLARE_DYNCREATE(CPfcDoc)

// Attributes
public:

	World* world;
	unsigned long lastUpdate;
	bool sceneLoaded;		// Flag q indica si hay un escenario cargado

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPfcDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPfcDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPfcDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PFCDOC_H__36B9013E_45BD_48B2_B3FA_35187EF2D6FB__INCLUDED_)
