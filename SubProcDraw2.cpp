#include "stdafx.h"

#include <array>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "DlgSetLength.h"
#include "Line.h"
#include "Pnt.h"
#include "PrimLine.h"
#include "PrimState.h"
#include "Seg.h"
#include "SegsDet.h"
#include "SubProcDraw2.h"
#include "UserAxis.h"
#include "Vec.h"

struct SubProcDraw2State {
    double distanceBetweenLines{6.0};
    double centerLineEccentricity{0.0};
    WORD previousKeyDown{0};
    bool continueCorner{false};
    CSeg* beginWallSectionSegment{nullptr};
    CSeg* endWallSectionSegment{nullptr};
    CSeg* saveSegment{nullptr};
    CPrimLine* beginWallSectionLine{nullptr};
    CPrimLine* endWallSectionLine{nullptr};
    CPnt previousPosition{};
    std::array<CLine, 4> parallelLines{};

    // Constants for readability
    static constexpr size_t LEFT_PREV = 0;
    static constexpr size_t RIGHT_PREV = 1;
    static constexpr size_t LEFT_NEW = 2;
    static constexpr size_t RIGHT_NEW = 3;

private:
    void ProcessCommon(CPnt& cursorPosition, CLine& line, bool isSnapped, CPegDoc* document, double scaledDistance, CPnt& intersectionPoint, CPrimLine*& linePrimitive, CPnt& beginPoint, CPnt& endPoint)
    {
        if (isSnapped) {
            cursorPosition = UserAxisSnapLn(previousPosition, cursorPosition);
        }
        parallelLines[LEFT_PREV] = parallelLines[LEFT_NEW];
        parallelLines[RIGHT_PREV] = parallelLines[RIGHT_NEW];
        line = CLine{previousPosition, cursorPosition};
        line.GetParallels(scaledDistance, centerLineEccentricity, parallelLines[LEFT_NEW], parallelLines[RIGHT_NEW]);

        if (continueCorner) {
            UpdateIntersections(intersectionPoint);
            document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, saveSegment);
            delete saveSegment->RemoveTail();
            auto* pos = saveSegment->GetTailPosition();
            linePrimitive = static_cast<CPrimLine*>(saveSegment->GetPrev(pos));
            linePrimitive->SetPt1(parallelLines[RIGHT_PREV][1]);
            linePrimitive = static_cast<CPrimLine*>(saveSegment->GetPrev(pos));
            linePrimitive->SetPt1(parallelLines[LEFT_PREV][1]);
        }
        else if (beginWallSectionSegment != nullptr) {
            AdjustWallSection(linePrimitive, beginPoint, endPoint, document, intersectionPoint);
        }
        else if (previousKeyDown == ID_OP2) {
            saveSegment = new CSeg;
            document->WorkLayerAddTail(saveSegment);
            saveSegment->AddTail(new CPrimLine(parallelLines[LEFT_NEW][0], parallelLines[RIGHT_NEW][0]));
        }
    }

