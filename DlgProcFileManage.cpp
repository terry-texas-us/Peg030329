#include "stdafx.h"

#include <afxstr.h>

#include <cstdlib>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Block.h"
#include "DlgProcFileManage.h"
#include "Layer.h"
#include "Messages.h"
#include "Preview.h"
#include "Prim.h"

void DlgProcFileManageDoLayerColor(HWND hDlg);

TCHAR szLayerName[64]{};

INT_PTR CALLBACK DlgProcFileManage(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept {
  WORD wNotifyCode{HIWORD(wParam)};

  if (nMsg == WM_INITDIALOG) {
    DlgProcFileManageInit(hDlg);
    return (TRUE);
  } else if (nMsg == WM_COMMAND) {
    CPegDoc* pDoc = CPegDoc::GetDoc();

    switch (LOWORD(wParam)) {
      case IDC_BLOCKS_LIST:
        if (wNotifyCode == LBN_SELCHANGE) {
          CString blockName{};
          GetCurSel(hDlg, IDC_BLOCKS_LIST, blockName);
          CBlock* block{nullptr};
          pDoc->BlksLookup(blockName, block);
          if (block == nullptr) { break; }
          SetDlgItemInt(hDlg, IDC_SEGS, static_cast<UINT>(block->GetCount()), FALSE);
          SetDlgItemInt(hDlg, IDC_REFERENCES, static_cast<UINT>(pDoc->BlockGetRefCount(blockName)), FALSE);
          WndProcPreviewUpdate(hDlg, block);
        }
        break;

      case IDC_COLOR_ID:
        if (wNotifyCode == CBN_EDITCHANGE || wNotifyCode == CBN_SELENDOK) DlgProcFileManageDoLayerColor(hDlg);
        break;

      case IDC_LAY:
        if (wNotifyCode == LBN_SELCHANGE) {
          CString strName;
          GetCurSel(hDlg, IDC_LAY, strName);

          CLayer* pLayer = pDoc->LayersGet(strName);

          ::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, pLayer->IsHot(), 0L);
          ::SendDlgItemMessage(hDlg, IDC_LAYACTIVE, BM_SETCHECK, pLayer->IsWarm(), 0L);
          ::SendDlgItemMessage(hDlg, IDC_LAYSTATIC, BM_SETCHECK, pLayer->IsCold(), 0L);
          ::SendDlgItemMessage(hDlg, IDC_LAYHIDDEN, BM_SETCHECK, pLayer->IsOff(), 0L);

          SetDlgItemInt(hDlg, IDC_SEGS, static_cast<UINT>(pLayer->GetCount()), FALSE);
          SetDlgItemInt(hDlg, IDC_COLOR_ID, pLayer->PenColor(), TRUE);
          ::SendMessage(::GetDlgItem(hDlg, IDC_PENSTYLE), CB_SETCURSEL, static_cast<WPARAM>(pLayer->PenStyle()), TRUE);

          CPrim::LayerPenColor() = pLayer->PenColor();
          CPrim::LayerPenStyle() = pLayer->PenStyle();

          WndProcPreviewUpdate(hDlg, pLayer);
        }
        break;

      case IDC_LAYACTIVE:
        DlgProcFileManageDoLayerActive(hDlg);
        break;

      case IDC_LAYDELETE:
        DlgProcFileManageDoLayerDelete(hDlg);
        break;

      case IDC_LAYHIDDEN:
        DlgProcFileManageDoLayerHidden(hDlg);
        break;

      case IDC_LAYMELT:
        DlgProcFileManageDoLayerMelt(hDlg);
        break;

      case IDC_LAYNEW:
        szLayerName[0] = 0;
        if (pGetLayerName()) {
          if (_tcslen(szLayerName) > 0) {
            auto* pLayer = new CLayer(szLayerName);
            pDoc->LayersAdd(pLayer);
            ::SendDlgItemMessage(hDlg, IDC_LAY, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szLayerName);
          }
        }
        break;

      case IDC_LAYRENAME:
        DlgProcFileManageDoLayerRename(hDlg);
        break;

      case IDC_LAYSTATIC:
        DlgProcFileManageDoLayerStatic(hDlg);
        break;

      case IDC_LAYWORK:
        DlgProcFileManageDoLayerWork(hDlg);
        break;

      case IDC_PENSTYLE:
        if (wNotifyCode == CBN_SELCHANGE) {
          CString strName;
          if (GetCurSel(hDlg, IDC_LAY, strName) != LB_ERR) {
            CLayer* pLayer = pDoc->LayersGet(strName);

            HWND hWndComboBox = HWND(lParam);
            LRESULT lrCurSel = ::SendMessage(hWndComboBox, CB_GETCURSEL, 0, 0L);
            if (lrCurSel != CB_ERR) {
              ::SendMessage(hWndComboBox, CB_GETLBTEXT, (WPARAM)lrCurSel, (LPARAM)(LPCSTR)strName);
              pLayer->SetPenStyleName(strName);
            }
          }
        }
        break;

      case IDC_SOLO:
        break;

      case IDC_TRA:
        if (wNotifyCode == LBN_SELCHANGE) {
          CString strName;
          GetCurSel(hDlg, IDC_TRA, strName);
          CLayer* pLayer = pDoc->LayersGet(strName);

          ::SendDlgItemMessage(hDlg, IDC_TRAOPEN, BM_SETCHECK, pLayer->IsOpened(), 0L);
          ::SendDlgItemMessage(hDlg, IDC_TRAMAP, BM_SETCHECK, pLayer->IsMapped(), 0L);
          ::SendDlgItemMessage(hDlg, IDC_TRAVIEW, BM_SETCHECK, pLayer->IsViewed(), 0L);
          ::SendDlgItemMessage(hDlg, IDC_TRACLOAK, BM_SETCHECK, pLayer->IsOff(), 0L);

          SetDlgItemInt(hDlg, IDC_SEGS, static_cast<UINT>(pLayer->GetCount()), FALSE);
          WndProcPreviewUpdate(hDlg, pLayer);
        }
        break;

      case IDC_TRACLOAK:
        DlgProcFileManageDoTracingCloak(hDlg);
        break;

      case IDC_TRAEXCLUDE:
        DlgProcFileManageDoTracingExclude(hDlg);
        break;

      case IDC_TRAFUSE:
        DlgProcFileManageDoTracingFuse(hDlg);
        break;

      case IDC_TRAINCLUDE:
        DlgProcFileManageDoTracingInclude(hDlg);
        return (TRUE);

      case IDC_TRAMAP:
        DlgProcFileManageDoTracingMap(hDlg);
        break;

      case IDC_TRAOPEN:
        DlgProcFileManageDoTracingOpen(hDlg);
        break;

      case IDC_TRAVIEW:
        DlgProcFileManageDoTracingView(hDlg);
        break;

      case IDOK:
      case IDCANCEL:
        ::EndDialog(hDlg, TRUE);
        return (TRUE);
    }
  }
  return (FALSE);
}

