#include "stdafx.h"

#include <string>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "CharCellDef.h"
#include "FontDef.h"
#include "Pnt.h"
#include "PrimState.h"
#include "PrimText.h"
#include "RefSys.h"
#include "Seg.h"
#include "Text.h"

INT_PTR CALLBACK DlgProcModeLetter(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    CPegDoc* pDoc = CPegDoc::GetDoc();

    static CPnt ptPvt{};

    HWND hWndTextCtrl = ::GetDlgItem(hDlg, IDC_TEXT);

    switch (nMsg)
    {
    case WM_INITDIALOG:
    {
        ptPvt = app.CursorPosGet();
        return (TRUE);
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            CCharCellDef ccd{};
            pstate.GetCharCellDef(ccd);
            CRefSys rs(ptPvt, ccd);

            CFontDef fd{};
            pstate.GetFontDef(fd);

            int textLength{::GetWindowTextLength(hWndTextCtrl)};
            if (textLength != 0)
            {
                std::string text(textLength + 1U, '\0');

                ::GetWindowText(hWndTextCtrl, text.data(), textLength + 1);
                ::SetWindowText(hWndTextCtrl, "");

                auto segment = new CSeg{new CPrimText{fd, rs, text.c_str()}};
                pDoc->WorkLayerAddTail(segment);
                pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
            }
            ptPvt = text_GetNewLinePos(fd, rs, 1.0, 0);
            ::SetFocus(hWndTextCtrl);
            return (TRUE);
        }
        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;

    case WM_SIZE:
        ::SetWindowPos(hWndTextCtrl, 0, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOMOVE);
        break;
    }
    return (FALSE);
}
