#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>
#include <afxwin.h>

#include <string.h>

#include <string>

#include "Line.h"
#include "Pnt.h"
#include "Pnt4.h"
#include "TMat.h"
#include "Vec.h"

HTREEITEM tvAddItem(HWND hTree, HTREEITEM hParent, LPCTSTR pszText, CObject* pOb);

typedef short PENSTYLE;
typedef unsigned short PENCOLOR;

class CFile;
class CPegView;
class CSegs;
class CSeg;

class CPrim: public CObject
{
public:
    static inline constexpr size_t BUFFER_SIZE = 2048;

    static const PENCOLOR PENCOLOR_BYBLOCK = 0;
    static const PENCOLOR PENCOLOR_BYLAYER = 256;
    static const PENSTYLE PENSTYLE_BYBLOCK = 32766;
    static const PENSTYLE PENSTYLE_BYLAYER = 32767;

    enum class Type: WORD
    {
        Mark = 256,      // 0x0100
        Insert = 257,    // 0x0101
        SegRef = 258,    // 0x0102
        Line = 512,      // 0x0200
        Polygon = 1024,  // 0x0400
        Arc = 4099,      // 0x1003
        BSpline = 8192,  // 0x2000
        CSpline = 8193,  // 0x2001
        Polyline = 8194, // 0x2002
        Text = 16384,    // 0x4000
        Tag = 16640,     // 0x4100
        Dim = 16896      // 0x4200
    };

protected:
    PENCOLOR	m_nPenColor{1};
    PENSTYLE	m_nPenStyle{1};

    static	PENCOLOR	mS_nLayerPenColor;
    static	PENSTYLE	mS_nLayerPenStyle;
    static	PENCOLOR	mS_nSpecPenColor;
    static	PENSTYLE	mS_nSpecPenStyle;
    static	short		mS_nSpecPolygonStyle;
    static	WORD		mS_wCtrlPt;
    static	double		mS_dRel;
    static	double		mS_dPicApertSiz;

public: // Constructors and destructor
    CPrim() noexcept = default;
    explicit CPrim(PENCOLOR color, PENSTYLE style) noexcept
        : m_nPenColor(color), m_nPenStyle(style)
    { }

    CPrim(const CPrim&) = delete;
    CPrim& operator=(const CPrim&) = delete;

    virtual ~CPrim() { }

public: // Methods - absolute virtuals
    virtual void	AddToTreeViewControl(HWND, HTREEITEM) const = 0;
    virtual void	Assign(CPrim*) = 0;
    virtual CPrim*& Copy(CPrim*&) const = 0;
    virtual void	Display(CPegView* pView, CDC* pDC) const = 0;
    virtual void	DisRep(const CPnt&) const = 0;
    virtual void	FormatExtra(CString& str) const = 0;
    virtual void	FormatGeometry(CString& str) const = 0;
    virtual void	GetAllPts(CPnts& pts) = 0;
    virtual CPnt	GetCtrlPt() const = 0;
    virtual void	GetExtents(CPnt&, CPnt&, const CTMat&) const = 0;
    virtual CPnt	GoToNxtCtrlPt() const = 0;
    virtual bool	Is(CPrim::Type type) const = 0;
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

    virtual void	CutAt2Pts(CPnt*, CSegs*, CSegs*) { }
    virtual void	CutAtPt(const CPnt&, CSeg*) { }
    virtual int 	IsWithinArea(const CPnt&, const CPnt&, CPnt*) { return 0; }
    virtual void	ModifyState();
    virtual bool	PvtOnCtrlPt(CPegView*, const CPnt4&) const { return false; }

public: // Methods
    std::string StdFormatPenColor() const;
    std::string StdFormatPenStyle() const;
    PENCOLOR	LogicalPenColor() const;
    PENSTYLE	LogicalPenStyle() const;
    PENCOLOR& PenColor() { return m_nPenColor; }
    PENSTYLE& PenStyle() { return m_nPenStyle; }

public:	// Methods - static 
    static WORD& CtrlPt() { return mS_wCtrlPt; }
    static bool	IsSupportedTyp(int iTyp) { return (iTyp <= 7 && iTyp != 4 && iTyp != 5); }
    static PENCOLOR& LayerPenColor() { return mS_nLayerPenColor; }
    static PENSTYLE& LayerPenStyle() { return mS_nLayerPenStyle; }
    static double& PicApertSiz() { return mS_dPicApertSiz; }
    static double& Rel() { return mS_dRel; }
    static PENSTYLE& SpecPenStyle() { return mS_nSpecPenStyle; }
    static PENCOLOR& SpecPenColor() { return mS_nSpecPenColor; }
    static short& SpecPolygonStyle() { return mS_nSpecPolygonStyle; }
};

CVec Prim_ComputeArbitraryAxis(const CVec& vNorm);

inline bool Prim_IsByBlockPenStyle(const char* sName) { return (_stricmp(sName, "ByBlock") == 0 ? true : false); }
inline bool Prim_IsByLayerPenStyle(const char* sName) { return (_stricmp(sName, "ByLayer") == 0 ? true : false); }
