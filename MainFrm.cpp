#include "stdafx.h"

#include <afxext.h>
#include <afxmsg_.h>

#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "Dde.h"
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
static int numberOfIndicators = sizeof(indicators) / sizeof(UINT);

// CMainFrame construction/destruction

CMainFrame::CMainFrame() = default;
CMainFrame::~CMainFrame() = default;
auto CMainFrame::PreCreateWindow(CREATESTRUCT& cs) -> BOOL {
  if (!CMDIFrameWnd::PreCreateWindow(cs)) return FALSE;

  return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const { CMDIFrameWnd::AssertValid(); }

void CMainFrame::Dump(CDumpContext& dc) const { CMDIFrameWnd::Dump(dc); }

#endif  //_DEBUG

// CMainFrame message handlers

auto CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) -> int {
  if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

  if (!m_wndToolBar.CreateEx(
          this, TBSTYLE_FLAT,
          WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
    TRACE0(_T("Failed to create toolbar\n"));
    return -1;  // fail to create
  }
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, numberOfIndicators)) {
    TRACE0(_T("Failed to create status bar\n"));
    return -1;  // fail to create
  }

  app.m_wpMainWnd = reinterpret_cast<WNDPROC>(::GetWindowLongPtr(m_hWnd, GWLP_WNDPROC));

  lex::Init();

  return 0;
}
void CMainFrame::OnDestroy() {
  TRACE(_T("CMainFrame::OnDestroy() - Entering\n"));

  auto* activeView = CPegView::GetActiveView();
  CDC* pDC = (activeView == nullptr) ? nullptr : activeView->GetDC();

  if (pDC != nullptr) {
    // Stock objects are never left `current` so it is safe to delete whatever the old object is
    pDC->SelectStockObject(BLACK_PEN)->DeleteObject();
    pDC->SelectStockObject(WHITE_BRUSH)->DeleteObject();
    // activeView->ReleaseDC();	// Release to DC associated with the view
  }

  opengl::Release();

  app.LineFontRelease();
  app.StrokeFontRelease();

  dde::Uninitialize();

  PostQuitMessage(0);  // Force WM_QUIT message to terminate message loop
}
