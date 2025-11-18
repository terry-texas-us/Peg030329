#pragma once

class CPrimPolyline : public CPrim
{
private:
	WORD	m_wFlags;
	CPnts	m_pts;

public:	// Constructors and destructor
	CPrimPolyline();
	CPrimPolyline(char*);
	CPrimPolyline(PAD_ENT);
	CPrimPolyline(CPnts& pts);
	CPrimPolyline(WORD wPts, CPnt* pPts);

	CPrimPolyline(const CPrimPolyline&);
public: // Operators
	const CPrimPolyline& operator=(const CPrimPolyline&);

	~CPrimPolyline() {}
	
public: // Methods - absolute virtuals
	void	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	void	Assign(CPrim* pPrim) {*this = *static_cast<CPrimPolyline*>(pPrim);}
	CPrim*& Copy(CPrim*&) const;
	void	Display(CPegView* pView, CDC* pDC) const;
	void	DisRep(const CPnt&) const;
	void	GetAllPts(CPnts& pts) {pts.SetSize(0); pts.Copy(m_pts);}
	void	FormatExtra(CString& str) const;
	void    FormatGeometry(CString& str) const;
	WORD	GetFlag() const {return m_wFlags;}
	CPnt	GetCtrlPt() const;
	void	GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPnt	GoToNxtCtrlPt() const;
	bool	Is(WORD wType) const {return wType == PRIM_POLYLINE;}
	bool	IsInView(CPegView* pView) const;
	bool	IsPtACtrlPt(CPegView*, const CPnt4&) const {return false;}
	bool	PvtOnCtrlPt(CPegView* pView, const CPnt4& ptView) const;
	void	Read(CFile&);
	CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
	bool	SelUsingLine(CPegView*, const CLine&, CPnts&) {return false;}
	bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
	bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
	void	SetFlag(const WORD w) {m_wFlags = w;}
	void	SetPt(WORD w, const CPnt& pt) {m_pts[w] = pt;}
	void	Transform(const CTMat&);	
	void	Translate(const CVec&);
	void	TranslateUsingMask(const CVec&, const DWORD);
	bool	Write(CFile&) const;
	void	Write(CFile&, char*) const;
	bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);

public: // Methods
	bool	IsLooped() const {return (m_wFlags & AD_LWPLINE_IS_CLOSED) == AD_LWPLINE_IS_CLOSED;}

private:
	WORD	SwingVertex() const;

private:
	static WORD mS_wEdgeToEvaluate;
	static WORD mS_wEdge;
	static WORD mS_wPivotVertex;
public:
	static WORD& EdgeToEvaluate() {return mS_wEdgeToEvaluate;}
	static WORD& Edge() {return mS_wEdge;}
};
