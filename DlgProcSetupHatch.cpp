#include "stdafx.h"

#include <algorithm>

#include "ExpProcs.h"
#include "Hatch.h"
#include "Polygon.h"
#include "PrimState.h"
#include "SafeMath.h"

INT_PTR CALLBACK DlgProcSetupHatch(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
    short	nStyleId;

    switch (anMsg)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(hDlg, IDC_FIL_AREA_HAT_ID, pstate.PolygonIntStyleId(), FALSE);
        DlgBoxSetItemDouble(hDlg, IDC_FIL_AREA_HAT_X_SCAL, hatch::dXAxRefVecScal);
        DlgBoxSetItemDouble(hDlg, IDC_FIL_AREA_HAT_Y_SCAL, hatch::dYAxRefVecScal);
        DlgBoxSetItemDouble(hDlg, IDC_FIL_AREA_HAT_ROT_ANG, hatch::dOffAng / RADIAN);
        return (TRUE);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:
            pstate.SetPolygonIntStyle(POLYGON_HATCH);
            nStyleId = short(GetDlgItemInt(hDlg, IDC_FIL_AREA_HAT_ID, 0, FALSE));
            pstate.SetPolygonIntStyleId(nStyleId);
            hatch::dXAxRefVecScal = std::max(0.01, DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_X_SCAL));
            hatch::dYAxRefVecScal = std::max(0.01, DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_Y_SCAL));
            hatch::dOffAng = Arc(DlgBoxGetItemDouble(hDlg, IDC_FIL_AREA_HAT_ROT_ANG));
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}

