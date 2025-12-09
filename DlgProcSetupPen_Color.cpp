#include "stdafx.h"

#include <atltypes.h>

#include <string>

#include "PegAEsys.h"

#include "OwnerDraw.h"
#include "Prim.h"
#include "PrimState.h"

void SetupPenColor_DrawEntire(LPDRAWITEMSTRUCT, int);

INT_PTR CALLBACK DlgProcSetupPenColor(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept {
  std::string szBuf;
  PENCOLOR nPenColor;

  switch (anMsg) {
    case WM_INITDIALOG:
      for (size_t i = 0; i < sizeof(crHotCols) / sizeof(COLORREF); i++) {
        szBuf = std::to_string(i);
        ::SendDlgItemMessage(hDlg, IDC_COL_LIST, CB_ADDSTRING, 0, (LPARAM)szBuf.c_str());
      }
      nPenColor = pstate.PenColor();
      ::SendDlgItemMessage(hDlg, IDC_COL_LIST, CB_SETCURSEL, nPenColor, 0L);
      return (TRUE);

    case WM_COMMAND: {
      switch (LOWORD(wParam)) {
        case IDOK:
          nPenColor = (PENCOLOR)::GetDlgItemInt(hDlg, IDC_COL_LIST, nullptr, FALSE);
          pstate.SetPenColor(nPenColor);
          [[fallthrough]];  // Intentional fallthrough

        case IDCANCEL:
          ::EndDialog(hDlg, TRUE);
          return (TRUE);
      }
      break;
    }
    case WM_DRAWITEM: {
      LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
      if (lpDIS->itemID == -1)      // Empty combo box
        OwnerDraw_Focus(lpDIS, 0);  // Draw only focus rectangle
      else {
        switch (lpDIS->itemAction) {
          case ODA_DRAWENTIRE:
            SetupPenColor_DrawEntire(lpDIS, -2);
            break;

          case ODA_SELECT:
            OwnerDraw_Select(lpDIS, 0);
            break;

          case ODA_FOCUS:
            OwnerDraw_Focus(lpDIS, 0);
            break;
        }
      }
      return (TRUE);
    }
  }
  return (FALSE);
}
///<summary>Draw the pen color number and a rectangle filled with the pen color.</summary>
void SetupPenColor_DrawEntire(LPDRAWITEMSTRUCT lpDIS, int inflate) {
  CDC dc;
  dc.Attach(lpDIS->hDC);

  std::string buffer(32, '\0');

  CRect rc;
  ::CopyRect(&rc, &lpDIS->rcItem);

  LRESULT result = ::SendMessage(lpDIS->hwndItem, CB_GETLBTEXT, lpDIS->itemID, reinterpret_cast<LPARAM>(&buffer[0]));
  if (result == CB_ERR) {
    dc.Detach();
    return;
  }
  buffer.resize(static_cast<size_t>(result));
  UINT length = static_cast<UINT>(result);

  dc.SetTextColor(RGB(0, 0, 0));
  dc.SetBkColor(RGB(255, 255, 255));

  dc.ExtTextOut(rc.right - 16, rc.top + 2, ETO_CLIPPED, &rc, buffer.c_str(), length, nullptr);

  ::InflateRect(&rc, inflate - 2, inflate - 2);
  rc.right -= 24;

  CBrush brush(crHotCols[lpDIS->itemID]);
  dc.FillRect(&rc, &brush);

  OwnerDraw_Select(lpDIS, inflate);
  OwnerDraw_Focus(lpDIS, inflate);

  dc.Detach();
}
