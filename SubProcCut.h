#pragma once

#include <Windows.h>

#include <afxwin.h>

#include "Pnt.h"

LRESULT CALLBACK SubProcCut(HWND, UINT, WPARAM, LPARAM) noexcept;

namespace cut
{
    void CutPrimAt2Pts(CDC* pDC, CPnt, CPnt);
    void CutPrimsAtPt(CPnt pt);
    void CutPrimsByLn(CPnt, CPnt);
    void CutSegsByArea(CDC* pDC, CPnt, CPnt);
}