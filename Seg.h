#pragma once

class CBlock;

class CSeg : public CObList
{
public:
	CSeg() {}
	CSeg(CPrim* p) {AddTail(p);}
	CSeg(const CSeg&);	
	CSeg(const CBlock&);

	void		AddPrimsToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	HTREEITEM	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	void		BreakPolylines();
	void		BreakSegRefs();
	void		Display(CPegView* pView, CDC* pDC);
	POSITION	FindAndRemovePrim(CPrim*);
	CPrim*		GetAt(POSITION pos) const {return (CPrim*) CObList::GetAt(pos);}
	int			GetBlockRefCount(const CString& strName) const;
	void		GetExtents(CPnt&, CPnt&, const CTMat&) const;
	CPrim*		GetNext(POSITION& pos) const {return ((CPrim*) CObList::GetNext(pos));}
	int			GetPenStyleRefCount(PENSTYLE nPenStyle) const;
	void		InsertBefore(POSITION pos, CSeg* pSeg);
	bool		IsInView(CPegView* pView) const;
	void		ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt = 0);
	void		ModifyPenColor(PENCOLOR nPenColor);
	void		ModifyPenStyle(PENSTYLE nPenStyle);
	void		PenTranslation(WORD, PENCOLOR*, PENCOLOR*);
	int			RemoveEmptyNotes();
	void		RemovePrims();
	CPrim*		SelPrimAtCtrlPt(CPegView* pView, const CPnt4&, CPnt*) const;
	CPrim*		SelPrimUsingPoint(CPegView* pView, const CPnt4&, double&, CPnt&);
	bool		SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2) const;
	void		SortTextOnY();
	void		Square();
	void		Transform(const CTMat& tm);
	void		Translate(const CVec&) const;
	void		Write(CFile&);
	void		Write(CFile&, char*);
#if ODA_FUNCTIONALITY
	void		Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

public:
	static CPrim*& IgnorePrim() {return mS_pPrimIgnore;}

private:
	static CPrim* mS_pPrimIgnore;
};
