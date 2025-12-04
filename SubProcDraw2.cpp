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
#include "UserAxis.h"
#include "Vec.h"

static double p2LNdDisBetLns{6.0};		// Distance between parallel line (user units)
static double p2LNdCenLnEcc{0.0}; 		// Center line eccentricity for parallel lines

LRESULT CALLBACK SubProcDraw2(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    static WORD	wPrvKeyDwn{0};

    static	bool bContCorn{false};
    static	CSeg* pSegBegWallSec{nullptr};
    static	CSeg* pSegEndWallSec{nullptr};
    static	CSeg* pSegSav{nullptr};
    static	CPrimLine* pLineBegWallSec{nullptr};
    static	CPrimLine* pLineEndWallSec{nullptr};
    static	CPnt ptPrvPos{};
    static auto lnPar{std::array<CLine, 4>{}};

    CSeg* pSeg{nullptr};
    auto ln{CLine{}};
    auto ptEnd{CPnt{}};
    auto ptBeg{CPnt{}};
    auto ptInt{CPnt{}};

    auto* document{CPegDoc::GetDoc()};

    if (anMsg == WM_COMMAND)
    {
        CPrimLine* pLine{nullptr};
        auto dDisBetLns{p2LNdDisBetLns / app.GetScale()};
        auto lResult{long{0}};
        auto ptCurPos{app.CursorPosGet()};

        switch (LOWORD(wParam))
        {
            // a test for non-zero z used to result in error message if 0-9 

        case ID_OP0:
        {
            CDlgSetLength dlg;
            dlg.m_strTitle = "Set Distance Between Lines";
            dlg.m_dLength = p2LNdDisBetLns / app.GetScale();

            if (dlg.DoModal() == IDOK)
            {
                p2LNdDisBetLns = dlg.m_dLength * app.GetScale();
            }
            break;
        }
        case ID_OP1:										// Search for an endcap in proximity of current location
            pSeg = detsegs.SelSegAndPrimUsingPoint(ptCurPos);

            if (pSeg != nullptr)
            {
                ptCurPos = detsegs.DetPt();
                if (wPrvKeyDwn == 0)
                {	// Starting at existing wall
                    pSegBegWallSec = pSeg;
                    pLineBegWallSec = static_cast<CPrimLine*>(detsegs.DetPrim());
                }
                else
                {	// Ending at existing wall
                    pSegEndWallSec = pSeg;
                    pLineEndWallSec = static_cast<CPrimLine*>(detsegs.DetPrim());
                }
                app.CursorPosSet(ptCurPos);
            }
            break;

        case ID_OP2:
            if (wPrvKeyDwn != 0)
            {
                app.RubberBandingDisable();
            }
            if (pSegEndWallSec != nullptr) 					// Into existing wall
            {
                lnPar[0] = lnPar[2];
                lnPar[1] = lnPar[3];

                ln = CLine{ptPrvPos, ptCurPos};
                ln.GetParallels(dDisBetLns, p2LNdCenLnEcc, lnPar[2], lnPar[3]);
                if (bContCorn)
                {	// Clean up previous set
                    for (size_t i{0}; i < 2; i++)
                    {
                        if (line::Intersection_xy(lnPar[i], lnPar[i + 2], ptInt))
                        {
                            lnPar[i][1] = ptInt;
                            lnPar[i + 2][0] = ptInt;
                        }
                        else
                        {   // Lines are parallel
                            bContCorn = false;
                        }
                    }
                    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSav);

                    pSegSav->RemoveTail();
                    auto* pos = pSegSav->GetTailPosition();
                    pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
                    pLine->SetPt1(lnPar[1][1]);
                    pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
                    pLine->SetPt1(lnPar[0][1]);
                }
                else if (pSegBegWallSec != nullptr)
                {
                    pSegSav = pSegBegWallSec;

                    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, pSegBegWallSec);
                    ptBeg = pLineBegWallSec->Pt0();
                    ptEnd = pLineBegWallSec->Pt1();
                    for (size_t i = 2; i < 4; i++)
                    {
                        if (line::Intersection_xy(CLine(ptBeg, ptEnd), lnPar[i], ptInt))
                        {
                            lnPar[i][0] = ptInt;
                        }
                        else										// Lines are parallel
                        {
                            bContCorn = false;
                        }
                    }
                    pLine = new CPrimLine(*pLineBegWallSec);

                    if (CVec(ptBeg, lnPar[2][0]).Length() > CVec(ptBeg, lnPar[3][0]).Length())
                    {
                        pLineBegWallSec->SetPt1(lnPar[3][0]);
                        pLine->SetPt0(lnPar[2][0]);
                    }
                    else
                    {
                        pLineBegWallSec->SetPt1(lnPar[2][0]);
                        pLine->SetPt0(lnPar[3][0]);
                    }
                    pSegBegWallSec->AddTail(pLine);
                    pSegBegWallSec = nullptr;
                }
                else if (wPrvKeyDwn == ID_OP2)
                {
                    pSegSav = new CSeg;
                    document->WorkLayerAddTail(pSegSav);
                    pSegSav->AddTail(new CPrimLine(lnPar[2][0], lnPar[3][0]));
                }
                document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, pSegEndWallSec);
                ptBeg = pLineEndWallSec->Pt0();
                ptEnd = pLineEndWallSec->Pt1();

                pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[2]));
                pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[3]));
                document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSegSav);

                pLine = new CPrimLine(*pLineEndWallSec);
                if (CLine{ptPrvPos, ptCurPos}.DirRelOfPt(ptBeg) < 0.)
                {
                    pLineEndWallSec->SetPt1(lnPar[3][1]);
                    pLine->SetPt0(lnPar[2][1]);
                }
                else
                {
                    pLineEndWallSec->SetPt1(lnPar[2][1]);
                    pLine->SetPt0(lnPar[3][1]);
                }
                pSegEndWallSec->AddTail(pLine);
                document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSegEndWallSec);
                pSegEndWallSec = nullptr;

                app.RubberBandingDisable();
                app.ModeLineUnhighlightOp(wPrvKeyDwn);
                bContCorn = false;
            }
            else
            {
                if (wPrvKeyDwn != 0)
                {
                    ptCurPos = UserAxisSnapLn(ptPrvPos, ptCurPos);
                    lnPar[0] = lnPar[2];
                    lnPar[1] = lnPar[3];
                    ln = CLine{ptPrvPos, ptCurPos};
                    ln.GetParallels(dDisBetLns, p2LNdCenLnEcc, lnPar[2], lnPar[3]);

                    if (bContCorn)
                    {
                        for (size_t i{0}; i < 2; i++)
                        {
                            if (line::Intersection_xy(lnPar[i], lnPar[i + 2], ptInt))
                            {
                                lnPar[i][1] = ptInt;
                                lnPar[i + 2][0] = ptInt;
                            }
                            else									// Lines are parallel
                            {
                                bContCorn = false;
                            }
                        }
                        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, pSegSav);
                        delete pSegSav->RemoveTail();
                        auto* pos = pSegSav->GetTailPosition();
                        pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
                        pLine->SetPt1(lnPar[1][1]);
                        pLine = static_cast<CPrimLine*>(pSegSav->GetPrev(pos));
                        pLine->SetPt1(lnPar[0][1]);
                    }
                    else if (pSegBegWallSec != nullptr)
                    {
                        pSegSav = pSegBegWallSec;

                        document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_ERASE_SAFE, pSegBegWallSec);
                        ptBeg = pLineBegWallSec->Pt0();
                        ptEnd = pLineBegWallSec->Pt1();
                        for (size_t i{2}; i < 4; i++)
                        {
                            if (line::Intersection_xy(CLine(ptBeg, ptEnd), lnPar[i], ptInt))
                            {
                                lnPar[i][0] = ptInt;
                            }
                            else							// Lines are parallel
                            {
                                bContCorn = false;
                            }
                        }
                        pLine = new CPrimLine(*pLineBegWallSec);

                        if (CVec(ptBeg, lnPar[2][0]).Length() > CVec(ptBeg, lnPar[3][0]).Length())
                        {
                            pLineBegWallSec->SetPt1(lnPar[3][0]);
                            pLine->SetPt0(lnPar[2][0]);
                        }
                        else
                        {
                            pLineBegWallSec->SetPt1(lnPar[2][0]);
                            pLine->SetPt0(lnPar[3][0]);
                        }
                        pSegBegWallSec->AddTail(pLine);
                        pSegBegWallSec = nullptr;
                    }
                    else if (wPrvKeyDwn == ID_OP2)
                    {
                        pSegSav = new CSeg;
                        document->WorkLayerAddTail(pSegSav);
                        pSegSav->AddTail(new CPrimLine(lnPar[2][0], lnPar[3][0]));
                    }
                    pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[2]));
                    pSegSav->AddTail(new CPrimLine(pstate.PenColor(), pstate.PenStyle(), lnPar[3]));

                    pSegSav->AddTail(new CPrimLine(lnPar[3][1], lnPar[2][1]));
                    document->UpdateAllViews(nullptr, CPegDoc::HINT_SEG_SAFE, pSegSav);
                    bContCorn = true;
                }
                wPrvKeyDwn = ID_OP2;
                ptPrvPos = ptCurPos;
                app.CursorPosSet(ptPrvPos);
                app.RubberBandingStartAtEnable(ptPrvPos, Lines);
            }
            ptPrvPos = ptCurPos;
            break;

        case IDM_RETURN:
            if (wPrvKeyDwn != 0)
            {
                app.RubberBandingDisable();
                app.ModeLineUnhighlightOp(wPrvKeyDwn);
                bContCorn = false;
                pSegBegWallSec = nullptr;
                pLineBegWallSec = nullptr;
                pSegEndWallSec = nullptr;
                pLineEndWallSec = nullptr;
            }
            ptPrvPos = ptCurPos;
            break;

        case IDM_ESCAPE:
            app.RubberBandingDisable();
            app.ModeLineUnhighlightOp(wPrvKeyDwn);
            bContCorn = false;
            pSegBegWallSec = nullptr;
            pLineBegWallSec = nullptr;
            pSegEndWallSec = nullptr;
            pLineEndWallSec = nullptr;
            [[fallthrough]];

        default:
            lResult = !0;
        }
        if (lResult == 0) { return (lResult); }
    }
    return(CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}
