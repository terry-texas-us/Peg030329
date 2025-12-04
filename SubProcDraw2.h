#pragma once

#include <Windows.h>

/// @brief Window procedure that handles drawing-related messages and commands for the SubProcDraw2 drawing mode. It processes Windows messages (for example WM_COMMAND) to manage selection, line/segment creation and modification, rubber-banding, view updates, and related state.
/// @param hwnd Handle to the window that receives the message.
/// @param anMsg The message identifier (for example WM_COMMAND, WM_PAINT). Determines which action the procedure should perform.
/// @param wParam Message-specific additional information. For WM_COMMAND this typically contains the control or command identifier in the low-order word and notification in the high-order word.
/// @param lParam Message-specific additional information; often a pointer or handle (for example control handle for WM_COMMAND) or coordinates for input messages.
/// @return An LRESULT value whose meaning depends on the processed message. Returns a message-specific result according to the Windows API convention (often zero when the message is handled).
LRESULT CALLBACK SubProcDraw2(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept;
