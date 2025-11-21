#pragma once

void	DlgProcFileManageDoLayerActive(HWND);
void	DlgProcFileManageDoLayerDelete(HWND);
void	DlgProcFileManageDoLayerHidden(HWND);
void	DlgProcFileManageDoLayerMelt(HWND);
void	DlgProcFileManageDoLayerRename(HWND);
void	DlgProcFileManageDoLayerStatic(HWND);
void	DlgProcFileManageDoLayerWork(HWND);
void	DlgProcFileManageDoTracingCloak(HWND);
void	DlgProcFileManageDoTracingExclude(HWND);
void	DlgProcFileManageDoTracingFuse(HWND);
void	DlgProcFileManageDoTracingInclude(HWND);
void	DlgProcFileManageDoTracingMap(HWND);
void	DlgProcFileManageDoTracingOpen(HWND);
void	DlgProcFileManageDoTracingView(HWND);

void	DlgProcFileManageInit(HWND);

LRESULT	GetCurSel(HWND hDlg, int iListId, CString& strName);
INT_PTR	pGetLayerName();
