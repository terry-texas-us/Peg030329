#include "stdafx.h"

#include <sstream>

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "ddeGItms.h"
#include "FilePeg.h"
#include "Messages.h"
#include "ModelTransform.h"
#include "OpenGL.h"
#include "Polyline.h"
#include "PrimPolyline.h"
#include "PrimState.h"
#include "SafeMath.h"
#include "UnitsString.h"

WORD CPrimPolyline::mS_wEdgeToEvaluate = 0;
WORD CPrimPolyline::mS_wEdge = 0;
WORD CPrimPolyline::mS_wPivotVertex = 0;

CPrimPolyline::CPrimPolyline()
{
    m_wFlags = 0;
}
CPrimPolyline::CPrimPolyline(CPnts& pts)
{
    m_nPenColor = pstate.PenColor();
    m_nPenStyle = pstate.PenStyle();

    m_wFlags = 0;
    m_pts.Copy(pts);
}
CPrimPolyline::CPrimPolyline(WORD wPts, CPnt* pPts)
{
    m_nPenColor = pstate.PenColor();
    m_nPenStyle = pstate.PenStyle();

    m_pts.SetSize(wPts);

    m_wFlags = 0;
    for (WORD w = 0; w < wPts; w++)
    {
        m_pts[w] = pPts[w];
    }
}
CPrimPolyline::CPrimPolyline(const CPrimPolyline& src)
{
    m_nPenColor = src.m_nPenColor;
    m_nPenStyle = src.m_nPenStyle;
    m_wFlags = src.m_wFlags;
    m_pts.Copy(src.m_pts);
}

const CPrimPolyline& CPrimPolyline::operator=(const CPrimPolyline& src)
{
    m_nPenColor = src.m_nPenColor;
    m_nPenStyle = src.m_nPenStyle;
    m_wFlags = src.m_wFlags;
    m_pts.Copy(src.m_pts);
    return (*this);
}

void CPrimPolyline::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
    tvAddItem(hTree, hParent, "<Polyline>", (CObject*)this);
}

CPrim*& CPrimPolyline::Copy(CPrim*& pPrim) const
{
    pPrim = new CPrimPolyline(*this);
    return (pPrim);
}

