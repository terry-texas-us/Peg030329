#include "stdafx.h"

extern char szLeftMouseDown[60];
extern char	szRightMouseDown[60];
extern char	szLeftMouseUp[60];
extern char	szRightMouseUp[60];

INT_PTR CALLBACK DlgProcSetupMouse_Buttons(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept
{
    switch (anMsg)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_LEFT_DOWN, szLeftMouseDown);
        SetDlgItemText(hDlg, IDC_LEFT_UP, szLeftMouseUp);
        SetDlgItemText(hDlg, IDC_RIGHT_DOWN, szRightMouseDown);
        SetDlgItemText(hDlg, IDC_RIGHT_UP, szRightMouseUp);
        return (TRUE);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:
            ::GetDlgItemText(hDlg, IDC_LEFT_DOWN, szLeftMouseDown, sizeof(szLeftMouseDown));
            ::GetDlgItemText(hDlg, IDC_LEFT_UP, szLeftMouseUp, sizeof(szLeftMouseUp));
            ::GetDlgItemText(hDlg, IDC_RIGHT_DOWN, szRightMouseDown, sizeof(szRightMouseDown));
            ::GetDlgItemText(hDlg, IDC_RIGHT_UP, szRightMouseUp, sizeof(szRightMouseUp));
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}

