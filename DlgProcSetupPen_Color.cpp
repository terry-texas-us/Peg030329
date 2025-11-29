#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "OwnerDraw.h"
#include "PrimState.h"

void SetupPenColor_DrawEntire(LPDRAWITEMSTRUCT, int);

BOOL CALLBACK DlgProcSetupPenColor(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM  lParam)
{
	char	szBuf[16];
	
	int 	i;
	PENCOLOR nPenColor;

	switch (anMsg) 
	{
		case WM_INITDIALOG:
			for (i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++)
			{
				_itoa_s(i, szBuf, sizeof(szBuf), 10);
				::SendDlgItemMessage(hDlg, IDC_COL_LIST, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) szBuf);
			}
			nPenColor = pstate.PenColor();
			::SendDlgItemMessage(hDlg, IDC_COL_LIST, CB_SETCURSEL, nPenColor, 0L);
			return (TRUE);

		case WM_COMMAND:										
		{
			switch (LOWORD(wParam)) 
			{			
				case IDOK:
					nPenColor = (PENCOLOR) ::GetDlgItemInt(hDlg, IDC_COL_LIST, 0, FALSE);
					pstate.SetPenColor(nPenColor);

				case IDCANCEL:
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
			break;
		}
		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
			if (lpDIS->itemID == - 1)							// Empty combo box
				OwnerDraw_Focus(lpDIS, 0);					// Draw only focus rectangle
			else
			{
				switch (lpDIS->itemAction)
				{
					case ODA_DRAWENTIRE:
						SetupPenColor_DrawEntire(lpDIS, - 2);
						break;
					
					case ODA_SELECT:
						OwnerDraw_Select(lpDIS, 0);				  
						break;
					
					case ODA_FOCUS:
						OwnerDraw_Focus(lpDIS, 0);
						break;
				}
			}
			return (TRUE);
		}
	}
	return (FALSE); 		
}
///<summary>Draw the pen color number and a rectangle filled with the pen color.</summary>
void  SetupPenColor_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate)
{
	CDC dc;
	dc.Attach(lpDIS->hDC);

	char szBuf[32];

	CRect rc;
	::CopyRect(&rc, &lpDIS->rcItem);

	::SendMessage(lpDIS->hwndItem, CB_GETLBTEXT, lpDIS->itemID, (LPARAM) (LPCSTR) szBuf);

	dc.SetTextColor(RGB(0, 0, 0));
	dc.SetBkColor(RGB(255, 255, 255)), 	

	dc.ExtTextOut(rc.right - 16, rc.top + 2, ETO_CLIPPED, &rc, szBuf, lstrlen(szBuf), 0);

	::InflateRect(&rc, inflate - 2, inflate - 2);
	rc.right -= 24;
	
	CBrush brush(crHotCols[lpDIS->itemID]);
	dc.FillRect(&rc, &brush);

	OwnerDraw_Select(lpDIS, inflate);
	OwnerDraw_Focus(lpDIS, inflate);
	
	dc.Detach();
}