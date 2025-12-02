#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>
#include <afxwin.h>

#include "Line.h" // for CLine
#include "Pnt.h" // for CPnt member
#include "Pnt4.h" // for CPnt4
#include "Prim.h" // for CPrim base class
#include "TMat.h" // for CTMat
#include "Vec.h" // for CVec

class CPegView;

class CPrimBSpline: public CPrim
{
private:
    CPnts	m_pts;

public:	// Constructors and destructor
    CPrimBSpline() { }
    CPrimBSpline(char*, int iVer);
    CPrimBSpline(WORD, CPnt*);

    CPrimBSpline(const CPrimBSpline&);

    ~CPrimBSpline() { }

public: // Operators
    const CPrimBSpline& operator=(const CPrimBSpline&);

public: // Methods - absolute virtuals
    void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
    void	Assign(CPrim* pPrim) { *this = *static_cast<CPrimBSpline*>(pPrim); }
    CPrim*& Copy(CPrim*&) const;
    void	Display(CPegView* pView, CDC* pDC) const;
    void	DisRep(const CPnt&) const;
    int 	GenPts(const int, const CPnts& pts) const;
    void	GetAllPts(CPnts& pts) { pts.SetSize(0); pts.Copy(m_pts); }
    void	FormatExtra(CString& str) const;
    void    FormatGeometry(CString& str) const;
    CPnt	GetCtrlPt() const;
    void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
    CPnt	GoToNxtCtrlPt() const;
    bool	Is(CPrim::Type type) const { return type == CPrim::Type::BSpline; }
    bool	IsInView(CPegView* pView) const;
    bool	IsPtACtrlPt(CPegView*, const CPnt4&) const { return false; }
    void	Read(CFile&);
    CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
    bool	SelUsingLine(CPegView*, const CLine&, CPnts&) { return false; }
    bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
    bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
    void	SetPt(WORD w, const CPnt& pt) { m_pts[w] = pt; }
    void	Transform(const CTMat&);
    void	Translate(const CVec&);
    void	TranslateUsingMask(const CVec&, const DWORD);
    bool	Write(CFile&) const;
    void	Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
    bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif
};
