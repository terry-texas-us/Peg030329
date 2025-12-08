#include "stdafx.h"

#include <afxstr.h>

#include <algorithm>

#include <string.h>

#include "Prim.h"
#include "SegsTrap.h"

HTREEITEM tvAddItem(HWND hTree, HTREEITEM hParent, LPCTSTR pszText, CObject* pOb)
{
    TV_INSERTSTRUCT tvIS{ };
    tvIS.hParent = hParent;
    tvIS.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvIS.hInsertAfter = TVI_LAST;
    tvIS.item.pszText = (LPTSTR)pszText;
    tvIS.item.lParam = (LPARAM)(LPCSTR)pOb;
    return TreeView_InsertItem(hTree, &tvIS);
}

void DlgProcEditTrap_CommandsQueryFillExtraList(HWND hDlg, CPrim* pPrim)
{
    HWND hWndExtra = ::GetDlgItem(hDlg, IDC_EXTRA_LIST_CTRL);

    LVITEM lvi{};
    lvi.mask = LVIF_TEXT | LVIF_STATE;
    lvi.iSubItem = 0;

    int iItem = 0;

    CString str;
    pPrim->FormatExtra(str);

    int nOff = 0;
    const int strLen = str.GetLength();
    while (nOff < strLen) {
      // find property name terminated by ';'
      int nameEnd = str.Find(';', nOff);
      if (nameEnd == -1) break;  // no more complete property entries

      CString name = str.Mid(nOff, nameEnd - nOff);

      lvi.iItem = iItem;
      // ListView_InsertItem reads the text immediately, CString stays valid for this call
      lvi.pszText = const_cast<LPTSTR>((LPCTSTR)name);
      ListView_InsertItem(hWndExtra, &lvi);

      // advance to value
      nOff = nameEnd + 1;
      if (nOff >= strLen) break;

      // value ends at tab or end-of-string
      int valEnd = str.Find('\t', nOff);
      CString value;
      if (valEnd == -1)
        value = str.Mid(nOff);  // remainder is the final value
      else
        value = str.Mid(nOff, valEnd - nOff);

      ListView_SetItemText(hWndExtra, iItem, 1, const_cast<LPTSTR>((LPCTSTR)value))
      ++iItem;

      if (valEnd == -1) break;  // parsed final value, no trailing tab -> done

      // advance past tab to next property
      nOff = valEnd + 1;
    }
}

void DlgProcEditTrap_CommandsQueryFillGeometryList(HWND hDlg, CPrim* pPrim)
{
    HWND hWndGeometry = ::GetDlgItem(hDlg, IDC_GEOMETRY_LIST);

    LVITEM lvi{};

    lvi.mask = LVIF_TEXT | LVIF_STATE;

    char szBuf[64]{};
    int iItem = 0;

    CString strBuf;
    pPrim->FormatGeometry(strBuf);

    int nOff = 0;
    for (int nDel = strBuf.Mid(nOff).Find(';'); nDel != -1;)
    {
        lvi.iItem = iItem;
        strcpy_s(szBuf, sizeof(szBuf), strBuf.Mid(nOff, nDel));
        lvi.pszText = szBuf;
        ListView_InsertItem(hWndGeometry, &lvi);
        nOff += nDel + 1;
        nDel = strBuf.Mid(nOff).Find(';');
        strcpy_s(szBuf, sizeof(szBuf), strBuf.Mid(nOff, nDel));
        ListView_SetItemText(hWndGeometry, iItem, 1, szBuf)
        nOff += nDel + 1;
        nDel = strBuf.Mid(nOff).Find(';');
        strcpy_s(szBuf, sizeof(szBuf), strBuf.Mid(nOff, nDel));
        ListView_SetItemText(hWndGeometry, iItem, 2, szBuf)
        nOff += nDel + 1;
        nDel = strBuf.Mid(nOff).Find('\t');
        strcpy_s(szBuf, sizeof(szBuf), strBuf.Mid(nOff, nDel));
        ListView_SetItemText(hWndGeometry, iItem++, 3, szBuf)
        nOff += nDel + 1;
        nDel = strBuf.Mid(nOff).Find(';');
    }
}

INT_PTR CALLBACK DlgProcEditTrap_CommandsQuery(HWND hDlg, UINT nMsg, WPARAM wParam, LPARAM lParam) noexcept
{
    HWND hWndSegmentTree = ::GetDlgItem(hDlg, IDC_SEGMENT_TREE);
    HWND hWndExtra = ::GetDlgItem(hDlg, IDC_EXTRA_LIST_CTRL);
    HWND hWndGeometry = ::GetDlgItem(hDlg, IDC_GEOMETRY_LIST);
    switch (nMsg)
    {
    case WM_INITDIALOG:
    {
        HTREEITEM htiSegmentList = tvAddItem(hWndSegmentTree, TVI_ROOT, "<Segments>", (CObject*)&trapsegs);

        trapsegs.AddToTreeViewControl(hWndSegmentTree, htiSegmentList);

        LVCOLUMN lvc{};
        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
        lvc.fmt = LVCFMT_LEFT;
        lvc.cx = 75;
        lvc.iSubItem = 0; lvc.pszText = (LPTSTR)"Property";
        ListView_InsertColumn(hWndExtra, 0, &lvc);
        lvc.iSubItem = 1; lvc.pszText = (LPTSTR)"Value";
        ListView_InsertColumn(hWndExtra, 1, &lvc);

        lvc.iSubItem = 0; lvc.pszText = (LPTSTR)"Property";
        ListView_InsertColumn(hWndGeometry, 0, &lvc);
        lvc.iSubItem = 1; lvc.pszText = (LPTSTR)"X-Axis";
        ListView_InsertColumn(hWndGeometry, 1, &lvc);
        lvc.iSubItem = 2; lvc.pszText = (LPTSTR)"Y-Axis";
        ListView_InsertColumn(hWndGeometry, 2, &lvc);
        lvc.iSubItem = 3; lvc.pszText = (LPTSTR)"Z-Axis";
        ListView_InsertColumn(hWndGeometry, 3, &lvc);


        TreeView_Expand(hWndSegmentTree, htiSegmentList, TVE_EXPAND);
        return (TRUE);
    }
    case WM_NOTIFY:
    {
        NMHDR* pnm = (NMHDR*)lParam;
        if (pnm->idFrom == IDC_SEGMENT_TREE)
        {
            if (pnm->code == TVN_SELCHANGED)
            {
                NM_TREEVIEW* nmtv = (NM_TREEVIEW*)lParam;
                char szText[256]{};
                TV_ITEM item{};

                item.hItem = nmtv->itemNew.hItem;
                item.mask = TVIF_TEXT | TVIF_PARAM;
                item.pszText = szText;
                item.cchTextMax = sizeof(szText);
                TreeView_GetItem(hWndSegmentTree, &item);

                ListView_DeleteAllItems(hWndExtra);
                ListView_DeleteAllItems(hWndGeometry);

                if (strcmp(item.pszText, "<Segments>") == 0) { }
                else if (strcmp(item.pszText, "<Segment>") == 0) { }
                else
                {
                    CPrim* pPrim = (CPrim*)item.lParam;
                    DlgProcEditTrap_CommandsQueryFillExtraList(hDlg, pPrim);
                    DlgProcEditTrap_CommandsQueryFillGeometryList(hDlg, pPrim);
                }
                return (TRUE);
            }
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }

    }
    }
    return (FALSE);
}