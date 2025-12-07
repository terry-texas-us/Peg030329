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
  if (previousKeyDown != ID_OP2) {
    previousKeyDown = app.ModeLineHighlightOp(ID_OP2);
    pt[0] = cursorPosition;
  } else {
    app.RubberBandingDisable();
    pt[1] = UserAxisSnapLn(pt[0], cursorPosition);

    CSeg* segment = new CSeg(new CPrimLine(pt[0], pt[1]));
    document->WorkLayerAddTail(segment);
    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
    pt[0] = pt[1];
  }
  app.RubberBandingStartAtEnable(pt[0], Lines);
}

void CDrawHandler::HandleOp3(CPnt cursorPosition) {
  if (previousKeyDown != ID_OP3) {
    previousKeyDown = app.ModeLineHighlightOp(ID_OP3);
    numberOfPoints = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    auto* view = CPegView::GetActiveView();
    CDC* deviceContext = (view == nullptr) ? nullptr : view->GetDC();

    if (numberOfPoints >= pt.size()) {
      size_t newSize = std::max(pt.size() * 2, numberOfPoints + 1);
      pt.resize(newSize);
    }
    app.RubberBandingDisable();
    pt[numberOfPoints] = cursorPosition;
    if (pt[numberOfPoints - 1] != pt[numberOfPoints]) {
      CLine(pt[numberOfPoints - 1], pt[numberOfPoints]).Display(view, deviceContext);
      app.RubberBandingStartAtEnable(pt[numberOfPoints], Lines);
      numberOfPoints++;
    } else {
      EndFillAreaDef(numberOfPoints, pt);
      app.ModeLineUnhighlightOp(previousKeyDown);
      numberOfPoints = 0;
      pt.resize(4);
    }
  }
}

void CDrawHandler::HandleOp4(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (previousKeyDown != ID_OP4) {
    previousKeyDown = app.ModeLineHighlightOp(ID_OP4);
    numberOfPoints = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(pt[0], Lines);
  } else {
    pt[numberOfPoints] = cursorPosition;
    if (pt[numberOfPoints - 1] != pt[numberOfPoints]) {
      if (numberOfPoints == 1) {
        app.RubberBandingEnable(Quads);
        numberOfPoints++;
      } else {
        app.RubberBandingDisable();
        pt[3] = pt[0] + CVec(pt[1], pt[2]);

        CSeg* segment = new CSeg;

        for (size_t i = 0; i < 4; i++) segment->AddTail(new CPrimLine(pt[i], pt[(i + 1) % 4]));

        document->WorkLayerAddTail(segment);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
        app.ModeLineUnhighlightOp(previousKeyDown);
        numberOfPoints = 0;
        pt.resize(4);
      }
    } else {
      app.RubberBandingDisable();
      msgInformation(IDS_MSG_PTS_COINCIDE);
    }
  }
}

void CDrawHandler::HandleOp5(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (previousKeyDown != ID_OP5) {
    previousKeyDown = app.ModeLineHighlightOp(ID_OP5);
    numberOfPoints = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    pt[numberOfPoints] = cursorPosition;
    if (pt[numberOfPoints - 1] != pt[numberOfPoints]) {
      if (numberOfPoints == 1) {
        app.RubberBandingDisable();
        pt[++numberOfPoints] = pt[1];
      } else {
        if (eArcGenId == EArcGen::ThreePoint) {
          CPrimArc* arcPrimitive = new CPrimArc(pt[0], pt[1], pt[2]);
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
        app.ModeLineUnhighlightOp(previousKeyDown);
        numberOfPoints = 0;
        pt.resize(4);
      }
    } else
      msgInformation(IDS_MSG_PTS_COINCIDE);
  }
}

void CDrawHandler::HandleOp6(CPnt cursorPosition) {
  auto* view = CPegView::GetActiveView();
  CDC* deviceContext = (view == nullptr) ? nullptr : view->GetDC();
  if (previousKeyDown != ID_OP6) {  // Initial control point
    previousKeyDown = app.ModeLineHighlightOp(ID_OP6);
    numberOfPoints = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    // Ensure vector can grow dynamically for OP6
    if (numberOfPoints >= pt.size()) {
      size_t newSize = std::max(pt.size() * 2, numberOfPoints + 1);
      pt.resize(newSize);
    }

    if (numberOfPoints == 1)
      app.RubberBandingDisable();
    else {  // rubber banding for splines required display
      // in main window proc. It is erased here and displayed there.
      int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
      CPrimBSpline BSp(WORD(numberOfPoints + 1), pt.data());
      BSp.Display(view, deviceContext);
      pstate.SetROP2(deviceContext, iDrawMode);
    }
    pt[numberOfPoints] = cursorPosition;
    if (pt[numberOfPoints - 1] != pt[numberOfPoints]) {
      numberOfPoints++;
      // ensure space for the duplicated last point
      if (numberOfPoints >= pt.size()) pt.resize(numberOfPoints + 1);
      pt[numberOfPoints] = pt[numberOfPoints - 1];
    } else {
      EndSplineDef(eSplnGenId, numberOfPoints, pt);
      app.ModeLineUnhighlightOp(previousKeyDown);
      numberOfPoints = 0;
      pt.resize(4);
    }
  }
}

void CDrawHandler::HandleOp7(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  auto* view = CPegView::GetActiveView();
  if (previousKeyDown != ID_OP7) {
    previousKeyDown = app.ModeLineHighlightOp(ID_OP7);
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Circles);
  } else {
    pt[1] = cursorPosition;
    app.RubberBandingDisable();
    if (pt[0] != cursorPosition) {  // Radius not zero
      auto viewPlaneNormal = view->ModelViewGetDirection();
      CVec majorAxis(pt[0], cursorPosition);
      pt[1] = Pnt_RotAboutArbPtAndAx(cursorPosition, pt[0], viewPlaneNormal, HALF_PI);
      CVec minorAxis(pt[0], pt[1]);

      CSeg* segment = new CSeg(new CPrimArc(pt[0], majorAxis, minorAxis, TWOPI));
      document->WorkLayerAddTail(segment);
      document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
    }
    app.ModeLineUnhighlightOp(previousKeyDown);
  }
}

