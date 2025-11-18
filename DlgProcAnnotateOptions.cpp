#include "stdafx.h"

#include "SubProcAnnotate.h"

BOOL CALLBACK DlgProcAnnotateOptions(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM  )
{
	char	*szArrTyp[] = 
		{"Open", "Closed", "Mark", "Hash"};
	int 	i;
	
	switch (anMsg) 
	{
		case WM_INITDIALOG:
			SetDlgItemText(hDlg, IDC_ANN_DEF_TXT, annotate::szDefTxt);
			DlgBoxSetItemDouble(hDlg, IDC_ANN_GAP_SPACE_FAC, annotate::dGapSpaceFac);
			DlgBoxSetItemDouble(hDlg, IDC_ANN_HOOK_RAD, annotate::dHookRad);
			for (i = 0; i < sizeof(szArrTyp) / sizeof(szArrTyp[0]); i++)
				::SendDlgItemMessage(hDlg, IDC_ANN_ARR_TYP, CB_ADDSTRING, 0, (LPARAM) (LPCSTR) szArrTyp[i]);
			::SendDlgItemMessage(hDlg, IDC_ANN_ARR_TYP, CB_SETCURSEL, (WPARAM) (annotate::iArrowTyp - 1), 0L);
			DlgBoxSetItemDouble(hDlg, IDC_ANN_ARR_SIZ, annotate::dArrowSiz);
			DlgBoxSetItemDouble(hDlg, IDC_ANN_BUB_RAD, annotate::dBubRad);
			SetDlgItemInt(hDlg, IDC_ANN_BUB_FACETS, annotate::wBubFacets, FALSE);
			return (TRUE);

		case WM_COMMAND:

		switch (LOWORD(wParam)) 
		{
			case IDOK:
				::GetDlgItemText(hDlg, IDC_ANN_DEF_TXT, annotate::szDefTxt, sizeof(annotate::szDefTxt));
				annotate::dGapSpaceFac = DlgBoxGetItemDouble(hDlg, IDC_ANN_GAP_SPACE_FAC);
				annotate::dHookRad = DlgBoxGetItemDouble(hDlg, IDC_ANN_HOOK_RAD);
				annotate::iArrowTyp = (int) ::SendDlgItemMessage(hDlg, IDC_ANN_ARR_TYP, CB_GETCURSEL, 0, 0L) + 1;
				annotate::dArrowSiz = DlgBoxGetItemDouble(hDlg, IDC_ANN_ARR_SIZ);
				annotate::dBubRad = DlgBoxGetItemDouble(hDlg, IDC_ANN_BUB_RAD);
				annotate::wBubFacets = (WORD) GetDlgItemInt(hDlg, IDC_ANN_BUB_FACETS, 0, FALSE);								
			
			case IDCANCEL:
				::EndDialog(hDlg, TRUE);
				return (TRUE);
		}
		break;
	}
	return (FALSE); 		
}

