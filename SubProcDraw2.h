#pragma once

#include <Windows.h>

#include <array>

#include "Line.h"
#include "PegAEsysDoc.h"
#include "Pnt.h"

class CPrimLine;
class CSeg;

/// @brief Context structure containing data used during geometric construction operations, such as the associated document, the current line primitive, spacing information, and key points.
struct CommandContext {
  CPnt beginPoint;
  CPnt endPoint;
  CPnt intersectionPoint;
  double scaledDistanceBetweenLines;
  CPrimLine* linePrimitive;
  CPegDoc* document;
};

/// @brief State container and helper operations for the SubProcDraw2 drawing procedure. Encapsulates configuration (distanceBetweenLines, centerLineEccentricity), transient drawing state (previousKeyDown, continueCorner, begin/end wall section pointers, saveSegment, previousPosition, parallelLines) and provides methods to process cursor movement and line construction (ProcessCommon), show a dialog to set the distance between parallel lines (HandleSetDistanceBetweenLines), search for endcap segments at the cursor (HandleEndcapSearch), compute intersections between parallel line pairs (UpdateIntersections), and adjust or finalize wall sections (AdjustWallSection).
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
  /// @brief Adjusts the current wall section by computing intersections with parallel lines, updating endpoints, and creating a line primitive that is appended to the wall segment.
  /// @param context Reference to a CommandContext that provides document access and stores geometry/state used by the adjustment. The function reads and updates context.beginPoint, context.endPoint and context.intersectionPoint, sets context.linePrimitive, and calls context.document->UpdateAllViews(...) as part of the update.
  void AdjustWallSection(CommandContext& context);

  /// @brief Adjusts the end points of previous line primitives at a corner, updates intersections, refreshes document views, and removes the saved segment tail as needed.
  /// @param context Reference to the current command/drawing context. Used to update the intersection point, refresh document views, and set the previous line primitives' end points.
  void CorrectPreviousLinesAtCorner(CommandContext& context);

  /// @brief Perform common processing for line construction: optionally snap the cursor, compute and shift parallel lines, update the provided line, and apply context-specific updates to the document and construction state (may add/remove segments or adjust wall sections).
  /// @param context Reference to the CommandContext used for the operation. The function reads and modifies context fields (e.g., scaledDistanceBetweenLines, intersectionPoint, linePrimitive, begin/end points) and updates the associated document and views.
  /// @param cursorPosition Reference to the current cursor position. If isSnapped is true, the position is snapped and this parameter is modified accordingly.
  /// @param line Reference to a CLine that will be set to the line between previousPosition and cursorPosition and used to compute parallel lines; it is updated by the function.
  /// @param isSnapped Boolean flag indicating whether the cursorPosition should be snapped to the user axis before processing.
  void ProcessCommon(CommandContext& context, CPnt& cursorPosition, CLine& line, bool isSnapped);

  /// @brief Updates intersection points for two pairs of parallel lines. For each pair, if an intersection is found, sets the corresponding endpoints to the intersection point; otherwise sets continueCorner to false.
  /// @param intersectionPoint Reference to a point used to receive the computed intersection. Passed by reference and updated by line::Intersection_xy; when an intersection is found it is assigned to the corresponding line endpoints.
  void UpdateIntersections(CPnt& intersectionPoint);

 public:
  /// @brief Displays a modal dialog that allows the user to set the distance between lines. The dialog is initialized with the current distance (distanceBetweenLines / app.GetScale()); if the user confirms (IDOK), distanceBetweenLines is updated to the dialog value multiplied by app.GetScale().
  void HandleSetDistanceBetweenLines();

  /// @brief Searches for a segment at the given cursor position and updates the provided references and related application state.
  /// @param cursorPosition Reference to a CPnt used as the input search point. If a segment is found, this is updated to the detected point and the application's cursor position is set.
  /// @param segment Reference to a CSeg* that will be updated to the segment selected at cursorPosition. Will be set to nullptr if no segment is found.
  /// @note The detected primitve could be left or right line of a wall section, an endcap line, or a simple line. The special case of endcap lines is not handled here.
  void HandleEndcapSearch(CPnt& cursorPosition, CSeg*& segment);

  /// @brief Finalize and apply changes when the cursor exits a wall during construction: update the end wall section, create and attach the end line primitive, update document views, and adjust construction and application state.
  /// @param context Reference to the current CommandContext that holds document pointers, temporary segments/primitives, and construction state. This function updates fields in the context (for example begin/end points and linePrimitive) and adds primitives/segments to the document.
  /// @param cursorPosition The current cursor position (CPnt) used to compute direction and decide which of the parallel lines to use when finalizing the wall end.
  /// @param line The current CLine being processed; passed into common processing and used as the line context for finalizing the wall section.
  void IntoExitingWall(CommandContext& context, CPnt& cursorPosition, CLine& line);

  /// @brief Prepare and update state for general line drawing. If a previous key is down, process common drawing actions, append new line primitives to the save segment, update document views, and mark that a corner continuation should occur. Then set the previous key/position, update the application cursor, and start rubber-banding for line drawing.
  /// @param context Reference to the construction context providing access to document, drawing state, and helpers used to process and update views.
  /// @param cursorPosition Reference to the current cursor position; used to set the previous position and as the starting point for rubber-banding.
  /// @param line The line object involved in the current construction; passed to ProcessCommon for shared handling.
  void GeneralLineGeneration(CommandContext& context, CPnt& cursorPosition, CLine& line);

  /// @brief Resets interaction state and clears related flags and pointers.
  void Reset();
};

/// @brief Window procedure that handles drawing-related messages and commands for the SubProcDraw2 drawing mode. It processes Windows messages (for example WM_COMMAND) to manage selection, line/segment creation and modification, rubber-banding, view updates, and related state.
/// @param hwnd Handle to the window that receives the message.
/// @param anMsg The message identifier (for example WM_COMMAND, WM_PAINT). Determines which action the procedure should perform.
/// @param wParam Message-specific additional information. For WM_COMMAND this typically contains the control or command identifier in the low-order word and notification in the high-order word.
/// @param lParam Message-specific additional information; often a pointer or handle (for example control handle for WM_COMMAND) or coordinates for input messages.
/// @return An LRESULT value whose meaning depends on the processed message. Returns a message-specific result according to the Windows API convention (often zero when the message is handled).
LRESULT CALLBACK SubProcDraw2(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept;
