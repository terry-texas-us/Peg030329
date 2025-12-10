#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>    // for CString
#include <afxtempl.h>  // for CTypedPtrArray, CTypedPtrMap, CMapStringToOb, CObArray
#include <afxwin.h>    // for CDocument

#include <climits>

#include "PegAEsysView.h"  // for CPegView

#include "Block.h"  // for CBlocks
#include "Layer.h"  // for CLayer, CLayers
#include "PenStyle.h"
#include "Pnt.h"   // for CPnt
#include "Prim.h"  // for PENSTYLE, PENCOLOR, CPenStyles
#include "Segs.h"  // for CSegs
#include "TMat.h"  // For CTMat

class CSeg;

const PENSTYLE PENSTYLE_LookupErr = SHRT_MAX;

class CPegDoc : public CDocument {
 protected:  // create from serialization only
  CPegDoc();
  DECLARE_DYNCREATE(CPegDoc)

  // Explicitly delete copy constructor and assignment operator to fix C4625 and C4626 warnings
  CPegDoc(const CPegDoc&) = delete;
  CPegDoc& operator=(const CPegDoc&) = delete;

 public:
  static const WORD FILE_TYPE_NONE = 0;
  static const WORD FILE_TYPE_PEG = 1;
  static const WORD FILE_TYPE_TRACING = 2;
  static const WORD FILE_TYPE_JOB = 3;
  static const WORD FILE_TYPE_DXF = 4;
  static const WORD FILE_TYPE_DWG = 5;
  static const WORD FILE_TYPE_BLOCK = 6;
  static const WORD FILE_TYPE_FFA = 7;
  static const WORD FILE_TYPE_BMP = 8;

  static const LPARAM HINT_NONE = 0;

  static const LPARAM HINT_PRIM = 0x0001;
  static const LPARAM HINT_SEG = 0x0002;
  static const LPARAM HINT_SEGS = 0x0004;
  static const LPARAM HINT_LAYER = 0x0008;

  static const LPARAM HINT_SAFE = 0x0100;
  static const LPARAM HINT_ERASE = 0x0200;
  static const LPARAM HINT_TRAP = 0x0400;

  static const LPARAM HINT_PRIM_SAFE = HINT_PRIM | HINT_SAFE;
  static const LPARAM HINT_PRIM_ERASE_SAFE = HINT_PRIM | HINT_ERASE | HINT_SAFE;
  static const LPARAM HINT_SEG_SAFE = HINT_SEG | HINT_SAFE;
  static const LPARAM HINT_SEG_ERASE_SAFE = HINT_SEG | HINT_ERASE | HINT_SAFE;
  static const LPARAM HINT_SEG_SAFE_TRAP = HINT_SEG | HINT_SAFE | HINT_TRAP;
  static const LPARAM HINT_SEG_ERASE_SAFE_TRAP = HINT_SEG | HINT_ERASE | HINT_SAFE | HINT_TRAP;

  static const LPARAM HINT_SEGS_SAFE = HINT_SEGS | HINT_SAFE;
  static const LPARAM HINT_SEGS_TRAP = HINT_SEGS | HINT_TRAP;
  static const LPARAM HINT_SEGS_SAFE_TRAP = HINT_SEGS | HINT_SAFE | HINT_TRAP;
  static const LPARAM HINT_SEGS_ERASE_SAFE_TRAP = HINT_SEGS | HINT_ERASE | HINT_SAFE | HINT_TRAP;

  static const LPARAM HINT_LAYER_SAFE = HINT_LAYER | HINT_SAFE;
  static const LPARAM HINT_LAYER_ERASE = HINT_LAYER | HINT_ERASE;

  // Attributes
 public:
  CString m_strIdentifiedLayerName;
  WORD m_wOpenFileType = FILE_TYPE_NONE;

 private:
  CPenStyles m_PenStyles;
  CBlocks m_blks;
  CLayers m_layers;
  CLayer* m_pLayerWork = nullptr;
  CSegs m_segsDeleted;
  bool m_bTrapHighlight = true;
  double m_dMarkSize =
      0.0;  // in drawing units when greater than zero; in pixels when less than zero; default otherwise

  // Overrides
 public:
  BOOL OnNewDocument() override;
  BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
  void DeleteContents() override;
  void Serialize(CArchive& ar) override;

  // Implementation
 public:
  ~CPegDoc() override;
#ifdef _DEBUG
  void AssertValid() const override;
  void Dump(CDumpContext& dc) const override;
#endif

  // Operations
 public:
  void AddTextBlock(TCHAR* pszText);
  INT_PTR BlockGetRefCount(const CString& blockName) const;

