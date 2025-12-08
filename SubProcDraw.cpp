#include "stdafx.h"

#include <algorithm>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "SubProcDraw.h"

#include "Hatch.h"
#include "Messages.h"
#include "Pnt.h"
#include "PrimArc.h"
#include "PrimBSpline.h"
#include "PrimLine.h"
#include "PrimMark.h"
#include "PrimPolygon.h"
#include "PrimPolyline.h"
#include "PrimState.h"
#include "SafeMath.h"
#include "UserAxis.h"

INT_PTR CALLBACK DlgProcBlockInsert(HWND, UINT, WPARAM, LPARAM) noexcept;
INT_PTR CALLBACK DlgProcDrawOptions(HWND, UINT, WPARAM, LPARAM) noexcept;

LRESULT CALLBACK SubProcDraw(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept {
  static CDrawHandler handler;
  return handler.HandleMessage(hwnd, anMsg, wParam, lParam);
}

LRESULT CDrawHandler::HandleMessage(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) {
  auto* view = CPegView::GetActiveView();

  CDC* deviceContext = (view == nullptr) ? nullptr : view->GetDC();

  if (anMsg == WM_COMMAND) {
    long lResult{0};

    CPnt cursorPosition = app.CursorPosGet();

    switch (LOWORD(wParam)) {
      case ID_HELP_KEY:
        WinHelp(hwnd, "peg.hlp", HELP_KEY, reinterpret_cast<DWORD_PTR>("DRAW"));
        return 0;

      case ID_OP0:
        HandleOp0(hwnd);
        break;

      case ID_OP1:
        HandleOp1(cursorPosition);
        break;

      case ID_OP2:
        HandleOp2(cursorPosition);
        break;

      case ID_OP3:
        HandleOp3(cursorPosition);
        break;

      case ID_OP4:
        HandleOp4(cursorPosition);
        break;

      case ID_OP5:
        HandleOp5(cursorPosition);
        break;

      case ID_OP6:
        HandleOp6(cursorPosition);
        break;

      case ID_OP7:
        HandleOp7(cursorPosition);
        break;

      case ID_OP8:
        HandleOp8(cursorPosition);
        break;

      case ID_OP9:
        HandleOp9(hwnd);
        break;

      case IDM_RETURN:
        HandleReturn();
        break;

      case ID_SHIFT_RETURN:
        HandleShiftReturn();
        break;

      case IDM_ESCAPE:
        HandleEscape();
        break;

      default:
        lResult = !lResult;
    }
    if (lResult == 0) return (lResult);
  } else if (anMsg == WM_MOUSEMOVE) {
    HandleMouseMove(app.CursorPosGet(), deviceContext);
  }
  return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}

void CDrawHandler::HandleOp0(HWND hwnd) {
  ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_DRAW_OPTIONS), hwnd, DlgProcDrawOptions);
  app.StatusLineDisplay();
}

void CDrawHandler::HandleOp1(const CPnt& cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  CSeg* segment = new CSeg(new CPrimMark(pstate.PenColor(), pstate.MarkStyle(), cursorPosition));
  document->WorkLayerAddTail(segment);
  document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
}

void CDrawHandler::HandleOp2(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (state.previousKeyDown != ID_OP2) {
    state.previousKeyDown = app.ModeLineHighlightOp(ID_OP2);
    state.points[0] = cursorPosition;
  } else {
    app.RubberBandingDisable();
    state.points[1] = UserAxisSnapLn(state.points[0], cursorPosition);

    CSeg* segment = new CSeg(new CPrimLine(state.points[0], state.points[1]));
    document->WorkLayerAddTail(segment);
    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
    state.points[0] = state.points[1];
  }
  app.RubberBandingStartAtEnable(state.points[0], Lines);
}

