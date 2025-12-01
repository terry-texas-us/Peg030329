#pragma once

#include <windows.h>

#include <afxwin.h>

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include <afxstr.h>
#include <atltypes.h>

#include <cfloat>

#include "Pnt.h"
#include "Prim.h"
#include "resource.h"

class CSeg;

/// @brief Defines an enumeration of measurement units: Architectural, Engineering, Feet, Inches, Meters, Millimeters, Centimeters, Decimeters, and Kilometers.
enum EUnits { Architectural, Engineering, Feet, Inches, Meters, Millimeters, Centimeters, Decimeters, Kilometers };

/// @brief Enumeration that identifies a set of shape or primitive types.
enum ERubs { Lines = 1, Rectangles, Circles, Quads };

/// @brief Used to specify which items should be displayed or updated on the status line (at the bottom of the main window). This enum is part of the broader CPegApp class, which manages the application's state and UI interactions.
enum EStatusLineItem { WorkCnt, TrapCnt, Pen, Line, TextHeight, Scale, WndRatio, DimLen, DimAng, All };

extern COLORREF crHotCols[256];
extern COLORREF crWarmCols[16];

extern COLORREF* pColTbl;

extern double dPWids [];

class CPegApp: public CWinApp
{
public:
    WNDPROC		m_wpMainWnd;				// Pointer to main window procedure
    CString		m_strAppPath;
    CFont* m_pFontApp;

private:
    HMENU		m_hMenu;

    UINT		m_nClipboardFormatPegSegs;	// 0
    int 		m_iGinRubTyp;
    int			m_iUnitsPrec;				// 8
    EUnits		m_eUnits;					// Inches
    bool		m_bEditCFImage;
    bool		m_bEditCFSegments;
    bool		m_bEditCFText;
    double		m_dEngAngZ;					// 0.
    double		m_dEngLen;					// 0.
    double		m_dDimAngZ;					// 0.
    double		m_dDimLen;					// 0.
    double		m_dScale;					// 1.
    CPnt		m_ptCursorPosDev;
    CPnt		m_ptRubStart;
    CPnt		m_ptRubEnd;
    CPnt		m_ptEditSegBeg;
    CPnt		m_ptEditSegEnd;
    CPnt		m_ptHomePoint[9];
    WORD		m_wOpHighlighted;
    char* m_pStrokeFontDef;			// 0
    CString		m_strShadowDir;				// "PegAEsys Shadow Files"

public:
    bool		m_bViewStateInfo;
    bool		m_bViewModeInfo;
    bool		m_bTrapModeAddSegments;
    int			m_iModeId;
    int			m_iPrimModeId;
    CPoint		m_pntGinStart;
    CPoint		m_pntGinEnd;
    CPoint		m_pntGinCur;
    CPnt		m_ptCursorPosWorld;

public:
    CPegApp();
    CPegApp(const CPegApp&) = delete; // Disable copy constructor
    CPegApp& operator=(const CPegApp&) = delete; // Disable assignment operator

    CPnt		CursorPosGet();
    void		CursorPosSet(const CPnt& pt);

    void		PenStylesLoad(const CString& strFileName);
    void		PenColorsChoose();
    COLORREF	PenColorsGetHot(PENCOLOR nPenColor) const { return (crHotCols[nPenColor]); }
    void		PenColorsLoad(const CString& strFileName);

    void		PenWidthsLoad(const CString& strFileName);
    double		PenWidthsGet(PENCOLOR nPenColor) const { return (dPWids[nPenColor]); }

    void		HatchesLoad(const CString& strFileName);

    void		InitGbls(CDC* pDC);

    double		GetDimLen() const { return (m_dDimLen); }
    double		GetDimAngZ() const { return (m_dDimAngZ); }
    bool		GetEditCFImage() const { return m_bEditCFImage; }
    bool		GetEditCFSegments() const { return m_bEditCFSegments; }
    bool		GetEditCFText() const { return m_bEditCFText; }
    CPnt		GetEditSegBeg() const { return (m_ptEditSegBeg); }
    CPnt		GetEditSegEnd() const { return (m_ptEditSegEnd); }
    double		GetEngAngZ() const { return (m_dEngAngZ); }
    double		GetEngLen() const { return (m_dEngLen); }
    double		GetScale() const { return m_dScale; }
    EUnits		GetUnits() const { return (m_eUnits); }
    int			GetUnitsPrec() const { return (m_iUnitsPrec); }

    CPnt		HomePointGet(int i) const;
    void		HomePointSave(int i, const CPnt& pt);

