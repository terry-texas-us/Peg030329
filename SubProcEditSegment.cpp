#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcEditOps.h"
#include "PrimState.h"

INT_PTR CALLBACK DlgProcEditOps(HWND, UINT, WPARAM, LPARAM) noexcept;

extern CTMat tmEditSeg;

void DoCopy();
void DoEscape();
void DoTransform(WORD);

// Edits segment

LRESULT CALLBACK SubProcEditSegment(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept {
  if (nMsg != WM_COMMAND) return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));

  CSeg* pSeg;

  if (AppGetGinCur(pSeg) == 0) {
    app.SetWindowMode(app.m_iPrimModeId);
    AppSetGinCur();
    return 0;
  }

  WORD wId = LOWORD(wParam);

  switch (wId) {
    case ID_OP0:
      ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_EDIT_OPTIONS), hwnd,
                  reinterpret_cast<DLGPROC>(DlgProcEditOps));
      return 0;

    case ID_OP1:
    case ID_OP9:
      return 0;

    case ID_OP2:
    case ID_OP3:
    case ID_OP6:
    case ID_OP7:
    case ID_OP8:
      DoTransform(wId);
      return 0;

    case ID_OP5:
      DoCopy();
      return 0;

    case IDM_ESCAPE:
      DoEscape();
      return 0;

    case ID_OP4:
    case IDM_RETURN:
      app.SetWindowMode(app.m_iPrimModeId);
      AppSetGinCur();
      return 0;
  }
  return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}

void DoCopy() {
  CSeg* pSeg;

  if (AppGetGinCur(pSeg) != 0) {
    CPegDoc* pDoc = CPegDoc::GetDoc();
    CPegView* pView = CPegView::GetActiveView();
    CDC* pDC = (pView == nullptr) ? nullptr : pView->GetDC();

    CSeg* pSegNew = new CSeg(*pSeg);
    pDoc->WorkLayerAddTail(pSegNew);
    AppSetGinCur(pSegNew);

    int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSegNew);
    pstate.SetROP2(pDC, iDrawMode);
  }
}
void DoEscape() {
  CSeg* pSeg;

  if (AppGetGinCur(pSeg) != 0) {
    tmEditSeg.Inverse();

    CPegDoc* pDoc = CPegDoc::GetDoc();
    CPegView* pView = CPegView::GetActiveView();
    CDC* pDC = (pView == nullptr) ? nullptr : pView->GetDC();

    int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
    int iPrimState = pstate.Save();

    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG, pSeg);
    pSeg->Transform(tmEditSeg);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG, pSeg);

    pstate.Restore(pDC, iPrimState);
    pstate.SetROP2(pDC, iDrawMode);

    app.SetWindowMode(app.m_iPrimModeId);
    AppSetGinCur();
  }
}
void DoTransform(WORD wId) {
  CSeg* pSeg;

  if (AppGetGinCur(pSeg) != 0) {
    CTMat tm;

    tm.Translate(ORIGIN - app.GetEditSegBeg());

    if (wId == ID_OP2)
      tm *= dlgproceditops::GetRotTrnMat();
    else if (wId == ID_OP3)
      tm *= dlgproceditops::GetInvertedRotTrnMat();
    else if (wId == ID_OP6)
      tm.Scale(dlgproceditops::GetMirrorScale());
    else if (wId == ID_OP7)
      tm.Scale(dlgproceditops::GetInvertedScale());
    else
      tm.Scale(dlgproceditops::GetScale());

    tm.Translate(app.GetEditSegBeg());

    CPegDoc* pDoc = CPegDoc::GetDoc();
    CPegView* pView = CPegView::GetActiveView();
    CDC* pDC = (pView == nullptr) ? nullptr : pView->GetDC();

    int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
    int iPrimState = pstate.Save();

    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG, pSeg);
    pSeg->Transform(tm);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG, pSeg);

    tmEditSeg *= tm;
    pstate.Restore(pDC, iPrimState);
    pstate.SetROP2(pDC, iDrawMode);
  }
}
