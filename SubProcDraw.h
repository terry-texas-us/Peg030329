#pragma once

#include <Windows.h>
#include <vector>

#include "Pnt.h"

class CSeg;

class CDrawHandler {
  enum EArcGen { ThreePoint = 1, StartCenterEndPoint };
  enum ESplnGen { BSpline, CSpline };
  enum ESplnEndCnd { Fixed, Relaxed, Cyclic, Anticyclic };

 private:
  struct GeometryState {
    size_t numberOfPoints{0};
    WORD previousKeyDown{0};
    std::vector<CPnt> points{4};

    void reset() {
      numberOfPoints = 0;
      previousKeyDown = 0;
      points.resize(4);
    }
  } state;

  EArcGen eArcGenId{EArcGen::ThreePoint};
  ESplnGen eSplnGenId{ESplnGen::BSpline};
  ESplnEndCnd eSplnEndCndId{ESplnEndCnd::Relaxed};

  /// @brief Displays the modal Draw Options dialog and updates the application's status line.
  /// @param hwnd Handle to the parent window for the dialog.
  void HandleOp0(HWND hwnd);

  /// @brief Creates a mark segment at the given cursor position, adds it to the document's work layer, and notifies views to update.
  /// @param cursorPosition The cursor position where the new mark segment will be created (passed as a const reference).
  void HandleOp1(const CPnt& cursorPosition);

  /// @brief Handles the OP2 drawing operation: starts a new rubber-banded line or, when continuing, finalizes a snapped line segment and adds it to the document. Manages mode-line highlighting, rubber-banding state, and view updates.
  /// @param cursorPosition Current cursor position in document coordinates. On first activation it is recorded as the start point; on subsequent activation it is snapped to the user axis and used as the end point of a new line segment which is created and added to the document.
  void HandleOp2(CPnt cursorPosition);

  /// @brief Handles the OP3 drawing operation by starting or continuing a rubber-banded line sequence. It updates internal state, renders intermediate line segments as the cursor moves, finalizes the shape when appropriate, and reports errors if too many points are provided.
  /// @param cursorPosition The current cursor position used to start a new segment or extend the existing rubber-banded line.
  void HandleOp3(CPnt cursorPosition);

  /// @brief Handles the drawing operation for Op4: collects cursor positions (with rubber-banding) to form a four-point shape, creates a quadrilateral from distinct points, adds the segment to the document work layer, updates views, and manages mode highlighting and informational messages.
  /// @param cursorPosition The current cursor position used as the next vertex (in document coordinates).
  void HandleOp4(CPnt cursorPosition);

  /// @brief Handles the OP5 drawing operation: collects points from the cursor, manages rubber-banding and mode-line highlighting, and when three non-colinear points are provided, constructs an arc segment and adds it to the document. It also reports coincident or colinear point conditions to the user.
  /// @param cursorPosition The current cursor position (CPnt). Treated as the next point in the OP5 sequence; may start or stop rubber-banding, be stored in the internal point buffer, and trigger arc creation and document updates.
  void HandleOp5(CPnt cursorPosition);

  /// @brief Handles operation ID_OP6 for interactive B-spline control-point entry. On the first invocation it initializes rubber-banding and begins collecting control points; on subsequent invocations it updates the control points, toggles rubber-banding or draws a temporary spline using XOR drawing, finalizes the spline when the user repeats a point, and reports an error if the maximum number of control points is exceeded.
  /// @param cursorPosition The current cursor position used as the initial or next B-spline control point.
  void HandleOp6(CPnt cursorPosition);

  /// @brief Handles the user operation for creating a circle (operation 7). On the first invocation it begins rubber-banding and records the center; on the second invocation it finalizes the circle using the second point as a radius, adds the circle segment to the document, and updates views.
  /// @param cursorPosition The current cursor position for this event. On the first call it is stored as the circle center; on the second call it is used as the circumference point to determine the radius.
  void HandleOp7(CPnt cursorPosition);

  /// @brief Handle drawing operation 8: manage point selection and rubber-banding to create a full circular arc (using three points), add the resulting segment to the document, and update the UI state.
  /// @param cursorPosition The current cursor position supplied by the caller; used as the next point for the arc or as the rubber-band position while selecting points.
  void HandleOp8(CPnt cursorPosition);