public:
    /// @brief Displays a modal dialog that allows the user to set the distance between lines. The dialog is initialized with the current distance (distanceBetweenLines / app.GetScale()); if the user confirms (IDOK), distanceBetweenLines is updated to the dialog value multiplied by app.GetScale().
    void HandleSetDistanceBetweenLines()
    {
        CDlgSetLength SetLengthDialog;
        SetLengthDialog.m_strTitle = "Set Distance Between Lines";
        SetLengthDialog.m_dLength = distanceBetweenLines / app.GetScale();
        if (SetLengthDialog.DoModal() == IDOK) {
            distanceBetweenLines = SetLengthDialog.m_dLength * app.GetScale();
        }
    }

    /// @brief Searches for a segment at the given cursor position and updates the provided references and related application state.
    /// @param segment Reference to a CSeg* that will be updated to the segment selected at cursorPosition. Will be set to nullptr if no segment is found.
    /// @param cursorPosition Reference to a CPnt used as the input search point. If a segment is found, this is updated to the detected point and the application's cursor position is set.
    void HandleEndcapSearch(CSeg*& segment, CPnt& cursorPosition)
    {
        segment = detsegs.SelSegAndPrimUsingPoint(cursorPosition);

        if (segment != nullptr)
        {
            cursorPosition = detsegs.DetPt();
            if (previousKeyDown == 0)
            {	// Starting at existing wall
                beginWallSectionSegment = segment;
                beginWallSectionLine = static_cast<CPrimLine*>(detsegs.DetPrim());
            }
            else
            {	// Ending at existing wall
                endWallSectionSegment = segment;
                endWallSectionLine = static_cast<CPrimLine*>(detsegs.DetPrim());
            }
            app.CursorPosSet(cursorPosition);
        }
    }

    /// @brief Updates intersection points for two pairs of parallel lines. For each pair, if an intersection is found, sets the corresponding endpoints to the intersection point; otherwise sets continueCorner to false.
    /// @param intersectionPoint Reference to a point used to receive the computed intersection. Passed by reference and updated by line::Intersection_xy; when an intersection is found it is assigned to the corresponding line endpoints.
    void UpdateIntersections(CPnt& intersectionPoint)
    {
        for (size_t i = 0; i < 2; ++i) {
            if (line::Intersection_xy(parallelLines[i], parallelLines[i + 2], intersectionPoint)) {
                parallelLines[i][1] = intersectionPoint;
                parallelLines[i + 2][0] = intersectionPoint;
            }
            else {
                continueCorner = false;
            }
        }
    }

    /// @brief Adjusts the beginning wall section by computing intersections with parallel lines, updating endpoints, creating a new line primitive, and appending it to the current wall segment.
    /// @param linePrimitive Reference to a CPrimLine* that will be assigned a newly allocated CPrimLine representing the adjusted wall section. The new object is added to beginWallSectionSegment (ownership transferred there).
    /// @param beginPoint Reference to a CPnt that will be set from beginWallSectionLine->Pt0() and used as one endpoint for intersection and adjustment.
    /// @param endPoint Reference to a CPnt that will be set from beginWallSectionLine->Pt1() and used as the other endpoint for intersection and adjustment.
    /// @param document Pointer to CPegDoc used to notify views (calls UpdateAllViews) about segment erasure before adjusting the wall section.
    /// @param intersectionPoint Reference to a CPnt used as an output for intersection results when testing the current line against parallel lines; updated when intersections are found.
    void AdjustWallSection(CPrimLine*& linePrimitive, CPnt& beginPoint, CPnt& endPoint, CPegDoc* document, CPnt& intersectionPoint)
    {
        saveSegment = beginWallSectionSegment;
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, beginWallSectionSegment);
        beginPoint = beginWallSectionLine->Pt0();
        endPoint = beginWallSectionLine->Pt1();

        for (size_t i = 2; i < 4; ++i) {
            if (line::Intersection_xy(CLine(beginPoint, endPoint), parallelLines[i], intersectionPoint)) {
                parallelLines[i][0] = intersectionPoint;
            }
            else {
                continueCorner = false;
            }
        }

        linePrimitive = new CPrimLine(*beginWallSectionLine);
        auto vecToLeft = CVec(beginPoint, parallelLines[LEFT_NEW][0]).Length();
        auto vecToRight = CVec(beginPoint, parallelLines[RIGHT_NEW][0]).Length();

        if (vecToLeft > vecToRight) {
            beginWallSectionLine->SetPt1(parallelLines[RIGHT_NEW][0]);
            linePrimitive->SetPt0(parallelLines[LEFT_NEW][0]);
        }
        else {
            beginWallSectionLine->SetPt1(parallelLines[LEFT_NEW][0]);
            linePrimitive->SetPt0(parallelLines[RIGHT_NEW][0]);
        }

        beginWallSectionSegment->AddTail(linePrimitive);
        beginWallSectionSegment = nullptr;
    }

    /// @brief Handle the cursor entering an existing wall while drawing: compute parallel lines, update or create segment primitives, adjust endpoints and intersections, refresh the document views, and clear temporary drawing state.
    /// @param cursorPosition The current cursor position (input). Used to compute the active line and its parallels.
    /// @param line Reference to a CLine that will be initice between parallel lines (input).
    void IntoExitingWall(CPnt& cursorPosition, CLine& line)
    {
        auto* document = CPegDoc::GetDoc();
        auto scaledDistanceBetweenLines{distanceBetweenLines / app.GetScale()};

        auto beginPoint{CPnt{}};
        auto endPoint{CPnt{}};
        auto intersectionPoint{CPnt{}};
        CPrimLine* linePrimitive{nullptr};

        ProcessCommon(cursorPosition, line, false, document, scaledDistanceBetweenLines, intersectionPoint, linePrimitive, beginPoint, endPoint);

        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, endWallSectionSegment);
        beginPoint = endWallSectionLine->Pt0();
        endPoint = endWallSectionLine->Pt1();

        saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[LEFT_NEW]));
        saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[RIGHT_NEW]));
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, saveSegment);

        linePrimitive = new CPrimLine(*endWallSectionLine);
        auto dirRel = CLine{previousPosition, cursorPosition}.DirRelOfPt(beginPoint);
        if (dirRel < 0.) {
            endWallSectionLine->SetPt1(parallelLines[RIGHT_NEW][1]);
            linePrimitive->SetPt0(parallelLines[LEFT_NEW][1]);
        }
        else {
            endWallSectionLine->SetPt1(parallelLines[LEFT_NEW][1]);
            linePrimitive->SetPt0(parallelLines[RIGHT_NEW][1]);
        }

        endWallSectionSegment->AddTail(linePrimitive);
        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, endWallSectionSegment);
        endWallSectionSegment = nullptr;

        app.RubberBandingDisable();
        app.ModeLineUnhighlightOp(previousKeyDown);
        continueCorner = false;
    }

    /// @brief Generate parallel line primitives from the current and previous cursor positions, add them to the document, update intersections and views, and prepare the application for continued line placement (rubber-banding). Handles continuing corners, wall-section adjustments, and creating new saved segments as required.
    /// @param cursorPosition Reference to the current cursor position; may be adjusted (snapped) and is used as the new cursor location.
    /// @param line Reference to a CLine that will be set to the line between the previous position and the (possibly snapped) cursor position.
    void GeneralLineGeneration(CPnt& cursorPosition, CLine& line)
    {
        if (previousKeyDown != 0) {
            auto* document = CPegDoc::GetDoc();
            auto scaledDistanceBetweenLines{distanceBetweenLines / app.GetScale()};

            auto beginPoint{CPnt{}};
            auto endPoint{CPnt{}};
            auto intersectionPoint{CPnt{}};
            CPrimLine* linePrimitive{nullptr};

            ProcessCommon(cursorPosition, line, true, document, scaledDistanceBetweenLines, intersectionPoint, linePrimitive, beginPoint, endPoint);

            saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[LEFT_NEW]));
            saveSegment->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), parallelLines[RIGHT_NEW]));
            saveSegment->AddTail(new CPrimLine(parallelLines[RIGHT_NEW][1], parallelLines[LEFT_NEW][1]));
            document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, saveSegment);
            continueCorner = true;
        }
        previousKeyDown = ID_OP2;
        previousPosition = cursorPosition;
        app.CursorPosSet(previousPosition);
        app.RubberBandingStartAtEnable(previousPosition, Lines);
    }

    /// @brief Resets interaction state and clears related flags and pointers.
    void Reset()
    {
        app.RubberBandingDisable();
        app.ModeLineUnhighlightOp(previousKeyDown);
        continueCorner = false;
        beginWallSectionSegment = nullptr;
        beginWallSectionLine = nullptr;
        endWallSectionSegment = nullptr;
        endWallSectionLine = nullptr;
    }
};

