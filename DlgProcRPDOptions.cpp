#include "stdafx.h"

#include "Windows.h"
#include <afx.h>

#include <algorithm>

#include "PegAEsys.h"

#include "ExpProcs.h"
#include "resource.h"
#include "SubProcRLPD.h"
#include "UnitsString.h"

void DlgProcRLPDOptionsInit(HWND);

BOOL CALLBACK DlgProcRLPDOptions(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM)
{
    char szBuf[32];

    switch (nMsg)
    {
    case WM_INITDIALOG:
        DlgProcRLPDOptionsInit(hDlg);
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            rlpd::dInsideRadiusFactor = DlgBoxGetItemDouble(hDlg, IDC_RADIUS_FACTOR);
            ::GetDlgItemText(hDlg, IDC_DIAMETER, szBuf, sizeof(szBuf));
            rlpd::dSecDiam = UnitsString_ParseLength(std::max(app.GetUnits(), Inches), szBuf);
            rlpd::bGenCenterline = (IsDlgButtonChecked(hDlg, IDC_CENTERLINE) ? true : false);

            rlpd::eJust0 = rlpd::Center0;
            if (IsDlgButtonChecked(hDlg, IDC_LEFT))
            {
                rlpd::eJust0 = rlpd::Left0;
            }
            else if (IsDlgButtonChecked(hDlg, IDC_RIGHT))
            {
                rlpd::eJust0 = rlpd::Right0;
            }
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}

void DlgProcRLPDOptionsInit(HWND hDlg)
{
    EUnits eUnits = app.GetUnits();
    app.SetUnits(std::max(eUnits, Inches));
    DlgBoxSetItemUnitsText(hDlg, IDC_DIAMETER, rlpd::dSecDiam);
    app.SetUnits(eUnits);
    DlgBoxSetItemDouble(hDlg, IDC_RADIUS_FACTOR, rlpd::dInsideRadiusFactor);

    ::CheckRadioButton(hDlg, IDC_LEFT, IDC_RIGHT, IDC_CENTER + rlpd::eJust0);

    if (rlpd::bGenCenterline) { ::SendDlgItemMessage(hDlg, IDC_CENTERLINE, BM_SETCHECK, 1, 0L); }
}
