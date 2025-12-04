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

namespace draw
{
    EArcGen eArcGenId{StartPoint};
    ESplnGen eSplnGenId{BSpline};
    ESplnEndCnd eSplnEndCndId{Relaxed};
    int iSplnPntsS{8};
}

LRESULT CALLBACK SubProcDraw(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    static	WORD	wPts{0};
    static	WORD	wPrvOp{0};
    static	CPnt	pt[64]{};

    CPegDoc* pDoc{CPegDoc::GetDoc()};
    CPegView* pView{CPegView::GetActiveView()};

    CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

    if (anMsg == WM_COMMAND)
    {
        CSeg* pSeg{nullptr};
        long lResult{0};

        CPnt ptCurPos{app.CursorPosGet()};

        switch (LOWORD(wParam))
        {
        case ID_HELP_KEY:
            WinHelp(hwnd, "peg.hlp", HELP_KEY, reinterpret_cast<DWORD_PTR>("DRAW"));
            return 0;

        case ID_OP0:
            ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_DRAW_OPTIONS), hwnd, DlgProcDrawOptions);
            app.StatusLineDisplay();
            break;

        case ID_OP1:
            pSeg = new CSeg(new CPrimMark(pstate.PenColor(), pstate.MarkStyle(), ptCurPos));
            pDoc->WorkLayerAddTail(pSeg);
            pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
            break;

        case ID_OP2:		// Define and generate one or more lines
            if (wPrvOp != ID_OP2)
            {
                wPrvOp = app.ModeLineHighlightOp(ID_OP2);
                pt[0] = ptCurPos;
            }
            else
            {
                app.RubberBandingDisable();
                pt[1] = UserAxisSnapLn(pt[0], ptCurPos);

                pSeg = new CSeg(new CPrimLine(pt[0], pt[1]));
                pDoc->WorkLayerAddTail(pSeg);
                pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
                pt[0] = pt[1];
            }
            app.RubberBandingStartAtEnable(pt[0], Lines);
            break;

        case ID_OP3:
            if (wPrvOp != ID_OP3)
            {
                wPrvOp = app.ModeLineHighlightOp(ID_OP3);
                wPts = 1;
                pt[0] = ptCurPos;
                app.RubberBandingStartAtEnable(ptCurPos, Lines);
            }
            else
            {
                if (wPts < sizeof(pt) / sizeof(pt[0]))
                {
                    app.RubberBandingDisable();
                    pt[wPts] = ptCurPos;
                    if (pt[wPts - 1] != pt[wPts])
                    {
                        CLine(pt[wPts - 1], pt[wPts]).Display(pView, pDC);
                        app.RubberBandingStartAtEnable(pt[wPts], Lines);
                        wPts++;
                    }
                    else
                    {
                        draw::EndFillAreaDef(wPts, pt);
                        app.ModeLineUnhighlightOp(wPrvOp);
                        wPts = 0;
                    }
                }
                else
                    msgInformation(IDS_MSG_TOO_MANY_PTS);
            }
            break;

        case ID_OP4:
            if (wPrvOp != ID_OP4)
            {
                wPrvOp = app.ModeLineHighlightOp(ID_OP4);
                wPts = 1;
                pt[0] = ptCurPos;
                app.RubberBandingStartAtEnable(pt[0], Lines);
            }
            else
            {
                pt[wPts] = ptCurPos;
                if (pt[wPts - 1] != pt[wPts])
                {
                    if (wPts == 1)
                    {
                        app.RubberBandingEnable(Quads);
                        wPts++;
                    }
                    else
                    {
                        app.RubberBandingDisable();
                        pt[3] = pt[0] + CVec(pt[1], pt[2]);

                        pSeg = new CSeg;

                        for (int i = 0; i < 4; i++)
                            pSeg->AddTail(new CPrimLine(pt[i], pt[(i + 1) % 4]));

                        pDoc->WorkLayerAddTail(pSeg);
                        pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
                        app.ModeLineUnhighlightOp(wPrvOp);
                        wPts = 0;
                    }
                }
                else
                {
                    app.RubberBandingDisable();
                    msgInformation(IDS_MSG_PTS_COINCIDE);
                }
            }
            break;

        case ID_OP5:
            if (wPrvOp != ID_OP5)
            {
                wPrvOp = app.ModeLineHighlightOp(ID_OP5);
                wPts = 1;
                pt[0] = ptCurPos;
                app.RubberBandingStartAtEnable(ptCurPos, Lines);
            }
            else
            {
                pt[wPts] = ptCurPos;
                if (pt[wPts - 1] != pt[wPts])
                {
                    if (wPts == 1)
                    {
                        app.RubberBandingDisable();
                        pt[++wPts] = pt[1];
                    }
                    else
                    {
                        if (draw::eArcGenId == draw::StartPoint)
                        {
                            CPrimArc* pArc = new CPrimArc(pt[0], pt[1], pt[2]);
                            if (pArc->GetSwpAng() != 0.)
                            {
                                pSeg = new CSeg(pArc);
                                pDoc->WorkLayerAddTail(pSeg);
                                pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
                            }
                            else
                            {
                                delete pArc;
                                msgInformation(IDS_MSG_PTS_COLINEAR);
                            }
                        }
                        app.ModeLineUnhighlightOp(wPrvOp);
                        wPts = 0;
                    }
                }
                else
                    msgInformation(IDS_MSG_PTS_COINCIDE);
            }
            break;

        case ID_OP6:
            if (wPrvOp != ID_OP6)
            {	// Intitial control point
                wPrvOp = app.ModeLineHighlightOp(ID_OP6);
                wPts = 1;
                pt[0] = ptCurPos;
                app.RubberBandingStartAtEnable(ptCurPos, Lines);
            }
            else
            {
                if (wPts < sizeof(pt) / sizeof(pt[0]))
                {
                    if (wPts == 1)
                        app.RubberBandingDisable();
                    else
                    {	// rubber banding for splines required display 
                        // in main window proc. It is erased here and displayed there.
                        int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
                        CPrimBSpline BSp(WORD(wPts + 1), pt);
                        BSp.Display(pView, pDC);
                        pstate.SetROP2(pDC, iDrawMode);
                    }
                    pt[wPts] = ptCurPos;
                    if (pt[wPts - 1] != pt[wPts])
                    {
                        wPts++;
                        pt[wPts] = pt[wPts - 1];
                    }
                    else
                    {
                        draw::EndSplineDef(draw::eSplnGenId, wPts, pt);
                        app.ModeLineUnhighlightOp(wPrvOp);
                        wPts = 0;
                    }
                }
                else										// Too many control points
                    msgInformation(IDS_MSG_TOO_MANY_PTS);
            }
            break;

        case ID_OP7:
            if (wPrvOp != ID_OP7)
            {
                wPrvOp = app.ModeLineHighlightOp(ID_OP7);
                pt[0] = ptCurPos;
                app.RubberBandingStartAtEnable(ptCurPos, Circles);
            }
            else
            {
                pt[1] = ptCurPos;
                app.RubberBandingDisable();
                if (pt[0] != ptCurPos)
                {	// Radius not zero
                    CVec vPlnNorm;
                    vPlnNorm = pView->ModelViewGetDirection();
                    CVec vMajAx(pt[0], ptCurPos);
                    pt[1] = Pnt_RotAboutArbPtAndAx(ptCurPos, pt[0], vPlnNorm, HALF_PI);
                    CVec vMinAx(pt[0], pt[1]);

                    pSeg = new CSeg(new CPrimArc(pt[0], vMajAx, vMinAx, TWOPI));
                    pDoc->WorkLayerAddTail(pSeg);
                    pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
                }
                app.ModeLineUnhighlightOp(wPrvOp);
            }
            break;

        case ID_OP8:										// generalized arc
            if (wPrvOp != ID_OP8)
            {
                wPrvOp = app.ModeLineHighlightOp(ID_OP8);
                wPts = 1;
                pt[0] = ptCurPos;
                app.RubberBandingStartAtEnable(ptCurPos, Lines);
            }
            else
            {
                pt[wPts] = ptCurPos;
                if (pt[wPts - 1] != ptCurPos)
                {
                    if (wPts == 1)
                    {
                        pt[2] = pt[0];
                        app.RubberBandingDisable();
                        app.CursorPosSet(pt[0]);
                        wPts++;
                    }
                    else
                    {
                        CVec vMajAx(pt[0], pt[1]);
                        CVec vMinAx(pt[0], pt[2]);

                        pSeg = new CSeg(new CPrimArc(pt[0], vMajAx, vMinAx, TWOPI));
                        pDoc->WorkLayerAddTail(pSeg);
                        pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
                        app.ModeLineUnhighlightOp(wPrvOp);
                        wPts = 0;
                    }
                }
                else
                    msgInformation(IDS_MSG_PTS_COINCIDE);
            }
            break;

        case ID_OP9:
            if (pDoc->BlksSize() > 0)
                ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_INSERT_BLOCK), hwnd, DlgProcBlockInsert);

            break;

        case IDM_RETURN:
            app.RubberBandingDisable();
            if (wPrvOp == ID_OP3)
                draw::EndFillAreaDef(wPts, pt);
            else if (wPrvOp == ID_OP6)
                draw::EndSplineDef(draw::eSplnGenId, wPts, pt);

            app.ModeLineUnhighlightOp(wPrvOp);
            wPts = 0;
            break;

        case ID_SHIFT_RETURN:
            app.RubberBandingDisable();
            if (wPrvOp == ID_OP3) { draw::EndPolylineDef(wPts, pt); }

            app.ModeLineUnhighlightOp(wPrvOp);
            wPts = 0;
            break;

        case IDM_ESCAPE:
            app.RubberBandingDisable();

            if (wPrvOp == ID_OP2)
            {
            }
            else if (wPrvOp == ID_OP3)
            {
                // erase tmp lines of poly
            }
            else if (wPrvOp == ID_OP6)
            {
                // erase tmp lines of poly
            }
            app.ModeLineUnhighlightOp(wPrvOp);
            break;

        default:
            lResult = !lResult;
        }
        if (lResult == 0)
            return (lResult);
    }
    else if (anMsg == WM_MOUSEMOVE)
    {
        if (wPrvOp == ID_OP5 && wPts == 2)
        {
            int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
            if (draw::eArcGenId == draw::StartPoint)
            {
                CPrimArc(pt[0], pt[1], pt[2]).Display(pView, pDC);
                pt[2] = app.CursorPosGet();
                CPrimArc(pt[0], pt[1], pt[2]).Display(pView, pDC);
            }
            pstate.SetROP2(pDC, iDrawMode);
        }
        else if (wPrvOp == ID_OP6 && wPts >= 2)
        {
            int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);

            CPrimBSpline BSp(WORD(wPts + 1), pt);
            if (pt[wPts - 1] != pt[wPts])
                BSp.Display(pView, pDC);
            pt[wPts] = app.CursorPosGet();
            BSp.SetPt(wPts, pt[wPts]);
            if (pt[wPts - 1] != pt[wPts])
                BSp.Display(pView, pDC);

            pstate.SetROP2(pDC, iDrawMode);
        }
        else if (wPrvOp == ID_OP8 && wPts == 2)
        {
            int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);
            CVec vMajAx(pt[0], pt[1]);
            CVec vMinAx(pt[0], pt[2]);
            CPrimArc arc(pt[0], vMajAx, vMinAx, TWOPI);
            if (!vMinAx.IsNull())
                arc.Display(pView, pDC);
            pt[2] = app.CursorPosGet();
            vMinAx = CVec(pt[0], pt[2]);
            if (!vMinAx.IsNull())
            {
                arc.SetMinAx(vMinAx);
                arc.Display(pView, pDC);
            }
            pstate.SetROP2(pDC, iDrawMode);
        }
    }
    return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}
