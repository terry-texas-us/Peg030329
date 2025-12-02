#pragma once

#include <afxwin.h>

#include "Pnt.h"

class CPegView;
class CSegs;

/// @brief Add segments to trap
/// @note Convex polygon trap (operation 3) not implemented yet. Area trap (operation 4) needs to be generalized to quad. Making trap invisible marks segments for potential deletion elsewhere.
LRESULT CALLBACK SubProcTrapAdd(HWND, UINT, WPARAM, LPARAM) noexcept;

namespace trap
{
    void AddLast();
    void AddByArea(CPegView* pView, CDC* pDC, CSegs*, CPnt, CPnt);
    void AddByLn(CPegView* pView, CDC* pDC, CSegs*, CPnt, CPnt);
    void PopupCommands(HWND);
}
