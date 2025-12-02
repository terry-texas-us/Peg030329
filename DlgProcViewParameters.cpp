#include "stdafx.h"

#include <afxwin.h>

#include <cstdlib>

#include "PegAEsysView.h"

#include "ExpProcs.h"

INT_PTR CALLBACK DlgProcViewParameters(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
    CPegView* pView = CPegView::GetActiveView();

    char szBuf[32]{};

    switch (anMsg)
    {
    case WM_INITDIALOG:
        DlgBoxSetItemDouble(hDlg, IDC_NEAR, pView->ModelViewGetNearClipDistance());
        DlgBoxSetItemDouble(hDlg, IDC_FAR, pView->ModelViewGetFarClipDistance());

        return (TRUE);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:
            ::GetDlgItemText(hDlg, IDC_NEAR, szBuf, sizeof(szBuf));
            pView->ModelViewSetNearClipDistance(atof(szBuf));
            ::GetDlgItemText(hDlg, IDC_FAR, szBuf, sizeof(szBuf));
            pView->ModelViewSetFarClipDistance(atof(szBuf));

            pView->InvalidateRect(NULL, TRUE);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}
