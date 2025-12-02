#include "stdafx.h"
#include <cmath>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgViewZoom.h"
#include "Line.h"
#include "Polygon.h"
#include "SafeMath.h"

HBITMAP		CDlgViewZoom::m_hbmKeyplan = NULL;
CRect		CDlgViewZoom::m_rcWnd;
bool		CDlgViewZoom::bKeyplan = false;

// CDlgViewZoom dialog

IMPLEMENT_DYNAMIC(CDlgViewZoom, CDialog)

CDlgViewZoom::CDlgViewZoom(CWnd* pParent /*=NULL*/) :
    CDialog(CDlgViewZoom::IDD, pParent), m_dRatio(0)
{ }
CDlgViewZoom::~CDlgViewZoom()
{ }
void CDlgViewZoom::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_RATIO, m_dRatio);
    DDV_MinMaxDouble(pDX, m_dRatio, .001, 1000.);
}
BOOL CDlgViewZoom::OnInitDialog()
{
    CPegView* pView = CPegView::GetActiveView();

    CDialog::OnInitDialog();

    double dViewportWidth = pView->GetWidthInInches();
    double dViewportHeight = pView->GetHeightInInches();

    double dUMin = -dViewportWidth * .5;
    double dUMax = dUMin + dViewportWidth;

    double dVMin = -dViewportHeight * .5;
    double dVMax = dVMin + dViewportHeight;

    pView->ModelViewAdjustWnd(dUMin, dVMin, dUMax, dVMax, 1.);

    double dUExt = fabs(dUMax - dUMin);
    double dVExt = fabs(dVMax - dVMin);

    CPnt ptCur = app.CursorPosGet();
    CVec vDirection = pView->ModelViewGetDirection();
    CPnt ptTarget = pView->ModelViewGetTarget();
    line::IntersectionWithPln(ptCur, vDirection, ptTarget, vDirection, &ptCur);

    dUMin = ptCur[0] - (dUExt * .5);
    dUMax = dUMin + dUExt;
    dVMin = ptCur[1] - (dVExt * .5);
    dVMax = dVMin + dVExt;

    CRect rcKey;
    GetDlgItem(IDC_KEYPLAN_AREA)->GetClientRect(&rcKey);

    double dUMinOverview = ptTarget[0] + pView->OverGetUMin();
    double dVMinOverview = ptTarget[1] + pView->OverGetVMin();

    m_rcWnd.left = Round((dUMin - dUMinOverview) / pView->OverGetUExt() * rcKey.right);
    m_rcWnd.right = Round((dUMax - dUMinOverview) / pView->OverGetUExt() * rcKey.right);
    m_rcWnd.top = Round((1. - (dVMax - dVMinOverview) / pView->OverGetVExt()) * rcKey.bottom);
    m_rcWnd.bottom = Round((1. - (dVMin - dVMinOverview) / pView->OverGetVExt()) * rcKey.bottom);

    CRect rcMoreArea;
    GetWindowRect(&rcMoreArea);

    char szBuf[16]{};
    wsprintf(szBuf, "%05u %05u", rcMoreArea.Width(), rcMoreArea.Height());

    GetDlgItem(IDC_LESS_AREA)->SetWindowText(szBuf);
    GetDlgItem(IDC_KEYPLAN_AREA)->EnableWindow(bKeyplan);

    Refresh();
    return TRUE;
}
void CDlgViewZoom::OnOK()
{
    CDialog::OnOK();

    CPegView* pView = CPegView::GetActiveView();

    if (bKeyplan)
    {
        CRect rcKey;

        GetDlgItem(IDC_KEYPLAN_AREA)->GetClientRect(&rcKey);

        CPnt ptTarget = pView->ModelViewGetTarget();
        double dUMinOverview = pView->OverGetUMin();
        double dVMinOverview = pView->OverGetVMin();

        double dUMin = m_rcWnd.left / double(rcKey.right) * pView->OverGetUExt() + dUMinOverview;
        double dUMax = m_rcWnd.right / double(rcKey.right) * pView->OverGetUExt() + dUMinOverview;

        double dVMin = (-m_rcWnd.bottom / double(rcKey.bottom + 1)) * pView->OverGetVExt() + dVMinOverview;
        double dVMax = (-m_rcWnd.top / double(rcKey.bottom + 1)) * pView->OverGetVExt() + dVMinOverview;

        double dRatio = pView->GetWidthInInches() / fabs(dUMax - dUMin);
        pView->ModelViewGetActive(pView->m_mvLast);
        pView->ModelViewAdjustWnd(dUMin, dVMin, dUMax, dVMax, dRatio);
        pView->ModelViewSetWnd(dUMin, dVMin, dUMax, dVMax);
    }
    else
    {
        pView->ModelViewGetActive(pView->m_mvLast);
        pView->DoWindowPan(m_dRatio);
    }
}
BEGIN_MESSAGE_MAP(CDlgViewZoom, CDialog)
    ON_BN_CLICKED(IDC_BEST, OnBnClickedBest)
    ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
    ON_BN_CLICKED(IDC_MORELESS, OnBnClickedMoreless)
    ON_BN_CLICKED(IDC_NORMAL, OnBnClickedNormal)
    ON_BN_CLICKED(IDC_OVERVIEW, OnBnClickedOverview)
    ON_BN_CLICKED(IDC_PAN, OnBnClickedPan)
    ON_EN_CHANGE(IDC_RATIO, OnEnChangeRatio)
    ON_BN_CLICKED(IDC_RECALL, OnBnClickedRecall)
    ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
