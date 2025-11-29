#pragma once

#include <afxwin.h> // for MFC CString, WORD, DWORD, HWND, HTREEITEM, CDC, CFile

#include "Line.h" // for CLine
#include "Messages.h"
#include "Pnt.h" // for CPnt member
#include "Pnt4.h" // for CPnt4
#include "Prim.h" // for CPrim base class
#include "TMat.h" // for CTMat
#include "Vec.h" // for CVec

class CPegView;

enum ECSplineEndCnds
{
	CSP_RELAXED, 
	CSP_CLAMPED, 
	CSP_CYCLIC, 
	CSP_ANTICYCLIC = 4
};
class CPrimCSpline : public CPrim
{
private:
	WORD	m_wPtsS;
	WORD	m_wEndCndId;
	CVec	m_TanVec[2];
	CPnts	m_pts;

public:	// Constructors and destructor
	CPrimCSpline() {}
	CPrimCSpline(char*);
	CPrimCSpline(WORD nPtsS, WORD, CVec*, const CPnts& pts);
	
	~CPrimCSpline() {}

	CPrimCSpline(const CPrimCSpline&);

public: // Operators
	const CPrimCSpline& operator=(const CPrimCSpline&);

public: // Methods - absolute virtuals
	void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimCSpline*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CPegView* pView, CDC* pDC) const;
	void	DisRep(const CPnt&) const { msgSetPaneText("C-Spline - "); }
	void	GetAllPts(CPnts& pts) { pts.SetSize(0); pts.Copy(m_pts);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	bool	Is(WORD wType) const { return wType == PRIM_CSPLINE;}
	bool	IsInView(CPegView* pView) const;
	bool	IsPtACtrlPt(CPegView*, const CPnt4&) const { return false; }
	void	Read(CFile&);
	CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
	bool	SelUsingLine(CPegView*, const CLine&, CPnts&) { return false; }
	bool	SelUsingPoint(CPegView*, const CPnt4&, double, CPnt&) { return false;}
	bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
	void	Transform(const CTMat&);
	void	Translate(const CVec&);
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Write(CFile&) const;
	void	Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
	bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT) { return false; }
#endif
};
