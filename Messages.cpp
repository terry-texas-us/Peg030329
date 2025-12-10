#include "stdafx.h"

#include <afxstr.h>
#include <afxwin.h>

#include <cctype>
#include <string>

#include "Mainfrm.h"
#include "PegAEsys.h"

/// @brief Loads a string resource from the string table and returns it as a std::string.
/// @param resourceIdentifier The resource identifier of the string to load.
/// @return A std::string containing the loaded resource text. The returned string may be empty if loading fails, and may be truncated if the resource exceeds the internal 256-byte buffer.
static std::string msgLoadStringResource(UINT resourceIdentifier) {
  TCHAR resourceString[256]{};
  ::LoadString(app.GetInstance(), resourceIdentifier, resourceString, sizeof(resourceString));
  return {resourceString};
}

int msgConfirm(UINT messageIdentifier, const CString& additionalText) {
  std::string confirm = msgLoadStringResource(messageIdentifier);

  size_t pos = confirm.find(_T("%s"));
  if (pos != std::string::npos) { confirm.replace(pos, 2, additionalText.GetString()); }
  size_t tabPos = confirm.find(_T("\t"));
  std::string message = (tabPos != std::string::npos) ? confirm.substr(0, tabPos) : confirm;
  std::string caption = (tabPos != std::string::npos) ? confirm.substr(tabPos + 1) : _T("");

  return (MessageBox(0, message.c_str(), caption.c_str(), MB_ICONINFORMATION | MB_YESNOCANCEL | MB_DEFBUTTON2));
}

void msgWarning(UINT messageIdentifier) {
  std::string warning = msgLoadStringResource(messageIdentifier);

  size_t tabPos = warning.find(_T("\t"));
  std::string message = (tabPos != std::string::npos) ? warning.substr(0, tabPos) : warning;
  std::string caption = (tabPos != std::string::npos) ? warning.substr(tabPos + 1) : _T("");

  MessageBox(0, message.c_str(), caption.c_str(), MB_ICONWARNING | MB_OK);
}

void msgWarning(UINT messageIdentifier, const CString& additionalText) {
  std::string warning = msgLoadStringResource(messageIdentifier);

  size_t pos = warning.find(_T("%s"));
  if (pos != std::string::npos) { warning.replace(pos, 2, additionalText.GetString()); }
  size_t tabPos = warning.find(_T("\t"));
  std::string message = (tabPos != std::string::npos) ? warning.substr(0, tabPos) : warning;
  std::string caption = (tabPos != std::string::npos) ? warning.substr(tabPos + 1) : _T("");

  MessageBox(0, message.c_str(), caption.c_str(), MB_ICONWARNING | MB_OK);
}

void msgInformation(const CString& strMes) {
  TCHAR information[256]{};
  int n = 0;
  for (; n < strMes.GetLength(); n++) { information[n] = isprint(strMes.GetAt(n)) ? strMes.GetAt(n) : '.'; }
  information[n] = 0;

  CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);

  pFrame->SetPaneText(0, information);
}

void msgSetPaneText(const std::string& paneText) {
  TCHAR paneTextBuffer[256]{};
  size_t n = 0;
  for (; n < paneText.length(); n++) { paneTextBuffer[n] = isprint(paneText[n]) ? paneText[n] : '.'; }
  paneTextBuffer[n] = 0;

  CMainFrame* mainFrame = (CMainFrame*)(AfxGetApp()->m_pMainWnd);

  mainFrame->SetPaneText(0, paneTextBuffer);
}

void msgInformation(UINT messageIdentifier) {
  std::string information;

  if (messageIdentifier == 0) {  // Use current mode indentifier.
    information = msgLoadStringResource(static_cast<UINT>(app.m_iModeId));

    size_t pos = information.find(_T("\n"));
    if (pos != std::string::npos) {  // Truncate at the newline, keeping only the mode name.
      information = information.substr(0, pos);
    }
  } else {
    information = msgLoadStringResource(messageIdentifier);
  }
  msgSetPaneText(information);
}

void msgInformation(UINT messageIdentifier, const CString& additionalText) {
  std::string information = msgLoadStringResource(messageIdentifier);
  size_t pos = information.find(_T("%s"));
  if (pos != std::string::npos) { information.replace(pos, 2, additionalText.GetString()); }
  msgSetPaneText(information);
}
