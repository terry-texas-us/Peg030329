#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "FileJob.h"
#include "Layer.h"
#include "SegsDet.h"
#include "SegsTrap.h"

CLayer::CLayer(const CString& strName, WORD wStateFlgs) {
  m_strName = strName;
  m_wTracingFlgs = 0;
  m_wStateFlgs = wStateFlgs;
  m_nPenColor = 1;
  m_strPenStyleName = _T("Continuous");
}
void CLayer::Display(CPegView* pView, CDC* pDC) {
  CPrim::LayerPenColor() = PenColor();
  CPrim::LayerPenStyle() = PenStyle();

  COLORREF* pCurColTbl = pColTbl;

  pColTbl = (IsOpened() || IsHot() || IsWarm()) ? crHotCols : crWarmCols;

  CSegs::Display(pView, pDC);
  pColTbl = pCurColTbl;
}
void CLayer::Display(CPegView* pView, CDC* pDC, bool bIdentifyTrap) {
  if (IsOn()) {
    CPrim::LayerPenColor() = PenColor();
    CPrim::LayerPenStyle() = PenStyle();

    COLORREF* pCurColTbl = pColTbl;

    bool bDetSegs = IsOpened() || IsHot() || IsWarm();

    pColTbl = bDetSegs ? crHotCols : crWarmCols;

    POSITION pos = GetHeadPosition();
    while (pos != nullptr) {
      CSeg* pSeg = GetNext(pos);

      if (pSeg->IsInView(pView)) {
        if (bDetSegs) detsegs.AddTail(pSeg);

        if (bIdentifyTrap && trapsegs.Find(pSeg) != nullptr) {
          CPrim::SpecPenColor() = trapsegs.PenColor();
          pSeg->Display(pView, pDC);
          CPrim::SpecPenColor() = 0;
        } else {
          pSeg->Display(pView, pDC);
        }
      }
    }
    pColTbl = pCurColTbl;
  }
}
PENSTYLE CLayer::PenStyle() const { return CPegDoc::GetDoc()->PenStylesLookup(m_strPenStyleName); }
void CLayer::PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol) {
  for (int i = 0; i < wCols; i++) {
    if (m_nPenColor == pCol[i]) {
      m_nPenColor = pColNew[i];
      break;
    }
  }
  CSegs::PenTranslation(wCols, pColNew, pCol);
}
void CLayer::SetStateCold() {
  ClrStateFlg(STATE_FLG_HOT | STATE_FLG_WARM | STATE_FLG_OFF);
  SetStateFlg(STATE_FLG_COLD);
}
void CLayer::SetStateHot() {
  ClrStateFlg(STATE_FLG_WARM | STATE_FLG_COLD | STATE_FLG_OFF);
  SetStateFlg(STATE_FLG_HOT);
}
void CLayer::SetStateOff() {
  ClrStateFlg(STATE_FLG_HOT | STATE_FLG_WARM | STATE_FLG_COLD);
  SetStateFlg(STATE_FLG_OFF);
}
void CLayer::SetStateWarm() {
  ClrStateFlg(STATE_FLG_HOT | STATE_FLG_COLD | STATE_FLG_OFF);
  SetStateFlg(STATE_FLG_WARM);
}
