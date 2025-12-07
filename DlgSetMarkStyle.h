#pragma once

#include "afxdialogex.h"

#include "res\resource.h"

class CDlgSetMarkStyle : public CDialog {
  DECLARE_DYNAMIC(CDlgSetMarkStyle)

 public:
  CDlgSetMarkStyle(CWnd* pParent = nullptr);  // standard constructor
  virtual ~CDlgSetMarkStyle();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_SET_MARK_STYLE };
#endif

  // Full flag value (radio index in low bits plus checkbox flags)

  int m_nMarkStyle = 0;  // current mark style (default 0)

  int m_nMarkRadio = 0;  // radio button selection
  BOOL m_bCheck1 = FALSE;  // corresponds to first 0x32 circle
  BOOL m_bCheck2 = FALSE;  // corresponds to second 0x64 box

  double m_dMarkSize = 0.0;  // mark size: positive=world units, negative=pixels

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

 protected:
  virtual BOOL OnInitDialog();
  virtual void OnOK();

  DECLARE_MESSAGE_MAP()
};
