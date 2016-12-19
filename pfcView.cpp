// pfcView.cpp : implementation of the CPfcView class
//

#include "stdafx.h"
#include "pfc.h"

#include "pfcDoc.h"
#include "pfcView.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPfcView

IMPLEMENT_DYNCREATE(CPfcView, CView)

BEGIN_MESSAGE_MAP(CPfcView, CView)
	//{{AFX_MSG_MAP(CPfcView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(SHOW_COHESION, OnShowCohesion)
	ON_UPDATE_COMMAND_UI(SHOW_COHESION, OnUpdateShowCohesion)
	ON_COMMAND(SHOW_COLLISION, OnShowCollision)
	ON_UPDATE_COMMAND_UI(SHOW_COLLISION, OnUpdateShowCollision)
	ON_COMMAND(SHOW_PATH, OnShowPath)
	ON_UPDATE_COMMAND_UI(SHOW_PATH, OnUpdateShowPath)
	ON_COMMAND(SHOW_GROUP, OnShowGroup)
	ON_UPDATE_COMMAND_UI(SHOW_GROUP, OnUpdateShowGroup)
	ON_COMMAND(SHOW_TOTARGET, OnShowToTotarget)
	ON_UPDATE_COMMAND_UI(SHOW_TOTARGET, OnUpdateShowToTotarget)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPfcView construction/destruction

CPfcView::CPfcView()
{
	m_PosLDown = (0,0);
	m_PosRDown = (0,0);
	m_ButtonLDown = false;	
	m_ButtonRDown = false;
}

CPfcView::~CPfcView()
{
}

BOOL CPfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPfcView drawing

void CPfcView::OnDraw(CDC* pDC)
{
	CPfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CPfcView diagnostics

#ifdef _DEBUG
void CPfcView::AssertValid() const
{
	CView::AssertValid();
}

void CPfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPfcDoc* CPfcView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPfcDoc)));
	return (CPfcDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPfcView message handlers

void CPfcView::OnSize(UINT nType, int cx, int cy) 
{
	paintClass.SetWindowSize(cx, cy);

	CView::OnSize(nType, cx, cy);
	Invalidate();
}

int CPfcView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nPixelFormat;					// Pixel format index
	m_hDC = ::GetDC(m_hWnd);			// Device context    

	static  PIXELFORMATDESCRIPTOR pfd= 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
			1,							// Version number
			PFD_DRAW_TO_WINDOW |		// Draw in window
			PFD_SUPPORT_OPENGL |        // Suport de crida a OpenGL a la finestra
			PFD_DOUBLEBUFFER |			// Doble buffer
			PFD_TYPE_RGBA,				// Requested An RGBA Format
			24,							// Color 24 bits
			0,0,0,0,0,0,				// Bits de color
			0,							// No Alpha Buffer
			0,							// Shift Bit Ignored
			0,							// No Accumulation Buffer
			0,0,0,0,					// Accumulation Bits Ignored
			16,							// 16 Bits Z-Buffer (Depth Buffer)
			0,							// No Stencil Buffer
			0,							// No Auxiliary Buffer
			PFD_MAIN_PLANE,				// Main plane
			0,							// Reserved
			0,0,0 						// Layer masks Ignored
	};
		
    // Choosing the pixel
	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	
	if (!nPixelFormat)
	{
		::MessageBox(0,"Error en el PixelFormat","Error",MB_OK|MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		
		return -1;
	}

	// Linking the pixel with the generation context
	if (!SetPixelFormat(m_hDC,nPixelFormat,&pfd))
	{
		::MessageBox(0,"Error en el SetPixelFormat","Error",MB_OK|MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		
		return -1;	
	}

	// Creating the generation context
	m_hRC = wglCreateContext(m_hDC);

	if (!m_hRC)
	{
		::MessageBox(0,"Error en el GL Rendering Context","Error",MB_OK|MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		
		return -1;
	}

	paintClass.InitializeOpenGL();

	return 0;
}

void CPfcView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

    wglMakeCurrent(m_hDC,m_hRC);

	// Bucle principal
	MainLoop();
	
	// Show content buffer
	SwapBuffers(m_hDC);

	// Permite multiples ventanas
	wglMakeCurrent(m_hDC,NULL);

	Invalidate();
}

void CPfcView::MainLoop()
{
	CPfcDoc *pDoc = GetDocument();
	unsigned long elapsedTime;
	
	elapsedTime = timeGetTime() - pDoc->lastUpdate;

	// evitamos grandes saltos
	if (elapsedTime > WAITING_TIME_UNTIL_RECALC * 4)
		elapsedTime = WAITING_TIME_UNTIL_RECALC * 2;

	// Recalculamos toda la escena solo cuando es preciso
	if ( elapsedTime > WAITING_TIME_UNTIL_RECALC && pDoc->sceneLoaded)
	{
		pDoc->lastUpdate = timeGetTime();
		pDoc->world->Update(elapsedTime);
		pDoc->world->stats.rpsAux++;

		// Actualizamos las demas vistas
		pDoc->UpdateAllViews(this);

		// Volvemos a permitir dibujar en la ventana actual
		CPaintDC dc(this);
	    wglMakeCurrent(m_hDC,m_hRC);
	}

	// Y pintamos la escena
	paintClass.Draw( pDoc->world, pDoc->sceneLoaded );

	// actualizamos frames per second cada 0.2 segundos
	if (pDoc->sceneLoaded && GetFPS( &(pDoc->world->stats.fps) ))
	{
		pDoc->world->stats.rps = pDoc->world->stats.rpsAux * 5;
		pDoc->world->stats.rpsAux = 0;
		WriteStatusBar( &(pDoc->world->stats) );
	}
}

// Metodo que se llama quando se repintan la ventanas no activas
void CPfcView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CPaintDC dc(this); // device context for painting

    wglMakeCurrent(m_hDC,m_hRC);

	// Pintamos la escena
	CPfcDoc *pDoc = GetDocument();
	paintClass.Draw( pDoc->world, pDoc->sceneLoaded );

	// Show content buffer
	SwapBuffers(m_hDC);

	// Permite multiples ventanas
	wglMakeCurrent(m_hDC,NULL);
}



