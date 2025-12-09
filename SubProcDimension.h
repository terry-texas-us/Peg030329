#pragma once

#include <Windows.h>

#include "PegAEsysView.h"
#include "Pnt.h"

LRESULT CALLBACK SubProcDimension(HWND, UINT, WPARAM, LPARAM) noexcept;

namespace dimension {
CPnt AddArrowHead(CPnt);
CPnt AddRadius(const CPnt& pt);
CPnt AddDiameter(const CPnt& pt);
CPnt CvtPrim(CPegView* pView, CPnt);
CPnt ProjPtToLn(CPnt arPt);
}  // namespace dimension
