#pragma once

#include <Windows.h>

#include "Pnt.h"

class CSeg;

LRESULT CALLBACK SubProcAnnotate(HWND, UINT, WPARAM, LPARAM) noexcept;

namespace annotate
{
    extern char		szDefTxt[16];
    extern double	dGapSpaceFac; 			// Edge space factor 25 percent of character height
    extern double	dHookRad;				// Hook radius
    extern int 		iArrowTyp;				// Arrow type
    extern double	dArrowSiz; 				// Arrow size
    extern double	dBubRad; 				// Bubble radius
    extern WORD		wBubFacets;				// Number of sides on bubble (indicating circle)

    void	GenArrowHead(int, double, const CPnt&, const CPnt&, CSeg*);
}