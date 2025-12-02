#include "stdafx.h"

#include "PegAEsysView.h"

#include "ExpProcs.h"
#include "Vec.h"

void DlgProcProjPerspectiveDoOK(HWND);
void DlgProcProjPerspectiveInit(HWND);

INT_PTR CALLBACK DlgProcProjPerspective(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
    switch (anMsg)
    {
    case WM_INITDIALOG:
        DlgProcProjPerspectiveInit(hDlg);
        return (TRUE);

    case WM_COMMAND:										// message: received a command
        switch (LOWORD(wParam))
        {
        case IDOK:
            DlgProcProjPerspectiveDoOK(hDlg);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
    }
    return (FALSE);
}

void DlgProcProjPerspectiveInit(HWND hDlg)
{
    CPegView* pView = CPegView::GetActiveView();

    CVec vDirection = pView->ModelViewGetDirection();

    double dDist = pView->ModelViewGetLensLength();

    DlgBoxSetItemDouble(hDlg, IDC_VIEW_DIS_TO_TARGET, dDist);
}

void DlgProcProjPerspectiveDoOK(HWND hDlg)
{
    CPegView* pView = CPegView::GetActiveView();

    double dDis = DlgBoxGetItemUnitsText(hDlg, IDC_VIEW_DIS_TO_TARGET);

    pView->ModelViewSetLensLength(dDis);
    pView->ModelViewSetEye(pView->ModelViewGetDirection());
    pView->ModelViewSetPerspectiveEnabled(true);
}
