#include "stdafx.h"

#include "PegAEsys.h"

BOOL CALLBACK DlgProcDrawOptions(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM  )
{
	switch (anMsg) 
	{
		case WM_INITDIALOG: 	// message: initialize dialog box
			::SendDlgItemMessage(hDlg, IDC_ARC_3_POINT, BM_SETCHECK, 1, 0L);
			::SendDlgItemMessage(hDlg, IDC_CURVE_SPLINE, BM_SETCHECK, 1, 0L);
			return (TRUE);


		case WM_COMMAND:		// message: received a command

			switch (LOWORD(wParam)) 
			{
				case IDOK:
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			
				case IDC_PEN:
					::EndDialog(hDlg, TRUE);
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_COLOR), app.GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupPenColor));
					app.StatusLineDisplay(Pen);
					break;

				case IDC_LINE:
					::EndDialog(hDlg, TRUE);
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_PENSTYLE), app.GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupPenStyle));
					app.StatusLineDisplay(Line);
					break;

				case IDC_TEXT:
					::EndDialog(hDlg, TRUE);
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_NOTE), app.GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupNote));
					break;

				case IDC_FILL:
					::EndDialog(hDlg, TRUE);
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_HATCH), app.GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupHatch));
					break;

				case IDC_CONSTRAINTS:
					::EndDialog(hDlg, TRUE);
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_CONSTRAINTS_GRID), app.GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupConstraints));
					break;
				
				default:
					break;
			}
			break;
	}
	return (FALSE); 		
}

