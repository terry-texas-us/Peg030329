#pragma once

#include "Pnt.h"
#include "Prim.h"
#include "Segs.h"
#include "TMat.h"
#include "Vec.h"

class CPegView;
class CPrim;
class CPrimText;
class CSegsDet;

class CSegsTrap : public CSegs {
 private:
  bool m_bIdentify;  // trap is displayed using special pen color and style
  PENCOLOR m_nPenColor;
  PENSTYLE m_nPenStyle;
  CPnt m_ptPvt;

 public:
  CSegsTrap();

  CSegsTrap(const CSegsTrap&) = delete;
  CSegsTrap& operator=(const CSegsTrap&) = delete;

  ~CSegsTrap() {}

  void AddSegsAtPt(CPegView* pView, CSegsDet*, CPnt);
  void Compress();
  void Copy(const CVec& vTrns);
  void DeleteSegs();
  void Expand();
  bool Identify() const { return m_bIdentify; }
  PENCOLOR PenColor() const { return m_nPenColor; }
  PENSTYLE PenStyle() const { return m_nPenStyle; }
  void RemoveSegsAtPt(CPegView* pView, CPnt pt);
  void SetIdentify(bool b) { m_bIdentify = b; }
  void Square();
  void TransformSegs(const CTMat& tm);
  CPnt& PvtPt() { return m_ptPvt; }
};

extern CSegsTrap trapsegs;
