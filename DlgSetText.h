#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>
#include <afxwin.h>

#include "resource.h"

class CDlgSetText: public CDialog
{
    DECLARE_DYNAMIC(CDlgSetText)

public:
    CDlgSetText(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgSetText();

    CDlgSetText(const CDlgSetText&) = delete; // Prevent copying
    CDlgSetText& operator=(const CDlgSetText&) = delete; // Prevent assignment

    // Dialog Data
    enum { IDD = IDD_SET_TEXT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog(void);

public:
    CString m_sText;
    CString m_strTitle;

protected:
    DECLARE_MESSAGE_MAP()
};
