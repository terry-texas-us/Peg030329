#include "stdafx.h"

#include "PegAEsys.h"

#include "DlgSetScale.h"

// CDlgSetScale dialog

IMPLEMENT_DYNAMIC(CDlgSetScale, CDialog)

CDlgSetScale::CDlgSetScale(CWnd* pParent /*=NULL*/) : 
	CDialog(CDlgSetScale::IDD, pParent), m_dScale(0)
{
}
CDlgSetScale::~CDlgSetScale()
{
}
void CDlgSetScale::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCALE, m_dScale);
	DDV_MinMaxDouble(pDX, m_dScale, .0001, 10000.);
}

BEGIN_MESSAGE_MAP(CDlgSetScale, CDialog)
END_MESSAGE_MAP()
