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

namespace draw {
EArcGen eArcGenId{StartPoint};
ESplnGen eSplnGenId{BSpline};
ESplnEndCnd eSplnEndCndId{Relaxed};
int iSplnPntsS{8};
}  // namespace draw

LRESULT CALLBACK SubProcDraw(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept {
  static WORD wPts{0};
  static WORD previousKeyDown{0};
  static CPnt pt[63]{};

  auto* document = CPegDoc::GetDoc();
  auto* view = CPegView::GetActiveView();

  CDC* deviceContext = (view == nullptr) ? NULL : view->GetDC();

  if (anMsg == WM_COMMAND) {
    CSeg* segment{nullptr};
    long lResult{0};

    CPnt cursorPosition = app.CursorPosGet();

    switch (LOWORD(wParam)) {
      case ID_HELP_KEY:
        WinHelp(hwnd, "peg.hlp", HELP_KEY, reinterpret_cast<DWORD_PTR>("DRAW"));
        return 0;

      case ID_OP0:
        ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_DRAW_OPTIONS), hwnd, DlgProcDrawOptions);
        app.StatusLineDisplay();
        break;

      case ID_OP1:
        segment = new CSeg(new CPrimMark(pstate.PenColor(), pstate.MarkStyle(), cursorPosition));
        document->WorkLayerAddTail(segment);
        document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
        break;

      case ID_OP2:  // Define and generate one or more lines
        if (previousKeyDown != ID_OP2) {
          previousKeyDown = app.ModeLineHighlightOp(ID_OP2);
          pt[0] = cursorPosition;
        } else {
          app.RubberBandingDisable();
          pt[1] = UserAxisSnapLn(pt[0], cursorPosition);

          segment = new CSeg(new CPrimLine(pt[0], pt[1]));
          document->WorkLayerAddTail(segment);
          document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
          pt[0] = pt[1];
        }
        app.RubberBandingStartAtEnable(pt[0], Lines);
        break;

      case ID_OP3:
        if (previousKeyDown != ID_OP3) {
          previousKeyDown = app.ModeLineHighlightOp(ID_OP3);
          wPts = 1;
          pt[0] = cursorPosition;
          app.RubberBandingStartAtEnable(cursorPosition, Lines);
        } else {
          if (wPts < sizeof(pt) / sizeof(pt[0])) {
            app.RubberBandingDisable();
            pt[wPts] = cursorPosition;
            if (pt[wPts - 1] != pt[wPts]) {
              CLine(pt[wPts - 1], pt[wPts]).Display(view, deviceContext);
              app.RubberBandingStartAtEnable(pt[wPts], Lines);
              wPts++;
            } else {
              draw::EndFillAreaDef(wPts, pt);
              app.ModeLineUnhighlightOp(previousKeyDown);
              wPts = 0;
            }
          } else
            msgInformation(IDS_MSG_TOO_MANY_PTS);
        }
        break;

      case ID_OP4:
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

              segment = new CSeg;

              for (int i = 0; i < 4; i++) segment->AddTail(new CPrimLine(pt[i], pt[(i + 1) % 4]));

              document->WorkLayerAddTail(segment);
              document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
              app.ModeLineUnhighlightOp(previousKeyDown);
              wPts = 0;
            }
          } else {
            app.RubberBandingDisable();
            msgInformation(IDS_MSG_PTS_COINCIDE);
          }
        }
        break;

      case ID_OP5:
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
              if (draw::eArcGenId == draw::StartPoint) {
                CPrimArc* pArc = new CPrimArc(pt[0], pt[1], pt[2]);
                if (pArc->GetSwpAng() != 0.) {
                  segment = new CSeg(pArc);
                  document->WorkLayerAddTail(segment);
                  document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
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
        break;

      case ID_OP6:
        if (previousKeyDown != ID_OP6) {  // Intitial control point
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
              draw::EndSplineDef(draw::eSplnGenId, wPts, pt);
              app.ModeLineUnhighlightOp(previousKeyDown);
              wPts = 0;
            }
          } else  // Too many control points
            msgInformation(IDS_MSG_TOO_MANY_PTS);
        }
        break;

      case ID_OP7:
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

            segment = new CSeg(new CPrimArc(pt[0], majorAxis, minorAxis, TWOPI));
            document->WorkLayerAddTail(segment);
            document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
          }
          app.ModeLineUnhighlightOp(previousKeyDown);
        }
        break;

      case ID_OP8:
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

              segment = new CSeg(new CPrimArc(pt[0], majorAxis, minorAxis, TWOPI));
              document->WorkLayerAddTail(segment);
              document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
              app.ModeLineUnhighlightOp(previousKeyDown);
              wPts = 0;
            }
          } else
            msgInformation(IDS_MSG_PTS_COINCIDE);
        }
        break;

      case ID_OP9:
        if (document->BlksSize() > 0)
          ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_INSERT_BLOCK), hwnd, DlgProcBlockInsert);

        break;

      case IDM_RETURN:
        app.RubberBandingDisable();
        if (previousKeyDown == ID_OP3) {
          draw::EndFillAreaDef(wPts, pt);
        } else if (previousKeyDown == ID_OP6) {
          draw::EndSplineDef(draw::eSplnGenId, wPts, pt);
        }
        app.ModeLineUnhighlightOp(previousKeyDown);
        wPts = 0;
        break;

      case ID_SHIFT_RETURN:
        app.RubberBandingDisable();
        if (previousKeyDown == ID_OP3) { draw::EndPolylineDef(wPts, pt); }

        app.ModeLineUnhighlightOp(previousKeyDown);
        wPts = 0;
        break;

      case IDM_ESCAPE:
        app.RubberBandingDisable();

        if (previousKeyDown == ID_OP2) {
        } else if (previousKeyDown == ID_OP3) {
          // erase tmp lines of poly
        } else if (previousKeyDown == ID_OP6) {
          // erase tmp lines of poly
        }
        app.ModeLineUnhighlightOp(previousKeyDown);
        break;

      default:
        lResult = !lResult;
    }
    if (lResult == 0) return (lResult);
  } else if (anMsg == WM_MOUSEMOVE) {
    if (previousKeyDown == ID_OP5 && wPts == 2) {
      int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
      if (draw::eArcGenId == draw::StartPoint) {
        CPrimArc(pt[0], pt[1], pt[2]).Display(view, deviceContext);
        pt[2] = app.CursorPosGet();
        CPrimArc(pt[0], pt[1], pt[2]).Display(view, deviceContext);
      }
      pstate.SetROP2(deviceContext, iDrawMode);
    } else if (previousKeyDown == ID_OP6 && wPts >= 2) {
      int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);

      CPrimBSpline BSp(WORD(wPts + 1), pt);
      if (pt[wPts - 1] != pt[wPts]) BSp.Display(view, deviceContext);
      pt[wPts] = app.CursorPosGet();
      BSp.SetPt(wPts, pt[wPts]);
      if (pt[wPts - 1] != pt[wPts]) BSp.Display(view, deviceContext);

      pstate.SetROP2(deviceContext, iDrawMode);
    } else if (previousKeyDown == ID_OP8 && wPts == 2) {
      int iDrawMode = pstate.SetROP2(deviceContext, R2_XORPEN);
      CVec majorAxis(pt[0], pt[1]);
      CVec minorAxis(pt[0], pt[2]);
      CPrimArc arc(pt[0], majorAxis, minorAxis, TWOPI);
      if (!minorAxis.IsNull()) arc.Display(view, deviceContext);
      pt[2] = app.CursorPosGet();
      minorAxis = CVec(pt[0], pt[2]);
      if (!minorAxis.IsNull()) {
        arc.SetMinAx(minorAxis);
        arc.Display(view, deviceContext);
      }
      pstate.SetROP2(deviceContext, iDrawMode);
    }
  }
  return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}
