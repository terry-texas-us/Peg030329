#include "stdafx.h"

#include <cstdlib>

#include "PegAEsysView.h"

void DlgBoxSetItemDouble(HWND hDlg, int control, double value);

INT_PTR CALLBACK DlgProcViewParameters(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept {
  CPegView* pView{CPegView::GetActiveView()};

  TCHAR szBuf[32]{};

  switch (anMsg) {
    case WM_INITDIALOG:
      DlgBoxSetItemDouble(hDlg, IDC_NEAR, pView->ModelViewGetNearClipDistance());
      DlgBoxSetItemDouble(hDlg, IDC_FAR, pView->ModelViewGetFarClipDistance());

      return (TRUE);

    case WM_COMMAND:

      switch (LOWORD(wParam)) {
        case IDOK:
          ::GetDlgItemText(hDlg, IDC_NEAR, szBuf, sizeof(szBuf));
          pView->ModelViewSetNearClipDistance(atof(szBuf));
          ::GetDlgItemText(hDlg, IDC_FAR, szBuf, sizeof(szBuf));
          pView->ModelViewSetFarClipDistance(atof(szBuf));

          pView->InvalidateRect(nullptr, TRUE);
          [[fallthrough]];  // Intentional fallthrough

        case IDCANCEL:
          ::EndDialog(hDlg, TRUE);
          return (TRUE);
      }
      break;
  }
  return (FALSE);
}
