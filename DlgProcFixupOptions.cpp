#include "stdafx.h"

#include "SubProcFixup.h"
#include "UserAxis.h"

BOOL CALLBACK DlgProcFixupOptions(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM  )
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
				
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
			break;
	}
	return (FALSE); 		
}
