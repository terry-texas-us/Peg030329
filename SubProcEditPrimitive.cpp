#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcEditOps.h"
#include "PrimState.h"

INT_PTR CALLBACK DlgProcEditOps(HWND, UINT, WPARAM, LPARAM) noexcept;

extern CTMat tmEditSeg;

void DoEditPrimitiveCopy();
void DoEditPrimitiveEscape();
void DoEditPrimitiveTransform(WORD);

// Edits primitive

LRESULT CALLBACK SubProcEditPrimitive(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept {
  if (nMsg == WM_COMMAND) {
    WORD wId = LOWORD(wParam);

    switch (wId) {
      case ID_OP0:
        ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_EDIT_OPTIONS), hwnd, DlgProcEditOps);
        return 0;

      case ID_OP2:
      case ID_OP3:
      case ID_OP6:
      case ID_OP7:
      case ID_OP8:
        DoEditPrimitiveTransform(wId);
        return 0;

      case ID_OP5:
        DoEditPrimitiveCopy();
        return 0;

      case IDM_ESCAPE:
        DoEditPrimitiveEscape();
        return 0;

      case ID_OP4:
      case IDM_RETURN:
        app.SetWindowMode(app.m_iPrimModeId);
        AppSetGinCur();
        return 0;
    }
  }
  return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}
void DoEditPrimitiveCopy() {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CPrim* pPrim;

  if (AppGetGinCur(pPrim) != 0) {
    CPrim* pPrimCopy;
    pPrim->Copy(pPrimCopy);
    CSeg* pSeg = new CSeg(pPrimCopy);

    pDoc->WorkLayerAddTail(pSeg);

    AppSetGinCur(pSeg, pPrimCopy);

    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);
  }
}
void DoEditPrimitiveEscape() {
  CPrim* pPrim;

  if (AppGetGinCur(pPrim) != 0) {
    tmEditSeg.Inverse();

    CPegDoc* pDoc = CPegDoc::GetDoc();
    CPegView* pView = CPegView::GetActiveView();
    CDC* pDC = (pView == nullptr) ? nullptr : pView->GetDC();

    int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
    int iPrimState = pstate.Save();

    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM, pPrim);
    pPrim->Transform(tmEditSeg);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM, pPrim);

    pstate.Restore(pDC, iPrimState);
    pstate.SetROP2(pDC, iDrawMode);

    app.SetWindowMode(app.m_iPrimModeId);
    AppSetGinCur();
  }
}
void DoEditPrimitiveTransform(WORD wId) {
  CPrim* pPrim;

  if (AppGetGinCur(pPrim) != 0) {
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

    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM, pPrim);
    pPrim->Transform(tm);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM, pPrim);

    tmEditSeg *= tm;
    pstate.Restore(pDC, iPrimState);
    pstate.SetROP2(pDC, iDrawMode);
  }
}
