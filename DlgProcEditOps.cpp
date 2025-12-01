#include "stdafx.h"

#include <Windows.h>

#include <afx.h>

#include <cfloat>
#include <cmath>

#include "DlgProcEditOps.h"
#include "ExpProcs.h"
#include "resource.h"
#include "TMat.h"
#include "Vec.h"

namespace dlgproceditops
{
    double dXRotAng;
    double dYRotAng;
    double dZRotAng;

    double dXScale;
    double dYScale;
    double dZScale;

    double dXMirrorScale;
    double dYMirrorScale;
    double dZMirrorScale;

    int iRotOrd[3];
}

CVec dlgproceditops::GetMirrorScale()
{
    return (CVec(dlgproceditops::dXMirrorScale, dlgproceditops::dYMirrorScale, dlgproceditops::dZMirrorScale));
}
CVec dlgproceditops::GetRotAng()
{
    return (CVec(dlgproceditops::dXRotAng, dlgproceditops::dYRotAng, dlgproceditops::dZRotAng));
}
CTMat dlgproceditops::GetInvertedRotTrnMat()
{
    CTMat tm(dlgproceditops::iRotOrd, dlgproceditops::GetRotAng());
    tm.Inverse();
    return (tm);
}
CTMat dlgproceditops::GetRotTrnMat()
{
    return (CTMat(dlgproceditops::iRotOrd, dlgproceditops::GetRotAng()));
}
CVec dlgproceditops::GetScale()
{
    return (CVec(dlgproceditops::dXScale, dlgproceditops::dYScale, dlgproceditops::dZScale));
}
CVec dlgproceditops::GetInvertedScale()
{
    double dX = fabs(dlgproceditops::dXScale) > DBL_EPSILON ? 1. / dlgproceditops::dXScale : 1.;
    double dY = fabs(dlgproceditops::dYScale) > DBL_EPSILON ? 1. / dlgproceditops::dYScale : 1.;
    double dZ = fabs(dlgproceditops::dZScale) > DBL_EPSILON ? 1. / dlgproceditops::dZScale : 1.;

    return (CVec(dX, dY, dZ));
}
void dlgproceditops::SetMirrorScale(double dX, double dY, double dZ)
{
    dlgproceditops::dXMirrorScale = dX;
    dlgproceditops::dYMirrorScale = dY;
    dlgproceditops::dZMirrorScale = dZ;
}
void dlgproceditops::SetRotAng(double dX, double dY, double dZ)
{
    dlgproceditops::dXRotAng = dX;
    dlgproceditops::dYRotAng = dY;
    dlgproceditops::dZRotAng = dZ;
}
void dlgproceditops::SetRotOrd(int i0, int i1, int i2)
{
    dlgproceditops::iRotOrd[0] = i0;
    dlgproceditops::iRotOrd[1] = i1;
    dlgproceditops::iRotOrd[2] = i2;
}
void dlgproceditops::SetScale(double dX, double dY, double dZ)
{
    dlgproceditops::dXScale = dX;
    dlgproceditops::dYScale = dY;
    dlgproceditops::dZScale = dZ;
}

BOOL CALLBACK DlgProcEditOps(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
    if (anMsg == WM_INITDIALOG)
    {
        DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_SIZ_X, dlgproceditops::dXScale);
        DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Y, dlgproceditops::dYScale);
        DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Z, dlgproceditops::dZScale);

        DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_ROT_X, dlgproceditops::dXRotAng);
        DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_ROT_Y, dlgproceditops::dYRotAng);
        DlgBoxSetItemDouble(hDlg, IDC_EDIT_OP_ROT_Z, dlgproceditops::dZRotAng);

        if (dlgproceditops::dXMirrorScale < 0.)
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_X, 1);
        else if (dlgproceditops::dYMirrorScale < 0.)
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Y, 1);
        else
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Z, 1);
        return (TRUE);
    }
    else if (anMsg == WM_COMMAND)
    {
        switch (LOWORD(wParam))
        {
        case IDC_EDIT_OP_SIZING:
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_X), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Y), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Z), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_X), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Y), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Z), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_X), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Y), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Z), FALSE);
            return (TRUE);

        case IDC_EDIT_OP_ROTATION:
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_X), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Y), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Z), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_X), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Y), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Z), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_X), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Y), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Z), FALSE);
            return (TRUE);

        case IDC_EDIT_OP_MIRRORING:
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_X), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Y), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_SIZ_Z), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_X), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Y), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_ROT_Z), FALSE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_X), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Y), TRUE);
            ::EnableWindow(::GetDlgItem(hDlg, IDC_EDIT_OP_MIR_Z), TRUE);
            return (TRUE);

        case IDC_EDIT_OP_MIR_X:
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Y, 0);
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Z, 0);
            return (TRUE);

        case IDC_EDIT_OP_MIR_Y:
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_X, 0);
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Z, 0);
            return (TRUE);

        case IDC_EDIT_OP_MIR_Z:
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_X, 0);
            ::CheckDlgButton(hDlg, IDC_EDIT_OP_MIR_Y, 0);
            return (TRUE);

        case IDOK:
            dlgproceditops::dXScale = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_SIZ_X);
            dlgproceditops::dYScale = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Y);
            dlgproceditops::dZScale = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_SIZ_Z);

            dlgproceditops::dXRotAng = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_ROT_X);
            dlgproceditops::dYRotAng = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_ROT_Y);
            dlgproceditops::dZRotAng = DlgBoxGetItemDouble(hDlg, IDC_EDIT_OP_ROT_Z);

            if (IsDlgButtonChecked(hDlg, IDC_EDIT_OP_MIR_X))
            {
                dlgproceditops::SetMirrorScale(-1, 1., 1.);
            }
            else if (IsDlgButtonChecked(hDlg, IDC_EDIT_OP_MIR_Y))
            {
                dlgproceditops::SetMirrorScale(1., -1., 1.);
            }
            else
            {
                dlgproceditops::SetMirrorScale(1., 1., -1.);
            }
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
    }
    return (FALSE);
}
