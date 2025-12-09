#pragma once

#include <Windows.h>  // for HWND, HTREEITEM

#include <afx.h>
#include <afxwin.h>  // for CObject (for casts), CString and CFile plus CDC, POSITION, HWND, HTREEITEM

#include <afxcoll.h>  // for CObList
#include <afxstr.h>   // for CString

#include "CharCellDef.h"  // for CCharCellDef
#include "FontDef.h"      // for CFontDef
#include "Pnt.h"          // for CPnt, CPnt4
#include "Pnt4.h"         // for CPnt4
#include "Prim.h"         // for PENSTYLE, PENCOLOR
#include "TMat.h"         // for CTMat
#include "Vec.h"          // for CVec

class CBlock;
class CPegView;

class CSeg : public CObList {
 public:
  /// @brief Default constructor.
  CSeg() {}

  /// @brief Constructor that adds a single primitive.
  /// @param primitive Pointer to the primitive to add.
  CSeg(CPrim* primitive) { AddTail(primitive); }

  /// @brief Copy constructor.
  /// @param other The segment to copy from.
  CSeg(const CSeg& other) noexcept;

  /// @brief Assignment operator.
  /// @param other The segment to assign from.
  /// @returns Reference to this segment.
  CSeg& operator=(const CSeg& other);

  /// @brief Constructor from a block.
  /// @param block The block to initialize from.
  CSeg(const CBlock& block);

  /// @brief Adds primitives to the tree view control.
  /// @param tree Handle to the tree view.
  /// @param parent Parent tree item.
  void AddPrimsToTreeViewControl(HWND tree, HTREEITEM parent) const;

  /// @brief Adds the segment to the tree view control.
  /// @param tree Handle to the tree view.
  /// @param parent Parent tree item.
  /// @returns The tree item handle.
  HTREEITEM AddToTreeViewControl(HWND tree, HTREEITEM parent) const;

  /// @brief Breaks polylines into individual lines.
  void BreakPolylines();

  /// @brief Breaks segment references.
  void BreakSegRefs();

  /// @brief Displays the segment.
  /// @param view Pointer to the view.
  /// @param context Pointer to the device context.
  void Display(CPegView* view, CDC* context);

  /// @brief Finds and removes a primitive.
  /// @param primitive Pointer to the primitive to remove.
  /// @returns The position of the removed primitive.
  POSITION FindAndRemovePrim(CPrim* primitive);

  /// @brief Gets the primitive at the specified position.
  /// @param pos The position.
  /// @returns Pointer to the primitive.
  CPrim* GetAt(POSITION pos) const { return (CPrim*)CObList::GetAt(pos); }

  /// @brief Counts how many segment reference primitives (CPrimSegRef) in this segment have the specified block name.
  /// @param block The block name to match (provided as a const reference to a CString).
  /// @return The number of block references whose name equals the provided blockName.
  INT_PTR GetBlockRefCount(const CString& name) const;

  /// @brief Gets the count of primitives in the segment.
  /// @returns The count.
  INT_PTR GetCount() const { return CObList::GetCount(); }

  /// @brief Gets the extents of the segment.
  /// @param minPoint Minimum point.
  /// @param maxPoint Maximum point.
  /// @param transformMatrix Transformation matrix.
  void GetExtents(CPnt& minPoint, CPnt& maxPoint, const CTMat& transformMatrix) const;

  /// @brief Gets the next primitive in the list.
  /// @param pos The position, updated to the next.
  /// @returns Pointer to the primitive.
  CPrim* GetNext(POSITION& pos) const { return ((CPrim*)CObList::GetNext(pos)); }

  /// @brief Gets the count of primitives with the given pen style.
  /// @param penStyle The pen style.
  /// @returns The count.
  int GetPenStyleRefCount(PENSTYLE penStyle) const;

  /// @brief Inserts a segment before the specified position.
  /// @param pos The position.
  /// @param segment Pointer to the segment to insert.
  void InsertBefore(POSITION pos, CSeg* segment);

