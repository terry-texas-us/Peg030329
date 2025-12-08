#include "stdafx.h"

#include <atltypes.h>

#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Line.h"
#include "OwnerDraw.h"
#include "Pnt.h"
#include "Prim.h"
#include "PrimState.h"
#include "Vec.h"

void SetupPenStyle_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate);
void SetupPenStyle_Init(HWND);

INT_PTR CALLBACK DlgProcSetupPenStyle(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    BOOL bTranslated;
    char szBuf[32]{};
    PENSTYLE nPenStyle;

    switch (nMsg)
    {
    case WM_INITDIALOG:
        SetupPenStyle_Init(hDlg);
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            bTranslated = TRUE;
            nPenStyle = PENSTYLE(GetDlgItemInt(hDlg, IDC_PENSTYLES, &bTranslated, TRUE));
            if (bTranslated == 0)
            {
                bTranslated = (::GetDlgItemText(hDlg, IDC_PENSTYLES, (LPSTR)szBuf, sizeof(szBuf)) > 0);
                if (bTranslated)
                {
                    nPenStyle = CPegDoc::GetDoc()->PenStylesLookup(szBuf);
                }
                bTranslated = nPenStyle != PENSTYLE_LookupErr;
            }
            if (bTranslated)
            {
                pstate.SetPenStyle(nPenStyle);

                ::EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
        if (lpDIS->itemID == -1)	// Empty combo box .. Draw only focus rectangle
            OwnerDraw_Focus(lpDIS, 0);
        else
        {
            switch (lpDIS->itemAction)
            {
            case ODA_DRAWENTIRE:
                SetupPenStyle_DrawEntire(lpDIS, 0);
                break;

            case ODA_SELECT:
                OwnerDraw_Select(lpDIS, 0);
                break;

            case ODA_FOCUS:
                OwnerDraw_Focus(lpDIS, 0);
                break;
            }
        }
        return (TRUE);
    }
    }
    return (FALSE);
}
///<summary>Draw the pen style number and a sample line showing its appearance.</summary>
void  SetupPenStyle_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int)
{
    CDC dc{};
    dc.Attach(lpDIS->hDC);

    LRESULT lrIndex{::SendMessage(lpDIS->hwndItem, CB_GETCURSEL, 0, 0)};
    if (lrIndex == CB_ERR) { return; }

    char szBuf[32]{};
    ::SendMessage(lpDIS->hwndItem, CB_GETLBTEXT, lpDIS->itemID, (LPARAM)(LPCSTR)szBuf);

    PENSTYLE nStyle{CPegDoc::GetDoc()->PenStylesLookup(szBuf)};
    if (nStyle == PENSTYLE_LookupErr) { return; }

    CRect rc{};
    ::CopyRect(&rc, &lpDIS->rcItem);

    dc.SetTextColor(RGB(0, 0, 0));
    dc.SetBkColor(RGB(255, 255, 255));

    dc.ExtTextOut(rc.right - 72, rc.top + 2, ETO_CLIPPED, &rc, szBuf, static_cast<UINT>(lstrlen(szBuf)), nullptr);

    PENCOLOR nPenColor{pstate.PenColor()};
    PENSTYLE nPenStyle{pstate.PenStyle()};
    pstate.SetPen(&dc, 0, nStyle);

    rc.right -= 80;

    auto pView = CPegView::GetActiveView();

    pView->ViewportPushActive();
    pView->ViewportSet(rc.right + rc.left, rc.bottom + rc.top);

    double dUExt = double(rc.right + rc.left) / double(dc.GetDeviceCaps(LOGPIXELSX));
    double dVExt = double(rc.bottom + rc.top) / double(dc.GetDeviceCaps(LOGPIXELSY));

    pView->ModelViewPushActive();
    pView->ModelViewInitialize();
    pView->ModelViewSetWnd(0., 0., dUExt, dVExt);
    pView->ModelViewSetTarget(ORIGIN);
    pView->ModelViewSetEye(ZDIR);

    CLine ln(CPnt(0., dVExt / 2., 0.), CPnt(dUExt, dVExt / 2., 0.));
    ln.Display(pView, &dc);

    pView->ModelViewPopActive();
    pView->ViewportPopActive();

    OwnerDraw_Select(lpDIS, 0);
    OwnerDraw_Focus(lpDIS, 0);

    pstate.SetPen(&dc, nPenColor, nPenStyle);

    dc.Detach();
}
void SetupPenStyle_Init(HWND hDlg)
{
    HWND hWndComboBox{::GetDlgItem(hDlg, IDC_PENSTYLES)};

    CPegDoc::GetDoc()->PenStylesFillCB(hWndComboBox);
    PENSTYLE penStyle{pstate.PenStyle()};
    ::SendMessage(hWndComboBox, CB_SETCURSEL, static_cast<WPARAM>(penStyle), 0L);
}