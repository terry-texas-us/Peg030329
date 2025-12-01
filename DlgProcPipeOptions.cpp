#include "stdafx.h"

#include "ExpProcs.h"
#include "resource.h"
#include "SubProcPipe.h"

BOOL CALLBACK DlgProcPipeOptions(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
    switch (nMsg)
    {
    case WM_INITDIALOG:
        DlgBoxSetItemDouble(hDlg, IDC_TIC_SIZE, pipe::dTicSize);
        DlgBoxSetItemDouble(hDlg, IDC_RISEDROP_RADIUS, pipe::dCSize);
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            pipe::dTicSize = DlgBoxGetItemDouble(hDlg, IDC_TIC_SIZE);
            pipe::dCSize = DlgBoxGetItemDouble(hDlg, IDC_RISEDROP_RADIUS);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;

        break;
    }
    return (FALSE);
}