WORD draw::EndFillAreaDef(WORD wPts, CPnt* pt) {
  auto* document = CPegDoc::GetDoc();

  if (wPts >= 3) {  // Determine plane in which fill area lies
    CVec xAxis(pt[0], pt[1]);
    CVec yAxis(pt[0], pt[2]);
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

    auto* segment = new CSeg(new CPrimPolygon(wPts, pt[wPts], xAxis, yAxis, pt));
    document->WorkLayerAddTail(segment);
    document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, segment);
  }
  return (wPts);
}

CSeg* draw::EndPolylineDef(int numberOfPoints, CPnt* points) {
  auto* document = CPegDoc::GetDoc();

  auto* newSegment = new CSeg(new CPrimPolyline(static_cast<WORD>(numberOfPoints), points));
  document->WorkLayerAddTail(newSegment);
  document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, newSegment);

  return (newSegment);
}

CSeg* draw::EndSplineDef(ESplnGen splineType, int numberOfControlPoints, CPnt* controlPoints) {
  auto* document = CPegDoc::GetDoc();

  CSeg* newSegment = nullptr;

  if (numberOfControlPoints > 2) {
    if (splineType == draw::BSpline) {
      newSegment = new CSeg(new CPrimBSpline(static_cast<WORD>(numberOfControlPoints), controlPoints));
      document->WorkLayerAddTail(newSegment);
      document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, newSegment);
    }
  }
  return (newSegment);
}