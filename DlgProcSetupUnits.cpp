#include "stdafx.h"

#include <afxstr.h>

#include "PegAEsys.h"

#include "resource.h"

void UnitsDlgProcInit(HWND);
void UnitsDlgProcDoOK(HWND);

BOOL CALLBACK DlgProcUnits(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{	// Modifies the units of measure.

    switch (anMsg)
    {
    case WM_INITDIALOG:
        UnitsDlgProcInit(hDlg);
        return (TRUE);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:
            UnitsDlgProcDoOK(hDlg);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}
void UnitsDlgProcInit(HWND hDlg)
{
    CString strMetricUnits [] = {"Meters", "Millimeters", "Centimeters", "Decimeters", "Kilometers"};

    int	iCtrlId = std::min(IDC_ARCHITECTURAL + app.GetUnits(), IDC_METRIC);

    ::CheckRadioButton(hDlg, IDC_ARCHITECTURAL, IDC_METRIC, iCtrlId);

    SetDlgItemInt(hDlg, IDC_PRECISION, app.GetUnitsPrec(), FALSE);

    for (size_t i = 0; i < sizeof(strMetricUnits) / sizeof(strMetricUnits[0]); i++)
    {
        ::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)strMetricUnits[i]);
    }
    if (iCtrlId == IDC_METRIC)
    {
        int i = app.GetUnits() - Meters;
        ::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS, LB_SETCURSEL, (WPARAM)i, 0L);
    }
}
void UnitsDlgProcDoOK(HWND hDlg)
{
    if (IsDlgButtonChecked(hDlg, IDC_ARCHITECTURAL))
        app.SetUnits(Architectural);
    else if (IsDlgButtonChecked(hDlg, IDC_ENGINEERING))
        app.SetUnits(Engineering);
    else if (IsDlgButtonChecked(hDlg, IDC_FEET))
        app.SetUnits(Feet);
    else if (IsDlgButtonChecked(hDlg, IDC_INCHES))
        app.SetUnits(Inches);
    else
    {	// get units from metric list
        switch (::SendDlgItemMessage(hDlg, IDC_METRIC_UNITS, LB_GETCURSEL, 0, 0L))
        {
        case 0:
            app.SetUnits(Meters);
            break;
        case 1:
            app.SetUnits(Millimeters);
            break;
        case 2:
            app.SetUnits(Centimeters);
            break;
        case 3:
            app.SetUnits(Decimeters);
            break;
        default:
            app.SetUnits(Kilometers);
        }
    }
    BOOL bTranslated;
    int iPrec = GetDlgItemInt(hDlg, IDC_PRECISION, &bTranslated, FALSE);
    if (bTranslated)
        app.SetUnitsPrec(iPrec);
}


