#pragma once

#include <Windows.h>

#include "Pnt.h"
#include "Prim.h"

class CLine;
class CPegView;
class CPrimLine;
class CPrimMark;
class CSeg;

LRESULT CALLBACK SubProcLPD(HWND, UINT, WPARAM, LPARAM) noexcept;

enum	ETransition { None, Begin, End, Both };
enum	EElbow { Mittered, Radial };

namespace lpd
{
    enum    EJust { Left = -1, Center, Right };

    extern EJust	eJust;
    extern EElbow	eElbow;			// elbow type (0 mittered 1 radius)
    extern bool		bContSec;
    extern bool		bGenTurnVanes;	// turning vanes generation flag
    extern double	dSecWid;
    extern double	dSecDep;
    extern double	dInsRadElFac;		// inside radius elbow factor

    bool	Fnd2LnsGivLn(CPrimLine*, double, CPrim*&, CLine*, CPrim*&, CLine*);
    int 	GenElbow(double, double, CLine*);
    void	GenEndCap(const CPnt&, const CPnt&, double, double, CSeg*);
    int 	GenRiseDrop(char, double, double, CLine*);

    /// @brief Generates a section between two lines: creates end caps and line primitives, adds the resulting CSeg to the document's work layer, and updates all views.
    /// @param section_width Width of the section (used for end caps).
    /// @param section_depth Depth of the section (used for end caps).
    /// @param lines An array of two elements of CLine objects; lines[0] and lines[1] are used as the two boundary lines for the section.
    /// (in) set of parallel lines produced using before transition dimensions
    /// (out) set of parallel lines produced using after transition dimensions
    /// @return true if the section was successfully generated and added to the document; false if generation failed (e.g., the first line's length is zero or <= DBL_EPSILON).
    bool	GenSection(double section_width, double section_depth, CLine* lines);
    void	GenSizeNote(CPnt, double, double, double);
    bool	GenTap(CPegView* pView, CPnt, CPnt, double, double, CLine*);
    CPnt	GenTee(CPegView* pView, CPnt, CPnt, double*, double*, CLine*);
    CPnt	GenTakeoff(CPegView* pView, CPnt, CPnt, double*, double*, CLine*);
    int		GenTransition(ETransition, int*, double*, double*, double*, CLine*);

    /// @brief Searches the object's segments for an end-cap mark at the specified point. If a matching mark is found, returns the segment and mark pointers and reports success.
    /// @param view Pointer to the view used for model-to-view transformation and hit testing.
    /// @param model_point The test point (in model coordinates). It is transformed into view space before hit testing; a picture-aperture tolerance (detsegs.PicApertSiz()) is used.
    /// @param segment Output reference that receives the pointer to the segment that contains the found mark. Set to nullptr (0) if no match is found.
    /// @param mark_primitive Output reference that receives the pointer to the found CPrimMark (only marks with PenColor == 15 and MarkStyle == 8 are considered). Set to nullptr (0) if no match is found.
    /// @return true if a matching mark was found at the given point (in which case segment and mark_primitive are set to the found objects); false otherwise (segment and mark_primitive are set to nullptr).
    /// @note Only check for actual end-cap marker is by attributes. No error processing for invalid width or depth values.
    /// Segment data contains whatever primative follows marker (hopefully this is associated end-cap line).
    bool	SelEndCapUsingPoint(CPegView* view, const CPnt& model_point, CSeg*& segment, CPrimMark*& mark_primitive);
    void	SetOptions(double*, double*);
    double	TransitionLen(int aiJus, double adSlo, double* adWid, double* adDep, double* adWidChg, double* adDepChg);
}