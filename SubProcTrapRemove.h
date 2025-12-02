#pragma once

#include <Windows.h>

/// @brief Remove segments from trap
/// @note Convex polygon trap (operation 3) not implemented yet. Area trap (operation 4) needs to be generalized to quad. Making trap invisible marks segments for potential deletion elsewhere.
LRESULT CALLBACK SubProcTrapRemove(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept;
