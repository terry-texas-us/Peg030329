#include "stdafx.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
END_MESSAGE_MAP()

CChildFrame::CChildFrame() {
  // TODO: add member initialization code here
}

CChildFrame::~CChildFrame() {}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs) {
  // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
  if (!CMDIChildWnd::PreCreateWindow(cs)) return FALSE;

  return TRUE;
}

// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const { CMDIChildWnd::AssertValid(); }

void CChildFrame::Dump(CDumpContext& dc) const { CMDIChildWnd::Dump(dc); }

#endif  //_DEBUG

// CChildFrame message handlers
void CChildFrame::ActivateFrame(int) { CMDIChildWnd::ActivateFrame(SW_SHOWMAXIMIZED); }
