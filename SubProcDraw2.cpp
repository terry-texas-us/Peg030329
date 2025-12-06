#include "stdafx.h"

#include <array>

#include "SubProcDraw2.h"

#include "DlgSetLength.h"
#include "Line.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "Pnt.h"
#include "PrimLine.h"
#include "PrimState.h"
#include "Seg.h"
#include "SegsDet.h"
#include "UserAxis.h"
#include "Vec.h"

void SubProcDraw2State::AdjustWallSection(CommandContext& context) {
  saveSegment = beginWallSectionSegment;
  context.document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, beginWallSectionSegment);
  context.beginPoint = beginWallSectionLine->Pt0();
  context.endPoint = beginWallSectionLine->Pt1();

  for (size_t i = 2; i < 4; ++i) {
    if (line::Intersection_xy(CLine(context.beginPoint, context.endPoint), parallelLines[i],
                              context.intersectionPoint)) {
      parallelLines[i][0] = context.intersectionPoint;
    } else {
      continueCorner = false;
    }
  }

  context.linePrimitive = new CPrimLine(*beginWallSectionLine);
  auto vecToLeft = CVec(context.beginPoint, parallelLines[LEFT_NEW][0]).Length();
  auto vecToRight = CVec(context.beginPoint, parallelLines[RIGHT_NEW][0]).Length();

  if (vecToLeft > vecToRight) {
    beginWallSectionLine->SetPt1(parallelLines[RIGHT_NEW][0]);
    context.linePrimitive->SetPt0(parallelLines[LEFT_NEW][0]);
  } else {
    beginWallSectionLine->SetPt1(parallelLines[LEFT_NEW][0]);
    context.linePrimitive->SetPt0(parallelLines[RIGHT_NEW][0]);
  }

  beginWallSectionSegment->AddTail(context.linePrimitive);
  beginWallSectionSegment = nullptr;
}
void SubProcDraw2State::CorrectPreviousLinesAtCorner(CommandContext& context) {
  UpdateIntersections(context.intersectionPoint);
  context.document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, saveSegment);
  delete saveSegment->RemoveTail();
  auto* pos = saveSegment->GetTailPosition();

  auto setPrevPoint = [&](const CPnt& point) {
    context.linePrimitive = static_cast<CPrimLine*>(saveSegment->GetPrev(pos));
    if (context.linePrimitive != nullptr) { context.linePrimitive->SetPt1(point); }
  };
  setPrevPoint(parallelLines[RIGHT_PREV][1]);
  setPrevPoint(parallelLines[LEFT_PREV][1]);
}

void SubProcDraw2State::ProcessCommon(CommandContext& context, CPnt& cursorPosition, CLine& line, bool isSnapped) {
  if (isSnapped) { cursorPosition = UserAxisSnapLn(previousPosition, cursorPosition); }
  parallelLines[LEFT_PREV] = parallelLines[LEFT_NEW];
  parallelLines[RIGHT_PREV] = parallelLines[RIGHT_NEW];
  line = CLine{previousPosition, cursorPosition};
  line.GetParallels(context.scaledDistanceBetweenLines, centerLineEccentricity, parallelLines[LEFT_NEW],
                    parallelLines[RIGHT_NEW]);

  if (continueCorner) {
    CorrectPreviousLinesAtCorner(context);
  } else if (beginWallSectionSegment != nullptr) {
    AdjustWallSection(context);
  } else if (previousKeyDown == ID_OP2) {
    saveSegment = new CSeg;
    context.document->WorkLayerAddTail(saveSegment);
    saveSegment->AddTail(new CPrimLine(parallelLines[LEFT_NEW][0], parallelLines[RIGHT_NEW][0]));
  }
}

void SubProcDraw2State::UpdateIntersections(CPnt& intersectionPoint) {
  for (size_t i = 0; i < 2; ++i) {
    if (line::Intersection_xy(parallelLines[i], parallelLines[i + 2], intersectionPoint)) {
      parallelLines[i][1] = intersectionPoint;
      parallelLines[i + 2][0] = intersectionPoint;
    } else {
      continueCorner = false;
    }
  }
}

void SubProcDraw2State::HandleSetDistanceBetweenLines() {
  CDlgSetLength SetLengthDialog;
  SetLengthDialog.m_strTitle = "Set Distance Between Lines";
  SetLengthDialog.m_dLength = distanceBetweenLines / app.GetScale();
  if (SetLengthDialog.DoModal() == IDOK) { distanceBetweenLines = SetLengthDialog.m_dLength * app.GetScale(); }
}

