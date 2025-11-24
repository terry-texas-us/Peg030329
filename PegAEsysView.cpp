// PegAEsysView.cpp : implementation of the CPegView class

#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Dde.h"
#include "DdeGItms.h"
#include "DlgSetAngle.h"
#include "DlgSetLength.h"
#include "DlgViewZoom.h"
#include "DlgSetScale.h"
#include "FileBitmap.h"
#include "Grid.h"
#include "UnitsString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char szLeftMouseDown[60]  = "";
char szRightMouseDown[60] = ""; 
char szLeftMouseUp[60]	= "{13}"; //default return
char szRightMouseUp[60] = "{27}"; //default escape

extern CTMat tmEditSeg;

// CPegView

IMPLEMENT_DYNCREATE(CPegView, CView)

// CPegView construction/destruction

CPegView::CPegView()
{
	m_bViewOdometer = false;
	m_bViewPenWidths = true;
	m_bViewTrueTypeFonts = true;
	m_bViewBackgroundImage = false;
	m_bViewWireframe = false;
	m_bViewRendered = false;

	m_bPlot = false;
	m_dPlotScaleFactor = 1.;
}

CPegView::~CPegView()
{
}

BOOL CPegView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CView::PreCreateWindow(cs))
	{
		return FALSE;
	}
	cs.lpszClass = 
		AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			AfxGetApp()->LoadCursor(IDR_MAINFRAME),
			::CreateSolidBrush(crHotCols[0]));
	
	return (cs.lpszClass != NULL) ? TRUE : FALSE;
}

BEGIN_MESSAGE_MAP(CPegView, CView)
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_3DVIEWS_TOP, On3dViewsTop)
	ON_COMMAND(ID_3DVIEWS_BOTTOM, On3dViewsBottom)
	ON_COMMAND(ID_3DVIEWS_LEFT, On3dViewsLeft)
	ON_COMMAND(ID_3DVIEWS_RIGHT, On3dViewsRight)
	ON_COMMAND(ID_3DVIEWS_FRONT, On3dViewsFront)
	ON_COMMAND(ID_3DVIEWS_BACK, On3dViewsBack)
	ON_COMMAND(ID_3DVIEWS_ISOMETRIC, On3dViewsIsometric)
	ON_COMMAND(ID_3DVIEWS_AXONOMETRIC, On3dViewsAxonometric)
	ON_COMMAND(ID_3DVIEWS_OBLIQUE, On3dViewsOblique)
	ON_COMMAND(ID_3DVIEWS_PERSPECTIVE, On3dViewsPerspective)
	ON_COMMAND(ID_BACKGROUNDIMAGE_LOAD, OnBackgroundImageLoad)
	ON_COMMAND(ID_BACKGROUNDIMAGE_REMOVE, OnBackgroundImageRemove)
	ON_COMMAND(ID_CAMERA_ROTATELEFT, OnCameraRotateLeft)
	ON_COMMAND(ID_CAMERA_ROTATERIGHT, OnCameraRotateRight)
	ON_COMMAND(ID_CAMERA_ROTATEUP, OnCameraRotateUp)
	ON_COMMAND(ID_CAMERA_ROTATEDOWN, OnCameraRotateDown)
	ON_COMMAND(ID_CURSOR_DEFAULT, OnCursorDefault)
	ON_COMMAND(ID_CURSOR_SYMBOL, OnCursorSymbol)
	ON_COMMAND(ID_CURSOR_CROSSHAIR, OnCursorCrosshair)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_HELP_KEY, OnHelpKey)
	ON_COMMAND(ID_PRIM_PERPJUMP, OnPrimPerpJump)
	ON_COMMAND(ID_PRIM_SNAPTO, OnPrimSnapTo)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_DOWN, OnRelativeMovesEngDown)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_DOWNROTATE, OnRelativeMovesEngDownRotate)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_IN, OnRelativeMovesEngIn)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_LEFT, OnRelativeMovesEngLeft)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_LEFTROTATE, OnRelativeMovesEngLeftRotate)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_OUT, OnRelativeMovesEngOut)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_RIGHT, OnRelativeMovesEngRight)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_RIGHTROTATE, OnRelativeMovesEngRightRotate)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_UP, OnRelativeMovesEngUp)
	ON_COMMAND(ID_RELATIVEMOVES_ENG_UPROTATE, OnRelativeMovesEngUpRotate)
	ON_COMMAND(ID_RELATIVEMOVES_DOWN, OnRelativeMovesDown)
	ON_COMMAND(ID_RELATIVEMOVES_DOWNROTATE, OnRelativeMovesDownRotate)
	ON_COMMAND(ID_RELATIVEMOVES_IN, OnRelativeMovesIn)
	ON_COMMAND(ID_RELATIVEMOVES_LEFT, OnRelativeMovesLeft)
	ON_COMMAND(ID_RELATIVEMOVES_LEFTROTATE, OnRelativeMovesLeftRotate)
	ON_COMMAND(ID_RELATIVEMOVES_OUT, OnRelativeMovesOut)
	ON_COMMAND(ID_RELATIVEMOVES_RIGHT, OnRelativeMovesRight)
	ON_COMMAND(ID_RELATIVEMOVES_RIGHTROTATE, OnRelativeMovesRightRotate)
	ON_COMMAND(ID_RELATIVEMOVES_UP, OnRelativeMovesUp)
	ON_COMMAND(ID_RELATIVEMOVES_UPROTATE, OnRelativeMovesUpRotate)
	ON_COMMAND(ID_SETUP_DIMLENGTH, OnSetupDimLength)
	ON_COMMAND(ID_SETUP_DIMANGLE, OnSetupDimAngle)
	ON_COMMAND(ID_SETUP_SCALE, OnSetupScale)
	ON_COMMAND(ID_SETUP_UNITS, OnSetupUnits)
	ON_COMMAND(ID_SETUP_CONSTRAINTS_AXIS, OnSetupConstraintsAxis)
	ON_COMMAND(ID_SETUP_CONSTRAINTS_GRID, OnSetupConstraintsGrid)
	ON_COMMAND(ID_SETUP_MOUSEBUTTONS, OnSetupMouseButtons)
	ON_COMMAND(ID_VIEW_TRUETYPEFONTS, OnViewTrueTypeFonts)
	ON_COMMAND(ID_VIEW_PENWIDTHS, OnViewPenWidths)
	ON_COMMAND(ID_VIEW_ODOMETER, OnViewOdometer)
	ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_COMMAND(ID_VIEW_ZOOM, OnViewZoom)
	ON_COMMAND(ID_VIEW_WINDOW, OnViewWindow)
	ON_COMMAND(ID_VIEW_WIREFRAME, OnViewWireframe)
	ON_COMMAND(ID_VIEW_RENDERED, OnViewRendered)
	ON_COMMAND(ID_VIEW_PARAMETERS, OnViewParameters)
	ON_COMMAND(ID_VIEW_SOLID, OnViewSolid)
	ON_COMMAND(ID_VIEW_LIGHTING, OnViewLighting)
	ON_COMMAND(ID_WINDOW_NORMAL, OnWindowNormal)
	ON_COMMAND(ID_WINDOW_BEST, OnWindowBest)
	ON_COMMAND(ID_WINDOW_LAST, OnWindowLast)
	ON_COMMAND(ID_WINDOW_SHEET, OnWindowSheet)
	ON_COMMAND(ID_WINDOW_ZOOMIN, OnWindowZoomIn)
	ON_COMMAND(ID_WINDOW_ZOOMOUT, OnWindowZoomOut)
	ON_COMMAND(ID_WINDOW_PAN, OnWindowPan)
	ON_COMMAND(ID_WINDOW_PAN_LEFT, OnWindowPanLeft)
	ON_COMMAND(ID_WINDOW_PAN_RIGHT, OnWindowPanRight)
	ON_COMMAND(ID_WINDOW_PAN_UP, OnWindowPanUp)
	ON_COMMAND(ID_WINDOW_PAN_DOWN, OnWindowPanDown)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ODOMETER, OnUpdateViewOdometer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRUETYPEFONTS, OnUpdateViewTrueTypeFonts)
	ON_COMMAND(ID_VIEW_BACKGROUNDIMAGE, OnViewBackgroundImage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BACKGROUNDIMAGE, OnUpdateViewBackgroundImage)
	ON_UPDATE_COMMAND_UI(ID_BACKGROUNDIMAGE_LOAD, OnUpdateBackgroundimageLoad)
	ON_UPDATE_COMMAND_UI(ID_BACKGROUNDIMAGE_REMOVE, OnUpdateBackgroundimageRemove)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RENDERED, OnUpdateViewRendered)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME, OnUpdateViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PENWIDTHS, OnUpdateViewPenwidths)
	ON_COMMAND(ID_FILE_PLOT_QUARTER, OnFilePlotQuarter)
	ON_COMMAND(ID_FILE_PLOT_HALF, OnFilePlotHalf)
	ON_COMMAND(ID_FILE_PLOT_FULL, OnFilePlotFull)
