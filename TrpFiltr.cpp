#include "stdafx.h"

#include <afxstr.h>

#include <climits>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Prim.h"
#include "PrimArc.h"
#include "PrimLine.h"
#include "PrimMark.h"
#include "PrimPolygon.h"
#include "PrimPolyline.h"
#include "PrimSegRef.h"
#include "PrimText.h"

#include "SegsTrap.h"

void trapFilterByPenColor(PENCOLOR);
void trapFilterByLineStyle(int);
void trapFilterByPrimType(CPrim::Type);

INT_PTR CALLBACK DlgProcTrapFilter(HWND ahDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept {
  CPegDoc* pDoc{CPegDoc::GetDoc()};

  if (anMsg == WM_INITDIALOG) {
    CString strElementName[] = {_T("Arc"),  _T("Insert"), _T("Line"),    _T("Marker"),
                                _T("Text"), _T("Panel"),  _T("Polyline")};

    SetDlgItemInt(ahDlg, IDC_TRAP_FILTER_PEN_ID, 1, FALSE);

    int iTblSize = pDoc->PenStylesGetSize();

    for (int i = 0; i < iTblSize; i++) {
      CString strName = pDoc->PenStylesGetAt(i)->GetName();
      ::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_LINE_LIST, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)strName);
    }
    ::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_LINE_LIST, CB_SETCURSEL, 0, 0);

    for (WORD w = 0; w < sizeof(strElementName) / sizeof(strElementName[0]); w++)
      ::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_ELEMENT_LIST, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)strElementName[w]);

    ::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_ELEMENT_LIST, LB_SETCURSEL, 0, 0L);
    return (TRUE);
  } else if (anMsg == WM_COMMAND) {
    PENCOLOR nPenColor = SHRT_MAX;
    int iElementId = -1;

    switch (LOWORD(wParam)) {
      case IDOK:
        if (IsDlgButtonChecked(ahDlg, IDC_TRAP_FILTER_PEN)) {
          nPenColor = PENCOLOR(GetDlgItemInt(ahDlg, IDC_TRAP_FILTER_PEN_ID, nullptr, FALSE));
          trapFilterByPenColor(nPenColor);
        }
        if (IsDlgButtonChecked(ahDlg, IDC_TRAP_FILTER_LINE)) {
          TCHAR szBuf[32]{};

          BOOL bTranslated = TRUE;

          int iLineId = (WORD)GetDlgItemInt(ahDlg, IDC_TRAP_FILTER_LINE_LIST, &bTranslated, TRUE);

          if (bTranslated == 0) {
            bTranslated =
                (::GetDlgItemText(ahDlg, IDC_TRAP_FILTER_LINE_LIST, static_cast<LPSTR>(szBuf), sizeof(szBuf)) != 0);
            if (bTranslated != 0) iLineId = pDoc->PenStylesLookup(szBuf);
            bTranslated = iLineId != SHRT_MAX;
          }
          if (bTranslated) trapFilterByLineStyle(iLineId);
        }
        if (IsDlgButtonChecked(ahDlg, IDC_TRAP_FILTER_ELEMENT)) {
          iElementId = int(::SendDlgItemMessage(ahDlg, IDC_TRAP_FILTER_ELEMENT_LIST, LB_GETCURSEL, 0, 0));

          switch (iElementId)  // **PrimitiveTypes**
          {
            case 0:
              trapFilterByPrimType(CPrim::Type::Arc);
              break;
            case 1:
              trapFilterByPrimType(CPrim::Type::SegRef);
              break;
            case 2:
              trapFilterByPrimType(CPrim::Type::Line);
              break;
            case 3:
              trapFilterByPrimType(CPrim::Type::Mark);
              break;
            case 4:
              trapFilterByPrimType(CPrim::Type::Text);
              break;
            case 5:
              trapFilterByPrimType(CPrim::Type::Polygon);
              break;
            case 6:
              trapFilterByPrimType(CPrim::Type::Polyline);
          }
        }
        [[fallthrough]];  // Intentional fallthrough

      case IDCANCEL:
        ::EndDialog(ahDlg, TRUE);
        return (TRUE);
    }
  }
  return (FALSE);
}

void trapFilterByLineStyle(int iLnId) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  POSITION pos = trapsegs.GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = trapsegs.GetNext(pos);

    POSITION posPrim = pSeg->GetHeadPosition();
    while (posPrim != nullptr) {
      CPrim* pPrim = pSeg->GetNext(posPrim);
      if (pPrim->PenStyle() == iLnId) {  // Line match remove it
        trapsegs.Remove(pSeg);
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSeg);
        break;
      }
    }
  }
  app.StatusLineDisplay(TrapCnt);
}
void trapFilterByPenColor(PENCOLOR nPenColor) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  POSITION pos = trapsegs.GetHeadPosition();
  while (pos != nullptr) {
    CSeg* pSeg = trapsegs.GetNext(pos);

    POSITION posPrim = pSeg->GetHeadPosition();
    while (posPrim != nullptr) {
      CPrim* pPrim = pSeg->GetNext(posPrim);
      if (pPrim->PenColor() == nPenColor) {  // Color match remove it
        trapsegs.Remove(pSeg);
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSeg);
        break;
      }
    }
  }
  app.StatusLineDisplay(TrapCnt);
}
void trapFilterByPrimType(CPrim::Type type) {
  CPegDoc* document{CPegDoc::GetDoc()};

  POSITION pos{trapsegs.GetHeadPosition()};
  while (pos != nullptr) {
    bool filter{false};

    CSeg* segment = trapsegs.GetNext(pos);

    POSITION posPrimitive = segment->GetHeadPosition();
    while (posPrimitive != nullptr) {
      CPrim* primitive{segment->GetNext(posPrimitive)};

      switch (type) {
        case CPrim::Type::Line:
          filter = (dynamic_cast<CPrimLine*>(primitive) != nullptr);
          break;
        case CPrim::Type::Arc:
          filter = (dynamic_cast<CPrimArc*>(primitive) != nullptr);
          break;
        case CPrim::Type::SegRef:
          filter = (dynamic_cast<CPrimSegRef*>(primitive) != nullptr);
          break;
        case CPrim::Type::Text:
          filter = (dynamic_cast<CPrimText*>(primitive) != nullptr);
          break;
        case CPrim::Type::Polygon:
          filter = (dynamic_cast<CPrimPolygon*>(primitive) != nullptr);
          break;
        case CPrim::Type::Polyline:
          filter = (dynamic_cast<CPrimPolyline*>(primitive) != nullptr);
          break;
        case CPrim::Type::Mark:
          filter = (dynamic_cast<CPrimMark*>(primitive) != nullptr);
          break;
        case CPrim::Type::Insert:
          break;
        case CPrim::Type::Tag:
          break;
        case CPrim::Type::CSpline:
          break;
        case CPrim::Type::Dim:
          break;
        case CPrim::Type::BSpline:
          break;
      }
      if (filter) {
        trapsegs.Remove(segment);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
        break;
      }
    }
  }
  app.StatusLineDisplay(TrapCnt);
}
