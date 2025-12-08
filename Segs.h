#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxcoll.h>
#include <afxstr.h>
#include <afxwin.h>

#include "Prim.h"

class CCharCellDef;
class CFontDef;
class CPegView;
class CPnt;
class CSeg;
class CTMat;
class CVec;

class CSegs: public CObList
{
public: // Constructors and destructor
    CSegs() { }

    virtual ~CSegs() { }

    // Explicitly delete copy constructor and assignment operator to fix C4625 and C4626 warnings
    CSegs(const CSegs&) = delete;
    CSegs& operator=(const CSegs&) = delete;

public:	// Base class wrappers
    POSITION	AddHead(CSeg* pSeg) { return (CObList::AddHead((CObject*)pSeg)); }
    POSITION	AddTail(CSeg* pSeg) { return (CObList::AddTail((CObject*)pSeg)); }
    void		AddTail(CSegs* pSegs) { CObList::AddTail((CObList*)pSegs); }
    POSITION	Find(CSeg* pSeg) { return (CObList::Find((CObject*)pSeg)); }
    CSeg* GetNext(POSITION& pos) const { return (CSeg*)CObList::GetNext(pos); }
    CSeg* GetPrev(POSITION& pos) const { return (CSeg*)CObList::GetPrev(pos); }
    CSeg* RemoveHead() { return (CSeg*)CObList::RemoveHead(); }
    CSeg* RemoveTail() { return (CSeg*)CObList::RemoveTail(); }

public: // Methods
    void		AddToTreeViewControl(HWND hTree, HTREEITEM htiParent) const;
    void		BreakPolylines();
    void		BreakSegRefs();
    void		Display(CPegView* pView, CDC* pDC) const;
    INT_PTR		GetBlockRefCount(const CString& blockName) const;
    INT_PTR     GetCount() const { return CObList::GetCount(); }
    void		GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const;
    int			GetPenStyleRefCount(PENSTYLE nPenStyle) const;
    void		ModifyNotes(const CFontDef& cd, const CCharCellDef& ccd, int iAtt = 0);
    void		ModifyPenColor(PENCOLOR nPenColor);
    void		ModifyPenStyle(PENSTYLE nPenStyle);
    void		PenTranslation(WORD, PENCOLOR*, PENCOLOR*);
    int			RemoveEmptyNotes();
    int			RemoveEmptySegs();
    void		RemoveSegs();
    POSITION	Remove(CSeg* pSeg);
    CSeg* SelSegUsingPoint(const CPnt& pt);
    void		Transform(const CTMat& tm);
    void		Translate(const CVec& v) const;
    void		Write(CFile& f, char* p);
};