void SubProcDraw2State::HandleEndcapSearch(CPnt& cursorPosition, CSeg*& segment) {
  segment = detsegs.SelSegAndPrimUsingPoint(cursorPosition);

  if (segment == nullptr) { return; }
  cursorPosition = detsegs.DetPt();
  if (auto* detectedPrimitive = detsegs.DetPrim()) {
    if (auto* linePrimitive = dynamic_cast<CPrimLine*>(detectedPrimitive)) {
      // Line primitive detected .. don't know if it a simple line or part of a wall yet.
      if (previousKeyDown == 0) {
        // Starting on existing line
        beginWallSectionSegment = segment;
        beginWallSectionLine = linePrimitive;
      } else {
        // Ending on existing wall
        endWallSectionSegment = segment;
        endWallSectionLine = linePrimitive;
      }
      app.CursorPosSet(cursorPosition);
    } else {
      segment = nullptr;  // Not a line primitive
    }
  }
}

void SubProcDraw2State::IntoExitingWall(CommandContext& context, CPnt& cursorPosition, CLine& line) {
  ProcessCommon(context, cursorPosition, line, false);

  context.document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, endWallSectionSegment);
  context.beginPoint = endWallSectionLine->Pt0();
  context.endPoint = endWallSectionLine->Pt1();

  saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[LEFT_NEW]));
  saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[RIGHT_NEW]));
  context.document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, saveSegment);

  context.linePrimitive = new CPrimLine(*endWallSectionLine);
  auto dirRel = CLine{previousPosition, cursorPosition}.DirRelOfPt(context.beginPoint);
  if (dirRel < 0.) {
    endWallSectionLine->SetPt1(parallelLines[RIGHT_NEW][1]);
    context.linePrimitive->SetPt0(parallelLines[LEFT_NEW][1]);
  } else {
    endWallSectionLine->SetPt1(parallelLines[LEFT_NEW][1]);
    context.linePrimitive->SetPt0(parallelLines[RIGHT_NEW][1]);
  }

  endWallSectionSegment->AddTail(context.linePrimitive);
  context.document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, endWallSectionSegment);
  endWallSectionSegment = nullptr;

  app.RubberBandingDisable();
  app.ModeLineUnhighlightOp(previousKeyDown);
  continueCorner = false;
}

void SubProcDraw2State::GeneralLineGeneration(CommandContext& context, CPnt& cursorPosition, CLine& line) {
  if (previousKeyDown != 0) {
    ProcessCommon(context, cursorPosition, line, true);

    saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[LEFT_NEW]));
    saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[RIGHT_NEW]));
    saveSegment->AddTail(new CPrimLine(parallelLines[RIGHT_NEW][1], parallelLines[LEFT_NEW][1]));
    context.document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, saveSegment);
    continueCorner = true;
  }
  previousKeyDown = ID_OP2;
  previousPosition = cursorPosition;
  app.CursorPosSet(previousPosition);
  app.RubberBandingStartAtEnable(previousPosition, Lines);
}

void SubProcDraw2State::Reset() {
  app.RubberBandingDisable();
  app.ModeLineUnhighlightOp(previousKeyDown);
  continueCorner = false;
  beginWallSectionSegment = nullptr;
  beginWallSectionLine = nullptr;
  endWallSectionSegment = nullptr;
  endWallSectionLine = nullptr;
}

static SubProcDraw2State state;

static CommandContext context;

LRESULT CALLBACK SubProcDraw2(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept {
  auto line = CLine{};
  CSeg* segment{nullptr};

  if (anMsg == WM_COMMAND) {
    context.document = CPegDoc::GetDoc();
    if (context.document == nullptr) { return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam)); }
    context.beginPoint = CPnt{};
    context.endPoint = CPnt{};
    context.intersectionPoint = CPnt{};
    context.linePrimitive = nullptr;

    double scaleFactor = app.GetScale();
    if (scaleFactor == 0.0) {
      scaleFactor = 1.0;  // Prevent division by zero
    }
    context.scaledDistanceBetweenLines = state.distanceBetweenLines / scaleFactor;

    auto result = long{0};
    auto cursorPosition = app.CursorPosGet();

    switch (LOWORD(wParam)) {
      case ID_OP0:  // Set distance between lines
      {
        state.HandleSetDistanceBetweenLines();
        break;
      }
      case ID_OP1:  // Search for an endcap
        state.HandleEndcapSearch(cursorPosition, segment);
        break;

      case ID_OP2:  // Handle line drawing operation
        if (state.previousKeyDown != 0) { app.RubberBandingDisable(); }
        if (state.endWallSectionSegment != nullptr) {
          state.IntoExitingWall(context, cursorPosition, line);
        } else {
          state.GeneralLineGeneration(context, cursorPosition, line);
        }
        state.previousPosition = cursorPosition;
        break;

      case IDM_RETURN:  // Finish current operation
        if (state.previousKeyDown != 0) { state.Reset(); }
        state.previousPosition = cursorPosition;
        break;

      case IDM_ESCAPE:  // Cancel operation
        state.Reset();
        [[fallthrough]];

      default:  // Forward unhandled messages
        result = !0;
    }
    if (result == 0) { return (result); }
  }
  return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}