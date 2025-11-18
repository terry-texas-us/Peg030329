#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgSetText.h"

#include "SubProcAnnotate.h"
#include "UserAxis.h"

void	Polygon_GenPts(const CPnt&, const CVec&, const CVec&, int, CPnt*);

namespace annotate
{
	char	szCurTxt[16];
	char	szDefTxt[16] = "\0";
	double	dGapSpaceFac = .5;				// Edge space factor 25 percent of character height
	double	dHookRad = .03125;				// Hook radius
	int 	iArrowTyp = 1;					// Arrow type
	double	dArrowSiz = .1; 				// Arrow size
	double	dBubRad = .125; 				// Bubble radius
	WORD	wBubFacets = 0; 				// Number of sides on bubble (indicating circle)
}
LRESULT CALLBACK SubProcAnnotate(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	static WORD 	wPrvKeyDwn = 0;
	static CPnt pt[16];

	CSeg*	pSeg;
	CPrimText*	pText;
	double	dD12;
	int 	i;
	CVec	vPlnNorm, vXAx, vYAx;

	CPegDoc* pDoc = CPegDoc::GetDoc();
	CPegView* pView = CPegView::GetActiveView();
	CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

	switch (anMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
				case ID_OP0:
					::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ANNOTATE_OPTIONS), hwnd, DlgProcAnnotateOptions);
					return 0;

				case ID_OP1:									// unused
					return 0;

				case ID_OP2:									// Define and generate one or more lines
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn == 0) 					// No operation pending
					{
						pt[0] = pt[1];
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
					}
					else										// Operation pending
					{
						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON) 							// Points do not coincide
						{
							pSeg = new CSeg;
							if (wPrvKeyDwn == ID_OP2) 
								pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
							else 
							{
								if (wPrvKeyDwn == ID_OP3) 
								{
									annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[1], pt[0], pSeg);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
								}
								else if (wPrvKeyDwn == ID_OP4) 
								{
									if (dD12 > annotate::dBubRad + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dBubRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
									}
								}
								else 
								{
									if (dD12 > annotate::dHookRad + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
									}
								}
								app.ModeLineUnhighlightOp(wPrvKeyDwn);
								wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP2);
							}
							if (pSeg->IsEmpty())
								delete pSeg;
							else
							{
								pDoc->WorkLayerAddTail(pSeg);
								pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
							}
							pt[0] = pt[1];
						}
						else
							msgInformation(0);
					}
					// app.CursorPosSet(pt[0]);
					app.RubberBandingStartAtEnable(pt[0], Lines);
					return 0;

				case ID_OP3:										// Generate line with arrow head
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn == 0) 						// No operation pending
					{
						pt[0] = pt[1];
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP3);
					}
					else											// Operation pending
					{
						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON)
						{	// Two points do not coincide
							pSeg = new CSeg;
							if (wPrvKeyDwn == ID_OP3) 
							{
								annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[1], pt[0], pSeg);
								pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
								annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[0], pt[1], pSeg);
							}
							else 
							{
								if (wPrvKeyDwn == ID_OP2) 
								{
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
									annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[0], pt[1], pSeg);
								}
								else if (wPrvKeyDwn == ID_OP4) 
								{
									if (dD12 > annotate::dBubRad + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dBubRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
										annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[0], pt[1], pSeg);
									}
								}
								else 
								{
									if (dD12 > annotate::dHookRad + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[1]));
										annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[0], pt[1], pSeg);
									}
								}
								app.ModeLineUnhighlightOp(wPrvKeyDwn);
								wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP3);
							}
							if (pSeg->IsEmpty())
								delete pSeg;
							else
							{
								pDoc->WorkLayerAddTail(pSeg);
								pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
							}
							pt[0] = pt[1];
						}
						else		// Two points coincide
							msgInformation(0);
					}
					// app.CursorPosSet(pt[0]);
					app.RubberBandingStartAtEnable(pt[0], Lines);
					return 0;

				case ID_OP4:									// Generate a bubble
				{
					pt[1] = app.CursorPosGet();
					
					CDlgSetText dlg;
					dlg.m_strTitle = "Set Bubble Text";
					dlg.m_sText = annotate::szCurTxt;
					if (dlg.DoModal() == IDOK)
					{
						strcpy(annotate::szCurTxt, dlg.m_sText);
					}
					pSeg = new CSeg;
					if (wPrvKeyDwn == 0 || wPrvKeyDwn == ID_OP4) // No operation pending
						wPrvKeyDwn = ID_OP4;
					else											// Operation pending
					{
						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON) 
						{
							if (wPrvKeyDwn == ID_OP2) 
							{
								if (dD12 > annotate::dBubRad + DBL_EPSILON) 
								{
									pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dBubRad);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
								}
							}
							else if (wPrvKeyDwn == ID_OP3)
							{
								annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[1], pt[0], pSeg);
								if (dD12 > annotate::dBubRad + DBL_EPSILON) 
								{
									pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dBubRad);
									pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
								}
							}
							else 
							{
								if (dD12 > annotate::dHookRad + DBL_EPSILON) 
								{
									pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRad);
									if (CVec(pt[0], pt[1]).Length() > annotate::dBubRad + DBL_EPSILON) 
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dBubRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
							}
							app.ModeLineUnhighlightOp(wPrvKeyDwn);
							wPrvKeyDwn = ID_OP4;
						}
					}
					app.ModeLineHighlightOp(ID_OP4);

					pt[2] = pt[1];
					vPlnNorm = pView->ModelViewGetDirection();
					vYAx = pView->ModelViewGetVwUp();
					vYAx *= .1;
					vXAx = vYAx;
					vXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
					vXAx *= .6;
					CRefSys rs(pt[1], vXAx, vYAx);
					
					if (strlen(annotate::szCurTxt) > 0) 
					{
						int iPrimState = pstate.Save();
						pstate.SetPenColor(2);
						
						CFontDef fd;
						pstate.GetFontDef(fd);
						fd.TextHorAlignSet(CFontDef::HOR_ALIGN_CENTER);
						fd.TextVerAlignSet(CFontDef::VER_ALIGN_MIDDLE);
												
						CCharCellDef ccd;
						pstate.GetCharCellDef(ccd);
						ccd.TextRotAngSet(0.);
						pstate.SetCharCellDef(ccd);
						
						pSeg->AddTail(new CPrimText(fd, rs, annotate::szCurTxt));
						pstate.Restore(pDC, iPrimState);
					}
					vXAx *= annotate::dBubRad / .06;
					vYAx *= annotate::dBubRad / .1;
					
					if (annotate::wBubFacets == 0)						
					{	// Bubble is a circle
						pSeg->AddTail(new CPrimArc(1, 1, pt[1], vXAx, vYAx, TWOPI));
					}
					else					
					{	// Bubble is a polygon
						int iPrimState = pstate.Save();
						pstate.SetPen(pDC, 1, 1);
						pstate.SetPolygonIntStyle(POLYGON_HOLLOW);
						Polygon_GenPts(pt[1], vXAx, vYAx, annotate::wBubFacets + 1, &pt[3]);
						pSeg->AddTail(new CPrimPolygon(WORD(annotate::wBubFacets + 1), pt[1], vXAx, vYAx, &pt[3]));
						pstate.Restore(pDC, iPrimState);
					}
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

					pt[0] = pt[1];
					app.CursorPosSet(pt[0]);
					app.RubberBandingStartAtEnable(pt[0], Lines);
					
					return 0;
				}
				case ID_OP5:										// Generate a hook
					pt[1] = app.CursorPosGet();

					pSeg = new CSeg;

					if (wPrvKeyDwn == 0)
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP5);
					else
					{
						app.RubberBandingDisable();
						dD12 = CVec(pt[0], pt[1]).Length();
						if (dD12 > DBL_EPSILON) 
						{
							if (wPrvKeyDwn == ID_OP5) 
							{
								if (dD12 > annotate::dHookRad + DBL_EPSILON) 
								{
									pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dHookRad);
									if (CVec(pt[0], pt[1]).Length() > annotate::dHookRad + DBL_EPSILON) 
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
							}
							else 
							{
								if (wPrvKeyDwn == ID_OP2) 
								{
									if (dD12 > annotate::dHookRad + DBL_EPSILON) 
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
								else if (wPrvKeyDwn == ID_OP3) 
								{
									annotate::GenArrowHead(annotate::iArrowTyp, annotate::dArrowSiz, pt[1], pt[0], pSeg);
									if (dD12 > annotate::dHookRad + DBL_EPSILON) 
									{
										pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRad);
										pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
									}
								}
								else if (wPrvKeyDwn == ID_OP4) 
								{
									if (dD12 > annotate::dBubRad + DBL_EPSILON) 
									{
										pt[0] = Pnt_ProjPtTo(pt[0], pt[1], annotate::dBubRad);
										if (CVec(pt[0], pt[1]).Length() > annotate::dHookRad + DBL_EPSILON) 
										{
											pt[2] = Pnt_ProjPtTo(pt[1], pt[0], annotate::dHookRad);
											pSeg->AddTail(new CPrimLine(1, 1, pt[0], pt[2]));
										}
									}
								}
								app.ModeLineUnhighlightOp(wPrvKeyDwn);
								wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP5);
							}
						}
					}
					pt[2] = pt[1];
					vPlnNorm = pView->ModelViewGetDirection();
					vYAx = pView->ModelViewGetVwUp();
					vYAx *= annotate::dHookRad;
					//	Rotate view up vector - 90 degress about plane normal vector to get positive x-axis
					vXAx = vYAx;
					vXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
					pSeg->AddTail(new CPrimArc(1, 1, pt[1], vXAx, vYAx, TWOPI));
					pDoc->WorkLayerAddTail(pSeg);
					pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
					
					pt[0] = pt[1];
					app.RubberBandingStartAtEnable(pt[0], Lines);
					return 0;
			
				case ID_OP6:
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn != 0) 
					{
						app.RubberBandingDisable();
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					pText = detsegs.SelTextUsingPoint(pView, pt[1]);
					if (pText != 0)
					{
						CPnts ptsBox;
						pText->GetBoundingBox(ptsBox, annotate::dGapSpaceFac);
						
						pSeg = new CSeg;
						pSeg->AddTail(new CPrimLine(pstate.PenColor(), 1, ptsBox[0], ptsBox[1]));
							
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);

						app.CursorPosSet(pt[1]);
					}
					pt[0] = pt[1];
					return 0;

				case ID_OP7:
					pt[1] = app.CursorPosGet();
					if (wPrvKeyDwn != ID_OP7) 
					{
						if (wPrvKeyDwn != 0) 
						{
							app.RubberBandingDisable();
							app.ModeLineUnhighlightOp(wPrvKeyDwn);
						}
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP7);
						pt[0] = pt[1];
					}
					else 
					{
						CPnts ptsBox1;
						CPnts ptsBox2;
						bool bG1Flg = false;
						bool bG2Flg = false;
						pText = detsegs.SelTextUsingPoint(pView, pt[0]);
						if (pText != 0) 
						{
							pText->GetBoundingBox(ptsBox1, annotate::dGapSpaceFac);
							bG1Flg = true;
						}
						pText = detsegs.SelTextUsingPoint(pView, pt[1]);
						if (pText != 0) 
						{
							pText->GetBoundingBox(ptsBox2, annotate::dGapSpaceFac);
							bG2Flg = true;
						}
						if (bG1Flg && bG2Flg) 
						{
							pt[0] = ptsBox1[0];
							pt[2] = ptsBox1[0];
							for (i = 1; i < 4; i++) 
							{
								pt[0][0] = Min(pt[0][0], ptsBox1[i][0]);
								pt[2][0] = Max(pt[2][0], ptsBox1[i][0]);
								pt[0][1] = Min(pt[0][1], ptsBox1[i][1]);
								pt[2][1] = Max(pt[2][1], ptsBox1[i][1]);
							}
							for (i = 0; i < 4; i++) 
							{
								pt[0][0] = Min(pt[0][0], ptsBox2[i][0]);
								pt[2][0] = Max(pt[2][0], ptsBox2[i][0]);
								pt[0][1] = Min(pt[0][1], ptsBox2[i][1]);
								pt[2][1] = Max(pt[2][1], ptsBox2[i][1]);
							}
							pt[1][0] = pt[2][0];
							pt[1][1] = pt[0][1];
							pt[3][0] = pt[0][0];
							pt[3][1] = pt[2][1];
							
							pSeg = new CSeg;
					
							for (int i = 0; i < 4; i++)
								pSeg->AddTail(new CPrimLine(1, 1, pt[i], pt[(i + 1) % 4]));
							
							pDoc->WorkLayerAddTail(pSeg); 
							pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						}
						app.CursorPosSet(pt[1]);
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					return 0;

				case ID_OP8:
				{
					pt[1] = app.CursorPosGet();
										
					CSeg* pSeg = detsegs.SelLineUsingPoint(pView, pt[1]);
					if (pSeg != 0)
					{
						CPrimLine* pLine = static_cast<CPrimLine*>(detsegs.DetPrim());
						
						pt[1] = detsegs.DetPt();
						
						annotate::szCurTxt[0] = '\0';
						
						CDlgSetText dlg;
						dlg.m_strTitle = "Set Cut-in Text";
						dlg.m_sText = annotate::szCurTxt;
						if (dlg.DoModal() == IDOK)
						{
							strcpy(annotate::szCurTxt, dlg.m_sText);
						}
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_ERASE_SAFE, pSeg);
						
						int iPrimState = pstate.Save();
					
						if (strlen(annotate::szCurTxt) > 0) 
						{
							double dAng = pLine->GetAngAboutZAx();
							if (dAng > .25 * TWOPI && dAng <  .75 * TWOPI)
								dAng += PI;

							vPlnNorm = pView->ModelViewGetDirection();
							vYAx = pView->ModelViewGetVwUp();
							vYAx *= .1;
							vYAx.RotAboutArbAx(vPlnNorm, dAng);
							vXAx = vYAx;
							vXAx.RotAboutArbAx(vPlnNorm, - HALF_PI);
							vXAx *= .6;
							CRefSys rs(pt[1], vXAx, vYAx);
							PENCOLOR nPenColor = pstate.PenColor();
							pstate.SetPenColor(2);
							
							CFontDef fd;
							pstate.GetFontDef(fd);
							fd.TextHorAlignSet(CFontDef::HOR_ALIGN_CENTER);
							fd.TextVerAlignSet(CFontDef::VER_ALIGN_MIDDLE);
														
							CCharCellDef ccd;
							pstate.GetCharCellDef(ccd);
							ccd.TextRotAngSet(0.);
							pstate.SetCharCellDef(ccd);
							
							CPrimText* pText = new CPrimText(fd, rs, annotate::szCurTxt); 
							pstate.SetPenColor(nPenColor);

							pSeg->AddTail(pText);
							CPnts ptsBox;
                            pText->GetBoundingBox(ptsBox, annotate::dGapSpaceFac);
							
							double dGap = CVec(ptsBox[0], ptsBox[1]).Length();

							ptsBox[0] = Pnt_ProjPtTo(pt[1], pLine->Pt0(), dGap / 2.);
							ptsBox[1] = Pnt_ProjPtTo(pt[1], pLine->Pt1(), dGap / 2.);
										
							double dRel[2];
								
							dRel[0] = pLine->RelOfPt(ptsBox[0]);
							dRel[1] = pLine->RelOfPt(ptsBox[1]);

							if (dRel[0] > DBL_EPSILON && dRel[1] < 1. - DBL_EPSILON)
							{
								CPrimLine* pLineNew = new CPrimLine(*pLine);
								pLine->SetPt1(ptsBox[0]);
								pLineNew->SetPt0(ptsBox[1]);
								pSeg->AddTail(pLineNew);
							}
							else if (dRel[0] <= DBL_EPSILON)
								pLine->SetPt0(ptsBox[1]);
							else if (dRel[1] >= 1. - DBL_EPSILON)
								pLine->SetPt1(ptsBox[0]);

							pt[0] = pt[1];
							app.CursorPosSet(pt[0]);
						}
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG, pSeg);
						pstate.Restore(pDC, iPrimState);
					}

					return 0;
				}
				case ID_OP9:										// Generate construction line
					if (wPrvKeyDwn != ID_OP9) 
					{
						pt[0] = app.CursorPosGet();
						wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP9);
					}
					else
					{
						pt[1] = app.CursorPosGet();
						pt[1] = UserAxisSnapLn(pt[0], pt[1]);
					
						pt[2] = Pnt_ProjPtTo(pt[0], pt[1], 48.);
						pt[3] = Pnt_ProjPtTo(pt[2], pt[0], 96.);
					
						pSeg = new CSeg(new CPrimLine(15, 2, pt[2], pt[3]));
						pDoc->WorkLayerAddTail(pSeg);
						pDoc->UpdateAllViews(NULL, CPegDoc::HINT_SEG_SAFE, pSeg);
						app.CursorPosSet(pt[0]);
						app.ModeLineUnhighlightOp(wPrvKeyDwn);
					}
					return 0;
				
				case IDM_RETURN:
					app.RubberBandingDisable();
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					return 0;

				case IDM_ESCAPE:
					if (wPrvKeyDwn != 0) 
					{
						app.RubberBandingDisable();
					}
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					return 0;

			break;
		}
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}
///<summary>Generates arrow heads for annotation mode.</summary>
// Parameters:	aiArrowTyp	type of arrowhead
//				adArrowSiz,
//				ptTail		tail of line segment defining arrow head
//				ptHead		head of line segment defining arrow head
//				pSeg
void annotate::GenArrowHead(int aiArrowTyp, double adArrowSiz, const CPnt& ptTail, const CPnt& ptHead, CSeg* pSeg)
{
	CPegView* pView = CPegView::GetActiveView();
	
	CVec vVwPlnNorm = pView->ModelViewGetDirection();
	if (aiArrowTyp == 1 || aiArrowTyp == 2) 
	{
		double dAng = .244978663127;
		double dLen = adArrowSiz / .970142500145;
		CPnt pt3 = Pnt_ProjPtTo(ptHead, ptTail, dLen);
		CPnt pt4 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, dAng);
		CPnt pt5 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, - dAng);
		pSeg->AddTail(new CPrimLine(1, 1, ptHead, pt4));
		if (aiArrowTyp == 2)
			pSeg->AddTail(new CPrimLine(1, 1, pt4, pt5));
		pSeg->AddTail(new CPrimLine(1, 1, pt5, ptHead));
	}
	else if (aiArrowTyp == 3) 
	{
		double dAng = 9.96686524912e-2;
		double dLen = adArrowSiz / .99503719021;
		CPnt pt3 = Pnt_ProjPtTo(ptHead, ptTail, dLen);
		CPnt pt4 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, dAng);
		pSeg->AddTail(new CPrimLine(1, 1, pt4, ptHead));
	}
	else if (aiArrowTyp == 4) 
	{
		double dAng = .785398163397;
		double dLen = .5 * adArrowSiz / .707106781187;
		CPnt pt3 = Pnt_ProjPtTo(ptHead, ptTail, dLen);
		CPnt pt4 = Pnt_RotAboutArbPtAndAx(pt3, ptHead, vVwPlnNorm, dAng);
		CPnt pt5 = Pnt_RotAboutArbPtAndAx(pt4, ptHead, vVwPlnNorm, PI);
		pSeg->AddTail(new CPrimLine(1, 1, pt4, pt5));
	}
}
