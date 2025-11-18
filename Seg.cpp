#include "stdafx.h"

#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

HTREEITEM tvAddItem(HWND hTree, HTREEITEM hParent, LPCTSTR pszText, CObject* pOb);

// static member variable initializations

CPrim* CSeg::mS_pPrimIgnore = (CPrim*) 0;

CSeg::CSeg(const CSeg& seg)
{
	CPrim* pPrim;
	
	POSITION pos = seg.GetHeadPosition();
	while (pos != 0)
		AddTail((seg.GetNext(pos))->Copy(pPrim));
}
CSeg::CSeg(const CBlock& blk)
{
	CPrim* pPrim;
		
	POSITION pos = blk.GetHeadPosition();
	while (pos != 0)
		AddTail((blk.GetNext(pos))->Copy(pPrim));
}
void CSeg::AddPrimsToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->AddToTreeViewControl(hTree, hParent);
	}
}
HTREEITEM CSeg::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	HTREEITEM hti = tvAddItem(hTree, hParent, "<Segment>", (CObject*) this);
	AddPrimsToTreeViewControl(hTree, hti);
	return hti;
}
void CSeg::BreakPolylines()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		POSITION posPrim = pos;
		CPrim* pPrim = GetNext(pos);
		if (pPrim->Is(CPrim::PRIM_POLYLINE))
		{
			PENCOLOR nPenColor = pPrim->PenColor();
			PENSTYLE nPenStyle = pPrim->PenStyle();
			
			CPnts pts;
			static_cast<CPrimPolyline*>(pPrim)->GetAllPts(pts);
			
			for (WORD w = 0; w < pts.GetSize() - 1; w++)
				CObList::InsertBefore(posPrim, new CPrimLine(nPenColor, nPenStyle, pts[w], pts[w + 1]));
			
			if (static_cast<CPrimPolyline*>(pPrim)->IsLooped())
				CObList::InsertBefore(posPrim, new CPrimLine(nPenColor, nPenStyle, pts[pts.GetUpperBound()], pts[0]));
			
			this->RemoveAt(posPrim);
			delete pPrim;
		}
		else if(pPrim->Is(CPrim::PRIM_SEGREF))
		{
			CBlock* pBlock;
			if (CPegDoc::GetDoc()->BlksLookup(static_cast<CPrimSegRef*>(pPrim)->GetName(), pBlock) != 0)
			{
				pBlock->BreakPolylines();
			}
		}
	}
}
void CSeg::BreakSegRefs()
{
	int iSegRefs;
	do
	{
		iSegRefs = 0;
		POSITION pos = GetHeadPosition();
		while (pos != 0)
		{
			POSITION posPrim = pos;
			CPrim* pPrim = GetNext(pos);
			if (pPrim->Is(CPrim::PRIM_SEGREF))
			{
				iSegRefs++;
				CBlock* pBlock;
				if (CPegDoc::GetDoc()->BlksLookup(static_cast<CPrimSegRef*>(pPrim)->GetName(), pBlock) != 0)
				{
					CSeg* pSegT = new CSeg(*pBlock);
					CPnt ptBase = pBlock->GetBasePt();
					CTMat tm = static_cast<CPrimSegRef*>(pPrim)->BuildTransformMatrix(ptBase);
					pSegT->Transform(tm);
					
					this->InsertBefore(posPrim, pSegT);
					this->RemoveAt(posPrim);
					delete pPrim;
					pSegT->RemoveAll();
					delete pSegT;
				}
			}
		}
	}
	while (iSegRefs != 0);
}
void CSeg::Display(CPegView* pView, CDC* pDC)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->Display(pView, pDC);
	}
}
POSITION CSeg::FindAndRemovePrim(CPrim* pPrim)
{
	POSITION pos = Find(pPrim);
	
	if (pos != 0)
		RemoveAt(pos);
	
	return (pos);
}
void CSeg::InsertBefore(POSITION posPrim, CSeg* pSeg)
{
	POSITION pos = pSeg->GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = pSeg->GetNext(pos);
		CObList::InsertBefore(posPrim, (CObject*) pPrim);
	}
}
int CSeg::GetBlockRefCount(const CString& strBlkNam) const
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		if (pPrim->Is(CPrim::PRIM_SEGREF))
		{
			if (static_cast<CPrimSegRef*>(pPrim)->GetName() == strBlkNam)
				iCount++;
		}
	}
	return (iCount);
}
void CSeg::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->GetExtents(ptMin, ptMax, tm);
	}
}
int CSeg::GetPenStyleRefCount(PENSTYLE nPenStyle) const
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);

		if (pPrim->PenStyle() == nPenStyle)
		{
			iCount++;
		}
	}
	return (iCount);
}
bool CSeg::IsInView(CPegView* pView) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		if (pPrim->IsInView(pView))
		{
			return true;
		}
	}
	return false;
}
bool CSeg::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		if (pPrim->SelUsingRect(pView, pt1, pt2))
		{
			return true;
		}
	}
	return false;
}
void CSeg::ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		if (pPrim->Is(CPrim::PRIM_TEXT))
		{
			static_cast<CPrimText*>(pPrim)->ModifyNotes(fd, ccd, iAtt);
		}
	}
}
void CSeg::ModifyPenColor(PENCOLOR nPenColor)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->PenColor() = nPenColor;
	}
}
void CSeg::ModifyPenStyle(PENSTYLE nPenStyle)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->PenStyle() = nPenStyle;
	}
}
void CSeg::PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		
		for (WORD w = 0; w < wCols; w++)
		{
			if (pPrim->PenColor() == pCol[w])
			{
				pPrim->PenColor() = pColNew[w];
				break;
			}
		}
	}
}
int CSeg::RemoveEmptyNotes()
{
	int iCount = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		POSITION posPrev = pos;
		CPrim* pPrim = GetNext(pos);
		if (pPrim->Is(CPrim::PRIM_TEXT))
		{
			if (static_cast<CPrimText*>(pPrim)->Text().GetLength() == 0)
			{
				RemoveAt(posPrev);
				delete pPrim;	
				iCount++;
			}
		}
	}
	return (iCount);
}
///<summary>Picks a prim if close enough to point.  Working in view space.</summary>
CPrim* CSeg::SelPrimUsingPoint(CPegView* pView, const CPnt4& pt, double& dPicApert, CPnt& pDetPt)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		
		if (pPrim->SelUsingPoint(pView, pt, dPicApert, pDetPt))		
		{
			dPicApert = Pnt4_DistanceTo_xy(pt, CPnt4(pDetPt, 1.));			
			return (pPrim);
		}
	}
	return 0;
}	
CPrim* CSeg::SelPrimAtCtrlPt(CPegView* pView, const CPnt4& ptView, CPnt* ptCtrl) const
{
	CPrim*	pDetPrim = 0;
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);

		if (pPrim == mS_pPrimIgnore)
			continue;
		
		CPnt pt = pPrim->SelAtCtrlPt(pView, ptView);
		
		if (CPrim::CtrlPt() != USHRT_MAX)
		{
			pDetPrim = pPrim;
		
			CPnt4 ptView(pt, 1.);
			pView->ModelViewTransform(ptView);
			*ptCtrl = ptView;
		}
	}
	return (pDetPrim);
}
void CSeg::RemovePrims()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		delete (pPrim);
	}
	RemoveAll();
}
void CSeg::SortTextOnY()
{
	int iT;
	int iCount = (int) GetCount();
	
	do
	{
		iT = 0;
	
		POSITION pos = GetHeadPosition();
		for (int i = 1; i < iCount; i++)
		{
			POSITION pos1 = pos;
			CPrim* pPrim1 = GetNext(pos1);
			
			POSITION pos2 = pos1;
			CPrim* pPrim2 = GetNext(pos2);
				
			if (pPrim1->Is(CPrim::PRIM_TEXT) && pPrim2->Is(CPrim::PRIM_TEXT))
			{
				double dY1 = static_cast<CPrimText*>(pPrim1)->RefPt()[1];
				double dY2 = static_cast<CPrimText*>(pPrim2)->RefPt()[1];
				if (dY1 < dY2)
				{
					SetAt(pos, pPrim2);
					SetAt(pos1, pPrim1);
					iT = i;
				}
			}
			else if (pPrim1->Is(CPrim::PRIM_TEXT) || pPrim2->Is(CPrim::PRIM_TEXT))
			{
				SetAt(pos, pPrim2);
				SetAt(pos1, pPrim1);
				iT = i;
			}

			pos = pos1;
		}
		iCount = iT;
	}
	while (iT != 0);
}
void CSeg::Square()
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		if (pPrim->Is(CPrim::PRIM_LINE))
		{
			static_cast<CPrimLine*>(pPrim)->Square();
		}
	}
}
void CSeg::Transform(const CTMat& tm)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->Transform(tm);
	}
}
void CSeg::Translate(const CVec& v) const
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->Translate(v);
	}
}
void CSeg::Write(CFile& fl)
{
	FilePeg_WriteWord(fl, (WORD) GetCount());

	for (POSITION pos = GetHeadPosition(); pos != 0;)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->Write(fl);
	}
}
void CSeg::Write(CFile& f, char* p)
{
	// segment flags
	p[0] = 0;
	// number of primitives in segment
	*((short*) &p[1]) = short(GetCount());

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim =	GetNext(pos);
		pPrim->Write(f, p);
	}
}
void CSeg::Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen)
{
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = GetNext(pos);
		pPrim->Write(hdb, entlist, henhd, hen);
	}	
}