// ----- MOUSE FUNCTIONS ---------------------------------------------------------------
// -------------------------------------------------------------------------------------

void CPfcView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ButtonLDown = true;
	m_PosLDown = point;

	// ni SIFT ni CTRL
	if (!GetAsyncKeyState(VK_SHIFT) && !GetAsyncKeyState(VK_CONTROL))
	{
		CPaintDC dc(this); // device context for painting

		wglMakeCurrent(m_hDC,m_hRC);

		paintClass.SelectGroup( point, GetDocument()->world );

		// Show content buffer
		SwapBuffers(m_hDC);

		// Permite multiples ventanas
		wglMakeCurrent(m_hDC,NULL);
	}


	CView::OnLButtonDown(nFlags, point);
	Invalidate();
}

void CPfcView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_ButtonLDown = false;
//	m_PosLDown = point;
	
	CView::OnLButtonUp(nFlags, point);
	Invalidate();
}

void CPfcView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_ButtonRDown = true;
	m_PosRDown = point;

	if (!GetAsyncKeyState(VK_SHIFT) && !GetAsyncKeyState(VK_CONTROL))
		paintClass.SetNewTarget( point, GetDocument()->world );

	
	CView::OnRButtonDown(nFlags, point);
	Invalidate();
}

void CPfcView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_ButtonRDown = false;
//	m_PosRDown = point;
	
	CView::OnRButtonUp(nFlags, point);
	Invalidate();
}

void CPfcView::OnMouseMove(UINT nFlags, CPoint point) 
{
	bool shiftPressed = false;			// desplazamiento lateral d la camara
	if (GetAsyncKeyState(VK_SHIFT))
	{
		shiftPressed = true;
//		// Desactivamos el seguimiento de grupos
//		paintClass.camera.groupFollowed = NULL;
	}

	bool ctrlPressed = false;			// rotacion de la camara
	if (GetAsyncKeyState(VK_CONTROL))
		ctrlPressed = true;

	CSize delta = m_PosLDown - point;

	if (!ctrlPressed && !shiftPressed && m_ButtonRDown)
		paintClass.SetNewFormDirection( point, GetDocument()->world );

	// - Rotacion de la camara -
	if (ctrlPressed && m_ButtonLDown)
	{
		m_PosLDown = point;

		// Rotation left-right
		paintClass.camera.RotateY((GLfloat) delta.cx / 3);
		// Rotation up-down
		paintClass.camera.RotateX((GLfloat) delta.cy / 3);
	}

	// - Traslacion de la camara -
	if (shiftPressed && m_ButtonLDown)
	{
		m_PosLDown = point;

		// ---- Translation up-down -----
		paintClass.camera.MoveForward((GLfloat) delta.cy);
		// ---- Translation left-right --
		paintClass.camera.StrafeRight((GLfloat) - delta.cx);
	}


	// - Zoom -
	if (ctrlPressed && m_ButtonRDown)
	{
		// Recovering the amount of mouse movement
		CSize zoomincr = m_PosRDown - point;
		m_PosRDown = point;

		// Camera zoom
//		paintClass.camera.RotateZ((GLfloat) zoomincr.cx / 3);
		paintClass.camera.AddZoom( zoomincr.cy);
	}

	CView::OnMouseMove(nFlags, point);
	Invalidate();
}

// ----- KEYBOARD FUNCTIONS ------------------------------------------------------------
// -------------------------------------------------------------------------------------

void CPfcView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
		case 'Q':
			GetDocument()->world->Initialize();
			break;

		case 'C':
			paintClass.camera.isFollowing = !paintClass.camera.isFollowing;
			break;

		case VK_SPACE:
			GetDocument()->world->RegroupSelectedGroup();
			break;


		default:
			break;
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ----- OTHER STUFF FUNCTIONS ---------------------------------------------------------
// -------------------------------------------------------------------------------------

void CPfcView::OnCancelMode() 
{
	CView::OnCancelMode();
	
	Invalidate();
}

