#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxwin.h>  // for CMDIChildWnd

class CChildFrame : public CMDIChildWnd {
  DECLARE_DYNCREATE(CChildFrame)
 public:
  CChildFrame();

  // Explicitly delete copy constructor and assignment operator to fix C4625 and C4626 warnings
  CChildFrame(const CChildFrame&) = delete;
  CChildFrame& operator=(const CChildFrame&) = delete;

  // Attributes
 public:
  // Operations
 public:
  // Overrides
  BOOL PreCreateWindow(CREATESTRUCT& cs) override;

  // Implementation
 public:
  ~CChildFrame() override;
#ifdef _DEBUG
  void AssertValid() const override;
  void Dump(CDumpContext& dc) const override;
#endif
  void ActivateFrame(int nCmdShow = -1) override;

  // Generated message map functions
 protected:
  DECLARE_MESSAGE_MAP()
};
