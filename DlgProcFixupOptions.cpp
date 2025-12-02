#include "stdafx.h"

#include "ExpProcs.h"
#include "SubProcFixup.h"
#include "UserAxis.h"

INT_PTR CALLBACK DlgProcFixupOptions(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM) noexcept
{
    switch (nMsg)
    {
    case WM_INITDIALOG:
        DlgBoxSetItemDouble(hDlg, IDC_FIX_SIZ, fixup::dSiz);
        DlgBoxSetItemDouble(hDlg, IDC_FIX_AX_TOL, fixup::dAxTol);
        return (TRUE);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:
            fixup::dSiz = DlgBoxGetItemDouble(hDlg, IDC_FIX_SIZ);
            fixup::dAxTol = DlgBoxGetItemDouble(hDlg, IDC_FIX_AX_TOL);
            UserAxisSetInfAng(fixup::dAxTol);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}