END_MESSAGE_MAP()

// CDlgViewZoom message handlers

void CDlgViewZoom::OnBnClickedBest()
{
    CPegDoc* pDoc = CPegDoc::GetDoc();
    CPegView* pView = CPegView::GetActiveView();

    CPnt ptMin;
    CPnt ptMax;

    pDoc->GetExtents(ptMin, ptMax, pView->ModelViewGetMatrix());

    // extents return range - 1 to 1

    if (ptMin[0] < ptMax[0])
    {
        pView->ModelViewGetActive(pView->m_mvLast);

        double dUExt = pView->ModelViewGetUExt() * (ptMax[0] - ptMin[0]) / 2.;
        double dVExt = pView->ModelViewGetVExt() * (ptMax[1] - ptMin[1]) / 2.;

        pView->ModelViewSetCenteredWnd(dUExt, dVExt);

        CTMat tm;
        pDoc->GetExtents(ptMin, ptMax, tm);

        CPnt ptTarget = CPnt((ptMin[0] + ptMax[0]) / 2., (ptMin[1] + ptMax[1]) / 2., (ptMin[2] + ptMax[2]) / 2.);

        pView->ModelViewSetTarget(ptTarget);
        pView->ModelViewSetEye(pView->ModelViewGetDirection());

        app.CursorPosSet(ptTarget);
    }
    EndDialog(IDOK);
}

void CDlgViewZoom::OnBnClickedLast()
{
    CPegView* pView = CPegView::GetActiveView();

    CModelView vTmp;
    pView->ModelViewGetActive(vTmp);
    pView->ModelViewSetActive(pView->m_mvLast);
    pView->m_mvLast = vTmp;
    EndDialog(IDOK);
}

// Enables/disables the keyplan.  Enabling the keyplan causes the dialog to grow.
// Info indicating size and position are buried in a control.
void CDlgViewZoom::OnBnClickedMoreless()
{
    char szBuf[16]{};
    GetDlgItem(IDC_LESS_AREA)->GetWindowText(szBuf, sizeof(szBuf));

    if (bKeyplan)
    {
        CRect rcDef;

        GetDlgItem(IDC_LESS_AREA)->GetWindowRect(&rcDef);
        SetWindowPos(0, 0, 0, atoi(szBuf), rcDef.Height(), SWP_NOZORDER | SWP_NOMOVE);
    }
    else
    {
        SetWindowPos(0, 0, 0, atoi(szBuf), atoi(szBuf + 6), SWP_NOZORDER | SWP_NOMOVE);
    }
    bKeyplan = !bKeyplan;
    GetDlgItem(IDC_KEYPLAN_AREA)->EnableWindow(bKeyplan);

    Refresh();
}

