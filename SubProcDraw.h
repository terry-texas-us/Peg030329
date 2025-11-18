#pragma once

LRESULT CALLBACK SubProcDraw(HWND, UINT, WPARAM, LPARAM);

namespace draw
{
	enum EArcGen {StartPoint = 1, CenterPoint};
	enum ESplnGen {BSpline, CSpline};
	enum ESplnEndCnd {Fixed, Relaxed, Cyclic, Anticyclic};

	WORD	EndFillAreaDef(WORD, CPnt*);
	WORD	EndPolylineDef(WORD wPts, CPnt* pPts);
	int		EndSplineDef(ESplnGen, int, CPnt*);
}