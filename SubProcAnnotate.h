#pragma once

#include <afxwin.h> // for MFC HWND, UINT, WPARAM, LPARAM, LRESULT, CALLBACK

#include "Pnt.h" // for CPnt

class CSeg;

LRESULT CALLBACK SubProcAnnotate(HWND, UINT, WPARAM, LPARAM);

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