  void BlksGetNextAssoc(POSITION& pos, CString& strKey, CBlock*& pBlock) { m_blks.GetNextAssoc(pos, strKey, pBlock); }
  POSITION BlksGetStartPosition() { return m_blks.GetStartPosition(); }
  bool BlksIsEmpty() const { return m_blks.IsEmpty() == TRUE; }
  bool BlksLookup(CString strKey, CBlock*& pBlock) const;
  void BlksRemoveAll();
  void BlksRemoveUnused();
  void BlksSetAt(CString strKey, CBlock* pBlock) { m_blks.SetAt(strKey, pBlock); }
  WORD BlksSize() const { return (WORD(m_blks.GetSize())); }

  POSITION DeletedSegsAddHead(CSeg* pSeg) { return (m_segsDeleted.AddHead(pSeg)); }
  POSITION DeletedSegsAddTail(CSeg* pSeg) { return (m_segsDeleted.AddTail(pSeg)); }
  bool DeletedSegsIsEmpty() { return (m_segsDeleted.IsEmpty() == TRUE); }
  //#pragma tasMSG(CobList asserts on calls to RemoveHead & RemoveTail if list is empty!)
  CSeg* DeletedSegsRemoveHead() { return (m_segsDeleted.RemoveHead()); }
  void DeletedSegsRemoveSegs() { m_segsDeleted.RemoveSegs(); }
  CSeg* DeletedSegsRemoveTail() { return (m_segsDeleted.RemoveTail()); }
  void DeletedSegsRestore();

  void DoPrimDelete(const CPnt& pt);
  void GetExtents(CPnt&, CPnt&, const CTMat&) const;
  INT_PTR GetHotCount() const;
  INT_PTR GetWarmCount() const;
  /// Get the stored mark size. Positive => drawing units; negative => pixels.
  double GetMarkSize() const noexcept { return m_dMarkSize; }
  void InitAll();
  bool FileTypeIsTracing(WORD wType) const;

  void LayerBlank(const CString& strName);
  bool LayerMelt(CString& strName) const;

  void LayersAdd(CLayer* pLayer) { m_layers.Add(pLayer); }
  void LayersDisplayAll(CPegView* pView, CDC* pDC);
  CLayer* LayersGet(const CString& strName) const;
  CLayer* LayersGetAt(int) const;
  int LayersGetSize() const { return static_cast<int>(m_layers.GetSize()); }
  int LayersLookup(const CString& strName) const;
  void LayersRemove(const CString& strName);
  void LayersRemoveAll();
  void LayersRemoveAt(int);
  void LayersRemoveEmpty();
  CLayer* LayersSelUsingPoint(const CPnt&) const;

  int PenStyleGetRefCount(PENSTYLE nPenStyle) const;
  void PenStylesAdd(CPenStyle* pPenStyle) { m_PenStyles.Add(pPenStyle); }
  int PenStylesFillCB(HWND hwnd) const;
  CPenStyle* PenStylesGetAt(int i) const { return m_PenStyles.GetAt(i); }
  CPenStyle* PenStylesGetAtSafe(int);
  int PenStylesGetSize() const { return (int)m_PenStyles.GetSize(); }
  PENSTYLE PenStylesLookup(const CString& strName) const;
  void PenStylesRemoveUnused();
  void PenStylesInsert(int i, CPenStyle* pPenStyle) { m_PenStyles.InsertAt(i, pPenStyle); }

  void PenTranslation(WORD, PENCOLOR*, PENCOLOR*);

  int RemoveEmptyNotes();
  int RemoveEmptySegs();

  CLayer* AnyLayerRemove(CSeg* pSeg);

  /// Set the stored mark size. Positive => drawing units; negative => pixels.
  void SetMarkSize(double size) noexcept { m_dMarkSize = size; }
  void SetOpenFile(WORD wFileType, const CString& strFileName);

  /// @brief Tracing is converted to a static layer.
  /// @param layerName The name of the layer to be converted. The name is modified to remove any file extension.
  /// @remarks This is a cold state meaning the layer is displayed using warm color set, is not detectable, and may not have its segments modified or deleted. No new segments may be added to the layer.
  void TracingFuse(CString& layerName) const;
  bool TracingLoadLayer(const CString& strPathName, CLayer* pLayer) const;
  bool TracingMap(const CString& strPathName);
  bool TracingOpen(const CString& strPathName);
  bool TracingView(const CString& strPathName);

