#include "stdafx.h"

#include "SubProcPipe.h"

double DlgBoxGetItemDouble(HWND hDlg, int control);
void DlgBoxSetItemDouble(HWND hDlg, int control, double value);
void DlgBoxSetItemUnitsText(HWND hDlg, int control, double value);

INT_PTR CALLBACK DlgProcPipeOptions(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM) noexcept
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

