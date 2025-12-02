#include "stdafx.h"

#include <Windows.h>

#include <afx.h>
#include <afxwin.h>

#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "Layer.h"
#include "Messages.h"
#include "Preview.h"
#include "resource.h"
#include "SegsTrap.h"

/// @brief The current trap is copied to the clipboard. This is done with two independent clipboard formats. 
/// The standard enhanced metafile and the private PegSegs which is read exclusively by Peg.
void DoEditTrapCopy(CPegView* view);

UINT CALLBACK OFNHookProcFileTracing(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    CPegDoc* pDoc = CPegDoc::GetDoc();
    CPegView* pView = CPegView::GetActiveView();

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        WndProcPreviewClear(hDlg);
        return (TRUE);

    case WM_NOTIFY:
    {
        LPOFNOTIFY lpofn;
        lpofn = (LPOFNOTIFY)lParam;
        if (lpofn->hdr.code == CDN_FOLDERCHANGE)
        {
            WndProcPreviewClear(hDlg);
        }
        else if (lpofn->hdr.code == CDN_SELCHANGE)
        {
            char psz[MAX_PATH]{ };
            CommDlg_OpenSave_GetFilePath(GetParent(hDlg), psz, MAX_PATH);

            CFileStatus	fs;
            if (CFile::GetStatus(psz, fs))
            {
                CLayer* pLayer = pDoc->LayersGet(psz);

                if (pLayer != 0)
                    WndProcPreviewUpdate(hDlg, pLayer);
                else
                {
                    pLayer = new CLayer;

                    pDoc->TracingLoadLayer(psz, pLayer);
                    WndProcPreviewUpdate(hDlg, pLayer);

                    pLayer->RemoveSegs();
                    delete pLayer;
                }
            }
        }
        return (TRUE);
    }
    case WM_COMMAND:
    {
        char szFilePath[MAX_PATH]{};
        CommDlg_OpenSave_GetFilePath(GetParent(hDlg), szFilePath, MAX_PATH);

        CFileStatus	fs;
        if (!CFile::GetStatus(szFilePath, fs))
        {
            msgWarning(IDS_MSG_FILE_NOT_FOUND, szFilePath);
            return (TRUE);
        }
        switch (LOWORD(wParam))
        {
        case IDC_APPEND:
        {
            CLayer* pLayer = pDoc->WorkLayerGet();

            pDoc->TracingLoadLayer(szFilePath, pLayer);
            pDoc->UpdateAllViews(NULL, CPegDoc::HINT_LAYER_SAFE, pLayer);
            return (TRUE);
        }

        case IDC_TRAMAP:
            pDoc->TracingMap(szFilePath);
            return (TRUE);

        case IDC_TRAP:
        {
            CLayer* pLayer = new CLayer();

            pDoc->TracingLoadLayer(szFilePath, pLayer);

            trapsegs.RemoveAll();
            trapsegs.AddTail(pLayer);
            DoEditTrapCopy(pView);
            trapsegs.RemoveAll();

            pLayer->RemoveSegs();
            delete pLayer;

            return (TRUE);
        }

        case IDC_TRAVIEW:
            pDoc->TracingView(szFilePath);

            return (TRUE);
        }
    }
    }
    return (FALSE); 		// Message for default dialog handlers
}

