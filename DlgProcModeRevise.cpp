#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Text.h"

///<remarks>
///Text related attributes for all notes generated will be same as those of the text last picked.
///Upon exit attributes restored to their entry values.
///</remarks>
BOOL CALLBACK DlgProcModeRevise(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	static CFontDef fd;
	static CRefSys rs;
	static CPrimText* pText = 0;
	
	CPegView* pView = CPegView::GetActiveView();

	HWND hWndTextCtrl = ::GetDlgItem(hDlg, IDC_TEXT);

	switch (nMsg)
	{
		case WM_INITDIALOG:
			pText = detsegs.SelTextUsingPoint(pView, app.CursorPosGet());
			if (pText != 0)
			{
				pText->GetFontDef(fd);
				pText->GetRefSys(rs);
				::SetWindowText(hWndTextCtrl, pText->Text());
			}
			else
				::EndDialog(hDlg, TRUE);
			
			return (TRUE);
			
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{	
				case IDOK:
				{
					int iLen = ::GetWindowTextLength(hWndTextCtrl);
					
					char* pszNew = new char[iLen + 1];
					::GetWindowText(hWndTextCtrl, pszNew, iLen + 1);
					
					CPegDoc* pDoc = CPegDoc::GetDoc();

					if (pText != 0)
					{
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_ERASE_SAFE, pText);
						pText->SetText(pszNew);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_PRIM_SAFE, pText);
					}
					else
					{
						CSeg* pSeg = new CSeg(new CPrimText(fd, rs, pszNew)); 
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					}					
					delete [] pszNew;

					rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));
					
					pText = detsegs.SelTextUsingPoint(pView, rs.Origin());
					if (pText != 0)
					{
						pText->GetFontDef(fd);
						pText->GetRefSys(rs);
						::SetWindowText(hWndTextCtrl, pText->Text());
					}
					else
						SetWindowText(hWndTextCtrl, "");
					
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