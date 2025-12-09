#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxext.h>
#include <afxstr.h>
#include <afxwin.h>

class CMainFrame : public CMDIFrameWnd {
  DECLARE_DYNAMIC(CMainFrame)
 public:
  CMainFrame();

  // Explicitly delete copy constructor and assignment operator to fix C4625 and C4626 warnings
  CMainFrame(const CMainFrame&) = delete;
  CMainFrame& operator=(const CMainFrame&) = delete;

  // Attributes
 public:
  // Operations
 public:
  CString GetPaneText(int nIndex) { return m_wndStatusBar.GetPaneText(nIndex); }
  void SetPaneInfo(int nIndex, UINT nId, UINT nStyle, int cxWidth) {
    m_wndStatusBar.SetPaneInfo(nIndex, nId, nStyle, cxWidth);
  }
  BOOL SetPaneText(int nIndex, LPCSTR lpszNewText) { return m_wndStatusBar.SetPaneText(nIndex, lpszNewText); }
  void SetPaneStyle(int nIndex, UINT nStyle) { m_wndStatusBar.SetPaneStyle(nIndex, nStyle); }

  // Overrides
 public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  // Implementation
 public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
 protected:  // control bar embedded members
  CStatusBar m_wndStatusBar;
  CToolBar m_wndToolBar;

  // Generated message map functions
 protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();

 protected:
  DECLARE_MESSAGE_MAP()
};
