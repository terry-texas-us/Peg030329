#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Text.h"

BOOL CALLBACK DlgProcModeLetter(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();
						
	static CPnt ptPvt;
	
	HWND hWndTextCtrl = ::GetDlgItem(hDlg, IDC_TEXT);
	
	switch (nMsg) 
	{
		case WM_INITDIALOG:
		{
			ptPvt = app.CursorPosGet();
			return (TRUE);
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case IDOK:
				{
					CCharCellDef ccd;
					pstate.GetCharCellDef(ccd);
					CRefSys rs(ptPvt, ccd);
					
					CFontDef fd;
					pstate.GetFontDef(fd);
			
					int iLen = ::GetWindowTextLength(hWndTextCtrl);
					if (iLen != 0)
					{
						char* pszText = new char[iLen + 1];
						::GetWindowText(hWndTextCtrl, pszText, iLen + 1);
						::SetWindowText(hWndTextCtrl, "");
						
						CSeg* pSeg = new CSeg(new CPrimText(fd, rs, pszText));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						
						delete [] pszText;
					}
					ptPvt = text_GetNewLinePos(fd, rs, 1., 0);
					::SetFocus(hWndTextCtrl);
					return (TRUE);
				}
				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
			break;
		
		case WM_SIZE:
			::SetWindowPos(hWndTextCtrl, 0, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOMOVE);
        	break;
	}
	return (FALSE);
}
