#pragma once

class CPrimSegRef : public CPrim
{
private:
	CString		m_strName;
	CPnt		m_pt;
	CVec		m_vZ;
	CVec		m_vScale;
	double		m_dRotation;

	WORD		m_wColCnt;
	WORD		m_wRowCnt;
	double		m_dColSpac;
	double		m_dRowSpac;

public: // Constructors and destructor
	CPrimSegRef();
	CPrimSegRef(const CString& strName, const CPnt& pt);
#if ODA_FUNCTIONALITY
	CPrimSegRef(AD_DB_HANDLE, PAD_ENT_HDR, PAD_ENT);
#endif
	CPrimSegRef(const CPrimSegRef&);
	
	virtual ~CPrimSegRef() {};

public: // Operators
	const CPrimSegRef& operator=(const CPrimSegRef&);

public: // Methods - absolute virtuals
	void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimSegRef*>(pPrim);}
	CTMat	BuildTransformMatrix(const CPnt& ptBase) const;
	CPrim*& Copy(CPrim*&) const;
	void	Display(CPegView* pView, CDC* pDC) const;
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Add(m_pt);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const {return m_pt;}
	bool	Is(WORD wType) const {return wType == PRIM_SEGREF;}
	bool	IsInView(CPegView* pView) const;
	bool	IsPtACtrlPt(CPegView*, const CPnt4&) const {return false;}
	void	Read(CFile&);
	CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
	bool	SelUsingLine(CPegView*, const CLine&, CPnts&) {return false;}
	bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
	bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
	void	Transform(const CTMat&);
	void	Translate(const CVec& v) {m_pt += v;}
	void	TranslateUsingMask(const CVec& v, const DWORD dwMask);
	bool	Write(CFile& fl) const;
	void	Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
	bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

public: // Methods
	WORD&		ColCnt() {return m_wColCnt;}
	double& 	ColSpacing() {return m_dColSpac;}
	CString		GetName() const {return m_strName;}
	double 		GetRotation() {return m_dRotation;}
	CVec		GetScale() const {return m_vScale;}
	CPnt&		InsPt() {return m_pt;}
	CVec&		ZRefVec() {return m_vZ;}
	WORD&		RowCnt() {return m_wRowCnt;}
	double& 	RowSpacing() {return m_dRowSpac;}
};
