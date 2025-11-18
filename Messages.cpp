#include "stdafx.h"

#include "Mainfrm.h"
#include "PegAEsys.h"

int msgConfirm(UINT uiMsgId)
{
	char szMsg[256];

	::LoadString(app.GetInstance(), uiMsgId, szMsg, sizeof(szMsg));
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	return (MessageBox(0, pMsg, pCap, MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2));
}

int msgConfirm(UINT uiMsgId, const CString& strVal)
{
	char szFmt[256];
	char szMsg[256];

	::LoadString(app.GetInstance(), uiMsgId, szFmt, sizeof(szFmt));
	
	sprintf(szMsg, szFmt, strVal);
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	return (MessageBox(0, pMsg, pCap, MB_ICONINFORMATION | MB_YESNOCANCEL | MB_DEFBUTTON2));
}
void msgWarning(UINT uiMsgId)
{
	char szMsg[256];

	::LoadString(app.GetInstance(), uiMsgId, szMsg, sizeof(szMsg));
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	MessageBox(0, pMsg, pCap, MB_ICONWARNING | MB_OK);
}
void msgWarning(UINT uiMsgId, const CString& strVal)
{
	char szFmt[256];
	char szMsg[256];
	
	::LoadString(app.GetInstance(), uiMsgId, szFmt, sizeof(szFmt));
	
	sprintf(szMsg, szFmt, strVal);
	
	char* pMsg = strtok(szMsg, "\t");
	char* pCap = strtok(0, "\n");
	
	MessageBox(0, pMsg, pCap, MB_ICONWARNING | MB_OK);
}
void msgInformation(const CString& strMes)
{
	char szMsg[256];
	
	for (int n = 0; n < strMes.GetLength(); n++)
	{
		szMsg[n] = isprint(strMes.GetAt(n)) ? strMes.GetAt(n) : '.';
	}
	szMsg[n] = 0;

	CMainFrame* pFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);

	//pFrame->SetWindowText(szMsg);

	pFrame->SetPaneText(0, szMsg);
}

void msgInformation(UINT nId)
{
	char szMsg[256];
	if (nId == 0)
	{
		strcpy(szMsg, "PegAEsys");
		::LoadString(app.GetInstance(), app.m_iModeId, szMsg, sizeof(szMsg));
		strtok(szMsg, "\n");
	}
	else
	{
		::LoadString(app.GetInstance(), nId, szMsg, sizeof(szMsg));
	}
	msgInformation(szMsg);
}
void msgInformation(UINT nId, const CString& strVal)
{
	char szFmt[256];
	char szMsg[256];

	::LoadString(app.GetInstance(), nId, szFmt, sizeof(szFmt));
	
	sprintf(szMsg, szFmt, strVal);
	
	msgInformation(szMsg);
}