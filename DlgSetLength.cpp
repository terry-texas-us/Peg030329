#include "stdafx.h"

#include <Windows.h>

#include <afx.h>

#include <afxstr.h>
#include <afxwin.h>

#include "PegAEsys.h"

#include "DlgSetLength.h"
#include "resource.h"

#include "UnitsString.h"

// CDlgSetLength dialog

IMPLEMENT_DYNAMIC(CDlgSetLength, CDialog)

CDlgSetLength::CDlgSetLength(CWnd* pParent /*=NULL*/) :
    CDialog(CDlgSetLength::IDD, pParent)
{ }
CDlgSetLength::~CDlgSetLength()
{ }
void CDlgSetLength::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}
BOOL CDlgSetLength::OnInitDialog()
{
    char szBuf[32]{};

    CDialog::OnInitDialog();
    if (!m_strTitle.IsEmpty())
    {
        SetWindowText(m_strTitle);
    }
    UnitsString_FormatLength(szBuf, sizeof(szBuf), app.GetUnits(), m_dLength);
    SetDlgItemText(IDC_DISTANCE, szBuf);
    return TRUE;
}
void CDlgSetLength::OnOK()
{
    char szBuf[32]{};

    GetDlgItemText(IDC_DISTANCE, (LPSTR)szBuf, 32);
    m_dLength = UnitsString_ParseLength(app.GetUnits(), szBuf);

    CDialog::OnOK();
}
BEGIN_MESSAGE_MAP(CDlgSetLength, CDialog)
END_MESSAGE_MAP()