void CDlgViewZoom::OnBnClickedNormal()
{
    CPegView* pView = CPegView::GetActiveView();

    pView->ModelViewGetActive(pView->m_mvLast);
    pView->DoWindowPan(1.);
    EndDialog(IDOK);
}

void CDlgViewZoom::OnBnClickedOverview()
{
    CPegView* pView = CPegView::GetActiveView();
    pView->ModelViewInitialize();
    //pView->ModelViewGetActive(pView->m_mvOver);
    EndDialog(IDOK);
}

void CDlgViewZoom::OnBnClickedPan()
{
    CPegView* pView = CPegView::GetActiveView();

    pView->ModelViewGetActive(pView->m_mvLast);
    pView->DoWindowPan(m_dRatio);
    EndDialog(IDOK);
}
void CDlgViewZoom::OnBnClickedRecall()
{
    // TODO: Add your control notification handler code here
}
void CDlgViewZoom::OnBnClickedSave()
{
    // TODO: Add your control notification handler code here
}

void CDlgViewZoom::Refresh()
{
    CPegDoc* pDoc = CPegDoc::GetDoc();
    CPegView* pView = CPegView::GetActiveView();
    CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

    char szBuf[16]{};
    GetDlgItem(IDC_LESS_AREA)->GetWindowText(szBuf, sizeof(szBuf));

    if (bKeyplan)
    {
        GetDlgItem(IDC_MORELESS)->SetWindowText("&Less");

        CDC dcMem;
        dcMem.CreateCompatibleDC(NULL);
        if (m_hbmKeyplan == 0)
        {
            CRect rcKey;
            GetDlgItem(IDC_KEYPLAN_AREA)->GetClientRect(&rcKey);
            m_hbmKeyplan = CreateCompatibleBitmap(pDC->GetSafeHdc(), rcKey.right, rcKey.bottom);
        }
        dcMem.SelectObject(CBitmap::FromHandle(m_hbmKeyplan));

        BITMAP bitmap;

        ::GetObject(m_hbmKeyplan, sizeof(BITMAP), (LPSTR)&bitmap);
        dcMem.PatBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, BLACKNESS);

        // Build a view volume which provides an overview bitmap

        pView->ViewportPushActive();
        pView->ViewportSet(bitmap.bmWidth, bitmap.bmHeight);
        pView->SetDeviceWidthInInches(double(dcMem.GetDeviceCaps(HORZSIZE)) / 25.4);
        pView->SetDeviceHeightInInches(double(dcMem.GetDeviceCaps(VERTSIZE)) / 25.4);

        pView->ModelViewPushActive();
        pView->ModelViewInitialize();
        //pView->ModelViewGetActive(pView->m_mvOver);

        CPrim::SpecPolygonStyle() = POLYGON_HOLLOW;
        pDoc->LayersDisplayAll(pView, &dcMem);
        CPrim::SpecPolygonStyle() = -1;

        //::DeleteDC(hDCMem);
        GetDlgItem(IDC_KEYPLAN_AREA)->InvalidateRect(0, TRUE);

        pView->ModelViewPopActive();
        pView->ViewportPopActive();
    }
    else
    {
        GetDlgItem(IDC_MORELESS)->SetWindowText("&More");

        CRect rcLessArea;
        GetDlgItem(IDC_LESS_AREA)->GetWindowRect(&rcLessArea);
        SetWindowPos(0, 0, 0, atoi(szBuf), rcLessArea.Height(), SWP_NOZORDER | SWP_NOMOVE);
    }
}
void CDlgViewZoom::OnEnChangeRatio()
{
    if (GetFocus() == GetDlgItem(IDC_RATIO))
    {
        char szBuf[32]{};

        GetDlgItemText(IDC_RATIO, szBuf, sizeof(szBuf));
        double dRatio = atof(szBuf);

        if (dRatio >= DBL_EPSILON)
            SendDlgItemMessage(IDC_KEYPLAN_AREA, WM_USER_ON_NEW_RATIO, 0, (LPARAM)(LPDWORD)&dRatio);
    }
}
