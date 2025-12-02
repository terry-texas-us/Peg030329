#pragma once

#include <Windows.h>

#include "Pnt.h"
#include "Prim.h"

class CLine;
class CPegView;
class CPrimLine;
class CPrimMark;
class CSeg;

LRESULT CALLBACK SubProcRLPD(HWND, UINT, WPARAM, LPARAM) noexcept;

namespace rlpd
{
    enum EJust0 { Left0 = -1, Center0, Right0 };
    enum ETransition { None, Begin, End, Both };

    extern EJust0	eJust0;
    extern bool		bGenCenterline;
    extern bool		bContSec;
    extern double	dSecDiam;
    extern double	dInsideRadiusFactor;

    bool	ClnCornsOnParLns(CLine*, CLine*);
    bool	Fnd2LnsGivLn(CPrimLine*, double, CPrim*&, CLine*, CPrim*&, CLine*);
    int 	GenElbow(double, double, CLine*);
    void	GenEndCap(const CPnt&, const CPnt&, double, double, CSeg*);
    bool	GenSection(double, CLine*);
    void	GenSizeNote(CPnt, double, double);
    bool	GenTap(CPegView* pView, CPnt, CPnt, double, CLine*);
    int		GenTransition(ETransition, int*, double*, double*, double*, CLine*);
    bool	SelEndCapUsingPoint(CPegView* pView, const CPnt&, CSeg*&, CPrimMark*&);

    /// @brief This function acts as a bridge between the application's state and user input.
    /// It initializes a global diameter value, displays a dialog box for user configuration (e.g., allowing the user to input or adjust the duct diameter),
    /// and updates the caller's diameter variable if the dialog completes successfully. 
    /// @param section_diameter the diameter of the duct section to be set or modified.
    void	SetOptions(double* section_diameter);
    double	TransitionLen(int aiJus, double adSlo, double* adWid, double* adDep, double* adWidChg, double* adDepChg);
}
