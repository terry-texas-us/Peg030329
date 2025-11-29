#pragma once

#include <afxwin.h> // for CDC and WORD

#include "Line.h" // for CLine
#include "Pnt.h" // for CPnt and CPnts (typedef CArray<CPnt, const CPnt&> CPnts;)
#include "Pnt4.h" // for CPnt4
#include "Vec.h" // for CVec

class CPegView;

class CPolyline
{
private:

	WORD	m_wFlags;		// bit 1 set - polygon is closed
							// bit 8 set - segment oriented linetype mapping
	CPnts	m_pts;
	
public: // Constructors and destructor
	CPolyline() {m_wFlags = 0;}
	CPolyline(CPnts& pts);

	CPolyline(const CPolyline& src);

	~CPolyline() {}
	
public: // Operators
	CPolyline& operator=(const CPolyline& src);
	
	void operator+=(const CVec& v);
	void operator-=(const CVec& v);
	
	CPnt& operator[](int i) {return m_pts[i];}
	const CPnt& operator[](int i) const {return m_pts[i];}	
};

namespace polyline
{
	void BeginLineStrip();
	void Display(CPegView* pView, CDC* pDC, const CPnts& pts);
	void Display(CPegView* pView, CDC* pDC, const CPnts& pts, short nPenStyle);
	void DisplayLoop(CPegView* pView, CDC* pDC, const CPnts& pts);
	void End(CPegView* pView, CDC* pDC, short nPenStyle);
	bool SelUsingLine(CPegView* pView, const CLine& ln, CPnts& ptsInt);
	bool SelUsingPoint(CPegView* pView, const CPnt4& ptPic, double dTol, double& dRel, CPnt& ptProj);
	bool SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2);
	bool SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2, const CPnts& pts);
	void SetVertex(const CPnt& pt);
}
