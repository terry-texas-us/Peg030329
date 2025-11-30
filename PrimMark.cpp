#include "stdafx.h"

#include <iomanip>
#include <sstream>

#include "FilePeg.h"
#include "Messages.h"
#include "ModelTransform.h"
#include "OpenGL.h"
#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "PrimMark.h"
#include "PrimState.h"

CPrimMark::CPrimMark()
{
	m_nPenColor = 1;
	m_nMarkStyle = 1;
	m_pt = ORIGIN;
	m_Dats = 0;
	m_dDat = 0;
}
CPrimMark::CPrimMark(PENCOLOR nPenColor, short nMarkStyle, const CPnt& pt) : m_pt(pt)
{
	m_nPenColor = nPenColor;
	m_nMarkStyle = nMarkStyle;
	m_Dats = 0;
	m_dDat = 0;
}
CPrimMark::CPrimMark(const CPrimMark& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nMarkStyle = src.m_nMarkStyle;
	m_pt = src.m_pt;
	m_Dats = src.m_Dats;
	m_dDat = (m_Dats == 0) ? 0 : new double[m_Dats];

	for (WORD w = 0; w < m_Dats; w++) m_dDat[w] = src.m_dDat[w];
}
CPrimMark::~CPrimMark()
{
	if (m_Dats != 0)
		delete [] m_dDat;
}
const CPrimMark& CPrimMark::operator=(const CPrimMark& src)
{
	m_nPenColor = src.m_nPenColor;
	m_nMarkStyle = src.m_nMarkStyle;;
	m_pt = src.m_pt;
	if (m_Dats != src.m_Dats)
	{
		if (m_Dats != 0)
			delete [] m_dDat;

		m_Dats = src.m_Dats;

		m_dDat = (m_Dats == 0) ? 0 : new double[m_Dats];
	}
	for (WORD w = 0; w < m_Dats; w++) m_dDat[w] = src.m_dDat[w];

	return (*this);
}
void CPrimMark::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	tvAddItem(hTree, hParent, "<Mark>", (CObject*)this);
}
CPrim*& CPrimMark::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimMark(*this);
	return (pPrim);
}
void CPrimMark::Display(CPegView* pView, CDC* pDC) const
{
	if (pDC == 0)
	{
		PENCOLOR nPenColor = LogicalPenColor();
		opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

		opengl::BeginPoints();
		opengl::SetVertex(m_pt);
		opengl::End();
	}
	else
	{
		CPnt4 pt(m_pt, 1.);
		pView->ModelViewTransform(pt);

		if (Pnt4_IsInView(pt))
		{
			CPoint pnt = pView->DoProjection(pt);
			int i;
			switch (m_nMarkStyle)
			{
			case 0:
				pDC->SetPixel(pnt, app.PenColorsGetHot(LogicalPenColor()));
				break;

			case 1:
				break;

			case 2: // small plus
				for (i = -4; i <= 4; i++)
				{
					pDC->SetPixel(pnt.x + i, pnt.y, crHotCols[LogicalPenColor()]);
					pDC->SetPixel(pnt.x, pnt.y + i, crHotCols[LogicalPenColor()]);
				}
				break;

			case 3: // small cross
				for (i = -4; i <= 4; i++)
				{
					pDC->SetPixel(pnt.x + i, pnt.y - i, crHotCols[LogicalPenColor()]);
					pDC->SetPixel(pnt.x + i, pnt.y + i, crHotCols[LogicalPenColor()]);
				}
				break;
			default: // small plus

				for (i = -3; i <= 3; i++)
				{
					pDC->SetPixel(pnt.x + i, pnt.y, crHotCols[LogicalPenColor()]);
					pDC->SetPixel(pnt.x, pnt.y + i, crHotCols[LogicalPenColor()]);
				}
				break;
			}
		}
	}
}
void CPrimMark::DisRep(const CPnt&) const
{
	std::stringstream ss;
	ss << "<Mark>" << " Color: " << StdFormatPenColor() << " Style: " << std::setw(3) << std::setfill('0') << m_nMarkStyle;
	std::string str = ss.str();
	msgSetPaneText(str);
}
void CPrimMark::FormatExtra(CString& str) const
{
	std::stringstream ss;
	ss << "Color;" << StdFormatPenColor() << "\t" << "Style;" << std::setw(3) << std::setfill('0') << m_nMarkStyle;
	str = ss.str().c_str();
}
void CPrimMark::FormatGeometry(CString& str) const
{
	std::stringstream ss;
	ss << "Point;" << m_pt.ToStdString();
	str = ss.str().c_str();
}
CPnt CPrimMark::GetCtrlPt() const
{
	CPnt pt;
	pt = m_pt;
	return (pt);
}
///<summary>Determines the extent.</summary>
void CPrimMark::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnt pt = m_pt;

	mspace.Transform(pt);
	pt = tm * pt;
	ptMin = Min(ptMin, pt);
	ptMax = Max(ptMax, pt);
}
bool CPrimMark::IsInView(CPegView* pView) const
{
	CPnt4 pt(m_pt, 1.);

	pView->ModelViewTransform(pt);

	return (Pnt4_IsInView(pt));
}
CPnt CPrimMark::SelAtCtrlPt(CPegView* pView, const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;

	CPnt4 pt(m_pt, 1.);
	pView->ModelViewTransform(pt);

	if (Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz)
	{
		mS_wCtrlPt = 0;
		return m_pt;
	}
	return ORIGIN;
}
bool CPrimMark::SelUsingPoint(CPegView* pView, const CPnt4& ptPic, double dTol, CPnt& ptProj)
{
	CPnt4 pt(m_pt, 1.);

	pView->ModelViewTransform(pt);

	ptProj = pt;

	return (Pnt4_DistanceTo_xy(ptPic, pt) <= dTol) ? true : false;
}
bool CPrimMark::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
	CPnt4 pt(m_pt, 1.);
	pView->ModelViewTransform(pt);

	return ((pt[0] >= pt1[0] && pt[0] <= pt2[0] && pt[1] >= pt1[1] && pt[1] <= pt2[1]) ? true : false);
}
bool CPrimMark::IsPtACtrlPt(CPegView* pView, const CPnt4& ptPic) const
{
	CPnt4 pt(m_pt, 1.);
	pView->ModelViewTransform(pt);

	return ((Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz) ? true : false);
}
void CPrimMark::ModifyState()
{
	CPrim::ModifyState();
	m_nMarkStyle = pstate.MarkStyle();
}
void CPrimMark::Read(CFile& fl)
{
	CPrim::Read(fl);

	m_pt.Read(fl);
	FilePeg_ReadWord(fl, m_Dats);

	if (m_Dats == 0)
		m_Dats = 0;
	else
		m_dDat = new double[m_Dats];

	for (WORD w = 0; w < m_Dats; w++)
		FilePeg_ReadDouble(fl, m_dDat[w]);
}
void CPrimMark::SetDat(WORD wDats, double* dDat)
{
	if (m_Dats != wDats)
	{
		if (m_Dats != 0)
		{
			delete [] m_dDat;
		}
		m_Dats = wDats;
		m_dDat = (m_Dats == 0) ? 0 : new double[m_Dats];
	}
	for (WORD w = 0; w < m_Dats; w++)
	{
		m_dDat[w] = dDat[w];
	}
}
void CPrimMark::Transform(const CTMat& tm)
{
	m_pt = tm * m_pt;
}
void CPrimMark::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if (dwMask != 0)
		m_pt += v;
}
bool CPrimMark::Write(CFile& fl) const
{
	FilePeg_WriteWord(fl, static_cast<WORD>(CPrim::Type::Mark));
	FilePeg_WriteWord(fl, m_nPenColor);
	FilePeg_WriteWord(fl, m_nMarkStyle);
	m_pt.Write(fl);
	FilePeg_WriteWord(fl, m_Dats);
	for (WORD w = 0; w < m_Dats; w++)
		FilePeg_WriteDouble(fl, m_dDat[w]);

	return true;
}
