#include "stdafx.h"

#include "PegAEsysView.h"

HTREEITEM tvAddItem(HWND hTree, HTREEITEM hParent, LPCTSTR pszText, CObject* pOb);

void CSegs::AddToTreeViewControl(HWND hTree, HTREEITEM htiParent) const
{
	POSITION pos = GetHeadPosition();

	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		HTREEITEM htiSeg = pSeg->AddToTreeViewControl(hTree, htiParent);
		if (pSeg->GetCount() == 1) {TreeView_Expand(hTree, htiSeg, TVE_EXPAND);}
	}
}
			
void CSegs::BreakPolylines()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->BreakPolylines();
	}
}
void CSegs::BreakSegRefs()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->BreakSegRefs();
	}
}
void CSegs::Display(CPegView* pView, CDC* pDC) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->Display(pView, pDC);
	}
}
POSITION CSegs::Remove(CSeg* pSeg)
{
	POSITION pos = Find(pSeg);
	if (pos != 0)
		RemoveAt(pos);
	
	return (pos);
}

int CSegs::GetBlockRefCount(const CString& strBlkNam) const
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		iCount += pSeg->GetBlockRefCount(strBlkNam);
	}
	return (iCount);
}

///<summary>Determines the extent of all segments in list.</summary>
void CSegs::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->GetExtents(ptMin, ptMax, tm);
	}
}
int CSegs::GetPenStyleRefCount(PENSTYLE nPenStyle) const
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		iCount += pSeg->GetPenStyleRefCount(nPenStyle);
	}
	return (iCount);
}
void CSegs::ModifyPenColor(PENCOLOR nPenColor)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->ModifyPenColor(nPenColor);
	}
}

void CSegs::ModifyPenStyle(PENSTYLE nStyle)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
		(GetNext(pos))->ModifyPenStyle(nStyle);
}

void CSegs::ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->ModifyNotes(fd, ccd, iAtt);
	}
}

void CSegs::PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
		(GetNext(pos))->PenTranslation(wCols, pColNew, pCol);
}
int CSegs::RemoveEmptyNotes()
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		iCount += pSeg->RemoveEmptyNotes();
	}
	return (iCount);
}
int CSegs::RemoveEmptySegs()
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		POSITION posPrev = pos;
		CSeg* pSeg = GetNext(pos);
		if (pSeg->GetCount() == 0)
		{
			RemoveAt(posPrev);
			delete pSeg;
			iCount++;
		}
	}
	return (iCount);
}
void CSegs::RemoveSegs()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->RemovePrims();
		delete pSeg;
	}
	RemoveAll();
}
CSeg* CSegs::SelSegUsingPoint(const CPnt& pt)
{
	CPegView* pView = CPegView::GetActiveView();

	CPnt ptEng;
	
	CSeg* pPicSeg = 0;

	CPnt4 ptView(pt, 1.);
	pView->ModelViewTransform(ptView);	
	
	CTMat tm = pView->ModelViewGetMatrixInverse();
	
	double dPicApert = detsegs.PicApertSiz(); 
	
	CPrimPolygon::EdgeToEvaluate() = 0; 	

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		if (pSeg->SelPrimUsingPoint(pView, ptView, dPicApert, ptEng) != 0)
			pPicSeg = pSeg;
	}
	return (pPicSeg);		
}
void CSegs::Transform(const CTMat& tm)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->Transform(tm);
	}
}

void CSegs::Translate(const CVec& v) const
{
	if (v.IsNull())
		return;

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pSeg->Translate(v);
	}
}

void CSegs::Write(CFile& f, char* p)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0) 		
		GetNext(pos)->Write(f, p);
}
