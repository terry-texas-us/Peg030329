#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Preview.h"

void	BlockInsertDoOK(HWND);
LRESULT	BlockInsertGetCurSel(HWND, int, CString& strName);

CPnt* ptIns = 0;

BOOL CALLBACK DlgProcBlockInsert(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	switch (anMsg)
	{
		case WM_INITDIALOG:
		{
			ptIns = new CPnt(app.CursorPosGet());
				
			CString strKey;
			CBlock* pBlock;

			POSITION pos = pDoc->BlksGetStartPosition();
			while (pos != NULL)
			{
				pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
				if (!pBlock->IsAnonymous())
					::SendDlgItemMessage(hDlg, IDC_BLOCKS_LIST, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) strKey);
			}
			::SendDlgItemMessage(hDlg, IDC_BLOCKS_LIST, LB_SETCURSEL, 0, 0L);
			
			if (pDoc->BlksIsEmpty())
				WndProcPreviewClear(hDlg);
			else
			{
				pos = pDoc->BlksGetStartPosition();
				pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
				SetDlgItemInt(hDlg, IDC_SEGS, (UINT) pBlock->GetCount(), FALSE);
				SetDlgItemInt(hDlg, IDC_REFERENCES, pDoc->BlockGetRefCount(strKey), FALSE);
				WndProcPreviewUpdate(hDlg, pBlock);
			}
			return (TRUE);
		}
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_BLOCKS_LIST:
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						CBlock* pBlock;
						CString strName;
						BlockInsertGetCurSel(hDlg, IDC_BLOCKS_LIST, strName);
						pDoc->BlksLookup(strName, pBlock);
						SetDlgItemInt(hDlg, IDC_SEGS, (UINT) pBlock->GetCount(), FALSE);
						SetDlgItemInt(hDlg, IDC_REFERENCES, pDoc->BlockGetRefCount(strName), FALSE);
						WndProcPreviewUpdate(hDlg, pBlock);
					}
					break;

				case IDC_PURGE:
					pDoc->BlksRemoveUnused();
					delete ptIns;
					ptIns = 0;
					::EndDialog(hDlg, TRUE);
					return (TRUE);
				
				case IDOK:
					BlockInsertDoOK(hDlg);

				case IDCANCEL:
					delete ptIns;
					ptIns = 0;
					::EndDialog(hDlg, TRUE);
					return (TRUE);
			}
	}
	return (FALSE); 		
}
void BlockInsertDoOK(HWND hDlg)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	CString strBlkNam;
	if (BlockInsertGetCurSel(hDlg, IDC_BLOCKS_LIST, strBlkNam) != LB_ERR)
	{
		CPrimSegRef* pSegRef = new CPrimSegRef(strBlkNam, *ptIns);
	
		CSeg* pSeg = new CSeg(pSegRef);
		pDoc->WorkLayerAddTail(pSeg); 
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG, pSeg);
	}
}
LRESULT BlockInsertGetCurSel(HWND hDlg, int iListId, CString& strTitle)
{
	LRESULT lrCurSel = ::SendDlgItemMessage(hDlg, iListId, LB_GETCURSEL, 0, 0L);
	if (lrCurSel != LB_ERR)
	{
		char szTitle[MAX_PATH];
		::SendDlgItemMessage(hDlg, iListId, LB_GETTEXT, (WPARAM) lrCurSel, (LPARAM) ((LPSTR) szTitle));
		strTitle = szTitle;
	}
	return lrCurSel;
}