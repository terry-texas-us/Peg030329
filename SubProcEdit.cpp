#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "DlgProcEditOps.h"

// Edits segments in trap
LRESULT CALLBACK SubProcEdit(HWND hwnd, UINT anMsg, WPARAM wParam, LPARAM lParam)
{
	static CPnt pt;

	static int	iPts;
	static WORD wPrvKeyDwn = 0;

	if (anMsg == WM_COMMAND)
	{
		CTMat	tm;
		
		char	szKey[] = "EDIT";
		
		switch (LOWORD(wParam))
		{
			case ID_HELP_KEY:
				WinHelp(hwnd, "peg.hlp", HELP_KEY, (DWORD) (LPSTR) szKey);
				return 0;
			
			case ID_OP0:
				::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_EDIT_OPTIONS), hwnd, DlgProcEditOps);
				return 0;

			case ID_OP1:		// Reposition trap pivot point
				pt = app.CursorPosGet();
				trapsegs.PvtPt() = pt;
				// pSetSegPos(pTRAP_PVT_MRK_ID, pt);
				return 0;

			case ID_OP2:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm *= dlgproceditops::GetRotTrnMat();
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;
			
			case ID_OP3:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm *= dlgproceditops::GetInvertedRotTrnMat();
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;
				
			case ID_OP4:
				pt = app.CursorPosGet();
				if (wPrvKeyDwn != ID_OP4) 
				{
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP4);
					app.RubberBandingStartAtEnable(pt, Lines);
				}
				else 
				{
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					app.RubberBandingDisable();
					tm.Translate(CVec(trapsegs.PvtPt(), pt));
					trapsegs.TransformSegs(tm);
				}
				trapsegs.PvtPt() = pt;
				// pSetSegPos(pTRAP_PVT_MRK_ID, pt);
				return 0;
	
			case ID_OP5:
				pt = app.CursorPosGet();
				if (wPrvKeyDwn != ID_OP5) 
				{
					wPrvKeyDwn = app.ModeLineHighlightOp(ID_OP5);
					app.RubberBandingStartAtEnable(pt, Lines);
				}
				else 
				{
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					app.RubberBandingDisable();
					trapsegs.Copy(CVec(trapsegs.PvtPt(), pt));
				}		
				trapsegs.PvtPt() = pt;
				// pSetSegPos(pTRAP_PVT_MRK_ID, pt);
				return 0;

			case ID_OP6:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetMirrorScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case ID_OP7:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetInvertedScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;
			
			case ID_OP8:
				tm.Translate(ORIGIN - trapsegs.PvtPt());
				tm.Scale(dlgproceditops::GetScale());
				tm.Translate(trapsegs.PvtPt());
				trapsegs.TransformSegs(tm);
				return 0;

			case IDM_RETURN:
				return 0;

			case IDM_ESCAPE:
				if (wPrvKeyDwn == ID_OP4 || wPrvKeyDwn == ID_OP5) 
				{
					app.ModeLineUnhighlightOp(wPrvKeyDwn);
					app.RubberBandingDisable();
				}		
				return 0;
		}
	}
	return (CallWindowProc(app.GetMainWndProc(), hwnd, anMsg, wParam, lParam));
}