// Layer is made active.
// This is a warm state meaning the layer is displayed using hot color set,
// is detectable, and may have its segments modified or deleted.
// No new segments are added to an active layer.
// Zero or more layers may be active.

void DlgProcFileManageDoLayerActive(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  if (GetCurSel(hDlg, IDC_LAY, strName) != LB_ERR) {
    CLayer* pLayer = pDoc->LayersGet(strName);

    if (pLayer == nullptr) {
    } else {
      if (pLayer->IsHot()) {
        msgWarning(IDS_MSG_LAYER_NO_ACTIVE, strName);
        ::SendDlgItemMessage(hDlg, IDC_LAYACTIVE, BM_SETCHECK, 0, 0L);
        ::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, 1, 0L);
      } else {
        pLayer->SetStateWarm();
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_SAFE, pLayer);
      }
    }
  }
}

// Selected layer is deleted.
// Hot layer must be warm'ed before it may be deleted.
// Layer `0` may never be deleted.
void DlgProcFileManageDoLayerDelete(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;
  LRESULT lrCurSel = GetCurSel(hDlg, IDC_LAY, strName);
  if (lrCurSel != LB_ERR) {
    int iLayerId = pDoc->LayersLookup(strName);
    CLayer* pLayer = pDoc->LayersGetAt(iLayerId);

    if (iLayerId == 0) {
      msgWarning(IDS_MSG_LAYER_NO_DELETE_0);
    } else if (pLayer->IsHot()) {
      msgWarning(IDS_MSG_LAYER_NO_DELETE_WORK, strName);
    } else {
      pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_ERASE, pLayer);
      pDoc->LayersRemoveAt(iLayerId);
      ::SendDlgItemMessage(hDlg, IDC_LAY, LB_DELETESTRING, static_cast<WPARAM>(lrCurSel), 0L);
    }
  }
}

