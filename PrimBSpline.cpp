#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "FilePeg.h"
#include "Messages.h"
#include "ModelTransform.h"
#include "OpenGL.h"
#include "Polyline.h"
#include "PrimBSpline.h"
#include "PrimState.h"

CPrimBSpline::CPrimBSpline(WORD wPts, CPnt* pt) {
  m_nPenColor = pstate.PenColor();
  m_nPenStyle = pstate.PenStyle();

  for (WORD w = 0; w < wPts; w++) m_pts.Add(pt[w]);
}

CPrimBSpline::CPrimBSpline(const CPrimBSpline& src) {
  m_nPenColor = src.m_nPenColor;
  m_nPenStyle = src.m_nPenStyle;
  m_pts.Copy(src.m_pts);
}

const CPrimBSpline& CPrimBSpline::operator=(const CPrimBSpline& src) {
  m_nPenColor = src.m_nPenColor;
  m_nPenStyle = src.m_nPenStyle;
  m_pts.Copy(src.m_pts);

  return (*this);
}

void CPrimBSpline::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const {
  tvAddItem(hTree, hParent, "<BSpline>", (CObject*)this);
}

CPrim*& CPrimBSpline::Copy(CPrim*& pPrim) const {
  pPrim = new CPrimBSpline(*this);
  return (pPrim);
}

void CPrimBSpline::Display(CPegView* pView, CDC* pDC) const {
  PENCOLOR nPenColor = LogicalPenColor();
  PENSTYLE nPenStyle = LogicalPenStyle();

  if (pDC == 0) {
    opengl::SetCurrentColor(app.PenColorsGetHot(nPenColor));
    opengl::BeginLineStrip();

    for (WORD w = 0; w < m_pts.GetSize(); w++) { opengl::SetVertex(m_pts[w]); }
    opengl::End();
  } else {
    pstate.SetPen(pDC, nPenColor, nPenStyle);

    polyline::BeginLineStrip();
    GenPts(3, m_pts);
    polyline::End(pView, pDC, nPenStyle);
  }
}
void CPrimBSpline::DisRep(const CPnt&) const {
  std::string str = "<BSpline> Color: " + StdFormatPenColor() + " Style: " + StdFormatPenStyle();
  msgSetPaneText(str);
}
void CPrimBSpline::FormatGeometry(CString& str) const {
  std::string geometry;
  for (WORD w = 0; w < m_pts.GetSize(); w++) { geometry += "Control Point;" + m_pts[w].ToStdString(); }
  str = geometry.c_str();
}

void CPrimBSpline::FormatExtra(CString& str) const {
  std::string extra;
  extra = "Color;" + StdFormatPenColor() + "\t";
  extra += "Style;" + StdFormatPenStyle() + "\t";
  extra += "Control Points;" + std::to_string(m_pts.GetSize());
  str = extra.c_str();
}

CPnt CPrimBSpline::GetCtrlPt() const {
  CPnt pt;
  pt = m_pts[m_pts.GetSize() / 2];
  return (pt);
}

///<summary>Determines the extent.</summary>
void CPrimBSpline::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const {
  CPnt pt;

  for (WORD w = 0; w < m_pts.GetSize(); w++) {
    pt = m_pts[w];
    mspace.Transform(pt);
    pt = tm * pt;
    ptMin = Min(ptMin, pt);
    ptMax = Max(ptMax, pt);
  }
}
CPnt CPrimBSpline::GoToNxtCtrlPt() const {
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
bool CPrimBSpline::IsInView(CPegView* pView) const {
  CPnt4 pt[2];

  pt[0] = m_pts[0];

  pView->ModelViewTransform(pt[0]);

  for (WORD w = 1; w < m_pts.GetSize(); w++) {
    pt[1] = m_pts[w];

    pView->ModelViewTransform(pt[1]);

    if (Pnt4_ClipLine(pt[0], pt[1])) return true;

    pt[0] = pt[1];
  }
  return false;
}
CPnt CPrimBSpline::SelAtCtrlPt(CPegView*, const CPnt4& ptPicVw) const {
  mS_wCtrlPt = USHRT_MAX;
  return (ptPicVw);
}
bool CPrimBSpline::SelUsingPoint(CPegView* pView, const CPnt4& ptPic, double dTol, CPnt& ptProj) {
  polyline::BeginLineStrip();

  GenPts(3, m_pts);

  return (polyline::SelUsingPoint(pView, ptPic, dTol, mS_dRel, ptProj));
}
bool CPrimBSpline::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2) {
  return polyline::SelUsingRect(pView, pt1, pt2, m_pts);
}
void CPrimBSpline::Read(CFile& fl) {
  CPrim::Read(fl);
  WORD wPts;
  FilePeg_ReadWord(fl, wPts);

  CPnt pt;
  for (WORD w = 0; w < wPts; w++) {
    pt.Read(fl);
    m_pts.Add(pt);
  }
}
void CPrimBSpline::Transform(const CTMat& tm) {
  for (WORD w = 0; w < m_pts.GetSize(); w++) m_pts[w] = tm * m_pts[w];
}

void CPrimBSpline::Translate(const CVec& v) {
  for (WORD w = 0; w < m_pts.GetSize(); w++) m_pts[w] += v;
}

