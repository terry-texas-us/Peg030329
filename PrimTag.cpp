#include "stdafx.h"

#include <sstream>

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "FilePeg.h"
#include "Messages.h"
#include "ModelTransform.h"
#include "OpenGL.h"
#include "PrimTag.h"

CPrimTag::CPrimTag(const CPrimTag& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_Pt = src.m_Pt;
}

const CPrimTag& CPrimTag::operator=(const CPrimTag& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nPenStyle = src.m_nPenStyle;
	m_Pt = src.m_Pt;
	return (*this);
}

void CPrimTag::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	tvAddItem(hTree, hParent, "<Tag>", (CObject*)this);
}
CPrim*& CPrimTag::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimTag(*this);
	return (pPrim);
}
void CPrimTag::Display(CPegView*, CDC* pDC) const
{
	if (pDC == 0)
	{
		PENCOLOR nPenColor = LogicalPenColor();
		opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

		opengl::BeginPoints();
		opengl::SetVertex(m_Pt);
		opengl::End();
	}
}
void CPrimTag::DisRep(const CPnt&) const
{
	std::string str = "<Tag>";
	str += " Color: " + StdFormatPenColor();
	str += " Style: " + StdFormatPenStyle();
	msgSetPaneText(str);
}
void CPrimTag::FormatExtra(CString& str) const
{
	std::stringstream ss;

	ss << "Color;" << StdFormatPenColor() << "\t"
		<< "Style;" << StdFormatPenStyle();

	str = ss.str().c_str();
}
void CPrimTag::FormatGeometry(CString& str) const
{
	std::stringstream ss;
	ss << "Point; " << m_Pt.ToStdString();
	str = ss.str().c_str();
}
CPnt CPrimTag::GetCtrlPt() const
{
	CPnt pt;
	pt = m_Pt;
	return (pt);
}
///<summary>Determines the extent.</summary>
void CPrimTag::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnt pt = m_Pt;

	mspace.Transform(pt);
	pt = tm * pt;
	ptMin = Min(ptMin, pt);
	ptMax = Max(ptMax, pt);
}
bool CPrimTag::IsInView(CPegView* pView) const
{
	CPnt4 pt(m_Pt, 1.);

	pView->ModelViewTransform(pt);

	return (Pnt4_IsInView(pt));
}
CPnt CPrimTag::SelAtCtrlPt(CPegView* pView, const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;

	CPnt4 pt(m_Pt, 1.);
	pView->ModelViewTransform(pt);

	if (Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz)
	{
		mS_wCtrlPt = 0;
		return m_Pt;
	}
	return ORIGIN;
}
bool CPrimTag::SelUsingPoint(CPegView* pView, const CPnt4& ptPic, double dTol, CPnt& ptProj)
{
	CPnt4 pt(m_Pt, 1.);

	pView->ModelViewTransform(pt);

	ptProj = pt;

	return (Pnt4_DistanceTo_xy(ptPic, pt) <= dTol) ? true : false;
}

bool CPrimTag::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
	CPnt4 pt(m_Pt, 1.);
	pView->ModelViewTransform(pt);

	return ((pt[0] >= pt1[0] && pt[0] <= pt2[0] && pt[1] >= pt1[1] && pt[1] <= pt2[1]) ? true : false);
}

bool CPrimTag::IsPtACtrlPt(CPegView* pView, const CPnt4& ptPic) const
{
	CPnt4 pt(m_Pt, 1.);
	pView->ModelViewTransform(pt);

	return ((Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz) ? true : false);
}

void CPrimTag::Read(CFile& fl)
{
	CPrim::Read(fl);

	m_Pt.Read(fl);
}
void CPrimTag::Transform(const CTMat& tm)
{
	m_Pt = tm * m_Pt;
}

void CPrimTag::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if (dwMask != 0)
		m_Pt += v;
}

bool CPrimTag::Write(CFile& fl) const
{
	FilePeg_WriteWord(fl, static_cast<WORD>(CPrim::Type::Tag));
	FilePeg_WriteWord(fl, m_nPenColor);
	FilePeg_WriteWord(fl, m_nPenStyle);
	m_Pt.Write(fl);
	return true;
}