void CDrawHandler::HandleOp8(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (previousKeyDown != ID_OP8) {
    previousKeyDown = app.ModeLineHighlightOp(ID_OP8);
    numberOfPoints = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    pt[numberOfPoints] = cursorPosition;
    if (pt[numberOfPoints - 1] != cursorPosition) {
      if (numberOfPoints == 1) {
        pt[2] = pt[0];
        app.RubberBandingDisable();
        app.CursorPosSet(pt[0]);
        numberOfPoints++;
      } else {
        CVec majorAxis(pt[0], pt[1]);
        CVec minorAxis(pt[0], pt[2]);

        CSeg* segment = new CSeg(new CPrimArc(pt[0], majorAxis, minorAxis, TWOPI));
        document->WorkLayerAddTail(segment);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
        app.ModeLineUnhighlightOp(previousKeyDown);
        numberOfPoints = 0;
        pt.resize(4);
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
  if (previousKeyDown == ID_OP3) {
    EndFillAreaDef(numberOfPoints, pt);
  } else if (previousKeyDown == ID_OP6) {
    EndSplineDef(eSplnGenId, numberOfPoints, pt);
  }
  app.ModeLineUnhighlightOp(previousKeyDown);
  numberOfPoints = 0;
  pt.resize(4);
}

/// @brief Finalize the current polyline drawing action: disable rubber-banding, end the polyline definition if OP3 was previously active, unhighlight the mode-line operation, and reset the working point count.
void CDrawHandler::HandleShiftReturn() {
  app.RubberBandingDisable();
  if (previousKeyDown == ID_OP3) { EndPolylineDef(numberOfPoints, pt); }

  app.ModeLineUnhighlightOp(previousKeyDown);
  numberOfPoints = 0;
  pt.resize(4);
}

void CDrawHandler::HandleEscape() {
  app.RubberBandingDisable();

  if (previousKeyDown == ID_OP2) {
  } else if (previousKeyDown == ID_OP3) {
    // erase tmp lines of poly
  } else if (previousKeyDown == ID_OP6) {
    // erase tmp lines of poly
  }
  app.ModeLineUnhighlightOp(previousKeyDown);
}

void CDrawHandler::HandleMouseMove(CPnt cursorPosition, CDC* deviceContext) {
  auto* view = CPegView::GetActiveView();
  if (previousKeyDown == ID_OP5 && numberOfPoints == 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
    if (eArcGenId == EArcGen::ThreePoint) {
      CPrimArc(pt[0], pt[1], pt[2]).Display(view, deviceContext);
      pt[2] = cursorPosition;
      CPrimArc(pt[0], pt[1], pt[2]).Display(view, deviceContext);
    }
    pstate.SetROP2(deviceContext, iDrawMode);
  } else if (previousKeyDown == ID_OP6 && numberOfPoints >= 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);

    CPrimBSpline splinePrimitive(WORD(numberOfPoints + 1), pt.data());
    if (pt[numberOfPoints - 1] != pt[numberOfPoints]) splinePrimitive.Display(view, deviceContext);
    pt[numberOfPoints] = cursorPosition;
    splinePrimitive.SetPoint(numberOfPoints, pt[numberOfPoints]);
    if (pt[numberOfPoints - 1] != pt[numberOfPoints]) splinePrimitive.Display(view, deviceContext);

    pstate.SetROP2(deviceContext, iDrawMode);
  } else if (previousKeyDown == ID_OP8 && numberOfPoints == 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
    CVec majorAxis(pt[0], pt[1]);
    CVec minorAxis(pt[0], pt[2]);
    CPrimArc arc(pt[0], majorAxis, minorAxis, TWOPI);
    if (!minorAxis.IsNull()) arc.Display(view, deviceContext);
    pt[2] = cursorPosition;
    minorAxis = CVec(pt[0], pt[2]);
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