#include "stdafx.h"

#include <afxext.h>
#include <afxstr.h>
#include <afxwin.h>

#include <atlsimpstr.h>
#include <atltypes.h>

#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysView.h"

/// @brief Retrieves the DPI (dots per inch) for a window. The function tries to call GetDpiForWindow from user32.dll if available; if not available or the window handle is invalid, it falls back to the screen DPI obtained from the device context, defaulting to 96 if detection fails.
/// @param hwnd Handle to the window whose DPI should be retrieved. If nullptr or not a valid window, the function falls back to the primary screen DPI.
/// @return The DPI value (horizontal dots per inch) as a UINT for the specified window or screen. Returns 96 if the DPI cannot be determined.
static UINT GetWindowDpi(HWND hwnd) {
  UINT dpi = 96;
  HMODULE user32Module = ::GetModuleHandleW(L"user32.dll");
  if (user32Module != nullptr) {
    typedef UINT(WINAPI * GetDpiForWindow_t)(HWND);
    auto GetDpiForWindow = (GetDpiForWindow_t)::GetProcAddress(user32Module, "GetDpiForWindow");
    if (GetDpiForWindow != nullptr && hwnd != nullptr && ::IsWindow(hwnd)) {
      dpi = GetDpiForWindow(hwnd);
    } else {
      HDC screenContext = ::GetDC(NULL);
      if (screenContext != NULL) {
        dpi = static_cast<UINT>(::GetDeviceCaps(screenContext, LOGPIXELSX));
        ::ReleaseDC(NULL, screenContext);
      }
    }
  }
  return dpi;
}

/// @brief Scales a base width value according to the DPI of a given window.
/// @param hwnd Handle to the window whose DPI is used for scaling.
/// @param baseWidth The base width in pixels at the standard 96 DPI. If <= 0, the function returns 0.
/// @return The width scaled to the window's DPI (using 96 DPI as the baseline), or 0 if baseWidth <= 0 or the computed scaled value is not positive.
static int ScaleWidthForDpi(HWND hwnd, int baseWidth) {
  if (baseWidth <= 0) { return 0; }
  UINT dpi = GetWindowDpi(hwnd);
  int scaled = MulDiv(baseWidth, static_cast<int>(dpi), 96);
  return (scaled > 0) ? scaled : 0;
}

/// @brief Draws a line of text into a pane area of the given view using the provided device context and font.
/// @param context Pointer to the device context (CDC) used for drawing. If nullptr, the function returns immediately.
/// @param activeView Pointer to the view (CPegView) whose client rectangle is used to compute the pane area. If nullptr, the function returns immediately.
/// @param paneIndex Zero-based index of the pane in which to draw the text; used to compute the left and right boundaries for the pane.
/// @param paneText The text to draw (CString).
/// @param fontApp Font (CFont) to select into the device context for drawing the text.
/// @param textColor COLORREF specifying the text color to use while drawing.
static void DrawPaneText(CDC* context, CPegView* activeView, int paneIndex, const CString& paneText, CFont* fontApp,
                         COLORREF textColor) {
  if (context == nullptr || activeView == nullptr) return;

  auto* oldFont = context->SelectObject(fontApp);
  UINT oldTextAlign = context->SetTextAlign(TA_LEFT | TA_TOP);
  COLORREF oldTextColor = context->SetTextColor(textColor);
  COLORREF oldBkColor = context->SetBkColor(~AppGetTextCol());

  TEXTMETRIC metrics;
  context->GetTextMetrics(&metrics);

  CRect clientArea;
  activeView->GetClientRect(&clientArea);

  int maxCharacters = (clientArea.Width() / 10) / metrics.tmAveCharWidth;

  int left = paneIndex * maxCharacters * metrics.tmAveCharWidth;
  int top = clientArea.bottom - metrics.tmHeight;
  int right = (paneIndex + 1) * maxCharacters * metrics.tmAveCharWidth;
  int bottom = clientArea.bottom;
  CRect rc(left, top, right, bottom);

  int paneTextLength = static_cast<int>(paneText.GetLength());
  context->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, paneText, paneTextLength, nullptr);

  context->SetBkColor(oldBkColor);
  context->SetTextColor(oldTextColor);
  context->SetTextAlign(oldTextAlign);
  context->SelectObject(oldFont);
}

/// @brief Updates the application's status/pane bar with mode information loaded from a string resource and optionally draws the mode operation text into the active view. It measures text extents, computes a DPI-scaled pane width, sets pane IDs/text on the main frame, and may render the pane text when view-mode drawing is enabled.
void CPegApp::ModeLineDisplay() {
  if (m_iModeId == 0) { return; }

  m_wOpHighlighted = 0;

  CMainFrame* mainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
  CPegView* activeView = CPegView::GetActiveView();
  CDC* context = (activeView == nullptr) ? nullptr : activeView->GetDC();

  if (context == nullptr) return;

  CString strMode;
  if (!strMode.LoadString(m_hInstance, (UINT)m_iModeId)) { return; }

  CString strModeOp;

  for (int i = 0; i < 10; i++) {
    AfxExtractSubString(strModeOp, strMode, i + 1, '\n');
    int nLen = strModeOp.GetLength();

    CSize size = context->GetTextExtent(strModeOp, nLen);

    // compute base width in pixels and scale by current DPI so panes remain readable on high-DPI displays
    int baseWidth = size.cx - 2 * nLen;
    if (baseWidth < 0) baseWidth = 0;

    HWND hwnd = (mainFrame != nullptr) ? mainFrame->GetSafeHwnd() : nullptr;
    int scaledWidth = ScaleWidthForDpi(hwnd, baseWidth);

    mainFrame->SetPaneInfo(i + 1, static_cast<UINT>(ID_OP0 + i), SBPS_NORMAL, scaledWidth);
    mainFrame->SetPaneText(i + 1, strModeOp);
    if (m_bViewModeInfo && context != nullptr) {
      DrawPaneText(context, activeView, i, strModeOp, m_pFontApp, AppGetTextCol());
    }
  }
}

WORD CPegApp::ModeLineHighlightOp(WORD wOp) {
  ModeLineUnhighlightOp(m_wOpHighlighted);

  m_wOpHighlighted = wOp;

  if (wOp == 0) { return 0; }

  int i = m_wOpHighlighted - ID_OP0;

  CMainFrame* frame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
  frame->SetPaneStyle(i + 1, SBPS_POPOUT);

  if (m_bViewModeInfo) {
    CString paneText = frame->GetPaneText(i + 1);

    auto* activeView = CPegView::GetActiveView();
    CDC* context = (activeView == nullptr) ? nullptr : activeView->GetDC();

    if (context == nullptr) return wOp;

    DrawPaneText(context, activeView, i, paneText, m_pFontApp, RGB(255, 0, 0));
  }
  return (wOp);
}

void CPegApp::ModeLineUnhighlightOp(WORD& wOp) {
  m_wOpHighlighted = 0;

  if (wOp == 0) { return; }

  int i = wOp - ID_OP0;

  CMainFrame* frame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);
  frame->SetPaneStyle(i + 1, SBPS_NORMAL);

  if (m_bViewModeInfo) {
    CString paneText = frame->GetPaneText(i + 1);

    auto* activeView = CPegView::GetActiveView();
    CDC* context = (activeView == nullptr) ? nullptr : activeView->GetDC();

    if (context == nullptr) {
      wOp = 0;
      return;
    }

    DrawPaneText(context, activeView, i, paneText, m_pFontApp, AppGetTextCol());
  }
  wOp = 0;
}