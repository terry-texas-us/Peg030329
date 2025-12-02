#include "stdafx.h"

#include "PegAEsys.h"

#include "Pnt.h"
#include "SegsTrap.h"

INT_PTR CALLBACK DlgProcSetPasteLoc(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
    CPnt pt;

    switch (anMsg)
    {
    case WM_INITDIALOG:
        return (TRUE);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:
            pt = app.CursorPosGet();
            trapsegs.PvtPt() = pt;
            ::EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDCANCEL:
            ::EndDialog(hDlg, FALSE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}
