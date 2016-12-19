// pfcView.h : interface of the CPfcView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PFCVIEW_H__E77E3D8E_62D3_44E6_A408_B9F38834733D__INCLUDED_)
#define AFX_PFCVIEW_H__E77E3D8E_62D3_44E6_A408_B9F38834733D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dinamics/Paint.h"

class CPfcView : public CView
{
protected: // create from serialization only
	CPfcView();
	DECLARE_DYNCREATE(CPfcView)

// Attributes
public:
	CPfcDoc* GetDocument();

	// Clase para el render
	Paint paintClass;

	// Control de ventanas
	HGLRC m_hRC; // Generation Context
    HDC m_hDC;   // Device Context

	// Control del raton
	CPoint m_PosLDown, m_PosRDown; 
	bool m_ButtonLDown, m_ButtonRDown;

	// Control del tiempo
//	unsigned long lastUpdate;

// Operations
public:

	void MainLoop();

	bool GetFPS(int *fps);
	void WriteStatusBar(Tstats *stats);
	void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPfcView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPfcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPfcView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileOpen();
	afx_msg void OnShowCohesion();
	afx_msg void OnUpdateShowCohesion(CCmdUI* pCmdUI);
	afx_msg void OnShowCollision();
	afx_msg void OnUpdateShowCollision(CCmdUI* pCmdUI);
	afx_msg void OnShowPath();
	afx_msg void OnUpdateShowPath(CCmdUI* pCmdUI);
	afx_msg void OnShowGroup();
	afx_msg void OnUpdateShowGroup(CCmdUI* pCmdUI);
	afx_msg void OnShowToTotarget();
	afx_msg void OnUpdateShowToTotarget(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in pfcView.cpp
inline CPfcDoc* CPfcView::GetDocument()
   { return (CPfcDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PFCVIEW_H__E77E3D8E_62D3_44E6_A408_B9F38834733D__INCLUDED_)
