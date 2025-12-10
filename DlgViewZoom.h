#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxwin.h>

#include <atltypes.h>

#include "res\resource.h"

class CDataExchange;

// CDlgViewZoom dialog

// User defined message sent by CDlgViewZoom to WndProcKeyPlan when a new
// zoom ratio is set using an edit control
//		LPARAM is the new value of ratio
#define WM_USER_ON_NEW_RATIO WM_USER + 1

class CDlgViewZoom : public CDialog {
  DECLARE_DYNAMIC(CDlgViewZoom)

 public:
  CDlgViewZoom(CWnd* pParent = nullptr);  // standard constructor

  CDlgViewZoom(const CDlgViewZoom&) = delete;
  CDlgViewZoom& operator=(const CDlgViewZoom&) = delete;

  virtual ~CDlgViewZoom();

  // Dialog Data
  enum { IDD = IDD_VIEW_ZOOM };

  static HBITMAP m_hbmKeyplan;
  static CRect m_rcWnd;

  double m_dRatio;

 protected:
  static bool bKeyplan;

  void DoDataExchange(CDataExchange* pDX) override;  // DDX/DDV support
  BOOL OnInitDialog() override;
  void OnOK() override;

  void Refresh();

 public:
  afx_msg void OnBnClickedBest();
  afx_msg void OnBnClickedLast();
  afx_msg void OnBnClickedMoreless();
  afx_msg void OnBnClickedNormal();
  afx_msg void OnBnClickedOverview();
  afx_msg void OnBnClickedPan();
  afx_msg void OnBnClickedRecall();
  afx_msg void OnBnClickedSave();
  afx_msg void OnEnChangeRatio();

 protected:
  DECLARE_MESSAGE_MAP()
};