WORD draw::EndFillAreaDef(WORD wPts, CPnt* pt)
{
    CPegDoc* pDoc = CPegDoc::GetDoc();

    if (wPts >= 3)
    {	// Determine plane in which fill area lies
        CVec vXAx(pt[0], pt[1]);
        CVec vYAx(pt[0], pt[2]);
        CVec vPlnNorm(vXAx ^ vYAx);
        vPlnNorm.Normalize();

        if (vPlnNorm[2] < 0)
            vPlnNorm = -vPlnNorm;

        vXAx.Normalize();
        vXAx.RotAboutArbAx(vPlnNorm, hatch::dOffAng);
        vYAx = vXAx;
        vYAx.RotAboutArbAx(vPlnNorm, HALF_PI);
        vXAx *= hatch::dXAxRefVecScal;
        vYAx *= hatch::dYAxRefVecScal;

        // Project reference origin to plane

        CSeg* pSeg = new CSeg(new CPrimPolygon(wPts, pt[wPts], vXAx, vYAx, pt));
        pDoc->WorkLayerAddTail(pSeg);
        pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
    }
    return (wPts);
}

CSeg* draw::EndPolylineDef(int numberOfPoints, CPnt* points)
{
    CPegDoc* document = CPegDoc::GetDoc();

    CSeg* newSegment = new CSeg(new CPrimPolyline(static_cast<WORD>(numberOfPoints), points));
    document->WorkLayerAddTail(newSegment);
    document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, newSegment);

    return (newSegment);
}

CSeg* draw::EndSplineDef(ESplnGen splineType, int numberOfControlPoints, CPnt* controlPoints)
{
    CPegDoc* document = CPegDoc::GetDoc();

    CSeg* newSegment = nullptr;

    if (numberOfControlPoints > 2)
    {
        if (splineType == draw::BSpline)
        {
            newSegment = new CSeg(new CPrimBSpline(static_cast<WORD>(numberOfControlPoints), controlPoints));
            document->WorkLayerAddTail(newSegment);
            document->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, newSegment);
        }
    }
    return (newSegment);
}
