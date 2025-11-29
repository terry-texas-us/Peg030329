#include "stdafx.h"

#include "DlgViewZoom.h"

#include "PegAEsysView.h"
#include "SafeMath.h"

CPoint	pnt;
bool	bPtInRect = FALSE;

void WndProcKeyPlanOnDraw(HWND);
void WndProcKeyPlanOnMouseMove(HWND, WPARAM, LPARAM);
void WndProcKeyPlanOnNewRatio(HWND, LPARAM);

LRESULT CALLBACK WndProcKeyPlan(HWND hwnd, UINT nMsg, WPARAM nParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_USER_ON_NEW_RATIO:
		WndProcKeyPlanOnNewRatio(hwnd, lParam);
		break;

	case WM_PAINT:
		WndProcKeyPlanOnDraw(hwnd);
		return (FALSE);

	case WM_LBUTTONDOWN:
		SetFocus(hwnd);
		pnt.x = LOWORD(lParam); pnt.y = HIWORD(lParam);
		bPtInRect = ::PtInRect(&CDlgViewZoom::m_rcWnd, pnt) != 0;
		return (FALSE);

	case WM_MOUSEMOVE:
		WndProcKeyPlanOnMouseMove(hwnd, nParam, lParam);
		return (FALSE);
	}
	return DefWindowProc(hwnd, nMsg, nParam, lParam);
}

void WndProcKeyPlanOnDraw(HWND hwnd)
{
	PAINTSTRUCT ps;

	CDC dc;
	dc.Attach(::BeginPaint(hwnd, &ps));

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);

	dcMem.SelectObject(CBitmap::FromHandle(CDlgViewZoom::m_hbmKeyplan));
	BITMAP bitmap;
	::GetObject(CDlgViewZoom::m_hbmKeyplan, sizeof(BITMAP), (LPSTR)&bitmap);

	dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcMem, 0, 0, SRCCOPY);

	CBrush* pBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(0, 0, bitmap.bmWidth, bitmap.bmHeight);

#pragma tasMSG(TODO: Need to use the CWnd associated with Keyplan and not the active app view)

	CPegView* pView = CPegView::GetActiveView();

	CPnt ptTarget = pView->ModelViewGetTarget();

	double dUMin = ptTarget[0] + pView->ModelViewGetUMin();
	double dUMax = ptTarget[0] + pView->ModelViewGetUMax();
	double dVMin = ptTarget[1] + pView->ModelViewGetVMin();
	double dVMax = ptTarget[1] + pView->ModelViewGetVMax();
	double dUMinOverview = ptTarget[0] + pView->OverGetUMin();
	double dVMinOverview = ptTarget[1] + pView->OverGetVMin();

	CRect rc;
	rc.left = Round((dUMin - dUMinOverview) / pView->OverGetUExt() * bitmap.bmWidth);
	rc.right = Round((dUMax - dUMinOverview) / pView->OverGetUExt() * bitmap.bmWidth);
	rc.top = Round((1. - (dVMax - dVMinOverview) / pView->OverGetVExt()) * bitmap.bmHeight);
	rc.bottom = Round((1. - (dVMin - dVMinOverview) / pView->OverGetVExt()) * bitmap.bmHeight);

	int iDrawMode = dc.SetROP2(R2_XORPEN);

	// Show current window as light gray rectangle with no outline
	dc.SelectStockObject(LTGRAY_BRUSH);
	CPen* pPen = (CPen*)dc.SelectStockObject(NULL_PEN);
	dc.Rectangle(rc.left, rc.top, rc.right, rc.bottom);

	// Show defining window as hollow rectangle with dark gray outline
	dc.SelectStockObject(NULL_BRUSH);
	CPen penGray(PS_SOLID, 2, RGB(0x80, 0x80, 0x80));
	dc.SelectObject(&penGray);
	dc.Rectangle(CDlgViewZoom::m_rcWnd.left, CDlgViewZoom::m_rcWnd.top, CDlgViewZoom::m_rcWnd.right, CDlgViewZoom::m_rcWnd.bottom);

	// Restore device context
	dc.SelectObject(pPen);
	dc.SelectObject(pBrush);
	dc.SetROP2(iDrawMode);

	dc.Detach();

	::EndPaint(hwnd, &ps);
}

void WndProcKeyPlanOnMouseMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == MK_LBUTTON)
	{
		CPoint pntCur;

		pntCur.x = LOWORD(lParam);
		pntCur.y = HIWORD(lParam);

		CDC dcKeyPlan;
		HDC hDCKeyplan = ::GetDC(hwnd);
		int iDrawMode = ::SetROP2(hDCKeyplan, R2_XORPEN);

		// Show defining window as hollow rectangle with dark gray outline
		HBRUSH hBrush = (HBRUSH) ::SelectObject(hDCKeyplan, ::GetStockObject(NULL_BRUSH));
		HPEN hPenGray = ::CreatePen(PS_SOLID, 2, RGB(0x80, 0x80, 0x80));
		HPEN hPen = (HPEN) ::SelectObject(hDCKeyplan, hPenGray);
		::Rectangle(hDCKeyplan, CDlgViewZoom::m_rcWnd.left, CDlgViewZoom::m_rcWnd.top, CDlgViewZoom::m_rcWnd.right, CDlgViewZoom::m_rcWnd.bottom);

		if (bPtInRect)
			::OffsetRect(&CDlgViewZoom::m_rcWnd, (pntCur.x - pnt.x), (pntCur.y - pnt.y));
		else
		{
			if (pntCur.x > CDlgViewZoom::m_rcWnd.right)
				CDlgViewZoom::m_rcWnd.right += (pntCur.x - pnt.x);
			else if (pntCur.x < CDlgViewZoom::m_rcWnd.left)
				CDlgViewZoom::m_rcWnd.left += (pntCur.x - pnt.x);
			if (pntCur.y > CDlgViewZoom::m_rcWnd.bottom)
				CDlgViewZoom::m_rcWnd.bottom += (pntCur.y - pnt.y);
			else if (pntCur.y < CDlgViewZoom::m_rcWnd.top)
				CDlgViewZoom::m_rcWnd.top += (pntCur.y - pnt.y);
			::SendMessage(::GetParent(hwnd), WM_COMMAND, (WPARAM) ::GetWindowLong(hwnd, GWL_ID), (LPARAM)hwnd);
		}
		pnt = pntCur;
		::Rectangle(hDCKeyplan, CDlgViewZoom::m_rcWnd.left, CDlgViewZoom::m_rcWnd.top, CDlgViewZoom::m_rcWnd.right, CDlgViewZoom::m_rcWnd.bottom);

		::SelectObject(hDCKeyplan, hPen);
		::SelectObject(hDCKeyplan, hBrush);
		::DeleteObject(hPenGray);
		::SetROP2(hDCKeyplan, iDrawMode);
		::ReleaseDC(hwnd, hDCKeyplan);
	}
}

void WndProcKeyPlanOnNewRatio(HWND hwnd, LPARAM lParam)
{
	double dRatio = *(double*)(LPDWORD)lParam;

	CPegView* pView = CPegView::GetActiveView();

	CPnt ptTarget = pView->ModelViewGetTarget();

	double dUExt = pView->GetWidthInInches() / dRatio;
	double dUMin = ptTarget[0] - (dUExt * .5);
	double dUMax = dUMin + dUExt;
	double dVExt = pView->GetHeightInInches() / dRatio;
	double dVMin = ptTarget[1] - (dVExt * .5);
	double dVMax = dVMin + dVExt;

	HDC hDCKeyplan = ::GetDC(hwnd);
	int iDrawMode = ::SetROP2(hDCKeyplan, R2_XORPEN);

	HBRUSH hBrush = (HBRUSH) ::SelectObject(hDCKeyplan, ::GetStockObject(NULL_BRUSH));
	HPEN hPenGray = ::CreatePen(PS_SOLID, 2, RGB(0x80, 0x80, 0x80));
	HPEN hPen = (HPEN) ::SelectObject(hDCKeyplan, hPenGray);
	::Rectangle(hDCKeyplan, CDlgViewZoom::m_rcWnd.left, CDlgViewZoom::m_rcWnd.top, CDlgViewZoom::m_rcWnd.right, CDlgViewZoom::m_rcWnd.bottom);

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);

	dcMem.SelectObject(CBitmap::FromHandle(CDlgViewZoom::m_hbmKeyplan));
	BITMAP bitmap; ::GetObject(CDlgViewZoom::m_hbmKeyplan, sizeof(BITMAP), (LPSTR)&bitmap);

	double dUMinOverview = ptTarget[0] + pView->OverGetUMin();
	double dVMinOverview = ptTarget[1] + pView->OverGetVMin();

	CDlgViewZoom::m_rcWnd.left = Round((dUMin - dUMinOverview) / pView->OverGetUExt() * bitmap.bmWidth);
	CDlgViewZoom::m_rcWnd.top = Round((1. - (dVMax - dVMinOverview) / pView->OverGetVExt()) * bitmap.bmHeight);
	CDlgViewZoom::m_rcWnd.right = Round((dUMax - dUMinOverview) / pView->OverGetUExt() * bitmap.bmWidth);
	CDlgViewZoom::m_rcWnd.bottom = Round((1. - (dVMin - dVMinOverview) / pView->OverGetVExt()) * bitmap.bmHeight);

	::Rectangle(hDCKeyplan, CDlgViewZoom::m_rcWnd.left, CDlgViewZoom::m_rcWnd.top, CDlgViewZoom::m_rcWnd.right, CDlgViewZoom::m_rcWnd.bottom);
	::SelectObject(hDCKeyplan, hPen);
	::SelectObject(hDCKeyplan, hBrush);
	::DeleteObject(hPenGray);

	::SetROP2(hDCKeyplan, iDrawMode);
	::ReleaseDC(hwnd, hDCKeyplan);
}
