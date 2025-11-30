#pragma once

#include <afxwin.h> // for MFC CString, WORD, DWORD, HWND, HTREEITEM, CDC, CFile

#include "Line.h" // for CLine member
#include "Pnt.h" // for CPnt member
#include "Pnt4.h" // for CPnt4
#include "Prim.h" // for CPrim base class
#include "TMat.h" // for CTMat
#include "Vec.h" // for CVec member

class CPegView;

class CPrimPolygon: public CPrim
{
private:
	short	m_nIntStyle;
	short	m_nIntStyleId;
	WORD	m_wPts;
	CPnt	m_ptOrig;
	CVec	m_vPosXAx;
	CVec	m_vPosYAx;
	CPnt* m_Pt;

public:	// Constructors and destructor
	CPrimPolygon();
	CPrimPolygon(char*, int iVer);
#if ODA_FUNCTIONALITY
	CPrimPolygon(AD_DB_HANDLE, PAD_ENT_HDR, PAD_ENT);
#endif
	CPrimPolygon(WORD, CPnt*);
	CPrimPolygon(WORD, CPnt, const CVec&, const CVec&, const CPnt*);

	CPrimPolygon(const CPrimPolygon& src);
public: // Operators
	const CPrimPolygon& operator=(const CPrimPolygon&);

	~CPrimPolygon();

public: // Methods - absolute virtuals
	void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	void	Assign(CPrim* pPrim) { *this = *static_cast<CPrimPolygon*>(pPrim); }
	CPrim*& Copy(CPrim*&) const;
	void	Display(CPegView* pView, CDC* pDC) const;
	void	DisRep(const CPnt&) const;
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	void	GetAllPts(CPnts& pts);
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	bool    Is(CPrim::Type type) const { return type == CPrim::Type::Polygon; }
	bool	IsPtACtrlPt(CPegView* pView, const CPnt4&) const;
	bool	IsInView(CPegView* pView) const;
	void	Read(CFile&);
	CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
	bool	SelUsingLine(CPegView*, const CLine&, CPnts&) { return false; }
	bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
	bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
	void	Transform(const CTMat&);
	void	Translate(const CVec&);
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Write(CFile&) const;
	void	Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
	bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

	CString			FormatIntStyle() const;
	const short& IntStyle() const { return (m_nIntStyle); }
	const short& IntStyleId() const { return (m_nIntStyleId); }
	CPnt			GetPt(int i) const { return (m_Pt[i]); }
	int 			GetPts() { return (m_wPts); }
	void			ModifyState();
	bool			PvtOnCtrlPt(CPegView* pView, const CPnt4&) const;
	void			SetIntStyle(const short n) { m_nIntStyle = n; }
	void			SetIntStyleId(const short n) { m_nIntStyleId = n; }
	void			SetHatRefVecs(double, double, double);

private:
	WORD	SwingVertex() const;

private:
	static WORD mS_wEdgeToEvaluate;
	static WORD mS_wEdge;
	static WORD mS_wPivotVertex;
public:
	static WORD& EdgeToEvaluate() { return mS_wEdgeToEvaluate; }
	static WORD& Edge() { return mS_wEdge; }
};
