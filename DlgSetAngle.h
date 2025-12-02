#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>
#include <afxwin.h>

#include "resource.h"

class CDlgSetAngle: public CDialog
{
    DECLARE_DYNAMIC(CDlgSetAngle)

public:
    CDlgSetAngle(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDlgSetAngle();

    CDlgSetAngle(const CDlgSetAngle&) = delete; // Prevent copying
    CDlgSetAngle& operator=(const CDlgSetAngle&) = delete; // Prevent assignment

    // Dialog Data
    enum { IDD = IDD_SET_ANGLE };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
public:
    double m_dAngle;
    CString m_strTitle;

protected:
    DECLARE_MESSAGE_MAP()
};
