#include "stdafx.h"

#include <afxext.h>
#include <afxmsg_.h>

#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "dde.h"
#include "FileJob.h"
#include "Lex.h"
#include "OpenGL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
#pragma warning(push)
#pragma warning(disable : 4191)
ON_WM_CREATE()
#pragma warning(pop)
ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR,  // status line indicator
    ID_OP0,       ID_OP1, ID_OP2, ID_OP3, ID_OP4, ID_OP5, ID_OP6, ID_OP7, ID_OP8, ID_OP9,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() {}
CMainFrame::~CMainFrame() {}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if (!CMDIFrameWnd::PreCreateWindow(cs)) return FALSE;

  return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const { CMDIFrameWnd::AssertValid(); }

void CMainFrame::Dump(CDumpContext& dc) const { CMDIFrameWnd::Dump(dc); }

#endif  //_DEBUG

// CMainFrame message handlers

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

  if (!m_wndToolBar.CreateEx(
          this, TBSTYLE_FLAT,
          WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
    TRACE0("Failed to create toolbar\n");
    return -1;  // fail to create
  }
  if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT))) {
    TRACE0("Failed to create status bar\n");
    return -1;  // fail to create
  }
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  app.m_wpMainWnd = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC));

  lex::Init();

  return 0;
}
void CMainFrame::OnDestroy() {
  TRACE("CMainFrame::OnDestroy() - Entering\n");

  CPegView* pView = CPegView::GetActiveView();
  CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

  if (pDC != NULL) {
    // Stock objects are never left "current" so it is safe to delete whatever the old object is
    pDC->SelectStockObject(BLACK_PEN)->DeleteObject();
    pDC->SelectStockObject(WHITE_BRUSH)->DeleteObject();
    // pView->ReleaseDC();	// Release to DC associated with the view
  }

  opengl::Release();

  app.LineFontRelease();
  app.StrokeFontRelease();

  dde::Uninitialize();

  PostQuitMessage(0);  // Force WM_QUIT message to terminate message loop
}