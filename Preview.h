#pragma once

#include <Windows.h>

class CBlock;
class CSegs;

void WndProcPreviewClear(HWND hwnd);
void WndProcPreviewUpdate(HWND hwnd, CBlock* pBlock);
void WndProcPreviewUpdate(HWND hwnd, CSegs* pSegs);
void WndProcPreviewOnCreate(HWND hwnd);
void WndProcPreviewOnDestroy(HWND hwnd);
void WndProcPreviewOnDraw(HWND hwnd);
