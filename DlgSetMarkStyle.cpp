#include "stdafx.h"

#include "afxdialogex.h"

#include "DlgSetMarkStyle.h"

IMPLEMENT_DYNAMIC(CDlgSetMarkStyle, CDialog)

CDlgSetMarkStyle::CDlgSetMarkStyle(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_SET_MARK_STYLE, pParent),
      m_nMarkStyle(0),
      m_nMarkRadio(0),
      m_bCheck1(FALSE),
      m_bCheck2(FALSE),
      m_dMarkSize(0.0) {}

CDlgSetMarkStyle::~CDlgSetMarkStyle() {}

void CDlgSetMarkStyle::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);

  // When loading (pDX->m_bSaveAndValidate == FALSE) break out the parts of the
  // stored flag into the UI-backed variables before calling DDX_ helpers.
  if (!pDX->m_bSaveAndValidate) {
    m_nMarkRadio = m_nMarkStyle & 0x0F;      // radio index stored in low bits
    m_bCheck1 = (m_nMarkStyle & 0x20) != 0;  // 0x20 -> check1
    m_bCheck2 = (m_nMarkStyle & 0x40) != 0;  // 0x40 -> check2
  }
  // Transfer radio and checkbox states between controls and UI vars
  DDX_Radio(pDX, IDC_RADIO_MARK_STYLE_0, m_nMarkRadio);
  DDX_Check(pDX, IDC_CHECK1, m_bCheck1);
  DDX_Check(pDX, IDC_CHECK2, m_bCheck2);

  // new: DDX for mark size numeric edit control (assumes IDC_EDIT1)
  DDX_Text(pDX, IDC_EDIT1, m_dMarkSize);
  if (pDX->m_bSaveAndValidate) {
    DDV_MinMaxDouble(pDX, m_dMarkSize, -10000.0, 10000.0);  // arbitrary sane range
  }

  // When saving, combine UI-backed values back into the single flags integer.
  if (pDX->m_bSaveAndValidate) {
    m_nMarkStyle = (m_nMarkRadio & 0x0F) | (m_bCheck1 ? 0x20 : 0) | (m_bCheck2 ? 0x40 : 0);
  }
}
BOOL CDlgSetMarkStyle::OnInitDialog() {
  CDialog::OnInitDialog();

  // initialize controls based on m_nMarkStyle if needed
  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgSetMarkStyle::OnOK() {
  // Fetch values from controls
  if (!UpdateData(TRUE)) {
    // failed to get data from controls
    return;
  }

  // Validate: for example, accept only 0..4 (five radio styles).
  const int maxStyle = 4;
  int radioOnly = m_nMarkStyle & 0x0F;
  if (radioOnly < 0 || radioOnly > maxStyle) {
    AfxMessageBox(_T("Please select a valid mark style."), MB_ICONWARNING);
    return;  // don't close dialog
  }

  // If validation passes, proceed with default OK behavior
  CDialog::OnOK();
}
BEGIN_MESSAGE_MAP(CDlgSetMarkStyle, CDialog)
END_MESSAGE_MAP()