END_MESSAGE_MAP()

void CPegView::ModelViewPopActive()
{
	if (!m_mvs.IsEmpty()) 
	{
		m_mvActive = m_mvs.RemoveTail();
	}
	m_mvActive.BuildTransformMatrix();
}

void CPegView::ModelViewPushActive()
{
	m_mvs.AddTail(m_mvActive);
}
void CPegView::ModelViewAdjustWnd(double& dUMin, double& dVMin, double& dUMax, double& dVMax, double dRatio)
{
	double dAspectRatio = GetAspectRatio();
	
	double dUExt = fabs(dUMax - dUMin);
	double dVExt = fabs(dVMax - dVMin);
	
	double dXExt = 0.;
	double dYExt = 0.;
	
	double dScale = 1. - (GetWidthInInches() / dUExt) / dRatio;

	if (fabs(dScale) > DBL_EPSILON)
	{
		dXExt = dScale * dUExt;
		dYExt = dScale * dVExt;
	}
	if (dUExt <= DBL_EPSILON || dVExt / dUExt > dAspectRatio) // Extend x boundaries
		dXExt += (dVExt / dAspectRatio - dUExt) * .5;
	else														// Extend y boundaries
		dYExt += (dUExt * dAspectRatio - dVExt) * .5;
			
	dUMin -= dXExt;
	dUMax += dXExt;
	dVMin -= dYExt;
	dVMax += dYExt;
}
// CPegView drawing
void CPegView::BackgroundImageDisplay(CDC* pDC)
{
	if (m_bViewBackgroundImage && (HBITMAP) m_bmBackgroundImage != NULL)
	{
		int iWidDst = int(m_vpActive.GetWidth());
		int iHgtDst = int(m_vpActive.GetHeight());
		
		BITMAP bm;
		m_bmBackgroundImage.GetBitmap(&bm);
		CDC dcMem;
		dcMem.CreateCompatibleDC(NULL);
		CBitmap* pBitmap = dcMem.SelectObject(&m_bmBackgroundImage);
		CPalette* pPalette = pDC->SelectPalette(&m_palBackgroundImage, FALSE);
		pDC->RealizePalette();

		CPnt ptTargetActive = ModelViewGetTarget();
		CPnt ptTargetOver = OverGetTarget();
		double dU = ptTargetActive[0] - ptTargetOver[0];
		double dV = ptTargetActive[1] - ptTargetOver[1];
		
		// Determine the region of the bitmap to tranfer to display
		CRect rcWnd;
		rcWnd.left =   Round((ModelViewGetUMin() - OverGetUMin() + dU) / OverGetUExt() * (double) bm.bmWidth);
		rcWnd.top =    Round((1. - (ModelViewGetVMax() - OverGetVMin() + dV) / OverGetVExt()) * (double) bm.bmHeight);
		rcWnd.right =  Round((ModelViewGetUMax() - OverGetUMin() + dU) / OverGetUExt() * (double) bm.bmWidth);
		rcWnd.bottom = Round((1. - (ModelViewGetVMin() - OverGetVMin() + dV) / OverGetVExt()) * (double) bm.bmHeight);
	
		int iWidSrc = rcWnd.Width();
		int iHgtSrc = rcWnd.Height();
		
		pDC->StretchBlt(0, 0, iWidDst, iHgtDst, &dcMem, (int) rcWnd.left, (int) rcWnd.top, iWidSrc, iHgtSrc, SRCCOPY);
		
		dcMem.SelectObject(pBitmap);
		pDC->SelectPalette(pPalette, FALSE);
	}
}
void CPegView::ViewportPopActive()
{
	if (!m_vps.IsEmpty()) 
	{
		m_vpActive = m_vps.RemoveTail();
	}
	opengl::Viewport(0, 0, int(m_vpActive.GetWidth()), int(m_vpActive.GetHeight()));
}
void CPegView::ViewportPushActive()
{
	m_vps.AddTail(m_vpActive);
}
void CPegView::OnDraw(CDC* pDC)
{
	CPegDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rcUpdRect;
	
	if (m_bViewRendered)
	{
		opengl::Clear();
		pDoc->LayersDisplayAll(this, 0);
	}
	else
	{
		BackgroundImageDisplay(pDC);
		grid::Display(this, pDC);

		pDoc->LayersDisplayAll(this, pDC);
	}
	app.StatusLineDisplay();
	app.ModeLineDisplay();
}
// CPegView printing
void CPegView::OnFilePlotFull()
{
	m_bPlot = true;
	m_dPlotScaleFactor = 1.;
	CView::OnFilePrint();
}
void CPegView::OnFilePlotHalf()
{
	m_bPlot = true;
	m_dPlotScaleFactor = .5;
	CView::OnFilePrint();
}
void CPegView::OnFilePlotQuarter()
{
	m_bPlot = true;
	m_dPlotScaleFactor = .25;
	CView::OnFilePrint();
}
void CPegView::OnFilePrint()
{
	m_bPlot = false;
	m_dPlotScaleFactor = 1.;
	CView::OnFilePrint();
}
///<summary>Determines the number of pages for 1 to 1 print</summary>
UINT CPegView::NumPages(CDC* pDC, double dScaleFactor, UINT& nHorzPages, UINT& nVertPages)
{
	CPnt ptMin;
	CPnt ptMax;
	CTMat tm;

	GetDocument()->GetExtents(ptMin, ptMax, tm);

	double dHorzSizeInches = pDC->GetDeviceCaps(HORZSIZE) / 25.4;
	double dVertSizeInches = pDC->GetDeviceCaps(VERTSIZE) / 25.4; 
	
	nHorzPages = Round(((ptMax[0] - ptMin[0]) * dScaleFactor / dHorzSizeInches) + .5);
	nVertPages = Round(((ptMax[1] - ptMin[1]) * dScaleFactor / dVertSizeInches) + .5);

	return nHorzPages * nVertPages;
}
void CPegView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnPrepareDC(pDC, pInfo);
	
	if (pDC->IsPrinting())
	{
		if (m_bPlot)
		{
			double dHorzSizeInches = (pDC->GetDeviceCaps(HORZSIZE) / 25.4) / m_dPlotScaleFactor;
			double dVertSizeInches = (pDC->GetDeviceCaps(VERTSIZE) / 25.4) / m_dPlotScaleFactor; 
			
			ModelViewInitialize();
			ModelViewSetWnd(0., 0., dHorzSizeInches, dVertSizeInches);
			
			UINT nHorzPages;
			UINT nVertPages;
			
			NumPages(pDC, m_dPlotScaleFactor, nHorzPages, nVertPages);

			double dX = ((pInfo->m_nCurPage - 1) % nHorzPages) * dHorzSizeInches;
			double dY = ((pInfo->m_nCurPage - 1) / nHorzPages) * dVertSizeInches;		
			
			ModelViewSetTarget(CPnt(dX, dY, 0.));
			ModelViewSetEye(ZDIR);
		}
		else
		{
		}
	}
}
BOOL CPegView::OnPreparePrinting(CPrintInfo* pInfo)
{
	if (m_bPlot)
	{
		CPrintInfo pi;
		if (AfxGetApp()->GetPrinterDeviceDefaults(&pi.m_pPD->m_pd))
		{
			HDC hDC = pi.m_pPD->m_pd.hDC;
			if (hDC == NULL)
			{
				hDC = pi.m_pPD->CreatePrinterDC();
			}
			if (hDC != NULL)
			{
				UINT nHorzPages;
				UINT nVertPages;
				CDC dc;
				dc.Attach(hDC);
				pInfo->SetMaxPage(NumPages(&dc, m_dPlotScaleFactor, nHorzPages, nVertPages));
				::DeleteDC(dc.Detach());
			}
		}
	}
	return DoPreparePrinting(pInfo);
}
void CPegView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	ViewportPushActive();
	ModelViewPushActive();
	
	int iHorzRes = pDC->GetDeviceCaps(HORZRES);
	int iVertRes = pDC->GetDeviceCaps(VERTRES);
	
	ViewportSet(iHorzRes, iVertRes);

	double dHorzSizeInches = pDC->GetDeviceCaps(HORZSIZE) / 25.4;
	double dVertSizeInches = pDC->GetDeviceCaps(VERTSIZE) / 25.4; 
	
	SetDeviceWidthInInches(dHorzSizeInches);
	SetDeviceHeightInInches(dVertSizeInches);
	
	if (m_bPlot)
	{
		UINT nHorzPages;
		UINT nVertPages;
		pInfo->SetMaxPage(NumPages(pDC, m_dPlotScaleFactor, nHorzPages, nVertPages));
	}
	else
	{
		double dAspectRatio = double(iVertRes) / double(iHorzRes);			
		ModelViewAdjustWnd(dAspectRatio);
	}
}
void CPegView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	ModelViewPopActive();
	ViewportPopActive();
}
void CPegView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CDC* pDC = GetDC();

	int iPrimState = 0;
	int iDrawMode = 0;

	if ((lHint & CPegDoc::HINT_SAFE) == CPegDoc::HINT_SAFE)
	{
		iPrimState = pstate.Save();
	}
	if ((lHint & CPegDoc::HINT_ERASE) == CPegDoc::HINT_ERASE)
	{
		iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
	}
	if ((lHint & CPegDoc::HINT_TRAP) == CPegDoc::HINT_TRAP)
	{
		CPrim::SpecPenColor() = trapsegs.PenColor();
	}
	switch (lHint)
	{
		case CPegDoc::HINT_PRIM:
		case CPegDoc::HINT_PRIM_SAFE:
		case CPegDoc::HINT_PRIM_ERASE_SAFE:
			((CPrim*) pHint)->Display(this, pDC);
			break;

		case CPegDoc::HINT_SEG:
		case CPegDoc::HINT_SEG_SAFE:
		case CPegDoc::HINT_SEG_ERASE_SAFE:
		case CPegDoc::HINT_SEG_SAFE_TRAP:
		case CPegDoc::HINT_SEG_ERASE_SAFE_TRAP:
			((CSeg*) pHint)->Display(this, pDC);
			break;

		case CPegDoc::HINT_SEGS:
		case CPegDoc::HINT_SEGS_SAFE:
		case CPegDoc::HINT_SEGS_SAFE_TRAP:
		case CPegDoc::HINT_SEGS_ERASE_SAFE_TRAP:
			((CSegs*) pHint)->Display(this, pDC);
			break;

		case CPegDoc::HINT_LAYER:
		case CPegDoc::HINT_LAYER_ERASE:
			((CLayer*) pHint)->Display(this, pDC);
			break;

		default:
            CView::OnUpdate(pSender, lHint, pHint);
	}
	if ((lHint & CPegDoc::HINT_TRAP) == CPegDoc::HINT_TRAP)
	{
		CPrim::SpecPenColor() = 0;
	}
	if ((lHint & CPegDoc::HINT_ERASE) == CPegDoc::HINT_ERASE)
	{
		pstate.SetROP2(pDC, iDrawMode);
	}
	if ((lHint & CPegDoc::HINT_SAFE) == CPegDoc::HINT_SAFE)
	{
		pstate.Restore(pDC, iPrimState);
	}
}
// CPegView diagnostics
#ifdef _DEBUG
void CPegView::AssertValid() const
{
	CView::AssertValid();
}
void CPegView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CPegDoc* CPegView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPegDoc)));
	return (CPegDoc*)m_pDocument;
}
#endif //_DEBUG

