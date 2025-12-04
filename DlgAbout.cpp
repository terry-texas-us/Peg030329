#include "stdafx.h"

#include "PegAEsys.h"

// CDlgAbout dialog used for App About

class CDlgAbout: public CDialog
{
public:
    CDlgAbout();
    CDlgAbout(const CDlgAbout&) = delete; // Disable copy constructor
    CDlgAbout& operator=(const CDlgAbout&) = delete; // Disable assignment operator

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

    // Implementation
};

CDlgAbout::CDlgAbout() : CDialog{IDD}
{ }

void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

// App command to run the dialog
void CPegApp::OnAppAbout()
{
    auto dlg = CDlgAbout();
    dlg.DoModal();
}
