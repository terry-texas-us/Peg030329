#include "stdafx.h"

#include "ExpProcs.h"
#include "Line.h"
#include "resource.h"

double	UserAxis_dInfAng;
double	UserAxis_dAxOffAng;

///<summary>Set Axis constraint tolerance angle and offset axis constraint offset angle</summary>
void UserAxisInit()
{
    UserAxis_dInfAng = 2.;
    UserAxis_dAxOffAng = 0.;
}
void UserAxisSetInfAng(double dAng)
{
    UserAxis_dInfAng = dAng;
}
///<summary>Constrains a line to nearest axis pivoting on first endpoint.</summary>
// Notes:	Offset angle only support about z-axis
// Returns: Point after snap
CPnt UserAxisSnapLn(const CPnt& ptBeg, const CPnt& ptEnd)
{
    return (line::ConstrainToAx(CLine(ptBeg, ptEnd), UserAxis_dInfAng, UserAxis_dAxOffAng));

}
///<summary>Modifies the user grid and axis constraints.</summary>
BOOL CALLBACK DlgProcSetupConstraintsAxis(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM)
{
    switch (anMsg)
    {
    case WM_INITDIALOG:
        DlgBoxSetItemDouble(hDlg, IDC_USR_AX_INF_ANG, UserAxis_dInfAng);
        DlgBoxSetItemDouble(hDlg, IDC_USR_AX_Z_OFF_ANG, UserAxis_dAxOffAng);

        return (TRUE);

    case WM_COMMAND:

        switch (LOWORD(wParam))
        {
        case IDOK:
            UserAxis_dInfAng = DlgBoxGetItemDouble(hDlg, IDC_USR_AX_INF_ANG);
            UserAxis_dAxOffAng = DlgBoxGetItemDouble(hDlg, IDC_USR_AX_Z_OFF_ANG);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}

