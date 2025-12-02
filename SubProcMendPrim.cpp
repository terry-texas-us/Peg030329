#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Pnt.h"
#include "Prim.h"
#include "Vec.h"

DWORD dwMendPrimPtId{};
CPrim* pPrimToMend{nullptr};
CPrim* pPrimToMendCopy{nullptr};
CPnt ptMendPrimBeg{};

///<summary>Mends primatives.</summary>
LRESULT CALLBACK SubProcMendPrim(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    CPegDoc* pDoc{CPegDoc::GetDoc()};

    if (anMsg == WM_MOUSEMOVE)
    {
        CPnt ptCurPos = app.CursorPosGet();
        CVec vTrns(ptMendPrimBeg, ptCurPos);
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrimToMendCopy);
        pPrimToMendCopy->TranslateUsingMask(vTrns, dwMendPrimPtId);
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM_SAFE, pPrimToMendCopy);
        ptMendPrimBeg = ptCurPos;
    }
    else if (anMsg == WM_COMMAND)
    {
        WORD wId = LOWORD(wParam);

        if (wId == IDM_RETURN || wId == IDM_ESCAPE)
        {
            pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM_ERASE_SAFE, pPrimToMendCopy);

            if (wId == IDM_RETURN)
                pPrimToMend->Assign(pPrimToMendCopy);

            pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM_SAFE, pPrimToMend);

            delete pPrimToMendCopy;

            app.SetWindowMode(app.m_iPrimModeId);
            return 0;
        }
    }
    return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}
