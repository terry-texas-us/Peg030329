#include "stdafx.h"

#include "Mainfrm.h"
#include "PegAEsys.h"

int msgConfirm(UINT uiMsgId)
{
	char szMsg[256];

	::LoadString(app.GetInstance(), uiMsgId, szMsg, sizeof(szMsg));
	
	char* context = nullptr;
	char* pMsg = strtok_s(szMsg, "\t", &context);
	char* pCap = strtok_s(0, "\n", &context);
	
	return (MessageBox(0, pMsg, pCap, MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2));
}

int msgConfirm(UINT uiMsgId, const CString& strVal)
{
	char szFmt[256];
	char szMsg[256];

	::LoadString(app.GetInstance(), uiMsgId, szFmt, sizeof(szFmt));
	
	sprintf(szMsg, szFmt, strVal);
	
	char* context = nullptr;
	char* pMsg = strtok_s(szMsg, "\t", &context);
	char* pCap = strtok_s(0, "\n", &context);
	
	return (MessageBox(0, pMsg, pCap, MB_ICONINFORMATION | MB_YESNOCANCEL | MB_DEFBUTTON2));
}
void msgWarning(UINT uiMsgId)
{
	char szMsg[256];

	::LoadString(app.GetInstance(), uiMsgId, szMsg, sizeof(szMsg));
	
	char* context = nullptr;
	char* pMsg = strtok_s(szMsg, "\t", &context);
	char* pCap = strtok_s(0, "\n", &context);

	MessageBox(0, pMsg, pCap, MB_ICONWARNING | MB_OK);
}
void msgWarning(UINT uiMsgId, const CString& strVal)
{
	char szFmt[256];
	char szMsg[256];
	
	::LoadString(app.GetInstance(), uiMsgId, szFmt, sizeof(szFmt));

	sprintf(szMsg, szFmt, strVal);

	char* context = nullptr;
	char* pMsg = strtok_s(szMsg, "\t", &context);
	char* pCap = strtok_s(0, "\n", &context);

	MessageBox(0, pMsg, pCap, MB_ICONWARNING | MB_OK);
}
void msgInformation(const CString& strMes)
{
	char szMsg[256];
	int n = 0;
	for (; n < strMes.GetLength(); n++)
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
		strcpy_s(szMsg, sizeof(szMsg), "PegAEsys");
		::LoadString(app.GetInstance(), app.m_iModeId, szMsg, sizeof(szMsg));
		char* context = nullptr;
		strtok_s(szMsg, "\n", &context);
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