void CPegView::DoCameraRotate(int iDir)
{
	CVec vN = ModelViewGetEye() - ModelViewGetTarget();
	vN.Normalize();

	CVec vU(ModelViewGetVwUp() ^ vN);
	vU.Normalize();
	
	CVec vV(vN ^ vU);
	vU.Normalize();
	
	CPnt ptEye;

	switch (iDir) 
	{
		case ID_CAMERA_ROTATELEFT:
		{
			ptEye = Pnt_RotAboutArbPtAndAx(ModelViewGetEye(), ModelViewGetTarget(), vV, - 10. * RADIAN);	
			break;
		}
		case ID_CAMERA_ROTATERIGHT:
			ptEye = Pnt_RotAboutArbPtAndAx(ModelViewGetEye(), ModelViewGetTarget(), vV, 10. * RADIAN);	
			break;

		case ID_CAMERA_ROTATEUP:
			ptEye = Pnt_RotAboutArbPtAndAx(ModelViewGetEye(), ModelViewGetTarget(), vU, - 10. * RADIAN);	
			break;

		case ID_CAMERA_ROTATEDOWN:
			ptEye = Pnt_RotAboutArbPtAndAx(ModelViewGetEye(), ModelViewGetTarget(), vU, 10. * RADIAN);	
			break;
	}
	ModelViewSetEye(ptEye);
	ModelViewSetVwUp(vV);
	InvalidateRect(NULL, TRUE);
}
void CPegView::DoWindowPan(double dRatio)
{
	double dUExt = GetWidthInInches() / dRatio;
	double dVExt = GetHeightInInches() / dRatio;
	
	ModelViewSetCenteredWnd(dUExt, dVExt);

	CPnt ptCur = app.CursorPosGet();

	CVec vDirection = ModelViewGetDirection();
	CPnt ptTarget = ModelViewGetTarget();
	
	line::IntersectionWithPln(ptCur, vDirection, ptTarget, vDirection, &ptCur);
	
	ModelViewSetTarget(ptCur);
	ModelViewSetEye(vDirection);

	app.CursorPosSet(ptCur);
	InvalidateRect(NULL, TRUE);
}
void CPegView::DoWindowPan0(int dir)
{
	CPnt pt = app.CursorPosGet();
	
	double dRatio = GetWidthInInches() / ModelViewGetUExt();
	double d = 1. / dRatio;
	
	switch (dir)
	{
		case ID_WINDOW_PAN_LEFT: 				  
			pt[0] -= d;
			break;
		
		case ID_WINDOW_PAN_RIGHT:
			pt[0] += d;
			break;
		
		case ID_WINDOW_PAN_UP: 	   
			pt[1] += d;
			break;
		
		case ID_WINDOW_PAN_DOWN: 					
			pt[1] -= d;
			break;
	}
	ModelViewSetTarget(pt);
	ModelViewSetEye(ModelViewGetDirection());
	
	app.CursorPosSet(pt);
	InvalidateRect(NULL, TRUE);
}	

