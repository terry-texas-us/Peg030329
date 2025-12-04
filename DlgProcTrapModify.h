#pragma once

#include <Windows.h>

INT_PTR CALLBACK DlgProcTrapModify(HWND, UINT, WPARAM, LPARAM) noexcept;

const int TM_TEXT_ALL{0};
const int TM_TEXT_FONT{1};
const int TM_TEXT_HEIGHT{2};

void DlgProcTrapModifyPolygons();
void DlgProcTrapModifyDoOK(HWND hwnd);