void CPrimPolyline::Display(CPegView* pView, CDC* pDC) const
{
    if (pDC == 0)
    {
        PENCOLOR nPenColor = LogicalPenColor();
        opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));

        if (IsLooped())
            opengl::BeginLineLoop();
        else
            opengl::BeginLineStrip();

        for (WORD w = 0; w < m_pts.GetSize(); w++)
        {
            opengl::SetVertex(m_pts[w]);
        }
        opengl::End();
    }
    else
    {
        pstate.SetPen(pDC, m_nPenColor, m_nPenStyle);

        if (IsLooped())
            polyline::DisplayLoop(pView, pDC, m_pts);
        else
            polyline::Display(pView, pDC, m_pts);
    }
}
void CPrimPolyline::DisRep(const CPnt& ptPic) const
{
    std::string strRep = "<Polyline Edge> ";

    WORD wPts = WORD(m_pts.GetSize());

    if (mS_wEdge > 0 && mS_wEdge <= wPts)
    {
        CPnt ptBeg = m_pts[mS_wEdge - 1];
        CPnt ptEnd = m_pts[mS_wEdge % wPts];

        if (mS_wPivotVertex < wPts)
        {
            ptBeg = m_pts[mS_wPivotVertex];
            ptEnd = m_pts[SwingVertex()];
        }
        double dAng;
        double dLen = CVec(ptBeg, ptEnd).Length();						// Length of edge

        if (CVec(ptPic, ptBeg).Length() > dLen * .5)
            dAng = line::GetAngAboutZAx(CLine(ptEnd, ptBeg));
        else
            dAng = line::GetAngAboutZAx(CLine(ptBeg, ptEnd));

        char szBuf[24]{};
        UnitsString_FormatLength(szBuf, sizeof(szBuf), app.GetUnits(), dLen);
        strRep += szBuf;
        sprintf_s(szBuf, sizeof(szBuf), " @ %6.2f degrees", dAng / RADIAN);
        strRep += szBuf;
        msgSetPaneText(strRep);

        app.SetEngLen(dLen);
        app.SetEngAngZ(dAng);
        dde::PostAdvise(dde::EngLenInfo);
        dde::PostAdvise(dde::EngAngZInfo);
    }
}
void CPrimPolyline::FormatGeometry(CString& str) const
{
    std::stringstream ss;

    for (WORD w = 0; w < m_pts.GetSize(); w++)
    {
        ss << "Point;" << m_pts[w].ToStdString() << "\n";
    }
    str = ss.str().c_str();
}
void CPrimPolyline::FormatExtra(CString& str) const
{
    std::stringstream ss;
    ss << "Color;" << StdFormatPenColor() << "\t"
        << "Style;" << StdFormatPenStyle() << "\t"
        << "Points;" << m_pts.GetSize();

    str = ss.str().c_str();
}
CPnt CPrimPolyline::GetCtrlPt() const
{
    WORD wPts = WORD(m_pts.GetSize());
    WORD wBeg = WORD(mS_wEdge - 1);
    WORD wEnd = WORD(mS_wEdge % wPts);
    CPnt pt = CLine(m_pts[wBeg], m_pts[wEnd]).ProjPtAlong(.5);
    return (pt);
}
///<summary>Determines the extent.</summary>
void CPrimPolyline::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
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
CPnt CPrimPolyline::GoToNxtCtrlPt() const
{
    WORD wPts = WORD(m_pts.GetSize());

    if (mS_wPivotVertex >= wPts)
    {	// have not yet rocked to a vertex
        WORD wBeg = WORD(mS_wEdge - 1);
        WORD wEnd = WORD(mS_wEdge % wPts);

        if (m_pts[wEnd][0] > m_pts[wBeg][0])
            mS_wPivotVertex = wBeg;
        else if (m_pts[wEnd][0] < m_pts[wBeg][0])
            mS_wPivotVertex = wEnd;
        else if (m_pts[wEnd][1] > m_pts[wBeg][1])
            mS_wPivotVertex = wBeg;
        else
            mS_wPivotVertex = wEnd;
    }
    else if (mS_wPivotVertex == 0)
    {
        if (mS_wEdge == 1)
            mS_wPivotVertex = 1;
        else
            mS_wPivotVertex = static_cast<WORD>(wPts - 1);
    }
    else if (mS_wPivotVertex == wPts - 1)
    {
        if (mS_wEdge == wPts)
            mS_wPivotVertex = 0;
        else
            mS_wPivotVertex--;
    }
    else
    {
        if (mS_wEdge == mS_wPivotVertex)
            mS_wPivotVertex--;
        else
            mS_wPivotVertex++;
    }
    return (m_pts[mS_wPivotVertex]);
}
bool CPrimPolyline::IsInView(CPegView* pView) const
{
    CPnt4	pt[2];

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
bool CPrimPolyline::PvtOnCtrlPt(CPegView* pView, const CPnt4& ptView) const
{
    WORD wPts = WORD(m_pts.GetSize());

    if (mS_wPivotVertex >= wPts)
        // Not engaged at a vertex
        return false;

    CPnt4 ptCtrl(m_pts[mS_wPivotVertex], 1.);
    pView->ModelViewTransform(ptCtrl);

    if (Pnt4_DistanceTo_xy(ptCtrl, ptView) >= mS_dPicApertSiz)
        // Not on proper vertex
        return false;

    if (mS_wPivotVertex == 0)
        mS_wEdge = WORD(mS_wEdge == 1 ? wPts : 1);
    else if (mS_wPivotVertex == wPts - 1)
        mS_wEdge = WORD(mS_wEdge == wPts ? mS_wEdge - 1 : wPts);
    else if (mS_wPivotVertex == mS_wEdge)
        mS_wEdge++;
    else
        mS_wEdge--;

    return true;
}
CPnt CPrimPolyline::SelAtCtrlPt(CPegView* pView, const CPnt4& ptPic) const
{
    WORD wPts = WORD(m_pts.GetSize());

    mS_wCtrlPt = USHRT_MAX;
    double dApert = mS_dPicApertSiz;

    mS_wPivotVertex = wPts;

    for (WORD w = 0; w < wPts; w++)
    {
        CPnt4 pt(m_pts[w], 1.);
        pView->ModelViewTransform(pt);

        double dDis = Pnt4_DistanceTo_xy(ptPic, pt);

        if (dDis < dApert)
        {
            mS_wCtrlPt = w;
            dApert = dDis;

            mS_wEdge = WORD(w + 1);
            mS_wPivotVertex = w;
        }
    }
    return (mS_wCtrlPt == USHRT_MAX) ? ORIGIN : m_pts[mS_wCtrlPt];
}
bool CPrimPolyline::SelUsingPoint(CPegView* pView, const CPnt4& pt, double dTol, CPnt& ptProj)
{
    double dProjDis;

    WORD wPts = WORD(m_pts.GetSize());
    if (mS_wEdgeToEvaluate > 0 && mS_wEdgeToEvaluate <= wPts)
    {	// Evaluate specified edge of polyline
        CPnt4 ptBeg(m_pts[mS_wEdgeToEvaluate - 1], 1.);
        CPnt4 ptEnd(m_pts[mS_wEdgeToEvaluate % wPts], 1.);

        pView->ModelViewTransform(ptBeg);
        pView->ModelViewTransform(ptEnd);

        if (line::EvalPtProx_xy(CLine(ptBeg, ptEnd), pt, dTol, ptProj, &mS_dRel, &dProjDis))
        {
            ptProj[2] = ptBeg[2] + mS_dRel * (ptEnd[2] - ptBeg[2]);
            return true;
        }
    }
    else
    {	// Evaluate entire polyline
        WORD wEdges = (WORD)m_pts.GetSize();
        if (!IsLooped())
            wEdges--;

        CPnt4 ptBeg(m_pts[0], 1.);
        pView->ModelViewTransform(ptBeg);

        for (WORD w = 1; w <= wEdges; w++)
        {
            CPnt4 ptEnd(m_pts[w % wPts], 1.);
            pView->ModelViewTransform(ptEnd);

            if (line::EvalPtProx_xy(CLine(ptBeg, ptEnd), pt, dTol, ptProj, &mS_dRel, &dProjDis))
            {
                ptProj[2] = ptBeg[2] + mS_dRel * (ptEnd[2] - ptBeg[2]);
                mS_wEdge = w;
                mS_wPivotVertex = wPts;
                return true;
            }
            ptBeg = ptEnd;
        }
    }
    return false;
}
bool CPrimPolyline::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
    return polyline::SelUsingRect(pView, pt1, pt2, m_pts);
}
void CPrimPolyline::Read(CFile& fl)
{
    CPrim::Read(fl);
    WORD wPts;
    FilePeg_ReadWord(fl, wPts);

    CPnt pt;
    for (WORD w = 0; w < wPts; w++)
    {
        pt.Read(fl);
        m_pts.Add(pt);
    }
}
void CPrimPolyline::Transform(const CTMat& tm)
{
    for (WORD w = 0; w < m_pts.GetSize(); w++)
        m_pts[w] = tm * m_pts[w];
}
void CPrimPolyline::Translate(const CVec& v)
{
    for (WORD w = 0; w < m_pts.GetSize(); w++)
        m_pts[w] += v;
}
void CPrimPolyline::TranslateUsingMask(const CVec& v, const DWORD dwMask)
{
    for (WORD w = 0; w < m_pts.GetSize(); w++)
        if (((dwMask >> w) & 1UL) == 1)
            m_pts[w] += v;
}
bool CPrimPolyline::Write(CFile& fl) const
{
    FilePeg_WriteWord(fl, static_cast<WORD>(CPrim::Type::Polyline));
    FilePeg_WriteWord(fl, m_nPenColor);
    FilePeg_WriteWord(fl, static_cast<WORD>(m_nPenStyle));
    FilePeg_WriteWord(fl, (WORD)m_pts.GetSize());

    for (WORD w = 0; w < m_pts.GetSize(); w++)
        m_pts[w].Write(fl);

    return true;
}
WORD CPrimPolyline::SwingVertex() const
{
    WORD wPts = WORD(m_pts.GetSize());

    WORD wSwingVertex;

    if (mS_wPivotVertex == 0)
        wSwingVertex = WORD(mS_wEdge == 1 ? 1 : wPts - 1);
    else if (mS_wPivotVertex == WORD(wPts - 1))
        wSwingVertex = WORD(mS_wEdge == wPts ? 0 : mS_wPivotVertex - 1);
    else
        wSwingVertex = WORD(mS_wEdge == mS_wPivotVertex ? mS_wPivotVertex - 1 : mS_wPivotVertex + 1);

    return (wSwingVertex);
}