// CPegView message handlers

void CPegView::OnSetupScale()
{
	CDlgSetScale dlg;
	dlg.m_dScale = app.GetScale();
	if (dlg.DoModal() == IDOK)
	{
		app.SetScale(dlg.m_dScale);
		dde::PostAdvise(dde::ScaleInfo);
		app.StatusLineDisplay(Scale);
	}
}
void CPegView::OnLButtonDown(UINT, CPoint)
{				  
	DoCustomMouseClick(szLeftMouseDown);
}
void CPegView::OnLButtonUp(UINT, CPoint)
{		   
	DoCustomMouseClick(szLeftMouseUp);
}
void CPegView::OnRButtonDown(UINT, CPoint)
{		   
	DoCustomMouseClick(szRightMouseDown);
}
void CPegView::OnRButtonUp(UINT, CPoint)
{		 
	DoCustomMouseClick(szLeftMouseUp);
}
void CPegView::OnMouseMove(UINT, CPoint pnt0)
{
	app.SetModeCursor(app.m_iModeId);

	CDC* pDC = GetDC();
		
	DisplayOdometer();
			
	if (app.RubberBandingGetType() == Lines) 
	{
		CPoint pnt[2];
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);

		pnt[0] = app.m_pntGinStart;
		pnt[1] = app.m_pntGinEnd;
		pDC->Polyline(pnt, 2);
				
		app.m_pntGinEnd = pnt0;
		pnt[1] = app.m_pntGinEnd;
		pDC->Polyline(pnt, 2);
				
		pstate.SetROP2(pDC, iDrawMode);
	}
	else if (app.RubberBandingGetType() == Quads)
	{
		CPoint pnt[4];
				
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
				
		pnt[0] = app.m_pntGinStart;
		pnt[1].x = app.m_pntGinStart.x + (app.m_pntGinCur.x - app.m_pntGinEnd.x);
		pnt[1].y = app.m_pntGinStart.y + (app.m_pntGinCur.y - app.m_pntGinEnd.y);
		pnt[2] = app.m_pntGinCur;
		pnt[3] = app.m_pntGinEnd;
		
		if (pnt[1].x != app.m_pntGinStart.x || pnt[1].y != app.m_pntGinStart.y)
			pDC->Polyline(pnt, 4);
				
		app.m_pntGinCur = pnt0;
		pnt[1].x = app.m_pntGinStart.x + (app.m_pntGinCur.x - app.m_pntGinEnd.x);
		pnt[1].y = app.m_pntGinStart.y + (app.m_pntGinCur.y - app.m_pntGinEnd.y);
		pnt[2] = app.m_pntGinCur;
		pDC->Polyline(pnt, 4);
				
		pstate.SetROP2(pDC, iDrawMode);
	}
	else if (app.RubberBandingGetType() == Circles) 
	{
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
		CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);

		int iRadiusX = abs(app.m_pntGinEnd.x - app.m_pntGinStart.x);
		int iRadiusY = abs(app.m_pntGinEnd.y - app.m_pntGinStart.y);
		int iRadius = (int) sqrt((double) (iRadiusX * iRadiusX + iRadiusY * iRadiusY) + .5);

		pDC->Ellipse(app.m_pntGinStart.x - iRadius, app.m_pntGinStart.y + iRadius,
			app.m_pntGinStart.x + iRadius, app.m_pntGinStart.y - iRadius);
		app.m_pntGinEnd = pnt0;
		iRadiusX = abs(app.m_pntGinEnd.x - app.m_pntGinStart.x);
		iRadiusY = abs(app.m_pntGinEnd.y - app.m_pntGinStart.y);
		iRadius = (int) sqrt((double) (iRadiusX * iRadiusX + iRadiusY * iRadiusY) + .5);
		pDC->Ellipse(app.m_pntGinStart.x - iRadius, app.m_pntGinStart.y + iRadius,
			app.m_pntGinStart.x + iRadius, app.m_pntGinStart.y - iRadius);

		pDC->SelectObject(pBrushOld);
		pstate.SetROP2(pDC, iDrawMode);
	}
	else if (app.RubberBandingGetType() == Rectangles) 
	{
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
		CBrush* pBrushOld = (CBrush*) pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(app.m_pntGinStart.x, app.m_pntGinStart.y, app.m_pntGinEnd.x, app.m_pntGinEnd.y);

		app.m_pntGinEnd = pnt0;
		pDC->Rectangle(app.m_pntGinStart.x, app.m_pntGinStart.y, app.m_pntGinEnd.x, app.m_pntGinEnd.y);
		pDC->SelectObject(pBrushOld);
		pstate.SetROP2(pDC, iDrawMode);
	}
	
	CSeg* pSeg;
	CPrim* pPrim;

	if (AppGetGinCur(pSeg, pPrim))
	{
		CTMat tm;
		app.SetEditSegEnd(app.CursorPosGet());
		tm.Translate(CVec(app.GetEditSegBeg(), app.GetEditSegEnd()));
		
		int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
		int iPrimState = pstate.Save();
				
		if (trapsegs.Identify() && trapsegs.Find(pSeg) != 0)
			CPrim::SpecPenColor() = trapsegs.PenColor();
				
		if (pPrim != 0)
		{
			pPrim->Display(this, pDC);
			pPrim->Transform(tm);
			pPrim->Display(this, pDC);
		}
		else if (pSeg != 0)
		{
			pSeg->Display(this, pDC);
			pSeg->Transform(tm);
			pSeg->Display(this, pDC); 				
		}
		CPrim::SpecPenColor() = 0;
	
		tmEditSeg *= tm;
				
		pstate.Restore(pDC, iPrimState);
		pstate.SetROP2(pDC, iDrawMode);
			
		app.SetEditSegBeg(app.GetEditSegEnd());
	}
}
void CPegView::OnSetFocus(CWnd*)
{
    //app.SetWindowMode(app.m_iModeId);
}
void CPegView::OnSize(UINT, int cx, int cy)
{
	ViewportSet(cx, cy);
	ModelViewInitialize();
	m_mvOver = m_mvActive;
}
void CPegView::On3dViewsTop()
{
	ModelViewSetEye(ZDIR);
	ModelViewSetDirection(ZDIR);
	ModelViewSetVwUp(YDIR);
	ModelViewSetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsBottom()
{
	ModelViewSetEye(- ZDIR);
	ModelViewSetDirection(- ZDIR);
	ModelViewSetVwUp(YDIR);
	ModelViewSetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsLeft()
{
	ModelViewSetEye(- XDIR);
	ModelViewSetDirection(- XDIR);
	ModelViewSetVwUp(ZDIR);
	ModelViewSetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsRight()
{
	ModelViewSetEye(XDIR);
	ModelViewSetDirection(XDIR);
	ModelViewSetVwUp(ZDIR);
	ModelViewSetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsFront()
{
	ModelViewSetEye(- YDIR);
	ModelViewSetDirection(- YDIR);
	ModelViewSetVwUp(ZDIR);
	ModelViewSetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsBack()
{
	ModelViewSetEye(YDIR);
	ModelViewSetDirection(YDIR);
	ModelViewSetVwUp(ZDIR);
	ModelViewSetPerspectiveEnabled(false);
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsIsometric()
{
    ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_ISOMETRIC), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcProjIsometric));
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsAxonometric()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_AXONOMETRIC), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcProjAxonometric));
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsOblique()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_OBLIQUE), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcProjOblique));
	InvalidateRect(NULL, TRUE);
}
void CPegView::On3dViewsPerspective()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PROJ_PERSPECTIVE), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcProjPerspective));
	InvalidateRect(NULL, TRUE);
}
void CPegView::OnViewTrueTypeFonts()
{
	m_bViewTrueTypeFonts = !m_bViewTrueTypeFonts;
	InvalidateRect(NULL, TRUE);
}
void CPegView::OnViewPenWidths()
{
	m_bViewPenWidths = !m_bViewPenWidths;
	InvalidateRect(NULL, TRUE);
}
void CPegView::OnViewOdometer()
{
	m_bViewOdometer = !m_bViewOdometer;
	InvalidateRect(NULL, TRUE);
}
void CPegView::OnViewRefresh()
{
	InvalidateRect(NULL, TRUE);
}
void CPegView::OnViewZoom()
{
	CDlgViewZoom dlg;
	dlg.m_dRatio = GetWidthInInches() / ModelViewGetUExt();
	
	if (dlg.DoModal() == IDOK)
	{
		InvalidateRect(NULL, TRUE);
	}
}
void CPegView::OnViewWireframe()
{
	m_bViewWireframe = !m_bViewWireframe;
	InvalidateRect(NULL, TRUE);
}
void CPegView::OnViewRendered()
{
	m_bViewRendered = !m_bViewRendered;
	InvalidateRect(NULL, TRUE);
}
void CPegView::OnViewParameters()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_VIEWPARAMS), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcViewParameters));
}
void CPegView::OnViewSolid()
{
	// TODO: Add your command handler code here
}
void CPegView::OnViewLighting()
{
    ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_VIEWLIGHTING), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcViewLighting));
}
void CPegView::OnWindowNormal()
{
	DoWindowPan(1.);
}
void CPegView::OnWindowBest()
{
	CPnt ptMin;
	CPnt ptMax;
	
	CPegDoc::GetDoc()->GetExtents(ptMin, ptMax, ModelViewGetMatrix());
	
	// extents return range - 1 to 1

	if (ptMin[0] < ptMax[0])
	{
		ModelViewGetActive(m_mvLast);
		
		double dUExt = ModelViewGetUExt() * (ptMax[0] - ptMin[0]) / 2.;
		double dVExt = ModelViewGetVExt() * (ptMax[1] - ptMin[1]) / 2.;
		
		ModelViewSetCenteredWnd(dUExt, dVExt);
		
		CTMat tm; CPegDoc::GetDoc()->GetExtents(ptMin, ptMax, tm);
		
		CPnt ptTarget = CPnt((ptMin[0] + ptMax[0]) / 2., (ptMin[1] + ptMax[1]) / 2., (ptMin[2] + ptMax[2]) / 2.);
		
		ModelViewSetTarget(ptTarget);
		ModelViewSetEye(ModelViewGetDirection());
		
		app.CursorPosSet(ptTarget);
		InvalidateRect(NULL, TRUE);
	}
}
void CPegView::OnWindowLast()
{
	// TODO: Add your command handler code here
}
void CPegView::OnWindowSheet()
{
	// TODO: Add your command handler code here
}
void CPegView::OnWindowZoomIn()
{
	DoWindowPan(GetWidthInInches() / ModelViewGetUExt() * 1.25);
}
void CPegView::OnWindowZoomOut()
{
	DoWindowPan(GetWidthInInches() / ModelViewGetUExt() / 1.25);
}
void CPegView::OnWindowPan()
{
	DoWindowPan(GetWidthInInches() / ModelViewGetUExt());
}
void CPegView::OnWindowPanLeft()
{
	DoWindowPan0(ID_WINDOW_PAN_LEFT);
}
void CPegView::OnWindowPanRight()
{
	DoWindowPan0(ID_WINDOW_PAN_RIGHT);
}
void CPegView::OnWindowPanUp()
{
	DoWindowPan0(ID_WINDOW_PAN_UP);
}
void CPegView::OnWindowPanDown()
{
	DoWindowPan0(ID_WINDOW_PAN_DOWN);
}
void CPegView::OnCameraRotateLeft()
{
	DoCameraRotate(ID_CAMERA_ROTATELEFT);
}
void CPegView::OnCameraRotateRight()
{
	DoCameraRotate(ID_CAMERA_ROTATERIGHT);
}
void CPegView::OnCameraRotateUp()
{
	DoCameraRotate(ID_CAMERA_ROTATEUP);
}
void CPegView::OnCameraRotateDown()
{
	DoCameraRotate(ID_CAMERA_ROTATEDOWN);
}
void CPegView::OnViewWindow()
{
	CPoint pnt;
	::GetCursorPos(&pnt);

	HMENU hMenu = ::LoadMenu(app.GetInstance(), MAKEINTRESOURCE(IDR_WINDOW));
	hMenu = GetSubMenu(hMenu, 0);
	::TrackPopupMenu(hMenu, TPM_LEFTALIGN, pnt.x, pnt.y, 0, GetSafeHwnd(), 0);
	::DestroyMenu(hMenu);
}
void CPegView::OnSetupDimLength()
{
	CDlgSetLength dlg;
	
	dlg.m_strTitle = "Set Dimension Length";
	dlg.m_dLength = app.GetDimLen();
	if (dlg.DoModal() == IDOK)
	{
		app.SetDimLen(dlg.m_dLength);
		dde::PostAdvise(dde::DimLenInfo);
		app.StatusLineDisplay(DimLen);
	}
}
void CPegView::OnSetupDimAngle()
{
	CDlgSetAngle dlg;

	dlg.m_strTitle = "Set Dimension Angle";
	dlg.m_dAngle = app.GetDimAngZ();
	if (dlg.DoModal() == IDOK)
	{
		app.SetDimAngZ(dlg.m_dAngle);
		dde::PostAdvise(dde::DimAngZInfo);
		app.StatusLineDisplay(DimAng);
	}
}
void CPegView::OnSetupUnits()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_UNITS), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcUnits));
}
void CPegView::OnSetupConstraintsAxis()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_CONSTRAINTS_AXIS), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupConstraintsAxis));
	app.StatusLineDisplay();
}
void CPegView::OnSetupConstraintsGrid()
{
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_CONSTRAINTS_GRID), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupConstraints));
	app.StatusLineDisplay();
}
void CPegView::OnCursorDefault()
{
	// TODO: Add your command handler code here
}
void CPegView::OnCursorSymbol()
{
	// TODO: Add your command handler code here
}
void CPegView::OnCursorCrosshair()
{
	// TODO: Add your command handler code here
}
void CPegView::OnSetupMouseButtons()
{
    ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_MOUSEKEYS), GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupMouse_Buttons));
}
void CPegView::OnRelativeMovesEngDown()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] -= app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngDownRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] -= app.GetEngLen();
	pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngIn()
{
	CPnt pt = app.CursorPosGet();
	pt[2] -= app.GetEngLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngLeft()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] -= app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngLeftRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] -= app.GetEngLen();
	pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngOut()
{
	CPnt pt = app.CursorPosGet();
	pt[2] += app.GetEngLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngRight()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngRightRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngUp()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ());
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesEngUpRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] += app.GetEngLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetEngAngZ() + app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesRight()
{
	CPnt pt = app.CursorPosGet();
	pt[0] += app.GetDimLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesUp()
{
	CPnt pt = app.CursorPosGet();
	pt[1] += app.GetDimLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesLeft()
{
	CPnt pt = app.CursorPosGet();
	pt[0] -= app.GetDimLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesDown()
{
	CPnt pt = app.CursorPosGet();
	pt[1] -= app.GetDimLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesIn()
{
	CPnt pt = app.CursorPosGet();
	pt[2] -= app.GetDimLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesOut()
{
	CPnt pt = app.CursorPosGet();
	pt[2] += app.GetDimLen();
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesRightRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] += app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesUpRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] += app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesLeftRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[0] -= app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnRelativeMovesDownRotate()
{
	CPnt pt = app.CursorPosGet();
	CPnt ptSec = pt;
	ptSec[1] -= app.GetDimLen();
    pt = Pnt_RotateAboutPt_xy(ptSec, pt, app.GetDimAngZ() * RADIAN);
	app.CursorPosSet(pt);
}
void CPegView::OnPrimSnapTo()
{
	CPnt pt = app.CursorPosGet();

	CPnt ptDet;
	
	if (detsegs.IsSegEngaged())
	{
		CPrim* pPrim = detsegs.DetPrim();
		
		CPnt4 ptView(pt, 1.);
		ModelViewTransform(ptView);
		
		double dPicApertSiz = detsegs.PicApertSiz();
	
		CPrimPolygon::EdgeToEvaluate() = CPrimPolygon::Edge();

		if (pPrim->SelUsingPoint(this, ptView, dPicApertSiz, ptDet))
		{
			ptDet = pPrim->GoToNxtCtrlPt();
			detsegs.DetPt() = ptDet;
			
			pPrim->DisRep(ptDet);
			app.CursorPosSet(ptDet);
			return;
		}
	}
	if (detsegs.SelSegAndPrimUsingPoint(pt) != 0)
	{
		ptDet = detsegs.DetPt();
		detsegs.DetPrim()->DisRep(ptDet);
		app.CursorPosSet(ptDet);
	}
}
void CPegView::OnPrimPerpJump()
{
	CPnt pt = app.CursorPosGet();

	if (detsegs.SelSegAndPrimUsingPoint(pt) != 0)
	{
		if (detsegs.DetPrim()->Is(CPrim::PRIM_LINE))
		{
			CPrimLine* pPrimLine = static_cast<CPrimLine*>(detsegs.DetPrim());
			pt = pPrimLine->ProjPt(app.m_ptCursorPosWorld);
			app.CursorPosSet(pt);
		}
	}
}
void CPegView::OnHelpKey()
{
	::WinHelp(GetSafeHwnd(), "peg.hlp", HELP_KEY, reinterpret_cast<DWORD_PTR>("READY"));
}
void CPegView::DoCustomMouseClick(LPTSTR lpCmds)
{
	HWND hWndTarget = GetSafeHwnd();
	char* pIdx = &lpCmds[0];
	while (*pIdx != 0)
	{		   
		if (*pIdx == '{')
		{			
			*pIdx++;
			int iVkValue = atoi(pIdx);
			::PostMessage(hWndTarget,WM_KEYDOWN,iVkValue,0L);
			while ((*pIdx !=0) && (*pIdx != '}')) 
				pIdx++;
		}
		else
		{
			::PostMessage(hWndTarget,WM_CHAR,*pIdx,0L);
			pIdx++; 
		}
	}
}
// Returns a pointer to the currently active view.
CPegView* CPegView::GetActiveView(void)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*) (AfxGetApp()->m_pMainWnd);
	
	if (pFrame == NULL)
		return NULL;

	CMDIChildWnd* pChild = pFrame->MDIGetActive();

	if (pChild == NULL)
		return NULL;

	CView* pView = pChild->GetActiveView();

	// Fail if the view is the wrong kind (this could occur with splitter windows,
	// or additional views in a single document.
	if (!pView->IsKindOf(RUNTIME_CLASS(CPegView)))
		return NULL;

	return (CPegView*) pView;
}
void CPegView::OnUpdateViewOdometer(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewOdometer);
}
void CPegView::DisplayOdometer()
{
	static UINT fuOptions = ETO_CLIPPED | ETO_OPAQUE;
	CPnt pt = app.CursorPosGet();

	m_vRelPos = pt - grid::GetOrign();
	
	dde::PostAdvise(dde::RelPosXInfo);
	dde::PostAdvise(dde::RelPosYInfo);
	dde::PostAdvise(dde::RelPosZInfo);
	
	if (m_bViewOdometer)
	{
		char szBuf[32];
		if (app.RubberBandingGetType() == Lines)
		{
			CLine ln(app.RubberBandingGetStart(), pt);
		
			double dLen = ln.Length();
			double dAng = line::GetAngAboutZAx(ln);
			
			UnitsString_FormatLength(szBuf, sizeof(szBuf), max(app.GetUnits(), Engineering), dLen, 16, 4);
			std::string message = szBuf;
			message += UnitsString_FormatAngle(dAng, 0, 5);
			
			msgInformation(message.c_str());
		}
		CDC* pDC = GetDC();
	
		CFont* pFont = pDC->SelectObject(app.m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(AppGetTextCol()); 
		COLORREF crBk = pDC->SetBkColor(~AppGetTextCol());

		CRect rcClient; 
		GetClientRect(&rcClient);
		TEXTMETRIC tm; 
		pDC->GetTextMetrics(&tm);

		int iLeft = rcClient.right - 16 * tm.tmAveCharWidth; 
		
		CRect rc(iLeft, rcClient.top, rcClient.right, rcClient.top + tm.tmHeight);
		UnitsString_FormatLength(szBuf, sizeof(szBuf), app.GetUnits(), m_vRelPos[0], 16, 8);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, (UINT) strlen(szBuf), 0);
		
		rc.SetRect(iLeft, rcClient.top + 1 * tm.tmHeight, rcClient.right, rcClient.top + 2 * tm.tmHeight);
		UnitsString_FormatLength(szBuf, sizeof(szBuf), app.GetUnits(), m_vRelPos[1], 16, 8);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, (UINT) strlen(szBuf), 0);
		
		rc.SetRect(iLeft, rcClient.top + 2 * tm.tmHeight, rcClient.right, rcClient.top + 3 * tm.tmHeight);
		UnitsString_FormatLength(szBuf, sizeof(szBuf), app.GetUnits(), m_vRelPos[2], 16, 8);
		pDC->ExtTextOut(rc.left, rc.top, fuOptions, &rc, szBuf, (UINT) strlen(szBuf), 0);

		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);
	}  
}
void CPegView::OnUpdateViewTrueTypeFonts(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewTrueTypeFonts);		
}