static SubProcDraw2State state;

/// @brief Window procedure that handles drawing-related commands and mouse interactions for line/segment operations, updating application state and delegating unhandled messages to the main window procedure.
/// @param hwnd Handle to the window receiving the message.
/// @param anMsg Message identifier (e.g., WM_COMMAND).
/// @param wParam Additional message-specific information (WPARAM). For WM_COMMAND, the low word is the command identifier.
/// @param lParam Additional message-specific information (LPARAM). For WM_COMMAND, often a control handle or pointer depending on context.
/// @return An LRESULT indicating the result of message processing: returns 0 when the function handles the WM_COMMAND action, or the result of CallWindowProc(app.GetMainWndProc(), ...) when the message is forwarded to the main window procedure.
LRESULT CALLBACK SubProcDraw2(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    CSeg* segment{nullptr};
    auto line{CLine{}};

    if (anMsg == WM_COMMAND)
    {
        auto result{long{0}};
        auto cursorPosition{app.CursorPosGet()};

        switch (LOWORD(wParam))
        {
            // a test for non-zero z used to result in error message if 0-9 

        case ID_OP0:
        {
            state.HandleSetDistanceBetweenLines();
            break;
        }
        case ID_OP1: // Search for an endcap in proximity of current location
            state.HandleEndcapSearch(segment, cursorPosition);
            break;

        case ID_OP2:
            if (state.previousKeyDown != 0)
            {
                app.RubberBandingDisable();
            }

            if (state.endWallSectionSegment != nullptr)
            {
                state.IntoExitingWall(cursorPosition, line);
            }
            else
            {
                state.GeneralLineGeneration(cursorPosition, line);
            }
            state.previousPosition = cursorPosition;
            break;

        case IDM_RETURN:
            if (state.previousKeyDown != 0) { state.Reset(); }
            state.previousPosition = cursorPosition;
            break;

        case IDM_ESCAPE:
            state.Reset();
            [[fallthrough]];

        default:
            result = !0;
        }
        if (result == 0) { return (result); }
    }
    return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}