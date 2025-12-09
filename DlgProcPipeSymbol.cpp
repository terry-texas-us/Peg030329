#include "stdafx.h"

#include <string.h>

#include "PegAEsys.h"

#include "SubProcPipe.h"

void DlgProcPipeSymbolInit(HWND);

INT_PTR CALLBACK DlgProcPipeSymbol(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept {
  switch (anMsg) {
    case WM_INITDIALOG:
      DlgProcPipeSymbolInit(hDlg);
      return (TRUE);

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          pipe::wCurSymId = WORD(::SendDlgItemMessage(hDlg, IDC_LIST, LB_GETCURSEL, 0, 0L));
          [[fallthrough]];  // Intentional fallthrough

        case IDCANCEL:
          ::EndDialog(hDlg, TRUE);
          return (TRUE);
      }
      break;

      break;
  }
  return (FALSE);
}
void DlgProcPipeSymbolInit(HWND hDlg) {
  TCHAR szNames[512]{};
  TCHAR* pName;

  ::LoadString(app.GetInstance(), IDS_SUBPROC_PIPE_SYMBOL_NAMES, szNames, 256);

  TCHAR* context = nullptr;
  pName = strtok_s(szNames, "\t", &context);
  while (pName != nullptr) {
    ::SendDlgItemMessage(hDlg, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)pName);
    pName = strtok_s(nullptr, "\t", &context);
  }
  ::SendDlgItemMessage(hDlg, IDC_LIST, LB_SETCURSEL, pipe::wCurSymId, 0L);
}
