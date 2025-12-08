#include "stdafx.h"

#include <afxstr.h>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "Block.h"
#include "Pnt.h"
#include "Preview.h"
#include "PrimSegRef.h"
#include "Seg.h"

void	BlockInsertDoOK(HWND);
LRESULT	BlockInsertGetCurSel(HWND, int, CString& strName);

CPnt* ptIns{nullptr};

INT_PTR CALLBACK DlgProcBlockInsert(HWND hDlg, UINT anMsg, WPARAM wParam, LPARAM) noexcept
{
    CPegDoc* pDoc{CPegDoc::GetDoc()};

    switch (anMsg)
    {
    case WM_INITDIALOG:
    {
        ptIns = new CPnt(app.CursorPosGet());

        CString key{};
        CBlock* block{nullptr};

        POSITION pos{pDoc->BlksGetStartPosition()};
        while (pos != nullptr)
        {
            pDoc->BlksGetNextAssoc(pos, key, block);
            if (!block->IsAnonymous())
            {
                ::SendDlgItemMessage(hDlg, IDC_BLOCKS_LIST, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)key);
            }
        }
        ::SendDlgItemMessage(hDlg, IDC_BLOCKS_LIST, LB_SETCURSEL, 0, 0L);

        if (pDoc->BlksIsEmpty())
        {
            WndProcPreviewClear(hDlg);
        }
        else
        {
            pos = pDoc->BlksGetStartPosition();
            pDoc->BlksGetNextAssoc(pos, key, block);

            SetDlgItemInt(hDlg, IDC_SEGS, static_cast<UINT>(block->GetCount()), FALSE);
            SetDlgItemInt(hDlg, IDC_REFERENCES, static_cast<UINT>(pDoc->BlockGetRefCount(key)), FALSE);
            WndProcPreviewUpdate(hDlg, block);
        }
        return (TRUE);
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BLOCKS_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE)
            {
                CBlock* block{nullptr};
                CString blockName;
                BlockInsertGetCurSel(hDlg, IDC_BLOCKS_LIST, blockName);
                pDoc->BlksLookup(blockName, block);
                if (block == nullptr) { break; }
                SetDlgItemInt(hDlg, IDC_SEGS, static_cast<UINT>(block->GetCount()), FALSE);
                SetDlgItemInt(hDlg, IDC_REFERENCES, static_cast<UINT>(pDoc->BlockGetRefCount(blockName)), FALSE);
                WndProcPreviewUpdate(hDlg, block);
            }
            break;

        case IDC_PURGE:
            pDoc->BlksRemoveUnused();
            delete ptIns;
            ptIns = nullptr;
            ::EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
            BlockInsertDoOK(hDlg);
            [[fallthrough]]; // Intentional fallthrough

        case IDCANCEL:
            delete ptIns;
          ptIns = nullptr;
            ::EndDialog(hDlg, TRUE);
            return (TRUE);
        }
    }
    return (FALSE);
}
void BlockInsertDoOK(HWND hDlg)
{
    CPegDoc* pDoc = CPegDoc::GetDoc();

    CString strBlkNam;
    if (BlockInsertGetCurSel(hDlg, IDC_BLOCKS_LIST, strBlkNam) != LB_ERR)
    {
        CPrimSegRef* pSegRef = new CPrimSegRef(strBlkNam, *ptIns);

        CSeg* pSeg = new CSeg(pSegRef);
        pDoc->WorkLayerAddTail(pSeg);
        pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG, pSeg);
    }
}
LRESULT BlockInsertGetCurSel(HWND hDlg, int iListId, CString& strTitle)
{
    LRESULT lrCurSel = ::SendDlgItemMessage(hDlg, iListId, LB_GETCURSEL, 0, 0L);
    if (lrCurSel != LB_ERR)
    {
        ::GetDlgItemText(hDlg, iListId, strTitle.GetBuffer(MAX_PATH), MAX_PATH);
        strTitle.ReleaseBuffer();
    }
    return lrCurSel;
}