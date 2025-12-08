#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxwin.h> // for CMDIChildWnd

class CChildFrame: public CMDIChildWnd
{
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
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    // Implementation
public:
    virtual ~CChildFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void ActivateFrame(int nCmdShow = -1);

    // Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
};
