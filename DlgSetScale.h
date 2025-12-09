#pragma once

#include <afx.h>
#include <afxwin.h>

#include "res\resource.h"

class CDlgSetScale : public CDialog {
  DECLARE_DYNAMIC(CDlgSetScale)

 public:
  CDlgSetScale(CWnd* pParent = NULL);  // standard constructor
  virtual ~CDlgSetScale();

  CDlgSetScale(const CDlgSetScale&) = delete;             // Prevent copying
  CDlgSetScale& operator=(const CDlgSetScale&) = delete;  // Prevent assignment

  // Dialog Data
  enum { IDD = IDD_SET_SCALE };

 protected:
  virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support

 public:
  double m_dScale;

 protected:
  DECLARE_MESSAGE_MAP()
};