  /// @brief Checks the current document for available blocks and, if any exist, opens the modal `Insert Block` dialog.
  /// @param hwnd Handle to the owner/parent window for the dialog; used as the dialog's parent when displaying the modal dialog.
  void HandleOp9(HWND hwnd);

  /// @brief Finalize the current drawing action: disable rubber-banding, end any active fill-area or spline definition depending on the previously active operation key, unhighlight the mode-line operation, and reset the working point count.
  void HandleReturn();

  /// @brief Finalize the current polyline drawing action: disable rubber-banding, end the polyline definition if OP3 was previously active, unhighlight the mode-line operation, and reset the working point count.
  void HandleShiftReturn();

  /// @brief Handles an Escape action during drawing: disables rubber-banding, conditionally clears temporary polygon lines for certain previous operations, and unhighlights the mode line for the previous operation.
  void HandleEscape();

  /// @brief Handles mouse-move events to update and display temporary drawing previews (arc, B-spline, or elliptical arc) according to the current drawing mode and point state.
  /// @param cursorPosition Current cursor position (CPnt). Used to update the active primitive's control point(s) (e.g., pt[2] or pt[wPts]) and drive the rubber-band preview.
  /// @param deviceContext Pointer to the device context (CDC*) used for drawing previews. The function temporarily changes the ROP2 mode (to XOR) via pstate to draw and erase transient geometry.
  void HandleMouseMove(CPnt cursorPosition, CDC* deviceContext);

  /// @brief Finalizes a polygonal fill area from a list of points, creates a polygon segment, adds it to the document's work layer, and updates views.
  /// @param wPts The number of points provided in the pt array. If fewer than 3, the function does nothing.
  /// @param pt Pointer to an array of CPnt points representing the vertices of the fill area. The array must contain at least wPts points (the function reads the first three points to determine the plane and uses the array to build the polygon).
  /// @return The value of wPts (the number of points).

  size_t EndFillAreaDef(size_t wPts, std::vector<CPnt>& pts);

  /// @brief Finish creating a polyline segment from an array of points, add it to the document's work layer, and notify views.
  /// @param numberOfPoints The number of points in the polyline. This value is cast to WORD, so it should be non?negative and within the WORD range.
  /// @param points Pointer to the first element of an array of CPnt structures containing the polyline vertices. The array must contain at least numberOfPoints elements.
  /// @return Pointer to the newly created CSeg representing the polyline that was added to the document's work layer.
  CSeg* EndPolylineDef(size_t numberOfPoints, std::vector<CPnt>& points);

  /// @brief Finalize a spline definition. If the spline type is draw::BSpline and there are more than two control points, allocates a new B-spline segment, appends it to the document work layer, and requests view updates.
  /// @param splineType The spline generation type (ESplnGen). Only BSpline is handled by this function.
  /// @param numberOfControlPoints The number of control points provided; a new segment is created only when this value is greater than 2.
  /// @param controlPoints Pointer to an array of CPnt control points used to construct the spline.
  /// @return Pointer to the newly created CSeg representing the spline, or nullptr if no segment was created.
  CSeg* EndSplineDef(ESplnGen splineType, size_t numberOfControlPoints, std::vector<CPnt>& controlPoints);

 public:
  /// @brief Handles Windows messages for the drawing handler: processes WM_COMMAND command IDs by invoking the appropriate operation or control handlers (including help), handles WM_MOUSEMOVE to update drawing state, and forwards messages that are not handled to the original window procedure.
  /// @param hwnd Handle to the window that received the message.
  /// @param anMsg The Windows message code (for example WM_COMMAND or WM_MOUSEMOVE).
  /// @param wParam Additional message information; for WM_COMMAND the command ID is in LOWORD(wParam).
  /// @param lParam Additional message information (pointer or coordinates) whose meaning depends on the message.
  /// @return An LRESULT indicating the result of message processing: typically 0 when this handler handled the message, otherwise the value returned from the original window procedure (CallWindowProc).
  LRESULT HandleMessage(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam);
};

LRESULT CALLBACK SubProcDraw(HWND, UINT, WPARAM, LPARAM) noexcept;
