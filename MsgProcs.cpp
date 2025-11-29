#include "stdafx.h"

#include "PegAEsys.h"

#include "ddeGItms.h"
#include "DlgProcEditOps.h"
#include "DlgSetLength.h"
#include "ExpProcs.h"
#include "FileJob.h"
#include "UnitsString.h"

CSeg* pGinCurSeg;
CPrim* pGinCurPrim;
extern CTMat tmEditSeg;

double DlgBoxGetItemDouble(HWND hDlg, int iCtrlId)
{
	char szBuf[32];

	::GetDlgItemText(hDlg, iCtrlId, (LPSTR) szBuf, 32);
	return (atof(szBuf));
}				

double DlgBoxGetItemUnitsText(HWND hDlg, int iCtrlId)
{
	char szBuf[32];

	::GetDlgItemText(hDlg, iCtrlId, (LPSTR) szBuf, 32);
	return (UnitsString_ParseLength(app.GetUnits(), szBuf));
}				

void DlgBoxSetItemDouble(HWND hDlg, int iCtrlId, double dVal)
{
	CString str;
	str.Format("%f", dVal);	
	SetDlgItemText(hDlg, iCtrlId, str);
}

void DlgBoxSetItemUnitsText(HWND hDlg, int iCtrlId, double dVal)
{
	char szBuf[32];
	UnitsString_FormatLength(szBuf, sizeof(szBuf), app.GetUnits(), dVal);
	SetDlgItemText(hDlg, iCtrlId, szBuf);
}
bool AppGetGinCur(CPrim*& pPrim)
{
	pPrim = pGinCurPrim;
	return (pPrim != 0);
}
bool AppGetGinCur(CSeg*& pSeg)
{
	pSeg = pGinCurSeg;
	return (pSeg != 0);
}
bool AppGetGinCur(CSeg*& pSeg, CPrim*& pPrim)
{
	pSeg = pGinCurSeg;
	pPrim = pGinCurPrim;
	return (pSeg != 0);
}
void AppSetGinCur(CSeg* pSeg, CPrim* pPrim)
{
	tmEditSeg.Identity();
	
	pGinCurSeg = pSeg;
	pGinCurPrim = pPrim;
}
INT_PTR pGetLayerName()
{
	return ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_GET_LAYER_NAME), app.GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcGetLayerName));
}