void CPegView::OnBackgroundImageLoad()
{
	CFileDialog dlg(TRUE, "bmp", "*.bmp");
	dlg.m_ofn.lpstrTitle = "Load Background Image";

	if (dlg.DoModal() == IDOK)
	{
		CFileBitmap fbmp(dlg.GetPathName());
		
		fbmp.Load(dlg.GetPathName(), m_bmBackgroundImage, m_palBackgroundImage);
		m_bViewBackgroundImage = true;
		InvalidateRect(NULL, TRUE);
	}
}

void CPegView::OnBackgroundImageRemove()
{
	if ((HBITMAP) m_bmBackgroundImage != NULL)
	{
		m_bmBackgroundImage.DeleteObject();
		m_palBackgroundImage.DeleteObject();
        m_bViewBackgroundImage = false;
		InvalidateRect(NULL, TRUE);
	}
}

void CPegView::OnViewBackgroundImage()
{
	m_bViewBackgroundImage = !m_bViewBackgroundImage;
	InvalidateRect(NULL, TRUE);
}

void CPegView::OnUpdateViewBackgroundImage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((HBITMAP) m_bmBackgroundImage != NULL);
	pCmdUI->SetCheck(m_bViewBackgroundImage);
}

void CPegView::OnUpdateBackgroundimageLoad(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((HBITMAP) m_bmBackgroundImage == NULL);
}

void CPegView::OnUpdateBackgroundimageRemove(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((HBITMAP) m_bmBackgroundImage != NULL);		
}

void CPegView::OnUpdateViewRendered(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewRendered);
}

void CPegView::OnUpdateViewWireframe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewWireframe);
}

void CPegView::OnUpdateViewPenwidths(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bViewPenWidths);
}