// Selected layer is hidden.
void DlgProcFileManageDoLayerHidden(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  if (GetCurSel(hDlg, IDC_LAY, strName) != LB_ERR) {
    CLayer* pLayer = pDoc->LayersGet(strName);

    if (pLayer->IsHot()) {
      msgWarning(IDS_MSG_LAYER_NO_HIDDEN, strName);
      ::SendDlgItemMessage(hDlg, IDC_LAYHIDDEN, BM_SETCHECK, 0, 0L);
      ::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, 1, 0L);
    } else {
      pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_ERASE, pLayer);
      pLayer->SetStateOff();
    }
  }
}

///<summary>Selected layer is converted to a tracing.</summary>
void DlgProcFileManageDoLayerMelt(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  LRESULT lrCurSel = GetCurSel(hDlg, IDC_LAY, strName);
  if (lrCurSel != LB_ERR) {
    if (pDoc->LayerMelt(strName)) {
      ::SendDlgItemMessage(hDlg, IDC_LAY, LB_DELETESTRING, static_cast<WPARAM>(lrCurSel), 0L);
      ::SendDlgItemMessage(hDlg, IDC_TRA, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)strName);
    }
  }
}

void DlgProcFileManageDoLayerRename(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  LRESULT lrCurSel = GetCurSel(hDlg, IDC_LAY, strName);
  if (lrCurSel != LB_ERR) {
    int iLayerId = pDoc->LayersLookup(strName);
    CLayer* pLayer = pDoc->LayersGetAt(iLayerId);

    if (iLayerId == 0) {
      msgWarning(IDS_MSG_LAYER_NO_RENAME_0);
    } else {
      _tcscpy_s(szLayerName, sizeof(szLayerName), strName.GetString());
      if (pGetLayerName()) {
        if (_tcslen(szLayerName) > 0) {
          strName = szLayerName;
          pLayer->SetName(strName);

          if (pLayer->IsHot()) pDoc->WorkLayerSet(pLayer);

          ::SendDlgItemMessage(hDlg, IDC_LAY, LB_DELETESTRING, static_cast<WPARAM>(lrCurSel), 0L);
          lrCurSel = ::SendDlgItemMessage(hDlg, IDC_LAY, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)szLayerName);
          ::SendDlgItemMessage(hDlg, IDC_LAY, LB_SETCURSEL, (WPARAM)lrCurSel, 0L);
        }
      }
    }
  }
}

