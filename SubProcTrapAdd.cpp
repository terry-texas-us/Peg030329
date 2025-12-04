#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcTrapModify.h"
#include "Line.h"
#include "Messages.h"
#include "PrimState.h"
#include "SegsDet.h"
#include "SegsTrap.h"
#include "SubProcTrapAdd.h"

void DoEditTrapPopupCommands(HWND hwnd);

LRESULT CALLBACK SubProcTrapAdd(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    static int	iPts{0};
    static WORD wPrvKeyDwn{0};
    static CPnt pt[8]{};

    CPegDoc* pDoc{CPegDoc::GetDoc()};
    CPegView* pView{CPegView::GetActiveView()};

    CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

    switch (nMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_HELP_KEY:
            WinHelp(hwnd, "peg.hlp", HELP_KEY, reinterpret_cast<DWORD_PTR>("TRAP"));
            return 0;

        case ID_OP0:
            app.OnTrapCommandsAddSegments();
            return 0;

        case ID_OP1:
            pt[0] = app.CursorPosGet();
            trapsegs.AddSegsAtPt(pView, &detsegs, pt[0]);
            return 0;

        case ID_OP2:
            if (wPrvKeyDwn != ID_OP2)
            {
                pt[0] = app.CursorPosGet();
                app.RubberBandingStartAtEnable(pt[0], Lines);
                wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
            }
            else
            {
                pt[1] = app.CursorPosGet();
                trap::AddByLn(pView, pDC, &detsegs, pt[0], pt[1]);
                app.RubberBandingDisable();
                app.ModeLineUnhighlightOp(wPrvKeyDwn);
            }
            app.StatusLineDisplay(TrapCnt);
            return 0;

        case ID_OP3:
            return 0;

        case ID_OP4:
            if (wPrvKeyDwn != ID_OP4)
            {
                pt[0] = app.CursorPosGet();
                app.RubberBandingStartAtEnable(pt[0], Rectangles);
                wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
            }
            else
            {
                pt[1] = app.CursorPosGet();
                trap::AddByArea(pView, pDC, &detsegs, pt[0], pt[1]);
                app.RubberBandingDisable();
                app.ModeLineUnhighlightOp(wPrvKeyDwn);
            }
            app.StatusLineDisplay(TrapCnt);
            return 0;

        case ID_OP5:
            trap::AddLast();
            return 0;

        case ID_OP6:		// Check engaged segment
            if (detsegs.IsSegEngaged())
            {
                POSITION pos = pDoc->WorkLayerFind(detsegs.DetSeg());

                CSeg* pSeg = pDoc->WorkLayerGetNext(pos);

                if (trapsegs.Find(pSeg) == 0)
                {
                    trapsegs.AddTail(pSeg);
                    pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
                    app.StatusLineDisplay(TrapCnt);
                }
            }
            else { msgInformation(0); }

            return 0;

        case ID_OP7:
            return 0;

        case ID_OP8:
            DoEditTrapPopupCommands(hwnd);
            return 0;

        case ID_OP9:
            if (!trapsegs.IsEmpty())		// Trap is not empty
                ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRAP_MODIFY), hwnd, reinterpret_cast<DLGPROC>(DlgProcTrapModify));
            else { msgInformation(0); }
            return 0;


        case IDM_RETURN:
            return 0;

        case IDM_ESCAPE:
            return 0;
        }
        break;

    case WM_LBUTTONDBLCLK:
        pt[0] = app.CursorPosGet();
        trapsegs.AddSegsAtPt(pView, &detsegs, pt[0]);
        return 0;
    }
    return (CallWindowProc(app.GetMainWndProc(), hwnd, nMsg, wParam, lParam));
}

// Identifies segments which lie wholly or partially within a rectangular area.
void trap::AddByArea(CPegView* pView, CDC* pDC, CSegs* pSegs, CPnt pt0, CPnt pt1)
{
    CPegDoc* pDoc = CPegDoc::GetDoc();

    int iPrimState = pstate.Save();

    CPnt4 pt [] = {CPnt4(pt0, 1.), CPnt4(pt1, 1.)};

    pView->ModelViewTransform(2, pt);

    if (CVec(pt[0], pt[1]).IsNull())
        return;

    line::Extents_xy(CLine(pt[0], pt[1]), pt0, pt1);

    POSITION pos = pSegs->GetHeadPosition();
    while (pos != 0)
    {
        CSeg* pSeg = pSegs->GetNext(pos);

        if (trapsegs.Find(pSeg) != 0)					// already in trap
            continue;

        if (pSeg->SelUsingRect(pView, pt0, pt1))
        {
            trapsegs.AddTail(pSeg);
            pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
        }
    }
    pstate.Restore(pDC, iPrimState);
    app.StatusLineDisplay(TrapCnt);
}

void trap::AddByLn(CPegView* pView, CDC* pDC, CSegs* pSegs, CPnt ptBeg, CPnt ptEnd)
{	// Identifies segments which intersect with a line and adds them to the trap.

    CPegDoc* pDoc = CPegDoc::GetDoc();

    if (CVec(ptBeg, ptEnd).IsNull())
        return;

    CPnt4 ptView [] = {CPnt4(ptBeg, 1.), CPnt4(ptEnd, 1.)};

    pView->ModelViewTransform(2, ptView);

    CPnts ptsInt;

    int iPrimState = pstate.Save();

    POSITION pos = pSegs->GetHeadPosition();
    while (pos != 0)
    {
        CSeg* pSeg = pSegs->GetNext(pos);

        if (trapsegs.Find(pSeg) != 0)					// already in trap
            continue;

        POSITION posPrim = pSeg->GetHeadPosition();
        while (posPrim != 0)
        {
            CPrim* pPrim = pSeg->GetNext(posPrim);
            if (pPrim->SelUsingLine(pView, CLine(ptView[0], ptView[1]), ptsInt))
            {
                trapsegs.AddTail(pSeg);
                pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
                break;
            }
        }
    }
    pstate.Restore(pDC, iPrimState);
    app.StatusLineDisplay(TrapCnt);
}
void trap::AddLast()
{
    CPegDoc* pDoc = CPegDoc::GetDoc();

    // adds last editable segment to trap
    POSITION pos = pDoc->WorkLayerGetTailPosition();
    while (pos != 0)
    {
        CSeg* pSeg = pDoc->WorkLayerGetPrev(pos);

        if (!trapsegs.Find(pSeg))
        {	// not already in trap
            trapsegs.AddTail(pSeg);
            pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
            break;
        }
    }
    app.StatusLineDisplay(TrapCnt);
}
