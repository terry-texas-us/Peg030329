#include "stdafx.h"

#include "PegAEsysDoc.h"

#include "CharCellDef.h"
#include "DlgProcTrapModify.h"
#include "FontDef.h"
#include "Hatch.h"
#include "Prim.h"
#include "PrimPolygon.h"
#include "PrimState.h"
#include "Seg.h"
#include "SegsTrap.h"

/// @brief Modifies attributes of all segment primatives in current trap tocurrent settings.
/// @note Trap color index is not modified.
INT_PTR CALLBACK DlgProcTrapModify(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept
{
    CPegDoc* pDoc{CPegDoc::GetDoc()};

    switch (anMsg)
    {
    case WM_INITDIALOG:
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            DlgProcTrapModifyDoOK(hDlg);
          pDoc->UpdateAllViews(nullptr, 0L, nullptr);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
    }
    return (FALSE);
}

void DlgProcTrapModifyDoOK(HWND hDlg)
{
    if (::SendDlgItemMessage(hDlg, IDC_MOD_PEN, BM_GETCHECK, 0, 0L))
        trapsegs.ModifyPenColor(pstate.PenColor());
    if (::SendDlgItemMessage(hDlg, IDC_MOD_LINE, BM_GETCHECK, 0, 0L))
        trapsegs.ModifyPenStyle(pstate.PenStyle());
    if (::SendDlgItemMessage(hDlg, IDC_MOD_FILL, BM_GETCHECK, 0, 0L))
        DlgProcTrapModifyPolygons();

    CCharCellDef ccd{};
    pstate.GetCharCellDef(ccd);

    CFontDef fd{};
    pstate.GetFontDef(fd);

    if (::SendDlgItemMessage(hDlg, IDC_MOD_NOTE, BM_GETCHECK, 0, 0L))
        trapsegs.ModifyNotes(fd, ccd, TM_TEXT_ALL);
    else if (::SendDlgItemMessage(hDlg, IDC_FONT, BM_GETCHECK, 0, 0L))
        trapsegs.ModifyNotes(fd, ccd, TM_TEXT_FONT);
    else if (::SendDlgItemMessage(hDlg, IDC_HEIGHT, BM_GETCHECK, 0, 0L))
        trapsegs.ModifyNotes(fd, ccd, TM_TEXT_HEIGHT);

}

void DlgProcTrapModifyPolygons()
{
    POSITION pos = trapsegs.GetHeadPosition();
  while (pos != nullptr)
    {
        CSeg* pSeg = trapsegs.GetNext(pos);

        POSITION posPrim = pSeg->GetHeadPosition();
        while (posPrim != nullptr)
        {
            CPrim* pPrim = pSeg->GetNext(posPrim);

            if (pPrim->Is(CPrim::Type::Polygon))
            {
                CPrimPolygon* pPolygon = static_cast<CPrimPolygon*>(pPrim);
                pPolygon->SetIntStyle(pstate.PolygonIntStyle());
                pPolygon->SetIntStyleId(pstate.PolygonIntStyleId());
                pPolygon->SetHatRefVecs(hatch::dOffAng, hatch::dXAxRefVecScal, hatch::dYAxRefVecScal);
            }
        }
    }
}