void DlgProcFileManageDoLayerStatic(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;
  if (GetCurSel(hDlg, IDC_LAY, strName) != LB_ERR) {
    CLayer* pLayer = pDoc->LayersGet(strName);
    if (pLayer->IsHot()) {
      msgWarning(IDS_MSG_LAYER_NO_STATIC, strName);
      ::SendDlgItemMessage(hDlg, IDC_LAYSTATIC, BM_SETCHECK, 0, 0L);
      ::SendDlgItemMessage(hDlg, IDC_LAYWORK, BM_SETCHECK, 1, 0L);
    } else {
      pLayer->SetStateCold();
      pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_SAFE, pLayer);
    }
  }
}
// Selected layer is made the working layer.
// This is a hot state meaning the layer is displayed using hot color set,
// is detectable, and may have its segments modified or deleted.
// New segments may be added to the layer.
// Only one layer is in this state at a time.
void DlgProcFileManageDoLayerWork(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  if (GetCurSel(hDlg, IDC_LAY, strName) != LB_ERR) {
    CLayer* pLayer = pDoc->LayersGet(strName);

    pDoc->WorkLayerSet(pLayer);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_SAFE, pLayer);
  }
}

void DlgProcFileManageDoTracingCloak(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  if (GetCurSel(hDlg, IDC_TRA, strName) != LB_ERR) {
    CLayer* pLayer = pDoc->LayersGet(strName);
    if (pLayer->IsOpened()) {
      msgWarning(IDS_MSG_OPEN_TRACING_NO_CLOAK);
      ::SendDlgItemMessage(hDlg, IDC_TRACLOAK, BM_SETCHECK, 0, 0L);
      ::SendDlgItemMessage(hDlg, IDC_TRAOPEN, BM_SETCHECK, 1, 0L);
    } else {
      pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_ERASE, pLayer);
      pLayer->SetStateOff();
      pLayer->SetTracingFlg(CLayer::TRACING_FLG_CLOAKED);
    }
  }
}

void DlgProcFileManageDoTracingExclude(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;
  LRESULT lrCurSel = GetCurSel(hDlg, IDC_TRA, strName);

  if (lrCurSel != LB_ERR) {
    CLayer* pLayer = pDoc->LayersGet(strName);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_ERASE, pLayer);
    pDoc->LayersRemove(strName);
    ::SendDlgItemMessage(hDlg, IDC_TRA, LB_DELETESTRING, static_cast<WPARAM>(lrCurSel), 0L);
  }
}

void DlgProcFileManageDoTracingFuse(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;
  LRESULT lrCurSel = GetCurSel(hDlg, IDC_TRA, strName);

  if (lrCurSel != LB_ERR) {
    pDoc->TracingFuse(strName);

    ::SendDlgItemMessage(hDlg, IDC_TRA, LB_DELETESTRING, static_cast<WPARAM>(lrCurSel), 0L);
    ::SendDlgItemMessage(hDlg, IDC_LAY, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)strName);
  }
}

void DlgProcFileManageDoTracingInclude(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  static DWORD nFilterIndex{1};

  TCHAR szFilter[256]{};
  ::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));

  OPENFILENAME of{};

  of.lStructSize = sizeof(OPENFILENAME);
  of.hwndOwner = nullptr;
  of.hInstance = app.GetInstance();
  of.lpstrFilter = szFilter;
  of.nFilterIndex = nFilterIndex;
  of.lpstrFile = new TCHAR[MAX_PATH];
  of.lpstrFile[0] = 0;
  of.nMaxFile = MAX_PATH;
  of.lpstrTitle = _T("Include Tracing");
  of.Flags = OFN_HIDEREADONLY;
  of.lpstrDefExt = _T("tra");

  if (GetOpenFileName(&of)) {
    nFilterIndex = of.nFilterIndex;

    CString strName = of.lpstrFile;
    CString strPath = strName.Left(of.nFileOffset);

    strName = strName.Mid(of.nFileOffset);

    if (pDoc->LayersGet(strName) == nullptr) {
      if (pDoc->TracingMap(strName)) {
        CLayer* pLayer = pDoc->LayersGet(strName);
        pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);

        CString strOpenName = pDoc->GetPathName();

        if (strOpenName.Find(strPath) == -1) {
          pDoc->TracingFuse(strName);

          strName = of.lpstrFile;
          strName = strName.Mid(of.nFileOffset, of.nFileExtension - of.nFileOffset - 1);
          pLayer->SetName(strName);
        }
        ::SendDlgItemMessage(hDlg, IDC_TRA, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)strName);
      }
    }
  }
  delete[] of.lpstrFile;
}

