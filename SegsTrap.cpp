#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "PrimPolygon.h"
#include "SegsDet.h"
#include "SegsTrap.h"

CSegsTrap::CSegsTrap() {
  m_bIdentify = true;
  m_nPenColor = 15;
}
void CSegsTrap::AddSegsAtPt(CPegView* pView, CSegsDet* pSegs, CPnt pt) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CPnt rPt;
  double dPicApertSiz = detsegs.PicApertSiz();

  // Convert pick point to view coordinates
  CPnt4 ptView(pt, 1.);
  pView->ModelViewTransform(ptView);

  CPrimPolygon::EdgeToEvaluate() = 0;

  POSITION pos = pSegs->GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = pSegs->GetNext(pos);

    if (Find(pSeg) != nullptr) {  // already in trap
      continue;
}

    POSITION posPrim = pSeg->GetHeadPosition();
    while (posPrim != nullptr) {
      CPrim* pPrim = pSeg->GetNext(posPrim);
      if (pPrim->SelUsingPoint(pView, ptView, dPicApertSiz, rPt))  // Segment within tolerance
      {
        AddTail(pSeg);
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE_TRAP, pSeg);
        break;
      }
    }
  }
  app.StatusLineDisplay(TrapCnt);
}
void CSegsTrap::Compress() {
  // Builds a single segment from two or more segments in trap.
  // The new segment is added to the hot layer even if the trap contained
  // segments from one or more warm layers.

  if (GetCount() <= 1) return;

  CSeg* pSegNew = new CSeg;

  POSITION pos = GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = GetNext(pos);

    CPegDoc::GetDoc()->AnyLayerRemove(pSeg);
    detsegs.Remove(pSeg);
    pSegNew->AddTail(pSeg);
    // delete the original segment but not its primitives
    delete pSeg;
  }
  // emtpy trap segment list
  RemoveAll();
  CPegDoc::GetDoc()->WorkLayerAddTail(pSegNew);
  AddTail(pSegNew);

  pSegNew->SortTextOnY();
}
void CSegsTrap::Copy(const CVec& vTrns) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  POSITION pos = GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = GetNext(pos);
    CSeg* pNewSeg = new CSeg(*pSeg);

    pDoc->WorkLayerAddTail(pNewSeg);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG, pSeg);
    pSeg->Translate(vTrns);

    LPARAM lHint = (m_bIdentify) ? CPegDoc::HINT_SEG_SAFE_TRAP : CPegDoc::HINT_SEG_SAFE;
    pDoc->UpdateAllViews(nullptr, lHint, pSeg);
  }
}
void CSegsTrap::DeleteSegs() {
  POSITION pos = GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = GetNext(pos);
    CPegDoc::GetDoc()->AnyLayerRemove(pSeg);
    detsegs.Remove(pSeg);
    pSeg->RemovePrims();
    delete pSeg;
  }
  RemoveAll();
}
///<summary>Expands compressed segments.</summary>
// The new segments are added to the hot layer even if the trap contained
// segments from one or more warm layers.
void CSegsTrap::Expand() {
  if (IsEmpty()) return;

  CSeg* pSeg;
  CSeg* pNewSeg;
  CPrim* pPrim;

  auto* pSegs = new CSegs;
  pSegs->AddTail(this);
  RemoveAll();

  POSITION pos = pSegs->GetHeadPosition();
  while (pos != nullptr) {
    pSeg = pSegs->GetNext(pos);

    POSITION posPrim = pSeg->GetHeadPosition();
    while (posPrim != nullptr) {
      pPrim = pSeg->GetNext(posPrim);
      pNewSeg = new CSeg(pPrim);
      CPegDoc::GetDoc()->WorkLayerAddTail(pNewSeg);
      AddTail(pNewSeg);
    }
    CPegDoc::GetDoc()->AnyLayerRemove(pSeg);
    detsegs.RemoveAt(detsegs.Find(pSeg));
    delete pSeg;
  }
  delete pSegs;
}
void CSegsTrap::RemoveSegsAtPt(CPegView* pView, CPnt pt) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CPnt rPt;
  double dPicApertSiz = detsegs.PicApertSiz();

  int iNmbFnd = 0;

  // Convert pick point to view coordinates
  CPnt4 ptView(pt, 1.);
  pView->ModelViewTransform(ptView);

  CPrimPolygon::EdgeToEvaluate() = 0;

  POSITION pos = GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = GetNext(pos);

    POSITION posPrim = pSeg->GetHeadPosition();
    while (posPrim != nullptr) {
      CPrim* pPrim = pSeg->GetNext(posPrim);
      if (pPrim->SelUsingPoint(pView, ptView, dPicApertSiz, rPt))  // Segment within tolerance
      {
        RemoveAt(Find(pSeg));
        iNmbFnd++;
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSeg);
        break;
      }
    }
  }
}
void CSegsTrap::Square() {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_ERASE_SAFE_TRAP, this);

  POSITION pos = GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = GetNext(pos);
    pSeg->Square();
  }
  pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE_TRAP, this);
}
void CSegsTrap::TransformSegs(const CTMat& tm) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_ERASE_SAFE_TRAP, this);
  Transform(tm);
  pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE_TRAP, this);
}
