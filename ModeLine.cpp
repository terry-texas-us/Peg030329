#include "stdafx.h"

#include <Windows.h>

#include <afxext.h>
#include <afxstr.h>
#include <afxwin.h>

#include <atlsimpstr.h>
#include <atltypes.h>

#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysView.h"

#include "resource.h"

void CPegApp::ModeLineDisplay()
{
    if (m_iModeId == 0) { return; }

    m_wOpHighlighted = 0;

    CMainFrame* mainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    CPegView* activeView = CPegView::GetActiveView();
    CDC* context = (activeView == nullptr) ? nullptr : activeView->GetDC();

    if (context == nullptr) return;

    CString strMode;
    if (!strMode.LoadString(m_hInstance, (UINT)m_iModeId)) { return; }

    CString strModeOp;

    for (int i = 0; i < 10; i++)
    {
        AfxExtractSubString(strModeOp, strMode, i + 1, '\n');
        UINT nLen = strModeOp.GetLength();

        CSize size = context->GetTextExtent(strModeOp, nLen);

        mainFrame->SetPaneInfo(i + 1, ID_OP0 + i, SBPS_NORMAL, size.cx - 2 * nLen);
        mainFrame->SetPaneText(i + 1, strModeOp);
        if (m_bViewModeInfo && context != nullptr)
        {
            CFont* pFont = context->SelectObject(m_pFontApp);
            UINT nTextAlign = context->SetTextAlign(TA_LEFT | TA_TOP);
            COLORREF crText = context->SetTextColor(AppGetTextCol());
            COLORREF crBk = context->SetBkColor(~AppGetTextCol());

            TEXTMETRIC tm;
            context->GetTextMetrics(&tm);

            CRect rcClient;
            activeView->GetClientRect(&rcClient);

            int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;

            CRect rc(i * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom - tm.tmHeight, (i + 1) * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom);
            context->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, nLen, 0);

            context->SetBkColor(crBk);
            context->SetTextColor(crText);
            context->SetTextAlign(nTextAlign);
            context->SelectObject(pFont);
        }
    }
}

WORD CPegApp::ModeLineHighlightOp(WORD wOp)
{
    ModeLineUnhighlightOp(m_wOpHighlighted);

    m_wOpHighlighted = wOp;

    if (wOp == 0) { return 0; }

    int i = m_wOpHighlighted - ID_OP0;

    CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    pFrame->SetPaneStyle(i + 1, SBPS_POPOUT);

    if (m_bViewModeInfo)
    {
        CString strModeOp = pFrame->GetPaneText(i + 1);

        CPegView* activeView = CPegView::GetActiveView();
        CDC* context = (activeView == nullptr) ? nullptr : activeView->GetDC();

        if (context == nullptr) return wOp;

        CFont* pFont = context->SelectObject(m_pFontApp);
        UINT nTextAlign = context->SetTextAlign(TA_LEFT | TA_TOP);
        COLORREF crText = context->SetTextColor(RGB(255, 0, 0));
        COLORREF crBk = context->SetBkColor(~AppGetTextCol());

        TEXTMETRIC tm;
        context->GetTextMetrics(&tm);

        CRect rcClient;
        activeView->GetClientRect(&rcClient);

        int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;
        CRect rc(i * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom - tm.tmHeight, (i + 1) * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom);

        context->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, (UINT)strModeOp.GetLength(), 0);

        context->SetBkColor(crBk);
        context->SetTextColor(crText);
        context->SetTextAlign(nTextAlign);
        context->SelectObject(pFont);
    }
    return (wOp);
}

void CPegApp::ModeLineUnhighlightOp(WORD& wOp)
{
    m_wOpHighlighted = 0;

    if (wOp == 0) { return; }

    int i = wOp - ID_OP0;

    CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
    pFrame->SetPaneStyle(i + 1, SBPS_NORMAL);

    if (m_bViewModeInfo)
    {
        CString strModeOp = pFrame->GetPaneText(i + 1);

        CPegView* activeView = CPegView::GetActiveView();
        CDC* context = (activeView == nullptr) ? nullptr : activeView->GetDC();

        if (context == nullptr) { wOp = 0; return; }

        CFont* pFont = context->SelectObject(m_pFontApp);
        UINT nTextAlign = context->SetTextAlign(TA_LEFT | TA_TOP);
        COLORREF crText = context->SetTextColor(AppGetTextCol());
        COLORREF crBk = context->SetBkColor(~AppGetTextCol());

        TEXTMETRIC tm;
        context->GetTextMetrics(&tm);

        CRect rcClient;
        activeView->GetClientRect(&rcClient);

        int iMaxChrs = (rcClient.Width() / 10) / tm.tmAveCharWidth;
        CRect rc(i * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom - tm.tmHeight, (i + 1) * iMaxChrs * tm.tmAveCharWidth, rcClient.bottom);

        context->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, strModeOp, (UINT)strModeOp.GetLength(), 0);

        context->SetBkColor(crBk);
        context->SetTextColor(crText);
        context->SetTextAlign(nTextAlign);
        context->SelectObject(pFont);
    }
    wOp = 0;
}