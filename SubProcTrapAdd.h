#pragma once

#include <Windows.h>

#include <afxwin.h>

#include "Pnt.h"

class CPegView;
class CSegs;

LRESULT CALLBACK SubProcTrapAdd(HWND, UINT, WPARAM, LPARAM);

namespace trap
{
    void AddLast();
    void AddByArea(CPegView* pView, CDC* pDC, CSegs*, CPnt, CPnt);
    void AddByLn(CPegView* pView, CDC* pDC, CSegs*, CPnt, CPnt);
    void PopupCommands(HWND);
}
