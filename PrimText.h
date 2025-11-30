#pragma once

#include <afxwin.h> // for MFC CString, DWORD, HWND, HTREEITEM, CDC, CFile

#include "FontDef.h" // for CFontDef member
#include "Line.h" // for CLine member
#include "Pnt.h" // for CPnt
#include "Pnt4.h" // for CPnt4
#include "Prim.h" // for CPrim base class
#include "RefSys.h" // for CRefSys member
#include "TMat.h" // for CTMat
#include "Vec.h" // for CVec

class CPegView;

class CPrimText: public CPrim
{
private:
	CFontDef	m_fd;
	CRefSys		m_rs;
	CString		m_strText;

public:	// Constructors and destructor
	CPrimText() { }
	CPrimText(char*, int iVer);
#if ODA_FUNCTIONALITY
	CPrimText(AD_DB_HANDLE hdb, PAD_ENT_HDR henhd, PAD_ENT hen);
#endif
	CPrimText(const CFontDef& fd, const CRefSys& rs, const char* pszText);
	CPrimText(const CPrimText&);

	~CPrimText() { };

public: // Operators
	const CPrimText& operator=(const CPrimText&);

public: // Methods - absolute virtuals
	void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	void	Assign(CPrim* pPrim) { *this = *static_cast<CPrimText*>(pPrim); }
	CPrim*& Copy(CPrim*&) const;
	void	Display(CPegView* pView, CDC* pDC) const;
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) { pts.SetSize(0); CPnt pt = m_rs.Origin(); pts.Add(pt); }
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const { return (m_rs.Origin()); }
	bool	IsInView(CPegView* pView) const;
	bool	Is(CPrim::Type type) const { return type == CPrim::Type::Text; }
	bool	IsPtACtrlPt(CPegView* pView, const CPnt4&) const;
	void	ModifyState();
	void	ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt);
	CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
	bool	SelUsingLine(CPegView*, const CLine&, CPnts&) { return false; }
	bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
	bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
	void	Translate(const CVec& v) { m_rs.SetOrigin(m_rs.Origin() + v); }
	void	TranslateUsingMask(const CVec&, const DWORD);
	void	Read(CFile&);
	void	Transform(const CTMat&);
	bool	Write(CFile&) const;
	void	Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
	bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

public: // Methods
	void				GetBoundingBox(CPnts&, double) const;
	void				GetFontDef(CFontDef& fd) const { fd = m_fd; }
	void				GetRefSys(CRefSys& rs) const { rs = m_rs; }
	const CString& Text() const { return m_strText; }
	CVec				RefNorm() const { CVec vNorm; m_rs.GetUnitNormal(vNorm); return vNorm; }
	CPnt				RefPt() const { return m_rs.Origin(); }

	void				SetText(const CString& str) { m_strText = str; }
};
