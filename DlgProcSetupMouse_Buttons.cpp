#include "stdafx.h"

#include "resource.h"

extern char		szLeftMouseDown[60];
extern char		szRightMouseDown[60]; 
extern char		szLeftMouseUp[60];
extern char		szRightMouseUp[60];

BOOL CALLBACK DlgProcSetupMouse_Buttons(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
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
					// fall thru to end dialog
				
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
			break;
	}
	return (FALSE); 		
}

