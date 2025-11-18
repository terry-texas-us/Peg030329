#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

CSegsDet::CSegsDet()
{
	m_pDetSeg = 0;
	m_pDetPrim = 0;
	m_dPicApertSiz = .005;
}

void CSegsDet::AddTailSegsInActiveView(CPegView* pView, CLayer* pLayer)
{
	if (pLayer->IsOn())
	{
		if (pLayer->IsOpened() || pLayer->IsHot() || pLayer->IsWarm())
		{
			POSITION pos = pLayer->GetHeadPosition();
			while (pos != 0)
			{
				CSeg* pSeg = pLayer->GetNext(pos);
	
				if (pSeg->IsInView(pView))
				{
					AddTail(pSeg);
				}
			}
		}
	}
}
///<summary> Deletes last detectible segment.</summary>
void CSegsDet::DelLast()
{
	if (IsEmpty())
	{
		msgInformation(IDS_MSG_NO_DET_SEGS_IN_VIEW);
	}
	else
	{
		CPegDoc* pDoc = CPegDoc::GetDoc();
		CSeg* pSeg = RemoveTail();
		
		pDoc->AnyLayerRemove(pSeg);
		if (trapsegs.Remove(pSeg) != 0)
		{	// Display it normal color so the erase xor will work
			pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
			app.StatusLineDisplay(TrapCnt);
		}
		pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);
		pDoc->DeletedSegsAddTail(pSeg);
		msgInformation(IDS_SEG_DEL_TO_RESTORE);
	}
}
CSeg* CSegsDet::SelLineUsingPoint(CPegView* pView, const CPnt& pt)
{
	m_pDetSeg = 0;
	m_pDetPrim = 0;

	CPnt ptEng;
	
	CPnt4 ptView(pt, 1.);
	pView->ModelViewTransform(ptView);
	
	double tol = m_dPicApertSiz;

	CTMat tm = pView->ModelViewGetMatrixInverse();
	
	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->Is(CPrim::PRIM_LINE))
			{
				if (pPrim->SelUsingPoint(pView, ptView, tol, ptEng))
				{	
					tol = Pnt4_DistanceTo_xy(ptView, CPnt4(ptEng, 1.));
					
					m_ptDet = ptEng;
					m_ptDet = tm * m_ptDet;
					m_pDetSeg = pSeg;
					m_pDetPrim = pPrim;
				}
			}
		}
	}
	return (m_pDetSeg);
}

CPrimText* CSegsDet::SelTextUsingPoint(CPegView* pView, const CPnt& ptPic)
{
	CPnt4 ptView(ptPic, 1.);
	pView->ModelViewTransform(ptView);

	POSITION pos = GetHeadPosition();	
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		POSITION posPrim = pSeg->GetHeadPosition();
		while (posPrim !=0)
		{
			CPrim* pPrim = pSeg->GetNext(posPrim);
			if (pPrim->Is(CPrim::PRIM_TEXT))
			{
				CPnt ptProj;
				if (static_cast<CPrimText*>(pPrim)->SelUsingPoint(pView, ptView, 0., ptProj))
					return static_cast<CPrimText*>(pPrim);			
			}
		}
	}
	return 0;
}

CSeg* CSegsDet::SelSegAndPrimUsingPoint(const CPnt& pt)
{
	CPegView* pView = CPegView::GetActiveView();

	CPnt ptEng;
	
	m_pDetSeg = 0;
	m_pDetPrim = 0;

	CPnt4 ptView(pt, 1.);
	pView->ModelViewTransform(ptView);	
	
	CTMat tm = pView->ModelViewGetMatrixInverse();
	
	double dPicApert = m_dPicApertSiz;	
	
	CPrimPolygon::EdgeToEvaluate() = 0; 	

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		CPrim* pPrim = pSeg->SelPrimUsingPoint(pView, ptView, dPicApert, ptEng);
		if (pPrim != 0)
		{
			m_ptDet = ptEng;
			m_ptDet = tm * m_ptDet;
			m_pDetSeg = pSeg;
			m_pDetPrim = pPrim;
			return (pSeg);
		}
	}
	return 0; 	
}

CSeg* CSegsDet::SelSegAndPrimAtCtrlPt(const CPnt4& pt)
{
	CPegView* pView = CPegView::GetActiveView();

	CPrim* pPrim;
	CPnt ptEng;

	m_pDetSeg = 0;
	m_pDetPrim = 0;

	CTMat tm = pView->ModelViewGetMatrixInverse();

	POSITION pos = GetHeadPosition();
	while (pos != 0)
	{
		CSeg* pSeg = GetNext(pos);
		pPrim = pSeg->SelPrimAtCtrlPt(pView, pt, &ptEng);
		if (pPrim != 0)
		{
			m_ptDet = ptEng;
			m_ptDet = tm * m_ptDet;
			m_pDetSeg = pSeg;
			m_pDetPrim = pPrim;
		}
	}
	return (m_pDetSeg);
}	