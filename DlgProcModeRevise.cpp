#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "PrimText.h"
#include "SegsDet.h"
#include "Text.h"

///<remarks>
///Text related attributes for all notes generated will be same as those of the text last picked.
///Upon exit attributes restored to their entry values.
///</remarks>
INT_PTR CALLBACK DlgProcModeRevise(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept {
  static CFontDef fd;
  static CRefSys rs;
  static CPrimText* pText{nullptr};

  CPegView* pView = CPegView::GetActiveView();

  HWND hWndTextCtrl = ::GetDlgItem(hDlg, IDC_TEXT);

  switch (nMsg) {
    case WM_INITDIALOG:
      pText = detsegs.SelTextUsingPoint(pView, app.CursorPosGet());
      if (pText != nullptr) {
        pText->GetFontDef(fd);
        pText->GetRefSys(rs);
        ::SetWindowText(hWndTextCtrl, pText->Text());
      } else
        ::EndDialog(hDlg, TRUE);

      return (TRUE);

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK: {
          int iLen = ::GetWindowTextLength(hWndTextCtrl);

          char* pszNew = new char[iLen + 1u];
          ::GetWindowText(hWndTextCtrl, pszNew, iLen + 1);

          CPegDoc* pDoc = CPegDoc::GetDoc();

          if (pText != nullptr) {
            pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM_ERASE_SAFE, pText);
            pText->SetText(pszNew);
            pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_PRIM_SAFE, pText);
          } else {
            CSeg* pSeg = new CSeg(new CPrimText(fd, rs, pszNew));
            pDoc->WorkLayerAddTail(pSeg);
            pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSeg);
          }
          delete[] pszNew;

          rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));

          pText = detsegs.SelTextUsingPoint(pView, rs.Origin());
          if (pText != nullptr) {
            pText->GetFontDef(fd);
            pText->GetRefSys(rs);
            ::SetWindowText(hWndTextCtrl, pText->Text());
          } else
            SetWindowText(hWndTextCtrl, "");

          ::SetFocus(hWndTextCtrl);
          return (TRUE);
        }
        case IDCANCEL:
          ::EndDialog(hDlg, TRUE);
          return (TRUE);
      }
      break;

    case WM_SIZE:
      ::SetWindowPos(hWndTextCtrl, nullptr, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOMOVE);
      break;
  }
  return (FALSE);
}