void CDrawHandler::HandleOp3(CPnt cursorPosition) {
  if (state.previousKeyDown != ID_OP3) {
    state.previousKeyDown = app.ModeLineHighlightOp(ID_OP3);
    state.numberOfPoints = 1;
    state.points[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    auto* view = CPegView::GetActiveView();
    CDC* deviceContext = (view == nullptr) ? nullptr : view->GetDC();

    if (state.numberOfPoints >= state.points.size()) {
      size_t newSize = std::max(state.points.size() * 2, state.numberOfPoints + 1);
      state.points.resize(newSize);
    }
    app.RubberBandingDisable();
    state.points[state.numberOfPoints] = cursorPosition;
    if (state.points[state.numberOfPoints - 1] != state.points[state.numberOfPoints]) {
      CLine(state.points[state.numberOfPoints - 1], state.points[state.numberOfPoints]).Display(view, deviceContext);
      app.RubberBandingStartAtEnable(state.points[state.numberOfPoints], Lines);
      state.numberOfPoints++;
    } else {
      EndFillAreaDef(state.numberOfPoints, state.points);
      app.ModeLineUnhighlightOp(state.previousKeyDown);
      state.reset();
    }
  }
}

void CDrawHandler::HandleOp4(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (state.previousKeyDown != ID_OP4) {
    state.previousKeyDown = app.ModeLineHighlightOp(ID_OP4);
    state.numberOfPoints = 1;
    state.points[0] = cursorPosition;
    app.RubberBandingStartAtEnable(state.points[0], Lines);
  } else {
    state.points[state.numberOfPoints] = cursorPosition;
    if (state.points[state.numberOfPoints - 1] != state.points[state.numberOfPoints]) {
      if (state.numberOfPoints == 1) {
        app.RubberBandingEnable(Quads);
        state.numberOfPoints++;
      } else {
        app.RubberBandingDisable();
        state.points[3] = state.points[0] + CVec(state.points[1], state.points[2]);

        CSeg* segment = new CSeg;

        for (size_t i = 0; i < 4; i++) segment->AddTail(new CPrimLine(state.points[i], state.points[(i + 1) % 4]));

        document->WorkLayerAddTail(segment);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
        app.ModeLineUnhighlightOp(state.previousKeyDown);
        state.reset();
      }
    } else {
      app.RubberBandingDisable();
      msgInformation(IDS_MSG_PTS_COINCIDE);
    }
  }
}

void CDrawHandler::HandleOp5(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (state.previousKeyDown != ID_OP5) {
    state.previousKeyDown = app.ModeLineHighlightOp(ID_OP5);
    state.numberOfPoints = 1;
    state.points[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    state.points[state.numberOfPoints] = cursorPosition;
    if (state.points[state.numberOfPoints - 1] != state.points[state.numberOfPoints]) {
      if (state.numberOfPoints == 1) {
        app.RubberBandingDisable();
        state.points[++state.numberOfPoints] = state.points[1];
      } else {
        if (eArcGenId == EArcGen::ThreePoint) {
          CPrimArc* arcPrimitive = new CPrimArc(state.points[0], state.points[1], state.points[2]);
          if (arcPrimitive->GetSwpAng() != 0.0) {
            CSeg* segment = new CSeg(arcPrimitive);
            document->WorkLayerAddTail(segment);
            document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
          } else {
            delete arcPrimitive;
            msgInformation(IDS_MSG_PTS_COLINEAR);
          }
        } else if (eArcGenId == EArcGen::StartCenterEndPoint) {
        }
        app.ModeLineUnhighlightOp(state.previousKeyDown);
        state.reset();
      }
    } else
      msgInformation(IDS_MSG_PTS_COINCIDE);
  }
}

void CDrawHandler::HandleOp6(CPnt cursorPosition) {
  auto* view = CPegView::GetActiveView();
  CDC* deviceContext = (view == nullptr) ? nullptr : view->GetDC();
  if (state.previousKeyDown != ID_OP6) {  // Initial control point
    state.previousKeyDown = app.ModeLineHighlightOp(ID_OP6);
    state.numberOfPoints = 1;
    state.points[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    // Ensure vector can grow dynamically for OP6
    if (state.numberOfPoints >= state.points.size()) {
      size_t newSize = std::max(state.points.size() * 2, state.numberOfPoints + 1);
      state.points.resize(newSize);
    }

    if (state.numberOfPoints == 1)
      app.RubberBandingDisable();
    else {  // rubber banding for splines required display
      // in main window proc. It is erased here and displayed there.
      int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
      CPrimBSpline BSp(WORD(state.numberOfPoints + 1), state.points.data());
      BSp.Display(view, deviceContext);
      pstate.SetROP2(deviceContext, iDrawMode);
    }
    state.points[state.numberOfPoints] = cursorPosition;
    if (state.points[state.numberOfPoints - 1] != state.points[state.numberOfPoints]) {
      state.numberOfPoints++;
      // ensure space for the duplicated last point
      if (state.numberOfPoints >= state.points.size()) state.points.resize(state.numberOfPoints + 1);
      state.points[state.numberOfPoints] = state.points[state.numberOfPoints - 1];
    } else {
      EndSplineDef(eSplnGenId, state.numberOfPoints, state.points);
      app.ModeLineUnhighlightOp(state.previousKeyDown);
      state.reset();
    }
  }
}

void CDrawHandler::HandleOp7(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  auto* view = CPegView::GetActiveView();
  if (state.previousKeyDown != ID_OP7) {
    state.previousKeyDown = app.ModeLineHighlightOp(ID_OP7);
    state.points[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Circles);
  } else {
    state.points[1] = cursorPosition;
    app.RubberBandingDisable();
    if (state.points[0] != cursorPosition) {  // Radius not zero
      auto viewPlaneNormal = view->ModelViewGetDirection();
      CVec majorAxis(state.points[0], cursorPosition);
      state.points[1] = Pnt_RotAboutArbPtAndAx(cursorPosition, state.points[0], viewPlaneNormal, HALF_PI);
      CVec minorAxis(state.points[0], state.points[1]);

      CSeg* segment = new CSeg(new CPrimArc(state.points[0], majorAxis, minorAxis, TWOPI));
      document->WorkLayerAddTail(segment);
      document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
    }
    app.ModeLineUnhighlightOp(state.previousKeyDown);
  }
}

void CDrawHandler::HandleOp8(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (state.previousKeyDown != ID_OP8) {
    state.previousKeyDown = app.ModeLineHighlightOp(ID_OP8);
    state.numberOfPoints = 1;
    state.points[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    state.points[state.numberOfPoints] = cursorPosition;
    if (state.points[state.numberOfPoints - 1] != cursorPosition) {
      if (state.numberOfPoints == 1) {
        state.points[2] = state.points[0];
        app.RubberBandingDisable();
        app.CursorPosSet(state.points[0]);
        state.numberOfPoints++;
      } else {
        CVec majorAxis(state.points[0], state.points[1]);
        CVec minorAxis(state.points[0], state.points[2]);

        CSeg* segment = new CSeg(new CPrimArc(state.points[0], majorAxis, minorAxis, TWOPI));
        document->WorkLayerAddTail(segment);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
        app.ModeLineUnhighlightOp(state.previousKeyDown);
        state.reset();
      }
    } else
      msgInformation(IDS_MSG_PTS_COINCIDE);
  }
}

void CDrawHandler::HandleOp9(HWND hwnd) {
  auto* document = CPegDoc::GetDoc();
  if (document->BlksSize() > 0)
    ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_INSERT_BLOCK), hwnd, DlgProcBlockInsert);
}

/// @brief Finalize the current drawing action: disable rubber-banding, end any active fill-area or spline definition depending on the previously active operation key, unhighlight the mode-line operation, and reset the working point count.
void CDrawHandler::HandleReturn() {
  app.RubberBandingDisable();
  if (state.previousKeyDown == ID_OP3) {
    EndFillAreaDef(state.numberOfPoints, state.points);
  } else if (state.previousKeyDown == ID_OP6) {
    EndSplineDef(eSplnGenId, state.numberOfPoints, state.points);
  }
  app.ModeLineUnhighlightOp(state.previousKeyDown);
  state.reset();
}

/// @brief Finalize the current polyline drawing action: disable rubber-banding, end the polyline definition if OP3 was previously active, unhighlight the mode-line operation, and reset the working point count.
void CDrawHandler::HandleShiftReturn() {
  app.RubberBandingDisable();
  if (state.previousKeyDown == ID_OP3) { EndPolylineDef(state.numberOfPoints, state.points); }

  app.ModeLineUnhighlightOp(state.previousKeyDown);
  state.reset();
}

void CDrawHandler::HandleEscape() {
  app.RubberBandingDisable();

  if (state.previousKeyDown == ID_OP2) {
  } else if (state.previousKeyDown == ID_OP3) {
    // erase tmp lines of poly
  } else if (state.previousKeyDown == ID_OP6) {
    // erase tmp lines of poly
  }
  app.ModeLineUnhighlightOp(state.previousKeyDown);
}

void CDrawHandler::HandleMouseMove(CPnt cursorPosition, CDC* deviceContext) {
  auto* view = CPegView::GetActiveView();
  if (state.previousKeyDown == ID_OP5 && state.numberOfPoints == 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
    if (eArcGenId == EArcGen::ThreePoint) {
      CPrimArc(state.points[0], state.points[1], state.points[2]).Display(view, deviceContext);
      state.points[2] = cursorPosition;
      CPrimArc(state.points[0], state.points[1], state.points[2]).Display(view, deviceContext);
    }
    pstate.SetROP2(deviceContext, iDrawMode);
  } else if (state.previousKeyDown == ID_OP6 && state.numberOfPoints >= 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);

    CPrimBSpline splinePrimitive(WORD(state.numberOfPoints + 1), state.points.data());
    if (state.points[state.numberOfPoints - 1] != state.points[state.numberOfPoints]) splinePrimitive.Display(view, deviceContext);
    state.points[state.numberOfPoints] = cursorPosition;
    splinePrimitive.SetPoint(state.numberOfPoints, state.points[state.numberOfPoints]);
    if (state.points[state.numberOfPoints - 1] != state.points[state.numberOfPoints]) splinePrimitive.Display(view, deviceContext);

    pstate.SetROP2(deviceContext, iDrawMode);
  } else if (state.previousKeyDown == ID_OP8 && state.numberOfPoints == 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
    CVec majorAxis(state.points[0], state.points[1]);
    CVec minorAxis(state.points[0], state.points[2]);
    CPrimArc arc(state.points[0], majorAxis, minorAxis, TWOPI);
    if (!minorAxis.IsNull()) arc.Display(view, deviceContext);
    state.points[2] = cursorPosition;
    minorAxis = CVec(state.points[0], state.points[2]);
    if (!minorAxis.IsNull()) {
      arc.SetMinAx(minorAxis);
      arc.Display(view, deviceContext);
    }
    pstate.SetROP2(deviceContext, iDrawMode);
  }
}

size_t CDrawHandler::EndFillAreaDef(size_t numberOfPoints, std::vector<CPnt>& points) {
  auto* document = CPegDoc::GetDoc();

  if (numberOfPoints >= 3) {  // Determine plane in which fill area lies
    CVec xAxis(points[0], points[1]);
    CVec yAxis(points[0], points[2]);
    CVec planeNormal(xAxis ^ yAxis);
    planeNormal.Normalize();

    if (planeNormal[2] < 0) planeNormal = -planeNormal;

    xAxis.Normalize();
    xAxis.RotAboutArbAx(planeNormal, hatch::dOffAng);
    yAxis = xAxis;
    yAxis.RotAboutArbAx(planeNormal, HALF_PI);
    xAxis *= hatch::dXAxRefVecScal;
    yAxis *= hatch::dYAxRefVecScal;

    // Project reference origin to plane

    auto* polygonPrimitive = new CPrimPolygon(numberOfPoints, points[numberOfPoints], xAxis, yAxis, points.data());
    auto* segment = new CSeg(polygonPrimitive);
    document->WorkLayerAddTail(segment);
    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
  }
  return (numberOfPoints);
}

CSeg* CDrawHandler::EndPolylineDef(size_t numberOfPoints, std::vector<CPnt>& points) {
  auto* document = CPegDoc::GetDoc();

  auto* polylinePrimitive = new CPrimPolyline(numberOfPoints, points);
  auto* newSegment = new CSeg(polylinePrimitive);
  document->WorkLayerAddTail(newSegment);
  document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, newSegment);

  return (newSegment);
}

CSeg* CDrawHandler::EndSplineDef(ESplnGen splineType, size_t numberOfControlPoints, std::vector<CPnt>& controlPoints) {
  auto* document = CPegDoc::GetDoc();

  CSeg* newSegment = nullptr;

  if (numberOfControlPoints > 2) {
    if (splineType == BSpline) {
      auto* splinePrimitive = new CPrimBSpline(static_cast<WORD>(numberOfControlPoints), controlPoints.data());
      newSegment = new CSeg(splinePrimitive);
      document->WorkLayerAddTail(newSegment);
      document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, newSegment);
    }
  }
  return (newSegment);
}