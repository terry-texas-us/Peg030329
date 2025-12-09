#include "stdafx.h"

#include <algorithm>

#include "PegAEsys.h"

#include "SubProcLPD.h"
#include "UnitsString.h"

double DlgBoxGetItemDouble(HWND hDlg, int control);
void DlgBoxSetItemDouble(HWND hDlg, int control, double value);
void DlgBoxSetItemUnitsText(HWND hDlg, int control, double value);

void DlgProcLPDOptionsInit(HWND);

INT_PTR CALLBACK DlgProcLPDOptions(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM) noexcept {
  TCHAR szBuf[32]{};

  switch (nMsg) {
    case WM_INITDIALOG:
      DlgProcLPDOptionsInit(hDlg);
      return (TRUE);

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          lpd::dInsRadElFac = DlgBoxGetItemDouble(hDlg, IDC_RADIUS_FACTOR);
          ::GetDlgItemText(hDlg, IDC_WIDTH, szBuf, sizeof(szBuf));
          lpd::dSecWid = UnitsString_ParseLength(std::max(app.GetUnits(), Inches), szBuf);
          ::GetDlgItemText(hDlg, IDC_DEPTH, szBuf, sizeof(szBuf));
          lpd::dSecDep = UnitsString_ParseLength(std::max(app.GetUnits(), Inches), szBuf);
          lpd::bGenTurnVanes = (IsDlgButtonChecked(hDlg, IDC_GEN_VANES) ? true : false);
          lpd::eElbow = (IsDlgButtonChecked(hDlg, IDC_MITERED) ? Mittered : Radial);

          lpd::eJust = lpd::Center;
          if (IsDlgButtonChecked(hDlg, IDC_LEFT)) {
            lpd::eJust = lpd::Left;
          } else if (IsDlgButtonChecked(hDlg, IDC_RIGHT)) {
            lpd::eJust = lpd::Right;
          }
          [[fallthrough]];  // Intentional fallthrough

        case IDCANCEL:
          ::EndDialog(hDlg, TRUE);
          return (TRUE);
      }
      break;
  }
  return (FALSE);
}
void DlgProcLPDOptionsInit(HWND hDlg) {
  EUnits eUnits = app.GetUnits();

  app.SetUnits(std::max(eUnits, Inches));

  DlgBoxSetItemUnitsText(hDlg, IDC_WIDTH, lpd::dSecWid);
  DlgBoxSetItemUnitsText(hDlg, IDC_DEPTH, lpd::dSecDep);

  app.SetUnits(eUnits);

  DlgBoxSetItemDouble(hDlg, IDC_RADIUS_FACTOR, lpd::dInsRadElFac);

  ::CheckRadioButton(hDlg, IDC_LEFT, IDC_RIGHT, IDC_CENTER + lpd::eJust);

  if (lpd::bGenTurnVanes) ::SendDlgItemMessage(hDlg, IDC_GEN_VANES, BM_SETCHECK, 1, 0L);

  ::SendDlgItemMessage(hDlg, IDC_MITERED + lpd::eElbow, BM_SETCHECK, 1, 0L);
}
