#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

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
#include "SubProcDraw.h"
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
    wPts = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    auto* view = CPegView::GetActiveView();
    CDC* deviceContext = (view == nullptr) ? nullptr : view->GetDC();
    if (wPts < sizeof(pt) / sizeof(pt[0])) {
      app.RubberBandingDisable();
      pt[wPts] = cursorPosition;
      if (pt[wPts - 1] != pt[wPts]) {
        CLine(pt[wPts - 1], pt[wPts]).Display(view, deviceContext);
        app.RubberBandingStartAtEnable(pt[wPts], Lines);
        wPts++;
      } else {
        EndFillAreaDef(wPts, pt);
        app.ModeLineUnhighlightOp(previousKeyDown);
        wPts = 0;
      }
    } else
      msgInformation(IDS_MSG_TOO_MANY_PTS);
  }
}

void CDrawHandler::HandleOp4(CPnt cursorPosition) {
  auto* document = CPegDoc::GetDoc();
  if (previousKeyDown != ID_OP4) {
    previousKeyDown = app.ModeLineHighlightOp(ID_OP4);
    wPts = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(pt[0], Lines);
  } else {
    pt[wPts] = cursorPosition;
    if (pt[wPts - 1] != pt[wPts]) {
      if (wPts == 1) {
        app.RubberBandingEnable(Quads);
        wPts++;
      } else {
        app.RubberBandingDisable();
        pt[3] = pt[0] + CVec(pt[1], pt[2]);

        CSeg* segment = new CSeg;

        for (int i = 0; i < 4; i++) segment->AddTail(new CPrimLine(pt[i], pt[(i + 1) % 4]));

        document->WorkLayerAddTail(segment);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
        app.ModeLineUnhighlightOp(previousKeyDown);
        wPts = 0;
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
    wPts = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    pt[wPts] = cursorPosition;
    if (pt[wPts - 1] != pt[wPts]) {
      if (wPts == 1) {
        app.RubberBandingDisable();
        pt[++wPts] = pt[1];
      } else {
        if (eArcGenId == StartPoint) {
          CPrimArc* pArc = new CPrimArc(pt[0], pt[1], pt[2]);
          if (pArc->GetSwpAng() != 0.) {
            CSeg* segment = new CSeg(pArc);
            document->WorkLayerAddTail(segment);
            document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
          } else {
            delete pArc;
            msgInformation(IDS_MSG_PTS_COLINEAR);
          }
        }
        app.ModeLineUnhighlightOp(previousKeyDown);
        wPts = 0;
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
    wPts = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    if (wPts < sizeof(pt) / sizeof(pt[0])) {
      if (wPts == 1)
        app.RubberBandingDisable();
      else {  // rubber banding for splines required display
        // in main window proc. It is erased here and displayed there.
        int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
        CPrimBSpline BSp(WORD(wPts + 1), pt);
        BSp.Display(view, deviceContext);
        pstate.SetROP2(deviceContext, iDrawMode);
      }
      pt[wPts] = cursorPosition;
      if (pt[wPts - 1] != pt[wPts]) {
        wPts++;
        pt[wPts] = pt[wPts - 1];
      } else {
        EndSplineDef(eSplnGenId, wPts, pt);
        app.ModeLineUnhighlightOp(previousKeyDown);
        wPts = 0;
      }
    } else  // Too many control points
      msgInformation(IDS_MSG_TOO_MANY_PTS);
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
    wPts = 1;
    pt[0] = cursorPosition;
    app.RubberBandingStartAtEnable(cursorPosition, Lines);
  } else {
    pt[wPts] = cursorPosition;
    if (pt[wPts - 1] != cursorPosition) {
      if (wPts == 1) {
        pt[2] = pt[0];
        app.RubberBandingDisable();
        app.CursorPosSet(pt[0]);
        wPts++;
      } else {
        CVec majorAxis(pt[0], pt[1]);
        CVec minorAxis(pt[0], pt[2]);

        CSeg* segment = new CSeg(new CPrimArc(pt[0], majorAxis, minorAxis, TWOPI));
        document->WorkLayerAddTail(segment);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
        app.ModeLineUnhighlightOp(previousKeyDown);
        wPts = 0;
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
    EndFillAreaDef(wPts, pt);
  } else if (previousKeyDown == ID_OP6) {
    EndSplineDef(eSplnGenId, wPts, pt);
  }
  app.ModeLineUnhighlightOp(previousKeyDown);
  wPts = 0;
}

/// @brief Finalize the current polyline drawing action: disable rubber-banding, end the polyline definition if OP3 was previously active, unhighlight the mode-line operation, and reset the working point count.
void CDrawHandler::HandleShiftReturn() {
  app.RubberBandingDisable();
  if (previousKeyDown == ID_OP3) { EndPolylineDef(wPts, pt); }

  app.ModeLineUnhighlightOp(previousKeyDown);
  wPts = 0;
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
  if (previousKeyDown == ID_OP5 && wPts == 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
    if (eArcGenId == StartPoint) {
      CPrimArc(pt[0], pt[1], pt[2]).Display(view, deviceContext);
      pt[2] = cursorPosition;
      CPrimArc(pt[0], pt[1], pt[2]).Display(view, deviceContext);
    }
    pstate.SetROP2(deviceContext, iDrawMode);
  } else if (previousKeyDown == ID_OP6 && wPts >= 2) {
    int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);

    CPrimBSpline BSp(WORD(wPts + 1), pt);
    if (pt[wPts - 1] != pt[wPts]) BSp.Display(view, deviceContext);
    pt[wPts] = cursorPosition;
    BSp.SetPt(wPts, pt[wPts]);
    if (pt[wPts - 1] != pt[wPts]) BSp.Display(view, deviceContext);

    pstate.SetROP2(deviceContext, iDrawMode);
  } else if (previousKeyDown == ID_OP8 && wPts == 2) {
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

WORD CDrawHandler::EndFillAreaDef(WORD numberOfPoints, CPnt* points) {
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

    auto* segment = new CSeg(new CPrimPolygon(numberOfPoints, points[numberOfPoints], xAxis, yAxis, points));
    document->WorkLayerAddTail(segment);
    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, segment);
  }
  return (numberOfPoints);
}

CSeg* CDrawHandler::EndPolylineDef(int numberOfPoints, CPnt* points) {
  auto* document = CPegDoc::GetDoc();

  auto* newSegment = new CSeg(new CPrimPolyline(static_cast<WORD>(numberOfPoints), points));
  document->WorkLayerAddTail(newSegment);
  document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, newSegment);

  return (newSegment);
}

CSeg* CDrawHandler::EndSplineDef(ESplnGen splineType, int numberOfControlPoints, CPnt* controlPoints) {
  auto* document = CPegDoc::GetDoc();

  CSeg* newSegment = nullptr;

  if (numberOfControlPoints > 2) {
    if (splineType == BSpline) {
      newSegment = new CSeg(new CPrimBSpline(static_cast<WORD>(numberOfControlPoints), controlPoints));
      document->WorkLayerAddTail(newSegment);
      document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, newSegment);
    }
  }
  return (newSegment);
}