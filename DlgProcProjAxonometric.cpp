#include "stdafx.h"

#include "PegAEsysView.h"

#include "Pnt.h"
#include "Vec.h"

double DlgBoxGetItemDouble(HWND hDlg, int control);
void DlgBoxSetItemDouble(HWND hDlg, int control, double value);

INT_PTR CALLBACK DlgProcProjAxonometric(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept
{
    CPegView* pView{CPegView::GetActiveView()};

    CVec	vDirection{};
    CVec	vVwUp{};

    CPnt	ptEye{};
    CPnt	ptTarget{};

    switch (anMsg)
    {
    case WM_INITDIALOG:
        vDirection = pView->ModelViewGetDirection();
        DlgBoxSetItemDouble(hDlg, IDC_VPN_X, vDirection[0]);
        DlgBoxSetItemDouble(hDlg, IDC_VPN_Y, vDirection[1]);
        DlgBoxSetItemDouble(hDlg, IDC_VPN_Z, vDirection[2]);

        ptTarget = pView->ModelViewGetTarget();
        DlgBoxSetItemDouble(hDlg, IDC_TARGET_X, ptTarget[0]);
        DlgBoxSetItemDouble(hDlg, IDC_TARGET_Y, ptTarget[1]);
        DlgBoxSetItemDouble(hDlg, IDC_TARGET_Z, ptTarget[2]);

        ptEye = pView->ModelViewGetEye();
        DlgBoxSetItemDouble(hDlg, IDC_PRP_X, ptEye[0]);
        DlgBoxSetItemDouble(hDlg, IDC_PRP_Y, ptEye[1]);
        DlgBoxSetItemDouble(hDlg, IDC_PRP_Z, ptEye[2]);

        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            ptEye[0] = DlgBoxGetItemDouble(hDlg, IDC_PRP_X);
            ptEye[1] = DlgBoxGetItemDouble(hDlg, IDC_PRP_Y);
            ptEye[2] = DlgBoxGetItemDouble(hDlg, IDC_PRP_Z);

            pView->ModelViewSetEye(ptEye);

            ptTarget[0] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_X);
            ptTarget[1] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_Y);
            ptTarget[2] = DlgBoxGetItemDouble(hDlg, IDC_TARGET_Z);

            pView->ModelViewSetTarget(ptTarget);

            vDirection = ptTarget - ptEye;
            pView->ModelViewSetDirection(vDirection);

            vVwUp = ZDIR ^ vDirection;
            vVwUp.Normalize();

            if (vVwUp.IsNull())
                vVwUp = YDIR;

            pView->ModelViewSetVwUp(vVwUp);
            pView->ModelViewSetCenteredWnd();
            pView->ModelViewSetPerspectiveEnabled(false);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
    }
    return (FALSE);
}
