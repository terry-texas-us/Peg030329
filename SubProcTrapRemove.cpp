#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcTrapModify.h"
#include "Line.h"
#include "Messages.h"
#include "Pnt.h"
#include "Prim.h"
#include "Seg.h"
#include "SegsTrap.h"

void DoEditTrapPopupCommands(HWND hwnd);
void pTrapRemByArea(CPegView* pView, CPnt, CPnt);
void pTrapRemByLn(CPegView* pView, CPnt, CPnt);

LRESULT CALLBACK SubProcTrapRemove(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept {
  CSeg* pSeg{nullptr};
  static int iPts{0};
  static WORD wPrvKeyDwn{0};
  static CPnt rPt[8]{};

  CPegDoc* pDoc{CPegDoc::GetDoc()};
  CPegView* pView{CPegView::GetActiveView()};

  switch (anMsg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case ID_HELP_KEY:
          WinHelp(hwnd, _T("peg.hlp"), HELP_KEY, reinterpret_cast<DWORD_PTR>(_T("TRAP")));
          return 0;

        case ID_OP0:
          app.OnTrapCommandsAddSegments();
          return 0;

        case ID_OP1:
          rPt[0] = app.CursorPosGet();
          trapsegs.RemoveSegsAtPt(pView, rPt[0]);
          app.StatusLineDisplay(TrapCnt);
          return 0;

        case ID_OP2:
          if (wPrvKeyDwn != ID_OP2) {
            rPt[0] = app.CursorPosGet();
            app.RubberBandingStartAtEnable(rPt[0], Lines);
            wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
          } else {
            rPt[1] = app.CursorPosGet();
            pTrapRemByLn(pView, rPt[0], rPt[1]);
            app.RubberBandingDisable();
            app.ModeLineUnhighlightOp(wPrvKeyDwn);
          }
          app.StatusLineDisplay(TrapCnt);
          return 0;

        case ID_OP3:
          return 0;

        case ID_OP4:
          if (wPrvKeyDwn != ID_OP4) {
            rPt[0] = app.CursorPosGet();
            app.RubberBandingStartAtEnable(rPt[0], Rectangles);
            wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
          } else {
            rPt[1] = app.CursorPosGet();
            pTrapRemByArea(pView, rPt[0], rPt[1]);
            app.RubberBandingDisable();
            app.ModeLineUnhighlightOp(wPrvKeyDwn);
          }
          app.StatusLineDisplay(TrapCnt);
          return 0;

        case ID_OP5:
          if (!trapsegs.IsEmpty()) {
            pSeg = trapsegs.RemoveTail();
            pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSeg);
          }
          app.StatusLineDisplay(TrapCnt);
          return 0;

        case ID_OP7:
          return 0;

        case ID_OP8:
          DoEditTrapPopupCommands(hwnd);
          return 0;

        case ID_OP9:
          if (!trapsegs.IsEmpty()) {
            ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRAP_MODIFY), hwnd,
                        reinterpret_cast<DLGPROC>(DlgProcTrapModify));
          } else {
            msgInformation(0);
}
          return 0;

        case IDM_RETURN:
          app.StatusLineDisplay(TrapCnt);
          return 0;

        case IDM_ESCAPE:
          return 0;
      }
      break;
  }
  return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}
///<summary>Identifies segments which lie wholly or partially within a orthoganal rectangular area.</summary>
// Notes: This routine fails in all but top view. !!
// Parameters:	pt1 	one corner of the area
//				pt2 	other corner of the area
void pTrapRemByArea(CPegView* pView, CPnt pt1, CPnt pt2) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  if (CVec(pt1, pt2).IsNull()) return;

  line::Extents_xy(CLine(pt1, pt2), pt1, pt2);  // Rationalize the diagonal

  POSITION pos = trapsegs.GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = trapsegs.GetNext(pos);

    if (pSeg->SelUsingRect(pView, pt1, pt2)) {
      trapsegs.RemoveAt(trapsegs.Find(pSeg));
      pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSeg);
    }
  }
}
///<summary>Identifies segments which intersect with a line and removes them from the trap.</summary>
void pTrapRemByLn(CPegView* pView, CPnt ptBeg, CPnt ptEnd) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  if (CVec(ptBeg, ptEnd).IsNull()) return;

  CPnt4 ptView[] = {CPnt4(ptBeg, 1.), CPnt4(ptEnd, 1.)};

  pView->ModelViewTransform(2, ptView);

  CPnts ptsInt;

  POSITION pos = trapsegs.GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = trapsegs.GetNext(pos);

    POSITION posPrim = pSeg->GetHeadPosition();
    while (posPrim != nullptr) {
      CPrim* pPrim = pSeg->GetNext(posPrim);

      if (pPrim->SelUsingLine(pView, CLine(ptView[0], ptView[1]), ptsInt)) {
        trapsegs.RemoveAt(trapsegs.Find(pSeg));
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSeg);
        break;
      }
    }
  }
}