  /// @brief Checks if the segment is in view.
  /// @param view Pointer to the view.
  /// @returns True if in view.
  bool IsInView(CPegView* view) const;

  /// @brief Modifies notes in text primitives.
  /// @param fontDefinition Font definition.
  /// @param characterCellDefinition Character cell definition.
  /// @param attribute Attribute.
  void ModifyNotes(const CFontDef& fontDefinition, const CCharCellDef& characterCellDefinition, int attribute = 0);

  /// @brief Modifies the pen color of all primitives.
  /// @param penColor The new pen color.
  void ModifyPenColor(PENCOLOR penColor);

  /// @brief Modifies the pen style of all primitives.
  /// @param penStyle The new pen style.
  void ModifyPenStyle(PENSTYLE penStyle);

  /// @brief Translates pen colors.
  /// @param numberOfColors Number of colors.
  /// @param newColors New colors array.
  /// @param oldColors Old colors array.
  void PenTranslation(WORD numberOfColors, PENCOLOR* newColors, PENCOLOR* oldColors);

  /// @brief Removes empty notes.
  /// @returns The number of removed notes.
  int RemoveEmptyNotes();
  /// @brief Removes all primitives.
  void RemovePrims();

  /// @brief Selects a primitive at a control point near the given view point.
  /// @param view Pointer to the view for coordinate transformations.
  /// @param pointInView The view-space point to check for proximity to control points.
  /// @param controlPoint Output parameter for the transformed control point coordinates.
  /// @returns Pointer to the selected primitive, or null if none found.
  CPrim* SelPrimAtCtrlPt(CPegView* view, const CPnt4& pointInView, CPnt* controlPoint) const;

  /// @brief Selects a primitive in the given view using a point specified in view coordinates.
  /// @param view Pointer to the view/context used for the selection (CPegView*).
  /// @param pointInView The point in view coordinates used to perform the hit test (const CPnt4&).
  /// @param aperture Reference to the selection tolerance (double&); used as the hit-test radius and may be updated to reflect the aperture actually used or required for the selection.
  /// @param detectedPoint Output parameter that receives the point on the selected primitive where the hit occurred (CPnt&).
  /// @return Pointer to the selected CPrim if a hit was found; otherwise nullptr.
  CPrim* SelPrimUsingPoint(CPegView* view, const CPnt4& pointInView, double& aperture, CPnt& detectedPoint);

  /// @brief Selects primitives using a rectangle.
  /// @param view Pointer to the view.
  /// @param firstCornerPoint First corner.
  /// @param secondCornerPoint Second corner.
  /// @returns True if any selected.
  bool SelUsingRect(CPegView* view, const CPnt& firstCornerPoint, const CPnt& secondCornerPoint) const;

  /// @brief Sorts text primitives on Y.
  void SortTextOnY();

  /// @brief Squares the segment.
  void Square();

  /// @brief Transforms the segment.
  /// @param transformMatrix Transformation matrix.
  void Transform(const CTMat& transformMatrix);

  /// @brief Translates the segment.
  /// @param translation Translation vector.
  void Translate(const CVec& translation) const;

  /// @brief Writes the segment to a file.
  /// @param file The file.
  void Write(CFile& file);

  /// @brief Writes the segment to a file with buffer.
  /// @param file The file.
  /// @param buffer Buffer.
  void Write(CFile& file, char* buffer);

#if ODA_FUNCTIONALITY
  /// @brief Writes the segment for ODA functionality.
  /// @param hdb Database handle.
  /// @param entlist Entity list.
  /// @param henhd Entity header.
  /// @param hen Entity.
  void Write(AD_DB_HANDLE hdb, AD_VMADDR entlist, PAD_ENT_HDR henhd, PAD_ENT hen);
#endif

 public:
  /// @brief Gets the ignore primitive.
  /// @returns Reference to the ignore primitive.
  static CPrim*& IgnorePrim() { return mS_pPrimIgnore; }

 private:
  static CPrim* mS_pPrimIgnore;
};
