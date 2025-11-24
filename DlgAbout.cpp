#include "stdafx.h"

#include "PegAEsys.h"

// CDlgAbout dialog used for App About

class CDlgAbout : public CDialog
{
public:
	CDlgAbout();
	CDlgAbout(const CDlgAbout&) = delete; // Disable copy constructor
	CDlgAbout& operator=(const CDlgAbout&) = delete; // Disable assignment operator

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CDlgAbout::CDlgAbout() : CDialog(CDlgAbout::IDD)
{
}

void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CPegApp::OnAppAbout()
{
	CDlgAbout dlg;
	dlg.DoModal();
}