// Selected tracing is mapped.
// This is a cold state meaning the tracing is displayed using warm color set,
// is not detectable, and may not have its segments modified or deleted.
// No new segments may be added to the tracing.
// Any number of tracings may be active.
void DlgProcFileManageDoTracingMap(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  if (GetCurSel(hDlg, IDC_TRA, strName) != LB_ERR) { pDoc->TracingMap(strName); }
}

void DlgProcFileManageDoTracingOpen(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  if (GetCurSel(hDlg, IDC_TRA, strName) != LB_ERR) { pDoc->TracingOpen(strName); }
}

void DlgProcFileManageDoTracingView(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  if (GetCurSel(hDlg, IDC_TRA, strName) != LB_ERR) { pDoc->TracingView(strName); }
}

void DlgProcFileManageInit(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  TCHAR szBuf[MAX_PATH]{};
  ::GetWindowText(hDlg, szBuf, MAX_PATH);

  CString strName = szBuf;
  strName += _T(" - ");
  strName += pDoc->GetPathName();
  ::SetWindowText(hDlg, strName);

  ::SendDlgItemMessage(hDlg, IDC_TRA, LB_SETHORIZONTALEXTENT, 512, 0);

  for (int i = 0; i < pDoc->LayersGetSize(); i++) {
    CLayer* pLayer = pDoc->LayersGetAt(i);
    strName = pLayer->GetName();
    int iCtrlId = pLayer->IsInternal() ? IDC_LAY : IDC_TRA;
    ::SendDlgItemMessage(hDlg, iCtrlId, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)strName);
  }

  for (size_t i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++) {
    _itoa_s(static_cast<int>(i), szBuf, sizeof(szBuf), 10);
    ::SendDlgItemMessage(hDlg, IDC_COLOR_ID, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)szBuf);
  }
  pDoc->PenStylesFillCB(::GetDlgItem(hDlg, IDC_PENSTYLE));

  ::SendDlgItemMessage(hDlg, IDC_BLOCKS_LIST, LB_SETHORIZONTALEXTENT, 512, 0);

  CString strKey{};
  CBlock* pBlock{};

  POSITION pos = pDoc->BlksGetStartPosition();
  while (pos != nullptr) {
    pDoc->BlksGetNextAssoc(pos, strKey, pBlock);
    if (!pBlock->IsAnonymous()) ::SendDlgItemMessage(hDlg, IDC_BLOCKS_LIST, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)strKey);
  }
  WndProcPreviewClear(hDlg);
}

LRESULT GetCurSel(HWND hDlg, int iListId, CString& strTitle) {
  LRESULT lrCurSel = ::SendDlgItemMessage(hDlg, iListId, LB_GETCURSEL, 0, 0L);
  if (lrCurSel != LB_ERR) {
    TCHAR szTitle[MAX_PATH]{};
    ::SendDlgItemMessage(hDlg, iListId, LB_GETTEXT, (WPARAM)lrCurSel, (LPARAM)((LPSTR)szTitle));
    strTitle = szTitle;
  }
  return lrCurSel;
}
void DlgProcFileManageDoLayerColor(HWND hDlg) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CString strName;

  LRESULT lrCurSel = GetCurSel(hDlg, IDC_LAY, strName);
  if (lrCurSel != LB_ERR) {
    int iLayerId = pDoc->LayersLookup(strName);
    CLayer* pLayer = pDoc->LayersGetAt(iLayerId);

    auto nPenColor = (PENCOLOR)::GetDlgItemInt(hDlg, IDC_COLOR_ID, nullptr, FALSE);
    pLayer->SetPenColor(nPenColor);
    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_LAYER_SAFE, pLayer);

    CPrim::LayerPenColor() = pLayer->PenColor();
    CPrim::LayerPenStyle() = pLayer->PenStyle();

    WndProcPreviewUpdate(hDlg, pLayer);
  }
}
