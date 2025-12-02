#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>
#include <afxwin.h>

#include "Line.h"
#include "Pnt.h"
#include "Pnt4.h"
#include "Prim.h"
#include "TMat.h"
#include "Vec.h"

class CPegView;

class CPrimTag: public CPrim
{
private:
    CPnt	m_Pt;
public:	// Constructors and destructor
    CPrimTag() { }
    CPrimTag(const CPrimTag&);
    CPrimTag(char*);

public: // Operators
    const CPrimTag& operator=(const CPrimTag&);

public: // Methods - absolute virtuals
    void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
    void	Assign(CPrim* pPrim) { *this = *static_cast<CPrimTag*>(pPrim); }
    CPrim*& Copy(CPrim*&) const;
    void	Display(CPegView* pView, CDC* pDC) const;
    void	DisRep(const CPnt&) const;
    void	GetAllPts(CPnts& pts) { pts.SetSize(0); pts.Add(m_Pt); }
    void	FormatExtra(CString& str) const;
    void    FormatGeometry(CString& str) const;
    CPnt	GetCtrlPt() const;
    void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
    CPnt	GoToNxtCtrlPt() const { return (m_Pt); }
    bool	Is(CPrim::Type type) const { return type == CPrim::Type::Tag; }
    bool	IsInView(CPegView* pView) const;
    bool	IsPtACtrlPt(CPegView* pView, const CPnt4&) const;
    CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
    bool	SelUsingLine(CPegView*, const CLine&, CPnts&) { return false; }
    bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
    bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
    void	Translate(const CVec& v) { m_Pt += v; }
    void	TranslateUsingMask(const CVec&, const DWORD);
    void	Read(CFile&);
    void	Transform(const CTMat&);
    bool	Write(CFile&) const;
    void	Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
    bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif
};
