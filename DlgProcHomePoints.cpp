#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "Grid.h"
#include "Pnt.h"
#include "SegsTrap.h"

double DlgBoxGetItemUnitsText(HWND hDlg, int control);
void DlgBoxSetItemUnitsText(HWND hDlg, int control, double value);

INT_PTR CALLBACK DlgProcHomePointGo(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept {
  WORD wNotifyCode{HIWORD(wParam)};
  static CPnt pt{};

  if (anMsg == WM_INITDIALOG) {
    TCHAR szNames[256]{};
    ::LoadString(app.GetInstance(), IDS_HOME_POINT_GO_NAMES, szNames, 256);
    TCHAR* context = nullptr;
    TCHAR* pName = _tcstok_s(szNames, _T("\t"), &context);
    while (pName != nullptr) {
      ::SendDlgItemMessage(hDlg, IDC_LIST, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)pName);
      pName = _tcstok_s(nullptr, _T("\t"), &context);
    }
    ::SendDlgItemMessage(hDlg, IDC_LIST, CB_SETCURSEL, 9, 0L);

    pt = grid::GetOrign();

    DlgBoxSetItemUnitsText(hDlg, IDC_X, pt[0]);
    DlgBoxSetItemUnitsText(hDlg, IDC_Y, pt[1]);
    DlgBoxSetItemUnitsText(hDlg, IDC_Z, pt[2]);
    return (TRUE);
  } else if (anMsg == WM_COMMAND) {
    CPegView* pView = CPegView::GetActiveView();

    TCHAR szBuf[32]{};

    switch (LOWORD(wParam)) {
      case IDC_LIST:
        if (wNotifyCode == CBN_EDITUPDATE) {
          ::GetDlgItemText(hDlg, IDC_LIST, (LPSTR)szBuf, 32);
          int iId = int(::SendDlgItemMessage(hDlg, IDC_LIST, CB_FINDSTRING, (WPARAM)-1, (LPARAM)(LPCSTR)szBuf));
          if (iId != CB_ERR) {
            switch (iId) {
              case 9:
                pt = grid::GetOrign();
                break;
              case 10:
                pt = trapsegs.PvtPt();
                break;
              case 11:
                pt = pView->ModelViewGetTarget();
                break;
              case 12:
                pt = ORIGIN;
                break;
              default:
                pt = app.HomePointGet(iId);
            }
            app.CursorPosSet(pt);
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
          }
        } else if (wNotifyCode == CBN_SELCHANGE) {
          int iId = int(::SendDlgItemMessage(hDlg, IDC_LIST, CB_GETCURSEL, 0, 0L));
          if (iId != CB_ERR) {
            switch (iId) {
              case 9:
                pt = grid::GetOrign();
                break;
              case 10:
                pt = trapsegs.PvtPt();
                break;
              case 11:
                pt = pView->ModelViewGetTarget();
                break;
              case 12:
                pt = ORIGIN;
                break;
              default:
                pt = app.HomePointGet(iId);
            }
            DlgBoxSetItemUnitsText(hDlg, IDC_X, pt[0]);
            DlgBoxSetItemUnitsText(hDlg, IDC_Y, pt[1]);
            DlgBoxSetItemUnitsText(hDlg, IDC_Z, pt[2]);
          }
        }
        break;

      case IDOK:
        pt[0] = DlgBoxGetItemUnitsText(hDlg, IDC_X);
        pt[1] = DlgBoxGetItemUnitsText(hDlg, IDC_Y);
        pt[2] = DlgBoxGetItemUnitsText(hDlg, IDC_Z);

        app.CursorPosSet(pt);
        ::EndDialog(hDlg, TRUE);
        return (TRUE);

      case IDCANCEL:
        ::EndDialog(hDlg, TRUE);
        return (TRUE);
    }
  }
  return (FALSE);
}

INT_PTR CALLBACK DlgProcHomePointSet(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept {
  WORD wNotifyCode{HIWORD(wParam)};
  static CPnt pt{};

  int iId{};

  if (anMsg == WM_INITDIALOG) {
    TCHAR szNames[256]{};

    ::LoadString(app.GetInstance(), IDS_HOME_POINT_SET_NAMES, szNames, 256);

    TCHAR* context = nullptr;
    TCHAR* pName = _tcstok_s(szNames, _T("\t"), &context);
    while (pName != nullptr) {
      ::SendDlgItemMessage(hDlg, IDC_LIST, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)pName);
      pName = _tcstok_s(nullptr, _T("\t"), &context);
    }
    ::SendDlgItemMessage(hDlg, IDC_LIST, CB_SETCURSEL, 9, 0L);

    pt = app.CursorPosGet();

    DlgBoxSetItemUnitsText(hDlg, IDC_X, pt[0]);
    DlgBoxSetItemUnitsText(hDlg, IDC_Y, pt[1]);
    DlgBoxSetItemUnitsText(hDlg, IDC_Z, pt[2]);
    return (TRUE);
  } else if (anMsg == WM_COMMAND) {
    TCHAR szBuf[32]{};

    CPegView* pView = CPegView::GetActiveView();

    switch (LOWORD(wParam)) {
      case IDC_LIST:
        if (wNotifyCode == CBN_EDITUPDATE) {
          ::GetDlgItemText(hDlg, IDC_LIST, (LPSTR)szBuf, 32);
          iId = int(::SendDlgItemMessage(hDlg, IDC_LIST, CB_FINDSTRING, (WPARAM)-1, (LPARAM)(LPCSTR)szBuf));
          if (iId != CB_ERR) {
            switch (iId) {
              case 9:
                grid::SetOrign(pt);
                break;
              case 10:
                trapsegs.PvtPt() = pt;
                break;
              case 11:
                pView->ModelViewSetTarget(pt);
                break;
              default:
                app.HomePointSave(iId, pt);
            }
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
          }
        }
        break;

      case IDOK:
        pt[0] = DlgBoxGetItemUnitsText(hDlg, IDC_X);
        pt[1] = DlgBoxGetItemUnitsText(hDlg, IDC_Y);
        pt[2] = DlgBoxGetItemUnitsText(hDlg, IDC_Z);

        iId = int(::SendDlgItemMessage(hDlg, IDC_LIST, CB_GETCURSEL, 0, 0L));
        if (iId == CB_ERR) break;
        switch (iId) {
          case 9:
            grid::SetOrign(pt);
            break;
          case 10:
            trapsegs.PvtPt() = pt;
            break;
          case 11:
            pView->ModelViewSetTarget(pt);
            break;
          default:
            app.HomePointSave(iId, pt);
        }
        ::EndDialog(hDlg, TRUE);
        return (TRUE);

      case IDCANCEL:
        ::EndDialog(hDlg, TRUE);
        return (TRUE);
    }
  }
  return (FALSE);
}
