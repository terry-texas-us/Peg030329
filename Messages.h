#pragma once

#include "afxwin.h" // for CString and UINT
#include <string> // for std::string

/// @brief Displays a confirmation message dialog box with Yes/No buttons, using a string resource for the message and caption. 
/// @param messageIdentifier Identifier of the message template or resource to display in the confirmation dialog.
/// @param additionalText text to include in the message (e.g., inserted into the template) or additional text shown in the dialog.
/// @return An integer result code indicating the user's response (for example a dialog result such as yes/no or other response code)
int msgConfirm(UINT messageIdentifier, const CString& additionalText);

void msgInformation(const CString& strMes);

/// @brief Loads a string message from resources and displays it as informational text on the status bar. Special handling is done if messageIdentifier is 0.
/// @param messageIdentifier If messageIdentifier is 0, it uses a special mode-based identifier (app.m_iModeId) and truncates the message at the first "\n". Otherwise, it uses the provided identifier directly.
void msgInformation(UINT messageIdentifier = 0);

/// @brief Displays an informational message identified by the specified identifier, optionally including additional text.
/// @param messageIdentifier Identifier of the message to display (e.g., a resource ID).
/// @param additionalText Additional text to include in the message; appended or formatted into the displayed message.
void msgInformation(UINT messageIdentifier, const CString& additionalText);

void msgSetPaneText(const std::string& paneText);

/// @brief Displays a warning message identified by the specified message identifier.
/// @param messageIdentifier Identifier of the message to display (e.g., a resource ID).
void msgWarning(UINT messageIdentifier);

void msgWarning(UINT messageIdentifier, const CString& additionalText);
