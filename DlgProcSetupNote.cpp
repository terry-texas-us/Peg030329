#include "stdafx.h"

#include <string.h>

#include <cstdlib>

#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "CharCellDef.h"
#include "FontDef.h"
#include "PrimState.h"
#include "SafeMath.h"

void DlgBoxSetItemDouble(HWND hDlg, int control, double value);

void DlgProcSetupNoteInit(HWND hDlg);

INT_PTR CALLBACK DlgProcSetupNote(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM) noexcept {
  switch (nMsg) {
    case WM_INITDIALOG:

      DlgProcSetupNoteInit(hDlg);
      return (TRUE);

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK: {
          CPegView* pView = CPegView::GetActiveView();
          CDC* pDC = (pView == nullptr) ? nullptr : pView->GetDC();

          app.SetEditSegBeg(app.CursorPosGet());

          CCharCellDef ccd;
          pstate.GetCharCellDef(ccd);

          TCHAR szBuf[32]{};

          ::GetDlgItemText(hDlg, IDC_TEXT_HEIGHT, szBuf, sizeof(szBuf));
          ccd.ChrHgtSet(atof(szBuf));
          ::GetDlgItemText(hDlg, IDC_TEXT_ROTATION, szBuf, sizeof(szBuf));
          ccd.TextRotAngSet(atof(szBuf) * RADIAN);
          ::GetDlgItemText(hDlg, IDC_TEXT_EXP_FAC, szBuf, sizeof(szBuf));
          ccd.ChrExpFacSet(atof(szBuf));
          ::GetDlgItemText(hDlg, IDC_TEXT_INCLIN, szBuf, sizeof(szBuf));
          ccd.ChrSlantAngSet(atof(szBuf) * RADIAN);

          CFontDef fd;
          pstate.GetFontDef(fd);

          ::GetDlgItemText(hDlg, IDC_TEXT_SPACING, szBuf, sizeof(szBuf));
          fd.ChrSpacSet(atof(szBuf));

          WORD wHorAlign = IDC_TEXT_ALIGN_HOR_LEFT;
          while (!IsDlgButtonChecked(hDlg, wHorAlign)) wHorAlign++;
          wHorAlign = WORD(1 - IDC_TEXT_ALIGN_HOR_LEFT + wHorAlign);

          WORD wVerAlign = IDC_TEXT_ALIGN_VER_BOT;
          while (!IsDlgButtonChecked(hDlg, wVerAlign)) wVerAlign++;
          wVerAlign = WORD(4 + IDC_TEXT_ALIGN_VER_BOT - wVerAlign);

          fd.TextHorAlignSet(wHorAlign);
          fd.TextVerAlignSet(wVerAlign);

          WORD wPath = IDC_PATH_RIGHT;
          while (!IsDlgButtonChecked(hDlg, wPath)) wPath++;
          wPath = WORD(wPath - IDC_PATH_RIGHT);

          fd.TextPathSet(wPath);

          LRESULT lrSel = ::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_GETCURSEL, 0, 0L);
          if (lrSel != CB_ERR) {
            ::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_GETLBTEXT, (WPARAM)lrSel, (LPARAM)((LPSTR)szBuf));
            fd.TextFontSet(szBuf);

            WORD wPrec =
                (strcmp(szBuf, "Simplex.psf") != 0) ? CFontDef::PREC_TRUETYPEFONT : CFontDef::PREC_PEGSTROKEFONT;
            fd.TextPrecSet(wPrec);
          }
          pstate.SetFontDef(pDC, fd);
          pstate.SetCharCellDef(ccd);
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

int CALLBACK EnumFontFamProc(const LPLOGFONT lplf, const LPTEXTMETRIC, int, LPARAM lParam) {
  HWND hDlg = HWND(lParam);

  const LPENUMLOGFONT elf = (const LPENUMLOGFONT)lplf;
  ::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)elf->elfFullName);

  return 1;
}

void DlgProcSetupNoteInit(HWND hDlg) {
  CPegView* activeView = CPegView::GetActiveView();
  CDC* context = (activeView == nullptr) ? nullptr : activeView->GetDC();

  LOGFONT lf{};

  lf.lfFaceName[0] = '\0';
  lf.lfCharSet = ANSI_CHARSET;

  if (context != nullptr) {
    ::EnumFontFamiliesEx(context->GetSafeHdc(), &lf, (FONTENUMPROC)EnumFontFamProc, LPARAM(hDlg), 0);
  }
  ::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_ADDSTRING, 0, (LPARAM)(LPCSTR) "Simplex.psf");

  CCharCellDef ccd;
  pstate.GetCharCellDef(ccd);

  DlgBoxSetItemDouble(hDlg, IDC_TEXT_HEIGHT, ccd.ChrHgtGet());
  DlgBoxSetItemDouble(hDlg, IDC_TEXT_ROTATION, ccd.TextRotAngGet() / RADIAN);
  DlgBoxSetItemDouble(hDlg, IDC_TEXT_EXP_FAC, ccd.ChrExpFacGet());
  DlgBoxSetItemDouble(hDlg, IDC_TEXT_INCLIN, ccd.ChrSlantAngGet() / RADIAN);

  CFontDef fd;
  pstate.GetFontDef(fd);

  TCHAR szBuf[32]{};
  strcpy_s(szBuf, sizeof(szBuf), fd.TextFont());
  ::SendDlgItemMessage(hDlg, IDC_FONT_NAME, CB_SELECTSTRING, (WPARAM)-1, (LPARAM)((LPSTR)szBuf));

  DlgBoxSetItemDouble(hDlg, IDC_TEXT_SPACING, fd.ChrSpac());

  ::CheckRadioButton(hDlg, IDC_PATH_RIGHT, IDC_PATH_DOWN, IDC_PATH_RIGHT + fd.TextPath());
  ::CheckRadioButton(hDlg, IDC_TEXT_ALIGN_HOR_LEFT, IDC_TEXT_ALIGN_HOR_RIGHT,
                     IDC_TEXT_ALIGN_HOR_LEFT + fd.TextHorAlign() - 1);
  ::CheckRadioButton(hDlg, IDC_TEXT_ALIGN_VER_BOT, IDC_TEXT_ALIGN_VER_TOP,
                     IDC_TEXT_ALIGN_VER_BOT - fd.TextVerAlign() + 4);
}
