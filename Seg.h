#pragma once

class CBlock;

class CSeg : public CObList
{
public:
	/// <summary>Default constructor.</summary>
	CSeg() {}
	/// <summary>Constructor that adds a single primitive.</summary>
	/// <param name="p">Pointer to the primitive to add.</param>
	CSeg(CPrim* p) {AddTail(p);}
	/// <summary>Copy constructor.</summary>
	/// <param name="seg">The segment to copy from.</param>
	CSeg(const CSeg&);	
	/// <summary>Constructor from a block.</summary>
	/// <param name="blk">The block to initialize from.</param>
	CSeg(const CBlock&);

	/// <summary>Adds primitives to the tree view control.</summary>
	/// <param name="hTree">Handle to the tree view.</param>
	/// <param name="hParent">Parent tree item.</param>
	void		AddPrimsToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	/// <summary>Adds the segment to the tree view control.</summary>
	/// <param name="hTree">Handle to the tree view.</param>
	/// <param name="hParent">Parent tree item.</param>
	/// <returns>The tree item handle.</returns>
	HTREEITEM	AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
	/// <summary>Breaks polylines into individual lines.</summary>
	void		BreakPolylines();
	/// <summary>Breaks segment references.</summary>
	void		BreakSegRefs();
	/// <summary>Displays the segment.</summary>
	/// <param name="pView">Pointer to the view.</param>
	/// <param name="pDC">Pointer to the device context.</param>
	void		Display(CPegView* pView, CDC* pDC);
	/// <summary>Finds and removes a primitive.</summary>
	/// <param name="pPrim">Pointer to the primitive to remove.</param>
	/// <returns>The position of the removed primitive.</returns>
	POSITION	FindAndRemovePrim(CPrim* pPrim);
	/// <summary>Gets the primitive at the specified position.</summary>
	/// <param name="pos">The position.</param>
	/// <returns>Pointer to the primitive.</returns>
	CPrim*		GetAt(POSITION pos) const {return (CPrim*) CObList::GetAt(pos);}
	/// <summary>Gets the count of block references with the given name.</summary>
	/// <param name="strName">The block name.</param>
	/// <returns>The count.</returns>
	int			GetBlockRefCount(const CString& strName) const;
	/// <summary>Gets the count of primitives in the segment.</summary>
	/// <returns>The count.</returns>
	int         GetCount() const { return static_cast<int>(CObList::GetCount()); }
	/// <summary>Gets the extents of the segment.</summary>
	/// <param name="ptMin">Minimum point.</param>
	/// <param name="ptMax">Maximum point.</param>
	/// <param name="tm">Transformation matrix.</param>
	void		GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const;
	/// <summary>Gets the next primitive in the list.</summary>
	/// <param name="pos">The position, updated to the next.</param>
	/// <returns>Pointer to the primitive.</returns>
	CPrim*		GetNext(POSITION& pos) const {return ((CPrim*) CObList::GetNext(pos));}
	/// <summary>Gets the count of primitives with the given pen style.</summary>
	/// <param name="nPenStyle">The pen style.</param>
	/// <returns>The count.</returns>
	int			GetPenStyleRefCount(PENSTYLE nPenStyle) const;
	/// <summary>Inserts a segment before the specified position.</summary>
	/// <param name="pos">The position.</param>
	/// <param name="pSeg">Pointer to the segment to insert.</param>
	void		InsertBefore(POSITION pos, CSeg* pSeg);
	/// <summary>Checks if the segment is in view.</summary>
	/// <param name="pView">Pointer to the view.</param>
	/// <returns>True if in view.</returns>
	bool		IsInView(CPegView* pView) const;
	/// <summary>Modifies notes in text primitives.</summary>
	/// <param name="fd">Font definition.</param>
	/// <param name="ccd">Character cell definition.</param>
	/// <param name="iAtt">Attribute.</param>
	void		ModifyNotes(const CFontDef& fd, const CCharCellDef& ccd, int iAtt = 0);
	/// <summary>Modifies the pen color of all primitives.</summary>
	/// <param name="nPenColor">The new pen color.</param>
	void		ModifyPenColor(PENCOLOR nPenColor);
	/// <summary>Modifies the pen style of all primitives.</summary>
	/// <param name="nPenStyle">The new pen style.</param>
	void		ModifyPenStyle(PENSTYLE nPenStyle);
	/// <summary>Translates pen colors.</summary>
	/// <param name="wCols">Number of colors.</param>
	/// <param name="pColNew">New colors array.</param>
	/// <param name="pCol">Old colors array.</param>
	void		PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol);
	/// <summary>Removes empty notes.</summary>
	/// <returns>The number of removed notes.</returns>
	int			RemoveEmptyNotes();
	/// <summary>Removes all primitives.</summary>
	void		RemovePrims();
	/// <summary>Selects a primitive at a control point near the given view point.</summary>
	/// <param name="pView">Pointer to the view for coordinate transformations.</param>
	/// <param name="ptView">The view-space point to check for proximity to control points.</param>
	/// <param name="ptCtrl">Output parameter for the transformed control point coordinates.</param>
	/// <returns>Pointer to the selected primitive, or null if none found.</returns>
	CPrim*		SelPrimAtCtrlPt(CPegView* pView, const CPnt4& ptView, CPnt* ptCtrl) const;
	/// <summary>Selects a primitive using a point.</summary>
	/// <param name="pView">Pointer to the view.</param>
	/// <param name="pt">The point.</param>
	/// <param name="dPicApert">Aperture distance.</param>
	/// <param name="pDetPt">Detected point.</param>
	/// <returns>Pointer to the selected primitive.</returns>
	CPrim*		SelPrimUsingPoint(CPegView* pView, const CPnt4& pt, double& dPicApert, CPnt& pDetPt);
	/// <summary>Selects primitives using a rectangle.</summary>
	/// <param name="pView">Pointer to the view.</param>
	/// <param name="pt1">First corner.</param>
	/// <param name="pt2">Second corner.</param>
	/// <returns>True if any selected.</returns>
	bool		SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2) const;
	/// <summary>Sorts text primitives on Y.</summary>
	void		SortTextOnY();
	/// <summary>Squares the segment.</summary>
	void		Square();
	/// <summary>Transforms the segment.</summary>
	/// <param name="tm">Transformation matrix.</param>
	void		Transform(const CTMat& tm);
	/// <summary>Translates the segment.</summary>
	/// <param name="v">Translation vector.</param>
	void		Translate(const CVec& v) const;
	/// <summary>Writes the segment to a file.</summary>
	/// <param name="fl">The file.</param>
	void		Write(CFile& fl);
	/// <summary>Writes the segment to a file with buffer.</summary>
	/// <param name="f">The file.</param>
	/// <param name="p">Buffer.</param>
	void		Write(CFile& f, char* p);
#if ODA_FUNCTIONALITY
	/// <summary>Writes the segment for ODA functionality.</summary>
	/// <param name="hdb">Database handle.</param>
	/// <param name="entlist">Entity list.</param>
	/// <param name="henhd">Entity header.</param>
	/// <param name="hen">Entity.</param>
	void		Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen);
#endif

public:
	/// <summary>Gets the ignore primitive.</summary>
	/// <returns>Reference to the ignore primitive.</returns>
	static CPrim*& IgnorePrim() {return mS_pPrimIgnore;}

private:
	static CPrim* mS_pPrimIgnore;
};
