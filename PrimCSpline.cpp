#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "FilePeg.h"
#include "ModelTransform.h"
#include "OpenGL.h"
#include "Polyline.h"
#include "PrimCSpline.h"
#include "PrimState.h"

CPrimCSpline::CPrimCSpline(WORD nPtsS, WORD nEndCndId, CVec* vTan, const CPnts& pts)
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();

	m_wPtsS = nPtsS;
	m_wEndCndId = nEndCndId;
	m_TanVec[0] = vTan[0];
	m_TanVec[1] = vTan[1];
	m_pts.Copy(pts);
}

CPrimCSpline::CPrimCSpline(const CPrimCSpline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_wPtsS = src.m_wPtsS;
	m_wEndCndId = src.m_wEndCndId;
	m_TanVec[0] = src.m_TanVec[0];
	m_TanVec[1] = src.m_TanVec[1];
	m_pts.Copy(src.m_pts);
}
const CPrimCSpline& CPrimCSpline::operator=(const CPrimCSpline& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_wPtsS = src.m_wPtsS;
	m_wEndCndId = src.m_wEndCndId;
	m_TanVec[0] = src.m_TanVec[0];
	m_TanVec[1] = src.m_TanVec[1];
	m_pts.Copy(src.m_pts);

	return (*this);
}
void CPrimCSpline::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	tvAddItem(hTree, hParent, "<CSpline>", (CObject*)this);
}
CPrim*& CPrimCSpline::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimCSpline(*this);
	return (pPrim);
}
void CPrimCSpline::Display(CPegView*, CDC* pDC) const
{
	if (pDC == 0)
	{
		PENCOLOR nPenColor = LogicalPenColor();
		opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
		opengl::BeginLineStrip();

		for (WORD w = 0; w < m_pts.GetSize(); w++)
		{
			opengl::SetVertex(m_pts[w]);
		}
		opengl::End();
	}
}
void CPrimCSpline::FormatGeometry(CString& str) const
{
	std::string geometry;
	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		geometry += "Control Point;" + m_pts[w].ToStdString();
	}
	str = geometry.c_str();
}
void CPrimCSpline::FormatExtra(CString& str) const
{
	std::string extra;
	extra = "Color;" + StdFormatPenColor() + "\t";
	extra += "Style;" + StdFormatPenStyle() + "\t";
	extra += "Control Points;" + std::to_string(m_pts.GetSize());
	str = extra.c_str();
}
CPnt CPrimCSpline::GetCtrlPt() const
{
	CPnt pt;
	pt = m_pts[m_pts.GetSize() / 2];
	return (pt);
}
///<summary>Determines the extent.</summary>
void CPrimCSpline::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnt pt;

	for (WORD w = 0; w < m_pts.GetSize(); w++)
	{
		pt = m_pts[w];
		mspace.Transform(pt);
		pt = tm * pt;
		ptMin = Min(ptMin, pt);
		ptMax = Max(ptMax, pt);
	}
}
CPnt CPrimCSpline::GoToNxtCtrlPt() const
{
	CPnt pt;

	int i = static_cast<int>(m_pts.GetSize()) - 1;

	if (mS_dRel <= DBL_EPSILON)
		pt = m_pts[i];
	else if (mS_dRel >= 1. - DBL_EPSILON)
		pt = m_pts[0];
	else if (m_pts[i][0] > m_pts[0][0])
		pt = m_pts[0];
	else if (m_pts[i][0] < m_pts[0][0])
		pt = m_pts[i];
	else if (m_pts[i][1] > m_pts[0][1])
		pt = m_pts[0];
	else
		pt = m_pts[i];
	return (pt);
}
bool CPrimCSpline::IsInView(CPegView* pView) const
{
	CPnt4 pt[2];

	pt[0] = m_pts[0];
	pView->ModelViewTransform(pt[0]);

	for (WORD w = 1; w < m_pts.GetSize(); w++)
	{
		pt[1] = m_pts[w];
		pView->ModelViewTransform(pt[1]);

		if (Pnt4_ClipLine(pt[0], pt[1]))
			return true;

		pt[0] = pt[1];
	}
	return false;
}
bool CPrimCSpline::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
	return polyline::SelUsingRect(pView, pt1, pt2, m_pts);
}
void CPrimCSpline::Read(CFile& fl)
{
	CPrim::Read(fl);

	FilePeg_ReadWord(fl, m_wPtsS);
	WORD wPts;
	FilePeg_ReadWord(fl, wPts);
	FilePeg_ReadWord(fl, m_wEndCndId);

	m_TanVec[0].Read(fl);
	m_TanVec[1].Read(fl);

	CPnt pt;
	for (WORD w = 0; w < wPts; w++)
	{
		pt.Read(fl);
		m_pts.Add(pt);
	}
}
CPnt CPrimCSpline::SelAtCtrlPt(CPegView*, const CPnt4& ptPicVw) const
{
	mS_wCtrlPt = USHRT_MAX;
	return (ptPicVw);
}
void CPrimCSpline::Transform(const CTMat& tm)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] = tm * m_pts[w];

	if (m_wEndCndId == 1)
	{
		m_TanVec[0] = tm * m_TanVec[0];
		m_TanVec[1] = tm * m_TanVec[1];
	}
}
void CPrimCSpline::Translate(const CVec& v)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w] += v;
}
void CPrimCSpline::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	for (WORD w = 0; w < m_pts.GetSize(); w++)
		if (((dwMask >> w) & 1UL) == 1)
			m_pts[w] += v;
}
bool CPrimCSpline::Write(CFile& fl) const
{
	FilePeg_WriteWord(fl, static_cast<WORD>(CPrim::Type::CSpline));
	FilePeg_WriteWord(fl, m_nPenColor);
	FilePeg_WriteWord(fl, m_nPenStyle);
	FilePeg_WriteWord(fl, m_wPtsS);
	FilePeg_WriteWord(fl, (WORD)m_pts.GetSize());
	FilePeg_WriteWord(fl, m_wEndCndId);

	m_TanVec[0].Write(fl);
	m_TanVec[1].Write(fl);

	for (WORD w = 0; w < m_pts.GetSize(); w++)
		m_pts[w].Write(fl);

	return true;
}
