#pragma once

class CPrimMark : public CPrim
{
private:
	short	m_nMarkStyle;
	CPnt	m_pt;
	WORD	m_Dats;
	double* m_dDat;

public: // Constructors and destructor
	CPrimMark();
	CPrimMark(char*, int iVer);
#if ODA_FUNCTIONALITY
	CPrimMark(PAD_ENT);
#endif
	CPrimMark(PENCOLOR nPenColor, short nMarkStyle, const CPnt& pt);

	CPrimMark(const CPrimMark& src);

	~CPrimMark();		
	
public: // Operators
	const	CPrimMark& operator=(const CPrimMark& src);

public: // Methods - absolute virtuals
	void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimMark*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CPegView* pView, CDC* pDC) const;
	void	DisRep(const CPnt&) const;
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Add(m_pt);}
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const {return (m_pt);}
	bool	Is(WORD wType) const {return wType == PRIM_MARK;}
	bool	IsInView(CPegView* pView) const;
	bool	IsPtACtrlPt(CPegView* pView, const CPnt4&) const;
	void	Read(CFile&);
	CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
	bool	SelUsingLine(CPegView*, const CLine&, CPnts&) {return false;}
	bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
	bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
	void	Transform(const CTMat&); 
	void	Translate(const CVec& v) {m_pt += v;}
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Write(CFile&) const;
	void	Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
	bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

public: // Methods
	double	GetDat(WORD wDat) const {return (m_dDat[wDat]);}
	CPnt	GetPt() const {return (m_pt);}
	short&	MarkStyle() {return m_nMarkStyle;}
	void	ModifyState();
	void	SetDat(WORD, double*);
	void	SetPt(CPnt pt) {m_pt = pt;}
	
};