void CPrimBSpline::TranslateUsingMask(const CVec& v, const DWORD dwMask) {
  for (WORD w = 0; w < m_pts.GetSize(); w++)
    if (((dwMask >> w) & 1UL) == 1) m_pts[w] += v;
}

bool CPrimBSpline::Write(CFile& fl) const {
  FilePeg_WriteWord(fl, static_cast<WORD>(CPrim::Type::BSpline));
  FilePeg_WriteWord(fl, m_nPenColor);
  FilePeg_WriteWord(fl, static_cast<WORD>(m_nPenStyle));
  FilePeg_WriteWord(fl, (WORD)m_pts.GetSize());

  for (WORD w = 0; w < m_pts.GetSize(); w++) m_pts[w].Write(fl);

  return true;
}
///<summary>
///Generates the required B-spline basis dKnot vectors and B-spline curves of various iOrder
///using the Cox and de Boor algorithm.
///</summary>
// Notes:	If the iOrder equals the number of vertices, and
//			there are no multiple vertices, a Bezier curve will
//			be generated. As the iOrder decreases the curve
//			produced will lie closer to the defining polygon.
//			When the iOrder is two the generated curve is a
//			series of straight lines which are identical to the
//			defining polygon.  Increasing the iOrder "tightens"
//			the curve.	Additional shape control can be obtained by
//			use of repeating vertices.
// Parameters:	iOrder 		iOrder of B-spline basis
//				iPts			number of points generated along curve
//				pt			1 based
int CPrimBSpline::GenPts(const int iOrder, const CPnts& pts) const {
  double* dKnot = new double[65 * 66];
  if (dKnot == 0) {
    msgWarning(IDS_MSG_MEM_ALLOC_ERR);
    return 0;
  }
  int iPts = 8 * static_cast<int>(pts.GetSize());
  double* dWght = &dKnot[65];

  int i, i2, i4;

  int pointsArraySize = static_cast<int>(pts.GetSize());
  int iTMax = (pointsArraySize - 1) - iOrder + 2;
  int iKnotVecMax = (pointsArraySize - 1) + iOrder;  // Maximum number of dKnot vectors

  for (i = 0; i < 65 * 65; i++)  // Set weighting value array with zeros
    dWght[i] = 0.;

  for (i = 0; i <= iKnotVecMax; i++)  // Determine dKnot vectors
  {
    if (i <= iOrder - 1)  // Beginning of curve
      dKnot[i] = 0.;
    else if (i >= iTMax + iOrder)  // End of curve
      dKnot[i] = dKnot[i - 1];
    else {
      i2 = i - iOrder;
      if (pts[i2] == pts[i2 + 1])  // Repeating vertices
        dKnot[i] = dKnot[i - 1];
      else  // Successive internal vectors
        dKnot[i] = dKnot[i - 1] + 1.;
    }
  }
  if (dKnot[iKnotVecMax] != 0.) {
    double G = 0.;
    double H = 0.;
    double Z = 0.;
    double T, W1, W2;
    double dStep = dKnot[iKnotVecMax] / (double)(iPts - 1);
    int iPts2 = 0;
    for (i4 = iOrder - 1; i4 <= iOrder + iTMax; i4++) {
      for (i = 0; i <= iKnotVecMax - 1; i++)  // Calculate values for weighting value
      {
        if (i != i4 || dKnot[i] == dKnot[i + 1])
          dWght[65 * i + 1] = 0.;
        else
          dWght[65 * i + 1] = 1.;
      }
      for (T = dKnot[i4]; T <= dKnot[i4 + 1] - dStep; T += dStep) {
        iPts2++;
        for (i2 = 2; i2 <= iOrder; i2++) {
          for (i = 0; i <= (int)pts.GetSize() - 1; i++)  // Determine first term of weighting function equation
          {
            if (dWght[65 * i + i2 - 1] == 0.)
              W1 = 0.;
            else
              W1 = ((T - dKnot[i]) * dWght[65 * i + i2 - 1]) / (dKnot[i + i2 - 1] - dKnot[i]);

            if (dWght[65 * (i + 1) + i2 - 1] == 0.)  // Determine second term of weighting function equation
              W2 = 0.;
            else
              W2 = ((dKnot[i + i2] - T) * dWght[65 * (i + 1) + i2 - 1]) / (dKnot[i + i2] - dKnot[i + 1]);

            dWght[65 * i + i2] = W1 + W2;
            G = pts[i][0] * dWght[65 * i + i2] + G;
            H = pts[i][1] * dWght[65 * i + i2] + H;
            Z = pts[i][2] * dWght[65 * i + i2] + Z;
          }
          if (i2 == iOrder) break;
          G = 0.;
          H = 0.;
          Z = 0.;
        }
        polyline::SetVertex(CPnt(G, H, Z));
        G = 0.;
        H = 0.;
        Z = 0.;
      }
    }
    iPts = iPts2 + 1;
  } else  // either iOrder greater than number of control points or all control points coincidental
  {
    iPts = 2;
    polyline::SetVertex(pts[0]);
  }
  polyline::SetVertex(pts[pts.GetUpperBound()]);

  delete[] dKnot;

  return (iPts);
}
