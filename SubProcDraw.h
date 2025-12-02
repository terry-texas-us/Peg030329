#pragma once

#include <Windows.h>

#include "Pnt.h"

class CSeg;

LRESULT CALLBACK SubProcDraw(HWND, UINT, WPARAM, LPARAM) noexcept;

namespace draw
{
    enum EArcGen { StartPoint = 1, CenterPoint };
    enum ESplnGen { BSpline, CSpline };
    enum ESplnEndCnd { Fixed, Relaxed, Cyclic, Anticyclic };

    WORD	EndFillAreaDef(WORD, CPnt*);

    /// @brief Finalize a polyline definition using an array of points.
    /// @param numberOfPoints The number of points in the points array.
    /// @param points Pointer to the first element of an array of CPnt structures representing the polyline vertices.
    /// @return A pointer for the created CSeg or nullptr.
    CSeg* EndPolylineDef(int numberOfPoints, CPnt* points);

    /// @brief Finalizes a spline definition using the specified generation type and control points.
    /// @param splineType The spline generation type (ESplnGen enumeration) that determines how the spline is created.
    /// @param numberOfControlPoints The number of control points in the controlPoints array.
    /// @param controlPoints Pointer to an array of CPnt structures that define the spline's control points. The array must contain at least numberOfControlPoints elements.
    /// @return A pointer for the created segment or nullptr.
    CSeg* EndSplineDef(ESplnGen splineType, int numberOfControlPoints, CPnt* controlPoints);
}