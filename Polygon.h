#pragma once

#include <afxwin.h>

#include "Pnt.h"
#include "Pnt4.h"
#include "TMat.h"
#include "Vec.h"
#include "Vec4.h"

class CPegView;

const short POLYGON_HOLLOW = 0;
const short POLYGON_SOLID = 1;
const short POLYGON_PATTERN = 2;
const short POLYGON_HATCH = 3;

void DisplayFilAreaHatch(CPegView* pView, CDC* pDC, CTMat& tm, const int iSets, const int* iPtLstsId, CPnt*);
void Polygon_Display(CPegView* pView, CDC* pDC, CPnt4s&);
void Polygon_IntersectionWithPln(CPnt4s& ptaIn, const CPnt4& ptQ, const CVec4& vQNorm, CPnt4s& ptaOut);
void Polygon_GenPts(const CPnt& ptCP, const CVec& vXRefAx, const CVec& vYRefAx, int iPts, CPnt* pt);
