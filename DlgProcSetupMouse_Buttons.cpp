#include "stdafx.h"

#include <string>

extern std::string leftMouseDown;
extern std::string rightMouseDown;
extern std::string leftMouseUp;
extern std::string rightMouseUp;

static void DlgBoxGetItemText(HWND hDlg, int control, std::string& text) {
  int textLength{GetWindowTextLength(GetDlgItem(hDlg, control))};
  text.resize(static_cast<size_t>(textLength) + 1);
  UINT controlTextLength{::GetDlgItemText(hDlg, control, text.data(), textLength + 1)};
  text.resize(static_cast<size_t>(controlTextLength));
}

INT_PTR CALLBACK DlgProcSetupMouse_Buttons(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept {
  switch (anMsg) {
    case WM_INITDIALOG:
      SetDlgItemText(hDlg, IDC_LEFT_DOWN, leftMouseDown.data());
      SetDlgItemText(hDlg, IDC_LEFT_UP, leftMouseUp.data());
      SetDlgItemText(hDlg, IDC_RIGHT_DOWN, rightMouseDown.data());
      SetDlgItemText(hDlg, IDC_RIGHT_UP, rightMouseUp.data());
      return (TRUE);

    case WM_COMMAND:

      switch (LOWORD(wParam)) {
        case IDOK: {
          DlgBoxGetItemText(hDlg, IDC_LEFT_DOWN, leftMouseDown);
          DlgBoxGetItemText(hDlg, IDC_LEFT_UP, leftMouseUp);
          DlgBoxGetItemText(hDlg, IDC_RIGHT_DOWN, rightMouseDown);
          DlgBoxGetItemText(hDlg, IDC_RIGHT_UP, rightMouseUp);

          [[fallthrough]];  // Intentional fallthrough
        }
        case IDCANCEL:
          ::EndDialog(hDlg, TRUE);
          return (TRUE);
      }
      break;
  }
  return (FALSE);
}
