#include "stdafx.h"

#include <cfloat>
#include <cmath>

#include "PegAEsysView.h"

#include "Pnt.h"
#include "SafeMath.h"
#include "Vec.h"

double DlgBoxGetItemDouble(HWND hDlg, int control);
void DlgBoxSetItemDouble(HWND hDlg, int control, double value);

INT_PTR CALLBACK DlgProcProjOblique(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept {
  CPegView* pView{CPegView::GetActiveView()};

  double dAng, dPhi, dTheta;
  CPnt ptProjRefPt{};
  CVec vVwPlnNorm{};
  CVec vVwUp{};

  switch (anMsg) {
    case WM_INITDIALOG:
      vVwPlnNorm = pView->ModelViewGetDirection();
      dAng = acos(-vVwPlnNorm[2]) / RADIAN;
      DlgBoxSetItemDouble(hDlg, IDC_VIEW_VPN_Z_ANG, dAng);
      dAng = Vec_Angle_xy(-vVwPlnNorm) / RADIAN;
      DlgBoxSetItemDouble(hDlg, IDC_VIEW_VPN_X_ANG, dAng);
      dAng = acos(-ptProjRefPt[2]) / RADIAN;
      DlgBoxSetItemDouble(hDlg, IDC_VIEW_DOP_Z_ANG, dAng);
      dAng = Vec_Angle_xy(-(ptProjRefPt - ORIGIN)) / RADIAN;
      DlgBoxSetItemDouble(hDlg, IDC_VIEW_DOP_X_ANG, dAng);
      return (TRUE);

    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          dPhi = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_VPN_Z_ANG));
          dTheta = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_VPN_X_ANG));

          vVwPlnNorm = CVec(-sin(dPhi) * cos(dTheta), -sin(dPhi) * sin(dTheta), -cos(dPhi));

          dPhi = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_DOP_Z_ANG));
          dTheta = Arc(DlgBoxGetItemDouble(hDlg, IDC_VIEW_DOP_X_ANG));

          ptProjRefPt = CPnt(-sin(dPhi) * cos(dTheta), -sin(dPhi) * sin(dTheta), -cos(dPhi));

          if (fabs(dPhi) <= DBL_EPSILON) {
            vVwUp(-cos(dTheta), -sin(dTheta), 0.);
          } else if (fabs(dPhi - PI) <= DBL_EPSILON) {
            vVwUp(cos(dTheta), sin(dTheta), 0.);
          } else {
            vVwUp = vVwPlnNorm ^ ZDIR;
            vVwUp = vVwUp ^ vVwPlnNorm;
            vVwUp.Normalize();
            if (dPhi > PI) vVwUp = -vVwUp;
          }
          pView->ModelViewSetDirection(-vVwPlnNorm);
          pView->ModelViewSetVwUp(vVwUp);
          pView->ModelViewSetPerspectiveEnabled(false);
          [[fallthrough]];  // Intentional fallthrough

        case IDCANCEL:
          ::EndDialog(hDlg, TRUE);
          return (TRUE);
      }
  }
  return (FALSE);
}