BOOL CPfcView::OnEraseBkgnd(CDC* pDC) 
{
	// Evitamos el parpadeo (es openGL quien controla el refresco)

	return true;
	//return CView::OnEraseBkgnd(pDC);
}


void CPfcView::OnFileOpen() 
{
	CFileDialog openFile (TRUE, NULL, NULL,
					      OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST|OFN_HIDEREADONLY ,
						  "DGS Files(*.dgs)|*.dgs|All Files (*.*)|*.*||",NULL);

	if (openFile.DoModal() != IDOK)
		return;                 // stay with old data file
    else
	{
		BeginWaitCursor();
		GetDocument()->world->ReadScene( openFile.GetPathName() );
		EndWaitCursor();
		Scene* sc = GetDocument()->world->scene;
		Vector center = Vector( (sc->maxLeft + sc-> maxRight) / 2,
								(sc->maxBottom + sc->maxTop) / 2, 0);

		paintClass.camera.SetFocus( center );
		GetDocument()->sceneLoaded = true;
	}

	GetDocument()->SetTitle(openFile.GetPathName());	// titulo de la ventana
//	pDoc->SetPathName(fileName);						// path del archivo asociado a la ventana

    Invalidate();
}

void CPfcView::OnShowCohesion() 
{
	paintClass.viewProperties.showCohesionForce = !paintClass.viewProperties.showCohesionForce;

	Invalidate();
}


void CPfcView::OnUpdateShowCohesion(CCmdUI* pCmdUI) 
{
	if(paintClass.viewProperties.showCohesionForce)
		 pCmdUI->SetCheck(1);
	else
		 pCmdUI->SetCheck(0);
}

void CPfcView::OnShowCollision() 
{
	paintClass.viewProperties.showCollisionForce = !paintClass.viewProperties.showCollisionForce;

	Invalidate();
}

void CPfcView::OnUpdateShowCollision(CCmdUI* pCmdUI) 
{
	if(paintClass.viewProperties.showCollisionForce)
		 pCmdUI->SetCheck(1);
	else
		 pCmdUI->SetCheck(0);	
}

void CPfcView::OnShowPath() 
{
	paintClass.viewProperties.showPath = !paintClass.viewProperties.showPath;

	Invalidate();	
}

void CPfcView::OnUpdateShowPath(CCmdUI* pCmdUI) 
{
	if(paintClass.viewProperties.showPath)
		 pCmdUI->SetCheck(1);
	else
		 pCmdUI->SetCheck(0);
}

void CPfcView::OnShowGroup() 
{
	paintClass.viewProperties.showGroup = !paintClass.viewProperties.showGroup;

	Invalidate();	
}

void CPfcView::OnUpdateShowGroup(CCmdUI* pCmdUI) 
{
	if(paintClass.viewProperties.showGroup)
		 pCmdUI->SetCheck(1);
	else
		 pCmdUI->SetCheck(0);	
}

void CPfcView::OnShowToTotarget() 
{
	paintClass.viewProperties.showTargetForce = !paintClass.viewProperties.showTargetForce;

	Invalidate();		
}

void CPfcView::OnUpdateShowToTotarget(CCmdUI* pCmdUI) 
{
	if(paintClass.viewProperties.showTargetForce)
		 pCmdUI->SetCheck(1);
	else
		 pCmdUI->SetCheck(0);
}

bool CPfcView::GetFPS(int *fps)
{
   // Statics are used so that values exsist after the function has
   // ended so the next time it is called the values are still there.

	static char text[25] = {0};
	static int FPS = 0.0f;
	float nextSecond = 0.0f;
	static float prevSecond = 0.0f;

	// Add to the frames per second every time this function is called.
	FPS++;

	// Get the second
	nextSecond = GetTickCount() * 0.001f;				

	// 0.2 sec passed
	if(nextSecond - prevSecond > 0.2f)
	{
		prevSecond = nextSecond;

		*fps = FPS * 5;

		FPS = 0;

		// Updated
		return true;
	}

	// Not Updated
	return false;
}

// Write statistics in the status bar
void CPfcView::WriteStatusBar(Tstats *stats)
{
	// We use the paintClass method because we want the ACTIVE CV3DDoc,
	// with CV3DView::GetDocument() we would had the data of the window where the mouse is placed
	CPfcDoc *pDoc;
	
	//= paintClass.GetDocument();
	pDoc = GetDocument();

	char aux[200];

	if (pDoc->sceneLoaded)
		sprintf(aux, "Groups: %d | Characters: %d | Scene: %d x %d | fps: %d | ups: %d",
			stats->nGroups, stats->nCharacters, pDoc->world->scene->height, pDoc->world->scene->width, stats->fps, stats->rps);
	else
		sprintf(aux, "Groups: %d | Characters: %d | Scene: - x - | fps: %d | ups: %d",
			stats->nGroups, stats->nCharacters, stats->fps, stats->rps);

	CMainFrame *pChild = (CMainFrame*)AfxGetMainWnd();
	CStatusBar *StatusBar = pChild->GetStatusBar();
	StatusBar->SetPaneText(0, aux);
}
