#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "FileJob.h"
#include "Prim.h"
#include "Seg.h"
#include "SegsTrap.h"
#include "Vec.h"

//<summary>Get the drawing using a file opened somewhere else.</summary>
void mfGetAll(CFile& file, const CVec& translation) {
  auto* document = CPegDoc::GetDoc();

  if (!trapsegs.IsEmpty()) {
    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE, &trapsegs);
    trapsegs.RemoveAll();
  }
  char* buffer = new char[CPrim::BUFFER_SIZE];

  CSeg* segment;
  while (filejob_GetNextSeg(file, 3, buffer, segment)) {
    document->WorkLayerAddTail(segment);
    trapsegs.AddTail(segment);
  }
  delete[] buffer;

  trapsegs.Translate(translation);
  document->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE_TRAP, &trapsegs);
}
