#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "FileJob.h"
#include "Prim.h"
#include "Seg.h"
#include "SegsTrap.h"
#include "Vec.h"

///<summary>Get the drawing using a file opened somewhere else.</summary>
void mfGetAll(CFile& f, const CVec& vTrns) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  if (!trapsegs.IsEmpty()) {
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE, &trapsegs);
    trapsegs.RemoveAll();
  }
  TCHAR* pBuf = new TCHAR[CPrim::BUFFER_SIZE];

  CSeg* pSeg;
  while (filejob_GetNextSeg(f, 3, pBuf, pSeg)) {
    pDoc->WorkLayerAddTail(pSeg);
    trapsegs.AddTail(pSeg);
  }
  delete[] pBuf;

  trapsegs.Translate(vTrns);
  pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE_TRAP, &trapsegs);
}
