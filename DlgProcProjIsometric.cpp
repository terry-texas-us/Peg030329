#include "stdafx.h"

#include "PegAEsysView.h"

#include "Vec.h"

INT_PTR CALLBACK DlgProcProjIsometric(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
    static int iLeftRight, iFrontBack, iAboveUnder;

    CPegView* pView = CPegView::GetActiveView();

    CVec	vVwUp;
    CVec	vDirection;

    switch (anMsg)
    {
    case WM_INITDIALOG: 	// message: initialize dialog box
        ::SendDlgItemMessage(hDlg, IDC_VIEW_ISO_LEFT + iLeftRight, BM_SETCHECK, 1, 0L);
        ::SendDlgItemMessage(hDlg, IDC_VIEW_ISO_FRONT + iFrontBack, BM_SETCHECK, 1, 0L);
        ::SendDlgItemMessage(hDlg, IDC_VIEW_ISO_ABOVE + iAboveUnder, BM_SETCHECK, 1, 0L);
        return (TRUE);

    case WM_COMMAND:		// message: received a command
        switch (LOWORD(wParam))
        {
        case IDOK:
            iLeftRight = (IsDlgButtonChecked(hDlg, IDC_VIEW_ISO_LEFT) != 0) ? 0 : 1;
            vDirection[0] = .5773503;
            if (iLeftRight != 0)
                vDirection[0] = -.5773503;
            iFrontBack = (IsDlgButtonChecked(hDlg, IDC_VIEW_ISO_FRONT) != 0) ? 0 : 1;
            vDirection[1] = .5773503;
            if (iFrontBack != 0)
                vDirection[1] = -.5773503;
            iAboveUnder = (IsDlgButtonChecked(hDlg, IDC_VIEW_ISO_ABOVE) != 0) ? 0 : 1;
            vDirection[2] = -.5773503;
            if (iAboveUnder != 0)
                vDirection[2] = .5773503;

            pView->ModelViewSetEye(-vDirection);
            pView->ModelViewSetDirection(-vDirection);
            pView->ModelViewSetPerspectiveEnabled(false);

            vVwUp = vDirection ^ ZDIR;
            vVwUp = vVwUp ^ vDirection;
            vVwUp.Normalize();
            pView->ModelViewSetVwUp(vVwUp);
            pView->ModelViewSetCenteredWnd();
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }

    }
    return (FALSE);
}

