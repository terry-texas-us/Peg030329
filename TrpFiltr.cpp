#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

void trapFilterByPenColor(PENCOLOR);
void trapFilterByLineStyle(int);
void trapFilterByPrimType(WORD);

BOOL CALLBACK DlgProcTrapFilter(HWND ahDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	if (anMsg == WM_INITDIALOG)
	{
		CString strElementName[] = {"Arc", "Insert", "Line", "Marker", "Text", "Panel", "Polyline"};

		SetDlgItemInt(ahDlg, IDC_TRAP_FILTER_PEN_ID, 1, FALSE);

		int iTblSize = pDoc->PenStylesGetSize();
			
		for (int i = 0; i < iTblSize; i++)
		{
			CString strName = pDoc->PenStylesGetAt(i)->GetName();
			::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_LINE_LIST, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) strName);
		}
		::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_LINE_LIST, CB_SETCURSEL, 0, 0);

		for (WORD w = 0; w < sizeof(strElementName) / sizeof(strElementName[0]); w++)
			::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_ELEMENT_LIST, LB_ADDSTRING, 0, (LPARAM) (LPCSTR) strElementName[w]);
		
		::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_ELEMENT_LIST, LB_SETCURSEL, 0, 0L);
		return (TRUE);
	}
	else if (anMsg == WM_COMMAND)
	{
		PENCOLOR nPenColor = SHRT_MAX;
		int iElementId = - 1;
	
		switch (LOWORD(wParam))
		{
			case IDOK:
				if (IsDlgButtonChecked(ahDlg, IDC_TRAP_FILTER_PEN))
				{
					nPenColor = PENCOLOR(GetDlgItemInt(ahDlg, IDC_TRAP_FILTER_PEN_ID, 0, FALSE));
					trapFilterByPenColor(nPenColor);
				}
				if (IsDlgButtonChecked(ahDlg, IDC_TRAP_FILTER_LINE))
				{
					char szBuf[32];
					
					BOOL bTranslated = TRUE;
					
					int iLineId = (WORD) GetDlgItemInt(ahDlg, IDC_TRAP_FILTER_LINE_LIST, &bTranslated, TRUE);
					
					if (bTranslated == 0)
					{
						bTranslated = ::GetDlgItemText(ahDlg, IDC_TRAP_FILTER_LINE_LIST, (LPSTR) szBuf, sizeof(szBuf));
						if (bTranslated != 0)
							iLineId = pDoc->PenStylesLookup(szBuf);
						bTranslated = iLineId != SHRT_MAX;
					}
					if (bTranslated)
						trapFilterByLineStyle(iLineId);
				}
				if (IsDlgButtonChecked(ahDlg, IDC_TRAP_FILTER_ELEMENT))
				{
					iElementId = int(::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_ELEMENT_LIST, LB_GETCURSEL, 0, 0));
					
					switch (iElementId)	// **PrimitiveTypes**
					{
						case 0:
							trapFilterByPrimType(CPrim::PRIM_ARC);
							break;
						case 1:
							trapFilterByPrimType(CPrim::PRIM_SEGREF);
							break;
						case 2:
							trapFilterByPrimType(CPrim::PRIM_LINE);
							break;
						case 3:
							trapFilterByPrimType(CPrim::PRIM_MARK);
							break;
						case 4:
							trapFilterByPrimType(CPrim::PRIM_TEXT);
							break;
						case 5:
							trapFilterByPrimType(CPrim::PRIM_POLYGON);
							break;
						case 6:
							trapFilterByPrimType(CPrim::PRIM_POLYLINE);
					
					}
				}
			
			case IDCANCEL:
				::EndDialog(ahDlg, TRUE);
				return (TRUE);
		}
	}
	return (FALSE); 		
}

void trapFilterByLineStyle(int iLnId)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = trapsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->PenStyle() == iLnId)
			{	// Line match remove it
				trapsegs.Remove(pSeg);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
				break;							 
			}
		}
	}
	app.StatusLineDisplay(TrapCnt);
}
void trapFilterByPenColor(PENCOLOR nPenColor)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = trapsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->PenColor() == nPenColor)
			{	// Color match remove it
				trapsegs.Remove(pSeg);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
				break;							 
			}
		}
	}
	app.StatusLineDisplay(TrapCnt);
}
void trapFilterByPrimType(WORD wType)
{
	CPegDoc* pDoc = CPegDoc::GetDoc();

	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		bool bFilter = FALSE;
		
		CSeg* pSeg = trapsegs.GetNext(pos);
		
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			
			switch(wType)	// **PrimitiveTypes**
			{
				case CPrim::PRIM_LINE:
					bFilter = pPrim->Is(CPrim::PRIM_LINE);
					break;
				case CPrim::PRIM_ARC:
					bFilter = pPrim->Is(CPrim::PRIM_ARC);
					break;
				case CPrim::PRIM_SEGREF:
					bFilter = pPrim->Is(CPrim::PRIM_SEGREF);
					break;
				case CPrim::PRIM_TEXT:
					bFilter = pPrim->Is(CPrim::PRIM_TEXT);
					break;
				case CPrim::PRIM_POLYGON:
					bFilter = pPrim->Is(CPrim::PRIM_POLYGON);
					break;
				case CPrim::PRIM_POLYLINE:
					bFilter = pPrim->Is(CPrim::PRIM_POLYLINE);
					break;
				case CPrim::PRIM_MARK:
					bFilter = pPrim->Is(CPrim::PRIM_MARK);
					break;
			}
			if (bFilter)
			{
				trapsegs.Remove(pSeg);
				pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
				break;							 
			}
		}
	}
	app.StatusLineDisplay(TrapCnt);
}
