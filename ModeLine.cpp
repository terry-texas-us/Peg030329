#include "stdafx.h"

#include <afxext.h>

#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysView.h"

void CPegApp::ModeLineDisplay()
{
	if (m_iModeId == 0)
		return;

	m_wOpHighlighted = 0;

	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	CString strMode;
	strMode.LoadString(m_hInstance, (UINT) m_iModeId);

	CString strModeOp;
	
	for (int i = 0; i < 10; i++)
	{
		AfxExtractSubString(strModeOp, strMode, i + 1, '\n');
		UINT nLen = strModeOp.GetLength();

		CSize size = pDC->GetTextExtent(strModeOp, nLen);

		pFrame->SetPaneInfo(i + 1, ID_OP0 + i, SBPS_NORMAL, size.cx - 2 * nLen);
		pFrame->SetPaneText(i + 1, strModeOp);
		if (m_bViewModeInfo)	
		{
			CFont* pFont = pDC->SelectObject(m_pFontApp);
			UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
			COLORREF crText = pDC->SetTextColor(AppGetTextCol()); 
			COLORREF crBk = pDC->SetBkColor(~AppGetTextCol());

			TEXTMETRIC tm;		 
			pDC->GetTextMetrics(&tm);
	
			CRect rcClient;
			pView->GetClientRect(&rcClient);
		
			int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;
			
			CRect rc(i * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom - tm.tmHeight, (i + 1) * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom);
			pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, nLen, 0);
		
			pDC->SetBkColor(crBk);
			pDC->SetTextColor(crText);
			pDC->SetTextAlign(nTextAlign);
			pDC->SelectObject(pFont);
		}
	}
}

WORD CPegApp::ModeLineHighlightOp(WORD wOp)
{
	ModeLineUnhighlightOp(m_wOpHighlighted);
		
	m_wOpHighlighted = wOp;

	if (wOp == 0)
		return 0;

	int i = m_wOpHighlighted - ID_OP0;

	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);
	pFrame->SetPaneStyle(i + 1, SBPS_POPOUT);

	if (m_bViewModeInfo)	
	{
		CString strModeOp = pFrame->GetPaneText(i + 1);
			
		CPegView* pView = CPegView::GetActiveView();
		CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
		
		CFont* pFont = pDC->SelectObject(m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(RGB(255, 0, 0));
		COLORREF crBk = pDC->SetBkColor(~AppGetTextCol());

		TEXTMETRIC tm;		 
		pDC->GetTextMetrics(&tm);
	
		CRect rcClient;
		pView->GetClientRect(&rcClient);
		
		int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;
		CRect rc(i * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom - tm.tmHeight, (i + 1) * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom);

		pDC->ExtTextOut(rc.left , rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, (UINT) strModeOp.GetLength(), 0);
		
		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);
	}												  
	return (wOp) ;  
}

void CPegApp::ModeLineUnhighlightOp(WORD& wOp)
{
	m_wOpHighlighted = 0;

	if (wOp == 0)
		return;

	int i = wOp - ID_OP0;

	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);
	pFrame->SetPaneStyle(i + 1, SBPS_NORMAL);

	if (m_bViewModeInfo)	
	{
		CString strModeOp = pFrame->GetPaneText(i + 1);

		CPegView* pView = CPegView::GetActiveView();
		CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();
	
		CFont* pFont = pDC->SelectObject(m_pFontApp);
		UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
		COLORREF crText = pDC->SetTextColor(AppGetTextCol());
		COLORREF crBk = pDC->SetBkColor(~AppGetTextCol());

		TEXTMETRIC tm;		 
		pDC->GetTextMetrics(&tm);
	
		CRect rcClient;
		pView->GetClientRect(&rcClient);
		
		int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;
		CRect rc(i * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom - tm.tmHeight, (i + 1) * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom);

		pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, (UINT) strModeOp.GetLength(), 0);
		
		pDC->SetBkColor(crBk);
		pDC->SetTextColor(crText);
		pDC->SetTextAlign(nTextAlign);
		pDC->SelectObject(pFont);
	}
	wOp = 0;
}