  void WorkLayerAddTail(CSeg* pSeg);
  void WorkLayerAddTail(CSegs* pSegs);
  void WorkLayerClear();
  POSITION WorkLayerFind(CSeg* pSeg) const { return (m_pLayerWork->Find(pSeg)); }
  CLayer* WorkLayerGet() const { return m_pLayerWork; }
  POSITION WorkLayerGetHeadPosition() const { return m_pLayerWork->GetHeadPosition(); }
  CSeg* WorkLayerGetNext(POSITION& pos) const { return m_pLayerWork->GetNext(pos); }
  CSeg* WorkLayerGetPrev(POSITION& pos) const { return m_pLayerWork->GetPrev(pos); }
  CSeg* WorkLayerGetTail() const;
  POSITION WorkLayerGetTailPosition() const { return m_pLayerWork->GetTailPosition(); }
  void WorkLayerInit();
  void WorkLayerSet(CLayer* pLayer);
  void WriteShadowFile();

  // Generated message map functions
 public:
  afx_msg void OnBlocksLoad();
  afx_msg void OnBlocksRemoveUnused();
  afx_msg void OnBlocksUnload();
  afx_msg void OnClearActiveLayers();
  afx_msg void OnClearAllLayers();
  afx_msg void OnClearAllTracings();
  afx_msg void OnClearMappedTracings();
  afx_msg void OnClearViewedTracings();
  afx_msg void OnClearWorkingLayer();
  afx_msg void OnEditImageToClipboard();
  afx_msg void OnEditSegToWork();

  /// @brief Pastes clipboard to drawing. Only `PegSegs` format handled and no translation is performed.
  /// @note This function does not modify the clipboard contents.
  afx_msg void OnEditTrace();
  afx_msg void OnEditTrapCopy();
  afx_msg void OnEditTrapCut();
  afx_msg void OnEditTrapDelete();
  afx_msg void OnEditTrapQuit();
  afx_msg void OnEditTrapPaste();
  afx_msg void OnEditTrapWork();
  afx_msg void OnEditTrapWorkAndActive();
  afx_msg void OnFile();
  afx_msg void OnFileManage();
  afx_msg void OnFileQuery();
  afx_msg void OnFileSave();
  afx_msg void OnFileTracing();
  afx_msg void OnLayerActive();
  afx_msg void OnLayerHidden();
  afx_msg void OnLayerMelt();
  afx_msg void OnLayerStatic();
  afx_msg void OnLayerWork();
  afx_msg void OnLayersActiveAll();
  afx_msg void OnLayersStaticAll();
  afx_msg void OnLayersRemoveEmpty();
  afx_msg void OnMaintenanceRemoveEmptyNotes();
  afx_msg void OnMaintenanceRemoveEmptySegments();
  afx_msg void OnPensEditColors();
  afx_msg void OnPensLoadColors();
  afx_msg void OnPensRemoveUnusedStyles();
  afx_msg void OnPensTranslate();
  afx_msg void OnPrimBreak();
  afx_msg void OnPrimDelete();
  afx_msg void OnPrimExtractNum();
  afx_msg void OnPrimExtractStr();
  afx_msg void OnPrimGotoCenterPoint();
  afx_msg void OnPrimModifyAttributes();
  afx_msg void OnPrimSnapToEndPoint();
  afx_msg void OnSegBreak();
  afx_msg void OnSegDelete();
  afx_msg void OnSegDeleteLast();
  afx_msg void OnSegExchange();
  afx_msg void OnSegInsert();
  afx_msg void OnSetupFillHatch();
  afx_msg void OnSetupFillHollow();
  afx_msg void OnSetupFillPattern();
  afx_msg void OnSetupFillSolid();
  afx_msg void OnSetupGotoPoint();
  afx_msg void OnSetupMarkStyle();
  afx_msg void OnSetupNote();
  afx_msg void OnSetupOptionsDraw();
  afx_msg void OnSetupPenColor();
  afx_msg void OnSetupPenStyle();
  afx_msg void OnSetupSavePoint();
  afx_msg void OnTracingCloak();
  afx_msg void OnTracingFuse();
  afx_msg void OnTracingMap();
  afx_msg void OnTracingOpen();
  afx_msg void OnTracingView();

  /// @brief This function aims to encapsulate the segments in the trap into a new, named block for reuse.
  /// It generates a unique block name, creates the block, populates it with copies of the trap's segments,
  /// sets a base point, and stores the block in a global collection (m_blks, a map of blocks keyed by name).
  afx_msg void OnTrapCommandsBlock();
  afx_msg void OnTrapCommandsCompress();
  afx_msg void OnTrapCommandsExpand();
  afx_msg void OnTrapCommandsFilter();
  afx_msg void OnTrapCommandsHighlight();
  afx_msg void OnTrapCommandsInvert();
  afx_msg void OnTrapCommandsQuery();
  afx_msg void OnTrapCommandsSquare();
  afx_msg void OnTrapCommandsUnblock();
  // Returns a pointer to the currently active document.
  static CPegDoc* GetDoc();

 protected:
  DECLARE_MESSAGE_MAP()
 public:
};