    void		SetBackGround(COLORREF cr);
    void		SetDimLen(double d) { m_dDimLen = d; }
    void		SetDimAngZ(double d) { m_dDimAngZ = d; }
    void		SetEditSegBeg(const CPnt& pt) { m_ptEditSegBeg = pt; }
    void		SetEditSegEnd(const CPnt& pt) { m_ptEditSegEnd = pt; }
    void		SetEngAngZ(double d) { m_dEngAngZ = d; }
    void		SetEngLen(double d) { m_dEngLen = d; }
    void		SetModeCursor(int iModeId);
    void		SetScale(double d) { if (d > DBL_EPSILON) m_dScale = d; }
    void		SetUnits(EUnits eUnits) { m_eUnits = eUnits; }
    void		SetUnitsPrec(int iPrec) { if (iPrec > 0) m_iUnitsPrec = iPrec; }
    void		SetWindowMode(int iModeId);

    void		LineFontCreate();
    void		LineFontRelease();

    void		ModeLineDisplay();
    WORD		ModeLineHighlightOp(WORD wOp);
    void		ModeLineUnhighlightOp(WORD& wOp);

    CPnt		RubberBandingGetStart() const { return m_ptRubStart; }
    int			RubberBandingGetType() const { return (m_iGinRubTyp); }
    void		RubberBandingDisable();
    void		RubberBandingEnable(ERubs eRub);
    void		RubberBandingStartAtEnable(const CPnt& pt, ERubs eRub);

    void		StatusLineDisplay(EStatusLineItem = All);

    char* StrokeFontGet() const { return m_pStrokeFontDef; }
    void		StrokeFontLoad(const CString& strPathName);
    void		StrokeFontRelease();

    static WORD GetFileTypeFromPath(const CString& strPathName);
    HINSTANCE	GetInstance() const { return (m_hInstance); }
    WNDPROC		GetMainWndProc() const { return (m_wpMainWnd); }
    UINT		CheckMenuItem(UINT uId, UINT uCheck) { return (::CheckMenuItem(m_hMenu, uId, uCheck)); }
    HMENU		GetMenu() const { return (m_hMenu); }
    HWND		GetSafeHwnd() const { return ((AfxGetApp()->m_pMainWnd)->GetSafeHwnd()); }
    HMENU		GetSubMenu(int nPos) const { return (::GetSubMenu(m_hMenu, nPos)); }
    void		ModifyMenu(UINT, LPCTSTR);
    void		ModifyMenu(UINT uPos, UINT_PTR uIdNewItem, LPCTSTR lpNewItem)
    {
        ::ModifyMenu(m_hMenu, uPos, MF_BYPOSITION | MF_STRING | MF_POPUP, uIdNewItem, lpNewItem);
    }
    UINT		GetClipboardFormatPegSegs() const { return (m_nClipboardFormatPegSegs); }
    CString		GetShadowDir() { return m_strShadowDir; }
    void		SetShadowDir(const CString& strDir);

    // Overrides
public:
    virtual BOOL	InitInstance();

protected:
    afx_msg void OnAppAbout();
public:
    afx_msg void OnEditCfImage();
    afx_msg void OnEditCfSegments();
    afx_msg void OnEditCfText();
    afx_msg void OnFileRun();
    afx_msg void OnHelpContents();
    afx_msg void OnModeAnnotate();
    afx_msg void OnModeCut();
    afx_msg void OnModeDimension();
    afx_msg void OnModeDraw();
    afx_msg void OnModeDraw2();
    afx_msg void OnModeEdit();
    afx_msg void OnModeFixup();
    afx_msg void OnModeLPD();
    afx_msg void OnModeLetter();
    afx_msg void OnModeNodal();
    afx_msg void OnModePipe();
    afx_msg void OnModePower();
    afx_msg void OnModePrimEdit();
    afx_msg void OnModePrimMend();
    afx_msg void OnModeRevise();
    afx_msg void OnModeRLPD();
    afx_msg void OnModeSegEdit();
    afx_msg void OnModeTrap();
    afx_msg void OnTrapCommandsAddSegments();
    afx_msg void OnViewModeInformation();
    afx_msg void OnViewStateInformation();

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnUpdateModeAnnotate(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeCut(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeDimension(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeDraw(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeDraw2(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeFixup(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeLpd(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeNodal(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModePipe(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModePower(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeRlpd(CCmdUI* pCmdUI);
    afx_msg void OnUpdateModeTrap(CCmdUI* pCmdUI);
    afx_msg void OnUpdateViewModeinformation(CCmdUI* pCmdUI);
    afx_msg void OnUpdateViewStateinformation(CCmdUI* pCmdUI);
};

extern CPegApp	app;

bool		AppGetGinCur(CPrim*&);
bool		AppGetGinCur(CSeg*&);
bool		AppGetGinCur(CSeg*&, CPrim*&);
COLORREF	AppGetTextCol();
void		AppSetGinCur(CSeg* = 0, CPrim* = 0);
