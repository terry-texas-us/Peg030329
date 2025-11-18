#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "DlgProcTrapModify.h"
#include "Text.h"

CPrimText::CPrimText(const CFontDef& fd, const CRefSys& rs, const char* pszText)
{
	m_nPenColor = pstate.PenColor();
	m_fd = fd;
	m_rs = rs;
	m_strText = pszText;
}
CPrimText::CPrimText(const CPrimText& src)
{
	m_nPenColor = src.m_nPenColor;
	m_fd = src.m_fd;
	m_rs = src.m_rs;
	m_strText = src.m_strText;
}
const CPrimText& CPrimText::operator=(const CPrimText& src)
{
	m_nPenColor = src.m_nPenColor;
	m_fd = src.m_fd;
	m_rs = src.m_rs;
	m_strText = src.m_strText;
	
	return (*this);
}
void CPrimText::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	tvAddItem(hTree, hParent, "<Text>", (CObject*) this);
}
CPrim*& CPrimText::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimText(*this);
	return (pPrim);
}
void CPrimText::Display(CPegView* pView, CDC* pDC) const
{
	if (pDC == 0)
	{
		PENCOLOR nPenColor = LogicalPenColor();
		opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

		text_Display0(pView, 0, m_fd, m_rs, m_strText);
	}
	else
	{
		pstate.SetPenColor(LogicalPenColor());
				
		PENSTYLE nPenStyle = pstate.PenStyle();
		pstate.SetPenStyle(1);
			
		text_Display0(pView, pDC, m_fd, m_rs, m_strText);
		pstate.SetPenStyle(nPenStyle);
	}
}
void CPrimText::DisRep(const CPnt&) const
{
	CString str;
	str.Format("Color: %s Font: %s Precision: %s Path: %s Alignment: (%s,%s)",
		FormatPenColor(), m_fd.TextFont(), m_fd.FormatTextPrec(), m_fd.FormatTextPath(), m_fd.FormatTextHorAlign());
	msgInformation(str);
}
void CPrimText::FormatExtra(CString& str) const
{
	str.Format("Color;%s\tFont;%s\tPrecision;%s\tPath;%s\tAlignment;(%s,%s)\tSpacing;%f\tLength;%d\tText;%s",
		FormatPenColor(), m_fd.TextFont(), m_fd.FormatTextPrec(), m_fd.FormatTextPath(), m_fd.FormatTextHorAlign(), 
		m_fd.FormatTextVerAlign(), m_fd.ChrSpac(), m_strText.GetLength(), m_strText);
}
void CPrimText::FormatGeometry(CString& str) const
{
	str += "Origin;" + m_rs.Origin().ToString();
	str += "X Axis;" + m_rs.DirX().ToString();
	str += "Y Axis;" + m_rs.DirY().ToString();
}
///<summary>Get the bounding box of text.</summary>
void CPrimText::GetBoundingBox(CPnts& ptsBox, double dSpacFac) const
{
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), dSpacFac, ptsBox);
}
CPnt CPrimText::GetCtrlPt() const
{
	return m_rs.Origin();
}
///<summary>Determines the extent.</summary>
void CPrimText::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CPnts pts;
		
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	
	for (WORD w = 0; w < pts.GetSize(); w++)
	{
		mspace.Transform(pts[w]);
		pts[w] = tm * pts[w];
		ptMin = Min(ptMin, pts[w]);
		ptMax = Max(ptMax, pts[w]);
	}
}
bool CPrimText::IsInView(CPegView* pView) const
{
	CPnt4 pt[2];
	
	CPnts pts;
	
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	
	for (size_t n = 0; n <= 2; ) 
	{
		pt[0] = pts[n++];
		pt[1] = pts[n++];

		pView->ModelViewTransform(2, pt);
		
		if (Pnt4_ClipLine(pt[0], pt[1]))
			return true;
	}
	return false;
}
bool CPrimText::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
	CPnts pts;
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	return polyline::SelUsingRect(pView, pt1, pt2, pts);
}
bool CPrimText::IsPtACtrlPt(CPegView* pView, const CPnt4& ptPic) const
{
	CPnt4 pt(m_rs.Origin(), 1.);
	pView->ModelViewTransform(pt);

	return ((Pnt4_DistanceTo_xy(ptPic, pt) < mS_dPicApertSiz) ? true : false);
}
void CPrimText::ModifyState()
{
	CPrim::ModifyState();
	
	pstate.GetFontDef(m_fd);
	
	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);

	m_rs.Rescale(ccd);
}
void CPrimText::ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt)
{
	if (iAtt == TM_TEXT_ALL)
	{
		m_nPenColor = pstate.PenColor();
		m_fd = fd;
		m_rs.Rescale(ccd);
	}
	else if (iAtt == TM_TEXT_FONT)
	{
		m_fd.TextFontSet(fd.TextFont());
		m_fd.TextPrecSet(fd.TextPrec());
	}
	else if (iAtt == TM_TEXT_HEIGHT)
	{
		m_fd.ChrSpacSet(fd.ChrSpac());
		m_fd.TextPathSet(fd.TextPath());
		
		m_rs.Rescale(ccd);
	}
}
CPnt CPrimText::SelAtCtrlPt(CPegView*, const CPnt4& ptPicVw) const
{
	mS_wCtrlPt = USHRT_MAX;
	return (ptPicVw);
}
///<summary>Evaluates whether a point lies within the bounding region of text.</summary>
bool CPrimText::SelUsingPoint(CPegView* pView, const CPnt4& ptView, double, CPnt& ptProj) 
{
	if (m_strText.GetLength() == 0)
		return false;

	CPnts pts;
	
	text_GetBoundingBox(m_fd, m_rs, m_strText.GetLength(), 0., pts);
	
	CPnt4 pt0[] = {CPnt4(pts[0], 1.), CPnt4(pts[1], 1.), CPnt4(pts[2], 1.), CPnt4(pts[3], 1.)};

	pView->ModelViewTransform(4, pt0);

	for (size_t n = 0; n < 4; n++)
	{
		if (CLine(pt0[n], pt0[(n + 1) % 4]).DirRelOfPt(ptView) < 0)
			return false;
	}
	ptProj = ptView;

	return true;
}
void CPrimText::Read(CFile& fl)
{
	CPrim::Read(fl);
	
	m_fd.Read(fl);
	m_rs.Read(fl);
	
	FilePeg_ReadString(fl, m_strText);
}
void CPrimText::Transform(const CTMat& tm)
{
	m_rs.Transform(tm);
}
void CPrimText::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
	if (dwMask != 0)
		m_rs.SetOrigin(m_rs.Origin() + v);
}
bool CPrimText::Write(CFile& fl) const
{
	FilePeg_WriteWord(fl, PRIM_TEXT);
	FilePeg_WriteWord(fl, m_nPenColor);
	FilePeg_WriteWord(fl, m_nPenStyle);
	m_fd.Write(fl);
	m_rs.Write(fl);
	FilePeg_WriteString(fl, m_strText);

	return true;
}