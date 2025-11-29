#include "stdafx.h"
#include <algorithm>

#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "OpenGL.h"
#include "Polyline.h"
#include "PrimState.h"

namespace polyline
{
	CPnts	pts_;
}
CPolyline::CPolyline(CPnts& pts)
{
	m_wFlags = 0;
	m_pts.Copy(pts);
}
CPolyline::CPolyline(const CPolyline& src)
{
	m_wFlags = src.m_wFlags;
	m_pts.Copy(src.m_pts);
}
CPolyline& CPolyline::operator=(const CPolyline& src)
{
	m_wFlags = src.m_wFlags;
	m_pts.Copy(src.m_pts);

	return (*this);
}
void CPolyline::operator+=(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] += v;
}
void CPolyline::operator-=(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] -= v;
}
void polyline::BeginLineStrip()
{
	opengl::BeginLineStrip();
	polyline::pts_.SetSize(0);
}
///<summary>Outputs a simple polyline.</summary>
void polyline::Display(CPegView* pView, CDC* pDC, const CPnts& pts)
{
	if (pts.GetSize() < 2) return;

	CPnt4 ln[2];
	CPoint pnt[2];

	ln[0] = pts[0];
	pView->ModelViewTransform(ln[0]);

	for (int n = 1; n < pts.GetSize(); n++)
	{
		ln[1] = pts[n];

		pView->ModelViewTransform(ln[1]);
		CPnt ptT = ln[1];
		if (Pnt4_ClipLine(ln[0], ln[1]))
		{
			pView->DoProjection(pnt, 2, &ln[0]);
			pDC->Polyline(pnt, 2);
		}
		ln[0] = ptT;
	}
}
void polyline::Display(CPegView* pView, CDC* pDC, const CPnts& pts, short nPenStyle)
{
	if (CPrim::IsSupportedTyp(nPenStyle))
		polyline::Display(pView, pDC, pts);
	else
	{
		CPenStyle* pPenStyle = CPegDoc::GetDoc()->PenStylesGetAtSafe(nPenStyle);
		if (pPenStyle == 0) return;

		WORD wDefLen = pPenStyle->GetDefLen();
		if (wDefLen == 0) return;

		CPnt4 ln[2];
		CPoint pnt[2];
		CPnt pt[2];

		int iDashDefId = 0;

		double* dDash = new double[wDefLen];

		pPenStyle->GetDashLen(dDash);

		pstate.SetPenStyle(1);

		double dSecLen = std::max(.025/* * 96.*/, fabs(dDash[iDashDefId]));

		for (int i = 0; i < (int)pts.GetSize() - 1; i++)
		{
			CVec vLn(pts[i], pts[i + 1]);
			pt[0] = pts[i];

			double dVecLen = vLn.Length();
			double dRemDisToEnd = dVecLen;

			while (dSecLen <= dRemDisToEnd + DBL_EPSILON)
			{
				CVec vDash(vLn);
				vDash *= dSecLen / dVecLen;
				pt[1] = pt[0] + vDash;
				dRemDisToEnd -= dSecLen;
				if (dDash[iDashDefId] >= 0.)
				{
					ln[0] = pt[0];
					ln[1] = pt[1];

					pView->ModelViewTransform(2, ln);

					if (Pnt4_ClipLine(ln[0], ln[1]))
					{
						pView->DoProjection(pnt, 2, &ln[0]);
						pDC->Polyline(pnt, 2);
					}
				}
				pt[0] = pt[1];
				iDashDefId = (iDashDefId + 1) % wDefLen;
				dSecLen = std::max(0.025/* * 96.*/, fabs(dDash[iDashDefId]));
			}
			if (dRemDisToEnd > DBL_EPSILON)
			{	// Partial component of dash section must produced
				if (dDash[iDashDefId] >= 0.)
				{
					pt[1] = pts[i + 1];

					ln[0] = pt[0];
					ln[1] = pt[1];

					pView->ModelViewTransform(2, ln);

					if (Pnt4_ClipLine(ln[0], ln[1]))
					{
						pView->DoProjection(pnt, 2, &ln[0]);
						pDC->Polyline(pnt, 2);
					}
				}
			}
			// Length of dash remaining
			dSecLen -= dRemDisToEnd;
		}
		delete [] dDash;
		pstate.SetPenStyle(nPenStyle);
	}
}
///<summary>Outputs a simple polyline with closure.</summary>
void polyline::DisplayLoop(CPegView* pView, CDC* pDC, const CPnts& pts)
{
	if (pts.GetSize() < 2) return;

	CPnt4 ln[2];
	CPoint pnt[2];

	ln[0] = pts[0];
	pView->ModelViewTransform(ln[0]);

	for (int n = 1; n <= pts.GetSize(); n++)
	{
		ln[1] = pts[n % pts.GetSize()];

		pView->ModelViewTransform(ln[1]);
		CPnt ptT = ln[1];
		if (Pnt4_ClipLine(ln[0], ln[1]))
		{
			pView->DoProjection(pnt, 2, &ln[0]);
			pDC->Polyline(pnt, 2);
		}
		ln[0] = ptT;
	}
}
void polyline::End(CPegView* pView, CDC* pDC, short nPenStyle)
{
	if (pDC == 0)
		opengl::End();
	else
		polyline::Display(pView, pDC, polyline::pts_, nPenStyle);
}
///<summary>Determines how many times (if any), a line segment intersects with polyline.</summary>
bool polyline::SelUsingLine(CPegView* pView, const CLine& ln, CPnts& ptsInt)
{
	CPnt4 ptBeg(pts_[0], 1.);
	CPnt4 ptEnd;

	pView->ModelViewTransform(ptBeg);

	for (WORD w = 1; w < pts_.GetSize(); w++)
	{
		ptEnd = CPnt4(pts_[w], 1.);
		pView->ModelViewTransform(ptEnd);

		CPnt ptInt;
		if (line::Intersection_xy(ln, CLine(ptBeg, ptEnd), ptInt))
		{
			double dRel;
			line::RelOfPtToEndPts(ln, ptInt, dRel);
			if (dRel >= -DBL_EPSILON && dRel <= 1. + DBL_EPSILON)
			{
				line::RelOfPtToEndPts(CLine(ptBeg, ptEnd), ptInt, dRel);
				if (dRel >= -DBL_EPSILON && dRel <= 1. + DBL_EPSILON)
				{
					ptInt[2] = ptBeg[2] + dRel * (ptEnd[2] - ptBeg[2]);
					ptsInt.Add(ptInt);
				}
			}
		}
		ptBeg = ptEnd;
	}
	return (!ptsInt.IsEmpty());
}
bool polyline::SelUsingPoint(CPegView* pView, const CPnt4& ptPic, double dTol, double& dRel, CPnt& ptProj)
{
	bool bResult = false;

	double dProjDis;

	CPnt4 ptBeg(pts_[0], 1.);
	pView->ModelViewTransform(ptBeg);

	for (int i = 1; i < (int)pts_.GetSize(); i++)
	{
		CPnt4 ptEnd = CPnt4(pts_[i], 1.);
		pView->ModelViewTransform(ptEnd);
		if (line::EvalPtProx_xy(CLine(ptBeg, ptEnd), ptPic, dTol, ptProj, &dRel, &dProjDis))
		{
			ptProj[2] = ptBeg[2] + dRel * (ptEnd[2] - ptBeg[2]);
			bResult = true;
			break;
		}
		ptBeg = ptEnd;
	}
	return (bResult);
}
bool polyline::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
	CPnt4 ptBeg(pts_[0], 1.);
	pView->ModelViewTransform(ptBeg);

	for (WORD w = 1; w < pts_.GetSize(); w++)
	{
		CPnt4 ptEnd(pts_[w], 1.);
		pView->ModelViewTransform(ptEnd);

		if (p2DEvalWndLn(pt1, pt2, ptBeg, ptEnd))
			return true;

		ptBeg = ptEnd;
	}
	return false;
}
// Note considering possible closure (see pic)
bool polyline::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2, const CPnts& pts)
{
	CPnt4 ptBeg(pts[0], 1.);
	pView->ModelViewTransform(ptBeg);

	for (WORD w = 1; w < pts.GetSize(); w++)
	{
		CPnt4 ptEnd(pts[w], 1.);
		pView->ModelViewTransform(ptEnd);

		if (p2DEvalWndLn(pt1, pt2, ptBeg, ptEnd))
			return true;

		ptBeg = ptEnd;
	}
	return false;
}
void polyline::SetVertex(const CPnt& pt)
{
	opengl::SetVertex(pt);
	polyline::pts_.Add(pt);
}