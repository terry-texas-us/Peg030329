#pragma once

HTREEITEM tvAddItem(HWND hTree, HTREEITEM hParent, LPCTSTR pszText, CObject* pOb);

typedef short PENSTYLE;
typedef short PENCOLOR;

class CPegView;
class CSegs;
class CSeg;

class CPrim : public CObject
{
public:
	static const WORD BUFFER_SIZE = 2048;

	static const PENCOLOR PENCOLOR_BYBLOCK = 0;
	static const PENCOLOR PENCOLOR_BYLAYER = 256;
	static const PENSTYLE PENSTYLE_BYBLOCK = 32766;
	static const PENSTYLE PENSTYLE_BYLAYER = 32767;

	static const WORD PRIM_MARK		= 256;		// 0x0100
	static const WORD PRIM_INSERT	= 257;		// 0x0101
	static const WORD PRIM_SEGREF	= 258;		// 0x0102
	static const WORD PRIM_LINE		= 512;		// 0x0200
	static const WORD PRIM_POLYGON	= 1024;		// 0x0400
	static const WORD PRIM_ARC		= 4099;		// 0x1003
	static const WORD PRIM_BSPLINE	= 8192;		// 0x2000
	static const WORD PRIM_CSPLINE	= 8193;		// 0x2001
	static const WORD PRIM_POLYLINE	= 8194;		// 0x2002
	static const WORD PRIM_TEXT		= 16384;	// 0x4000
	static const WORD PRIM_TAG		= 16640;	// 0x4100
	static const WORD PRIM_DIM		= 16896;	// 0x4200

protected:
	PENCOLOR	m_nPenColor;
	PENSTYLE	m_nPenStyle;

	static	PENCOLOR	mS_nLayerPenColor;
	static	PENSTYLE	mS_nLayerPenStyle;
	static	PENCOLOR	mS_nSpecPenColor;
	static	PENSTYLE	mS_nSpecPenStyle;
	static	short		mS_nSpecPolygonStyle;
	static	WORD		mS_wCtrlPt;
	static	double		mS_dRel;
	static	double		mS_dPicApertSiz;

public: // Constructors and destructor
	CPrim() {m_nPenColor = 0; m_nPenStyle = 0;}
	
	virtual ~CPrim() {}
	
public: // Methods - absolute virtuals
	virtual void	AddToTreeViewControl(HWND, HTREEITEM) const = 0;
	virtual void	Assign(CPrim*) = 0;
	virtual CPrim*&	Copy(CPrim*&) const = 0; 
	virtual void	Display(CPegView* pView, CDC* pDC) const = 0;
	virtual void	DisRep(const CPnt&) const = 0;
	virtual void	FormatExtra(CString& str) const = 0;
	virtual void	FormatGeometry(CString& str) const = 0;
	virtual void	GetAllPts(CPnts& pts) = 0;
	virtual CPnt	GetCtrlPt() const = 0;
	virtual void	GetExtents(CPnt&, CPnt&, const CTMat&) const = 0;
	virtual CPnt	GoToNxtCtrlPt() const = 0;
	virtual bool	Is(WORD wType) const = 0;
	virtual bool	IsInView(CPegView* pView) const = 0;
	virtual bool	IsPtACtrlPt(CPegView* pView, const CPnt4&) const = 0;
	virtual void	Read(CFile&) = 0;
	virtual CPnt	SelAtCtrlPt(CPegView* pView, const CPnt4&) const = 0;
	virtual bool	SelUsingLine(CPegView* pView, const CLine& ln, CPnts& ptsInt) = 0;
	virtual bool	SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&) = 0;
	virtual bool	SelUsingRect(CPegView* pView, const CPnt&, const CPnt&) = 0;
	virtual void	Transform(const CTMat&) = 0;
	virtual void	Translate(const CVec&) = 0;
	virtual void	TranslateUsingMask(const CVec&, const DWORD) = 0;
	virtual bool	Write(CFile& fl) const = 0;
	virtual void	Write(CFile& fl, char*) const = 0;
#if ODA_FUNCTIONALITY
	virtual bool	Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT) = 0;
#endif

public: // Methods - virtuals

	virtual void	CutAt2Pts(CPnt*, CSegs*, CSegs*) {}
	virtual void	CutAtPt(const CPnt&, CSeg*) {}
	virtual int 	IsWithinArea(const CPnt&, const CPnt&, CPnt*) {return 0;}
	virtual void	ModifyState();
	virtual bool	PvtOnCtrlPt(CPegView*, const CPnt4&) const {return false;}
	
public: // Methods
	CString		FormatPenColor() const;
	CString		FormatPenStyle() const;
	PENCOLOR	LogicalPenColor() const;
	PENSTYLE	LogicalPenStyle() const;
	PENCOLOR&	PenColor() {return m_nPenColor;}
	PENSTYLE&	PenStyle() {return m_nPenStyle;}

public:	// Methods - static 
	static WORD&		CtrlPt() {return mS_wCtrlPt;}
	static bool			IsSupportedTyp(int iTyp) {return (iTyp <= 7 && iTyp != 4 && iTyp != 5);}
	static PENCOLOR&	LayerPenColor() {return mS_nLayerPenColor;}
	static PENSTYLE&	LayerPenStyle() {return mS_nLayerPenStyle;}
	static double&		PicApertSiz() {return mS_dPicApertSiz;}
	static double&		Rel() {return mS_dRel;}
	static PENSTYLE&	SpecPenStyle() {return mS_nSpecPenStyle;}
	static PENCOLOR&	SpecPenColor() {return mS_nSpecPenColor;}
	static short&		SpecPolygonStyle() {return mS_nSpecPolygonStyle;}
};

CVec Prim_ComputeArbitraryAxis(const CVec& vNorm);

inline bool Prim_IsByBlockPenStyle(const char* sName) {return (_stricmp(sName, "ByBlock") == 0 ? true : false);}
inline bool Prim_IsByLayerPenStyle(const char* sName) {return (_stricmp(sName, "ByLayer") == 0 ? true : false);}
