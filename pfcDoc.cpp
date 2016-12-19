// pfcDoc.cpp : implementation of the CPfcDoc class
//

#include "stdafx.h"
#include "pfc.h"

#include "pfcDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPfcDoc

IMPLEMENT_DYNCREATE(CPfcDoc, CDocument)

BEGIN_MESSAGE_MAP(CPfcDoc, CDocument)
	//{{AFX_MSG_MAP(CPfcDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPfcDoc construction/destruction

CPfcDoc::CPfcDoc()
{
	lastUpdate = timeGetTime() + 500;
	sceneLoaded		= false;
}

CPfcDoc::~CPfcDoc()
{
}

BOOL CPfcDoc::OnNewDocument()
{
	world = new World();

	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPfcDoc serialization

void CPfcDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPfcDoc diagnostics

#ifdef _DEBUG
void CPfcDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPfcDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPfcDoc commands
