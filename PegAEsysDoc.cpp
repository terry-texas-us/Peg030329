#include "stdafx.h"

#include <afxext.h>
#include <iomanip>
#include <sstream>

#include "Messages.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "ddeGItms.h"
#include "ExpProcs.h"
#include "FileBlock.h"
#include "FileJob.h"
#if ODA_FUNCTIONALITY
#include "FileOpenDWG.h"
#endif
#include "FilePeg.h"
#include "FileTracing.h"
#include "Layer.h"
#include "OpenGL.h"
#include "PenStyle.h"
#include "PrimDim.h"
#include "Polygon.h"
#include "PrimLine.h"
#include "PrimPolyline.h"
#include "PrimSegRef.h"
#include "PrimText.h"
#include "SegsDet.h"
#include "SegsTrap.h"
#include "PrimState.h"
#include "Text.h"
#include "lex.h"

extern double gbl_dExtNum;
extern char gbl_szExtStr[128];

UINT CALLBACK OFNHookProcFileTracing(HWND, UINT, WPARAM, LPARAM);

void ffaReadFile(const CString& strPathName);
void mfGetAll(CFile& f, const CVec& vTrns);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
void DoEditTrapPopupCommands(HWND hwnd)
{
	CPoint pnt;
	::GetCursorPos(&pnt);
	HMENU hMenu = ::LoadMenu(app.GetInstance(), MAKEINTRESOURCE(IDR_TRAP));
	hMenu = ::GetSubMenu(hMenu, 0);
	::TrackPopupMenu(hMenu, 0, pnt.x, pnt.y, 0, hwnd, 0);
	::DestroyMenu(hMenu);
}
///<summary>
///The current trap is copied to the clipboard. This is done with two independent clipboard formats. 
///The standard enhanced metafile and the private PegSegs which is read exclusively by Peg.
///</summary>
void DoEditTrapCopy(CPegView* view)
{
	::OpenClipboard(NULL);
	::EmptyClipboard();

	if (app.GetEditCFText())
	{
		// UNDONE possible
		CString strBuf;

		POSITION pos = trapsegs.GetHeadPosition();
		while (pos != 0)
		{
			CSeg* pSeg = trapsegs.GetNext(pos);

			POSITION posPrim = pSeg->GetHeadPosition();
			while (posPrim != 0)
			{
				CPrim* pPrim = pSeg->GetNext(posPrim);
				if (pPrim->Is(CPrim::Type::Text))
				{
					strBuf += static_cast<CPrimText*>(pPrim)->Text();
					strBuf += "\r\n";
				}
			}
		}
		GLOBALHANDLE hGMem = GlobalAlloc(GHND, strBuf.GetLength() + 1);
		LPSTR lpBuffer = (LPSTR)GlobalLock(hGMem);
		strcpy_s(lpBuffer, strBuf.GetLength() + 1, strBuf);
		GlobalUnlock(hGMem);
		::SetClipboardData(CF_TEXT, hGMem);
	}
	if (app.GetEditCFImage())
	{
		int iPrimState = pstate.Save();

		HDC hdcEMF = ::CreateEnhMetaFile(0, 0, 0, 0);
		trapsegs.Display(view, CDC::FromHandle(hdcEMF));
		HENHMETAFILE hemf = ::CloseEnhMetaFile(hdcEMF);
		::SetClipboardData(CF_ENHMETAFILE, hemf);

		pstate.Restore(CDC::FromHandle(hdcEMF), iPrimState);
	}
	if (app.GetEditCFSegments())
	{
		CPegView* activeView = CPegView::GetActiveView();

		CPnt ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
		CPnt ptMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		CMemFile mf;

		mf.SetLength(96);
		mf.SeekToEnd();

		char* pBuf = new char[CPrim::BUFFER_SIZE];

		trapsegs.Write(mf, pBuf);
		trapsegs.GetExtents(ptMin, ptMax, activeView->ModelViewGetMatrix());

		delete [] pBuf;

		ptMin = activeView->ModelViewGetMatrixInverse() * ptMin;

		ULONGLONG dwSizeOfBuffer = mf.GetLength();

		mf.SeekToBegin();
		mf.Write(&dwSizeOfBuffer, sizeof(DWORD));
		ptMin.Write(mf);

		GLOBALHANDLE hGMem = GlobalAlloc(GHND, SIZE_T(dwSizeOfBuffer));
		LPSTR lpBuffer = (LPSTR)GlobalLock(hGMem);

		mf.SeekToBegin();
		mf.Read(lpBuffer, UINT(dwSizeOfBuffer));

		GlobalUnlock(hGMem);
		::SetClipboardData(app.GetClipboardFormatPegSegs(), hGMem);
	}
	::CloseClipboard();
}

UINT AFXAPI HashKey(CString& str)
{
	LPCTSTR pStr = (LPCTSTR)str;
	UINT nHash = 0;
	while (*pStr)
	{
		nHash = (nHash << 5) + nHash + *pStr++;
	}
	return nHash;
}

// CPegDoc

IMPLEMENT_DYNCREATE(CPegDoc, CDocument)

// CPegDoc construction/destruction

CPegDoc::CPegDoc()
{
	m_bTrapHighlight = true;

	m_pLayerWork = NULL;
	m_wOpenFileType = FILE_TYPE_NONE;
}

CPegDoc::~CPegDoc()
{
	// Release resources allocated for line type table

	for (WORD w = 0; w < m_PenStyles.GetSize(); w++)
		delete m_PenStyles[w];

	m_PenStyles.RemoveAll();

	// Release resources allocated for blocks

	BlksRemoveAll();
	LayersRemoveAll();
}
///<summary>
// Called by the framework to delete the document's data without destroying the CDocument object itself.
// It is called just before the document is to be destroyed. It is also called to ensure that a document
// is empty before it is reused. Call this function to implement an "Edit Clear All" or similar command 
// that deletes all of the document's data.
///</summary>
void CPegDoc::DeleteContents()
{
	// PenStyles
	BlksRemoveAll();
	LayersRemoveAll();
	m_pLayerWork = NULL;
	DeletedSegsRemoveSegs();

	trapsegs.RemoveAll();
	detsegs.RemoveAll();
	detsegs.DetSeg() = 0;
}
BOOL CPegDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

#pragma tasMSG(TODO: Need to reload fresh set of penstyles)

	m_pLayerWork = new CLayer("0");
	LayersAdd(m_pLayerWork);
	SetOpenFile(FILE_TYPE_NONE, "0");

	UpdateAllViews(NULL, 0L, NULL);

	AppSetGinCur();

	return TRUE;
}
BOOL CPegDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return TRUE;
}

// CPegDoc serialization

void CPegDoc::Serialize(CArchive& ar)
{
	char szFileName[MAX_PATH];
	strcpy_s(szFileName, sizeof(szFileName), ar.m_strFileName.GetString());

	if (ar.IsStoring())
	{
		if (m_wOpenFileType == FILE_TYPE_DWG || m_wOpenFileType == FILE_TYPE_DXF)
		{
#if ODA_FUNCTIONALITY
			CFileOpenDWG fod;
			short nError;
			if (fod.Initialize(&nError))
			{
				char szName[MAX_PATH];
				strcpy_s(szName, sizeof(szName), ar.m_strFileName.GetString());
				fod.Create(szName, m_wOpenFileType);
			}
#endif
		}
		else if (m_wOpenFileType == FILE_TYPE_PEG)
		{
			CFilePeg* pFile = (CFilePeg*)ar.GetFile();
			pFile->Unload(this);
		}
		else if (FileTypeIsTracing(m_wOpenFileType))
		{
			CLayer* pLayer = LayersGet(szFileName);

			if (pLayer == 0)
				return;

			if (m_wOpenFileType == FILE_TYPE_JOB)
			{
				CFileJob fj;
				if (!fj.OpenForWrite(szFileName))
					return;
				fj.WriteHeader();
				fj.WriteSegs(pLayer);
			}
			else if (m_wOpenFileType == FILE_TYPE_TRACING)
			{
				CFileTracing ft;
				if (!ft.OpenForWrite(szFileName))
					return;
				ft.WriteHeader();
				ft.WriteSegs(pLayer);
			}
			msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, szFileName);
		}
		else
			msgWarning(IDS_MSG_NOTHING_TO_SAVE);

	}
	else
	{
		WORD wFileType = CPegApp::GetFileTypeFromPath(szFileName);

		if (wFileType == FILE_TYPE_DWG || wFileType == FILE_TYPE_DXF)
		{
#if ODA_FUNCTIONALITY
			CFileOpenDWG fod;
			short nError;
			if (fod.Initialize(&nError))
			{
				char szPathName[MAX_PATH];
				strcpy_s(szPathName, sizeof(szPathName), ar.m_strFileName.GetString());
				fod.Load(szPathName);
				SetOpenFile(wFileType, szPathName);
				WorkLayerSet(LayersGetAt(0));
			}
#endif
		}
		else if (wFileType == FILE_TYPE_FFA)
		{
#pragma tasMSG(TODO: .ffa file type now retained. should out of ffa reproduce jb1 set for backwards compatability)
			ffaReadFile(szFileName);
			SetOpenFile(FILE_TYPE_FFA, szFileName);
		}
		else if (wFileType == FILE_TYPE_PEG)
		{
#pragma tasMSG(TODO: was opening and retaining pFile for locking stuff. now using the pfile provided in ar which is released automatically)
			((CFilePeg*)ar.GetFile())->Load(this);
			SetOpenFile(FILE_TYPE_PEG, szFileName);
			UpdateAllViews(NULL, 0L, NULL);
		}
		else if (FileTypeIsTracing(wFileType))
		{
			TracingOpen(szFileName);
		}
	}
}

// CPegDoc diagnostics

#ifdef _DEBUG
void CPegDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPegDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

BEGIN_MESSAGE_MAP(CPegDoc, CDocument)
	ON_COMMAND(ID_BLOCKS_LOAD, OnBlocksLoad)
	ON_COMMAND(ID_BLOCKS_REMOVEUNUSED, OnBlocksRemoveUnused)
	ON_COMMAND(ID_BLOCKS_UNLOAD, OnBlocksUnload)
	ON_COMMAND(ID_CLEAR_ACTIVELAYERS, OnClearActiveLayers)
	ON_COMMAND(ID_CLEAR_ALLLAYERS, OnClearAllLayers)
	ON_COMMAND(ID_CLEAR_ALLTRACINGS, OnClearAllTracings)
	ON_COMMAND(ID_CLEAR_MAPPEDTRACINGS, OnClearMappedTracings)
	ON_COMMAND(ID_CLEAR_VIEWEDTRACINGS, OnClearViewedTracings)
	ON_COMMAND(ID_CLEAR_WORKINGLAYER, OnClearWorkingLayer)
	ON_COMMAND(ID_EDIT_IMAGETOCLIPBOARD, OnEditImageToClipboard)
	ON_COMMAND(ID_EDIT_SEGTOWORK, OnEditSegToWork)
	ON_COMMAND(ID_EDIT_TRACE, OnEditTrace)
	ON_COMMAND(ID_EDIT_TRAPCOPY, OnEditTrapCopy)
	ON_COMMAND(ID_EDIT_TRAPCUT, OnEditTrapCut)
	ON_COMMAND(ID_EDIT_TRAPDELETE, OnEditTrapDelete)
	ON_COMMAND(ID_EDIT_TRAPPASTE, OnEditTrapPaste)
	ON_COMMAND(ID_EDIT_TRAPQUIT, OnEditTrapQuit)
	ON_COMMAND(ID_EDIT_TRAPWORK, OnEditTrapWork)
	ON_COMMAND(ID_EDIT_TRAPWORKANDACTIVE, OnEditTrapWorkAndActive)
	ON_COMMAND(ID_FILE, OnFile)
	ON_COMMAND(ID_FILE_QUERY, OnFileQuery)
	ON_COMMAND(ID_FILE_MANAGE, OnFileManage)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SEND_MAIL, CDocument::OnFileSendMail)
	ON_COMMAND(ID_FILE_TRACING, OnFileTracing)
	ON_COMMAND(ID_LAYER_ACTIVE, OnLayerActive)
	ON_COMMAND(ID_LAYER_STATIC, OnLayerStatic)
	ON_COMMAND(ID_LAYER_HIDDEN, OnLayerHidden)
	ON_COMMAND(ID_LAYER_MELT, OnLayerMelt)
	ON_COMMAND(ID_LAYER_WORK, OnLayerWork)
	ON_COMMAND(ID_LAYERS_ACTIVEALL, OnLayersActiveAll)
	ON_COMMAND(ID_LAYERS_STATICALL, OnLayersStaticAll)
	ON_COMMAND(ID_LAYERS_REMOVEEMPTY, OnLayersRemoveEmpty)
	ON_COMMAND(ID_MAINTENANCE_REMOVEEMPTYNOTES, OnMaintenanceRemoveEmptyNotes)
	ON_COMMAND(ID_MAINTENANCE_REMOVEEMPTYSEGMENTS, OnMaintenanceRemoveEmptySegments)
	ON_COMMAND(ID_PENS_REMOVEUNUSEDSTYLES, OnPensRemoveUnusedStyles)
	ON_COMMAND(ID_PENS_EDITCOLORS, OnPensEditColors)
	ON_COMMAND(ID_PENS_LOADCOLORS, OnPensLoadColors)
	ON_COMMAND(ID_PENS_TRANSLATE, OnPensTranslate)
	ON_COMMAND(ID_PRIM_BREAK, OnPrimBreak)
	ON_COMMAND(ID_PRIM_EXTRACTNUM, OnPrimExtractNum)
	ON_COMMAND(ID_PRIM_EXTRACTSTR, OnPrimExtractStr)
	ON_COMMAND(ID_PRIM_SNAPTOENDPOINT, OnPrimSnapToEndPoint)
	ON_COMMAND(ID_PRIM_GOTOCENTERPOINT, OnPrimGotoCenterPoint)
	ON_COMMAND(ID_PRIM_DELETE, OnPrimDelete)
	ON_COMMAND(ID_PRIM_MODIFY_ATTRIBUTES, OnPrimModifyAttributes)
	ON_COMMAND(ID_SEG_BREAK, OnSegBreak)
	ON_COMMAND(ID_SEG_DELETE, OnSegDelete)
	ON_COMMAND(ID_SEG_DELETELAST, OnSegDeleteLast)
	ON_COMMAND(ID_SEG_EXCHANGE, OnSegExchange)
	ON_COMMAND(ID_SEG_INSERT, OnSegInsert)
	ON_COMMAND(ID_SETUP_PENCOLOR, OnSetupPenColor)
	ON_COMMAND(ID_SETUP_PENSTYLE, OnSetupPenStyle)
	ON_COMMAND(ID_SETUP_FILL_HOLLOW, OnSetupFillHollow)
	ON_COMMAND(ID_SETUP_FILL_SOLID, OnSetupFillSolid)
	ON_COMMAND(ID_SETUP_FILL_PATTERN, OnSetupFillPattern)
	ON_COMMAND(ID_SETUP_FILL_HATCH, OnSetupFillHatch)
	ON_COMMAND(ID_SETUP_NOTE, OnSetupNote)
	ON_COMMAND(ID_SETUP_SAVEPOINT, OnSetupSavePoint)
	ON_COMMAND(ID_SETUP_GOTOPOINT, OnSetupGotoPoint)
	ON_COMMAND(ID_SETUP_OPTIONS_DRAW, OnSetupOptionsDraw)
	ON_COMMAND(ID_TRACING_MAP, OnTracingMap)
	ON_COMMAND(ID_TRACING_VIEW, OnTracingView)
	ON_COMMAND(ID_TRACING_CLOAK, OnTracingCloak)
	ON_COMMAND(ID_TRACING_FUSE, OnTracingFuse)
	ON_COMMAND(ID_TRACING_OPEN, OnTracingOpen)
	ON_COMMAND(ID_TRAPCOMMANDS_HIGHLIGHT, OnTrapCommandsHighlight)
	ON_COMMAND(ID_TRAPCOMMANDS_COMPRESS, OnTrapCommandsCompress)
	ON_COMMAND(ID_TRAPCOMMANDS_EXPAND, OnTrapCommandsExpand)
	ON_COMMAND(ID_TRAPCOMMANDS_INVERT, OnTrapCommandsInvert)
	ON_COMMAND(ID_TRAPCOMMANDS_SQUARE, OnTrapCommandsSquare)
	ON_COMMAND(ID_TRAPCOMMANDS_QUERY, OnTrapCommandsQuery)
	ON_COMMAND(ID_TRAPCOMMANDS_FILTER, OnTrapCommandsFilter)
	ON_COMMAND(ID_TRAPCOMMANDS_BLOCK, OnTrapCommandsBlock)
	ON_COMMAND(ID_TRAPCOMMANDS_UNBLOCK, OnTrapCommandsUnblock)
END_MESSAGE_MAP()

///<summary>Constructs 0 to many seperate text primitives for each "\r\n" delimited substr.</summary>
void CPegDoc::AddTextBlock(char* pszText)
{
	CPnt ptPvt = app.CursorPosGet();

	CFontDef fd;
	pstate.GetFontDef(fd);

	CCharCellDef ccd;
	pstate.GetCharCellDef(ccd);

	CRefSys rs(ptPvt, ccd);

	char* context = nullptr;
	char* pText = strtok_s(pszText, "\r", &context);
	while (pText != 0)
	{
		if (strlen(pText) > 0)
		{
			CSeg* pSeg = new CSeg(new CPrimText(fd, rs, pText));
			WorkLayerAddTail(pSeg);
			UpdateAllViews(NULL, HINT_SEG, pSeg);
		}
		rs.SetOrigin(text_GetNewLinePos(fd, rs, 1., 0));
		pText = strtok_s(0, "\r", &context);
		if (pText == 0)
			break;
		pText++;
	}
}
int CPegDoc::BlockGetRefCount(const CString& strBlkNam) const
{
	int iCount = 0;

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		iCount += pLayer->GetBlockRefCount(strBlkNam);
	}

	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
		iCount += pBlock->GetBlockRefCount(strBlkNam);
	}
	return (iCount);
}
bool CPegDoc::BlksLookup(CString strKey, CBlock*& pBlock) const
{
	if (m_blks.Lookup(strKey, pBlock))
	{
		return true;
	}
	pBlock = NULL;
	return false;
}
///<summary>Removes all blocks and defining primitives.</summary>
void CPegDoc::BlksRemoveAll()
{
	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
		pBlock->RemovePrims();
		delete pBlock;
	}
	m_blks.RemoveAll();
}
///<summary>Removes all blocks which have no references.</summary>
void CPegDoc::BlksRemoveUnused()
{
	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
		if (BlockGetRefCount(strKey) == 0)
		{
#pragma tasMSG(TODO: Deletion by key may cause loop problems)
			m_blks.RemoveKey(strKey);
			pBlock->RemovePrims();
			delete pBlock;
		}
	}
}
//Restores the last segment added to the deleted segment list.
void CPegDoc::DeletedSegsRestore()
{
	if (!DeletedSegsIsEmpty())
	{
		// have at least one deleted segment to restore
		CSeg* pSeg = DeletedSegsRemoveTail();
		WorkLayerAddTail(pSeg);
		UpdateAllViews(NULL, HINT_SEG_SAFE, pSeg);
	}
}
///<summary>
/// Searches for closest detectible primitive.  
/// If found, primitive is lifted from its segment, inserted into a new segment 
/// which is added to deleted segment list. The primitive resources are not freed.
///</summary>
void CPegDoc::DoPrimDelete(const CPnt& pt)
{
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);

	if (pSeg != 0)
	{
		POSITION pos = trapsegs.Find(pSeg);

		LPARAM lHint = (pos != 0) ? HINT_SEG_ERASE_SAFE_TRAP : HINT_SEG_ERASE_SAFE;
		// erase entire segment even if segment has more than one primitive
		UpdateAllViews(NULL, lHint, pSeg);

		if (pSeg->GetCount() > 1)
		{	// remove primitive from segment
			CPrim* pPrim = detsegs.DetPrim();
			pSeg->FindAndRemovePrim(pPrim);
			lHint = (pos != 0) ? HINT_SEG_SAFE_TRAP : HINT_SEG_SAFE;
			// display the segment with the primitive removed
			UpdateAllViews(NULL, lHint, pSeg);
			// new segment required to allow primitive to be placed into deleted segment list
			pSeg = new CSeg(pPrim);
		}
		else
		{ // deleting an entire segment
			AnyLayerRemove(pSeg);
			detsegs.Remove(pSeg);

			if (trapsegs.Remove(pSeg) != 0)
			{
				app.StatusLineDisplay(TrapCnt);
			}
		}
		DeletedSegsAddTail(pSeg);
		msgInformation(IDS_MSG_PRIM_ADDED_TO_DEL_SEGS);
	}
}
bool CPegDoc::FileTypeIsTracing(WORD wType) const
{
	return ((wType == FILE_TYPE_TRACING) || (wType == FILE_TYPE_JOB));
}
void CPegDoc::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	ptMin(FLT_MAX, FLT_MAX, FLT_MAX);
	ptMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (pLayer->IsOn())
			pLayer->GetExtents(ptMin, ptMax, tm);
	}
}
int CPegDoc::GetHotCount() const
{
	int iCount = 0;

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (pLayer->IsHot()) { iCount += pLayer->GetCount(); }
	}
	return iCount;
}

int CPegDoc::PenStylesFillCB(HWND hwnd) const
{
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0L);

	int iSize = static_cast<int>(m_PenStyles.GetSize());

	for (int i = 0; i < iSize; i++)
	{
		CPenStyle* pPenStyle = m_PenStyles[i];
		::SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)pPenStyle->GetName());
	}
	return (iSize);
}

PENSTYLE CPegDoc::PenStylesLookup(const CString& strName) const
{
	if (this == NULL)
		return 0;

	if (strName.CompareNoCase("ByBlock") == 0)
	{
		return PENSTYLE(1);
	}
	for (WORD w = 0; w < m_PenStyles.GetSize(); w++)
	{
		CPenStyle* pPenStyle = m_PenStyles[w];
		if (strName.CompareNoCase(pPenStyle->GetName()) == 0)
		{
			return (PENSTYLE(w));
		}
	}
	return (PENSTYLE_LookupErr);
}

///<summary>Gets indicated line type. If invalid returns the last line type</summary>
CPenStyle* CPegDoc::PenStylesGetAtSafe(int i)
{
	int iType = static_cast<int>(m_PenStyles.GetSize()) - 1;

	if (iType < 0) { return 0; } // No line types defined

	if (i < iType) { iType = i; }

	return (m_PenStyles[iType]);
}

int CPegDoc::GetWarmCount() const
{
	int iCount = 0;

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (pLayer->IsWarm()) { iCount += pLayer->GetCount(); }
	}
	return iCount;
}

void CPegDoc::InitAll()
{
	WorkLayerInit();
	SetOpenFile(FILE_TYPE_NONE, "0");

	for (int i = (int)m_layers.GetSize() - 1; i > 0; i--)
		LayersRemoveAt(i);

	BlksRemoveAll();

	AppSetGinCur();
}
void CPegDoc::LayerBlank(const CString& strFileName)
{
	CLayer* pLayer = LayersGet(strFileName);

	if (pLayer == 0)
		msgWarning(IDS_LAYER_NOT_LOADED);
	else if (pLayer->IsResident())
		msgWarning(IDS_MSG_LAYER_IS_RESIDENT, strFileName);
	else if (pLayer->IsOpened())
	{
		if (msgConfirm(IDS_MSG_CONFIRM_BLANK, strFileName) == IDYES)
		{
			trapsegs.RemoveAll();
			detsegs.RemoveAll();
			detsegs.DetSeg() = 0;
			m_segsDeleted.RemoveSegs();

			SetOpenFile(FILE_TYPE_NONE, "0");

			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			LayersRemove(strFileName);
		}
	}
	else
	{
		UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
		LayersRemove(strFileName);
	}
}
///<summary>Displays drawing and determines which segments are detectable.</summary>
void CPegDoc::LayersDisplayAll(CPegView* pView, CDC* pDC)
{
	bool bIdentifyTrap = trapsegs.Identify() && !trapsegs.IsEmpty();

	detsegs.RemoveAll();

	if (pDC == 0)
	{
		if (pView->m_bViewWireframe)
			opengl::PolygonModeLine();
		else
			opengl::PolygonModeFill();

		for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
		{
			CLayer* pLayer = m_layers.GetAt(i);
			pLayer->Display(pView, pDC, bIdentifyTrap);
		}
		opengl::Flush();
	}
	else
	{
		CPrim::SpecPolygonStyle() = pView->m_bViewWireframe ? POLYGON_HOLLOW : short(-1);
		int iPrimState = pstate.Save();

		for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
		{
			CLayer* pLayer = m_layers.GetAt(i);
			pLayer->Display(pView, pDC, bIdentifyTrap);
		}
		pstate.Restore(pDC, iPrimState);
		CPrim::SpecPolygonStyle() = -1;
	}
}
CLayer* CPegDoc::LayersGet(const CString& strName) const
{
	int i = LayersLookup(strName);
	return (i < 0 ? (CLayer*)0 : m_layers.GetAt(i));
}
CLayer* CPegDoc::LayersGetAt(int i) const
{
	return (i >= (int)m_layers.GetSize() ? (CLayer*)NULL : m_layers.GetAt(i));
}
int CPegDoc::LayersLookup(const CString& strName) const
{
	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (strName.CompareNoCase(pLayer->GetName()) == 0) { return (i); }
	}
	return (-1);
}
///<summary>A layer is converted to a tracing or a job file</summary>
bool CPegDoc::LayerMelt(CString& strName) const
{
	CLayer* pLayer = LayersGet(strName);
	if (pLayer == 0)
		return false;

	bool bRetVal = false;

	char szFilter[256];
	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));

	OPENFILENAME of;

	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	of.lpstrFilter = szFilter;
	of.lpstrFile = new char[MAX_PATH];
	strcpy_s(of.lpstrFile, MAX_PATH, strName);
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Melt As";
	of.Flags = OFN_OVERWRITEPROMPT;
	of.lpstrDefExt = "tra";

	if (GetSaveFileName(&of))
	{
		strName = of.lpstrFile;

		WORD wFileType = CPegApp::GetFileTypeFromPath(strName);

		if (FileTypeIsTracing(wFileType))
		{
			if (wFileType == FILE_TYPE_TRACING)
			{
				CFileTracing ft;
				if (ft.OpenForWrite(strName))
				{
					ft.WriteHeader();
					ft.WriteSegs(pLayer);
					bRetVal = true;
				}
			}
			else
			{
				CFileJob fj;
				if (fj.OpenForWrite(strName))
				{
					fj.WriteHeader();
					fj.WriteSegs(pLayer);
					bRetVal = true;
				}
			}
			if (bRetVal)
			{
				pLayer->ClrStateFlg();
				pLayer->SetStateFlg(CLayer::STATE_FLG_RESIDENT);
				pLayer->SetStateCold();
				pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);

				strName = strName.Mid(of.nFileOffset);
				pLayer->SetName(strName);
			}
		}
	}
	delete [] of.lpstrFile;
	return (bRetVal);
}
void CPegDoc::LayersRemove(const CString& strName)
{
	int i = LayersLookup(strName);

	if (i >= 0)
		LayersRemoveAt(i);
}
void CPegDoc::LayersRemoveAll()
{
	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		pLayer->RemoveSegs();
		delete pLayer;
	}
	m_layers.RemoveAll();
}
void CPegDoc::LayersRemoveAt(int i)
{
	CLayer* pLayer = LayersGetAt(i);

	pLayer->RemoveSegs();
	delete pLayer;

	m_layers.RemoveAt(i);
}
void CPegDoc::LayersRemoveEmpty()
{
	for (int i = static_cast<int>(m_layers.GetSize()) - 1; i > 0; i--)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (pLayer->IsEmpty())
		{
			pLayer->RemoveSegs();
			delete pLayer;
			m_layers.RemoveAt(i);
		}
	}
}
int CPegDoc::PenStyleGetRefCount(PENSTYLE nPenStyle) const
{
	int iCount = 0;

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		iCount += pLayer->GetPenStyleRefCount(nPenStyle);
	}

	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
		iCount += pBlock->GetPenStyleRefCount(nPenStyle);
	}
	return (iCount);
}
///<summary>Removes line types which have no references.</summary>
void CPegDoc::PenStylesRemoveUnused()
{
#pragma tasMSG(TODO test new logic for PenStylesRemoveUnused)
	int i = static_cast<int>(m_PenStyles.GetSize());
	while (--i != 0)
	{
		CPenStyle* pPenStyle = m_PenStyles[i];
		PENSTYLE nPenStyle = PenStylesLookup(pPenStyle->GetName());
		if (PenStyleGetRefCount(nPenStyle) != 0 || i < 10)
		{
			m_PenStyles.SetSize(i + 1);
			break;
		}
		delete pPenStyle;
	}
}
void CPegDoc::PenTranslation(WORD wCols, PENCOLOR* pColNew, PENCOLOR* pCol)
{
	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		pLayer->PenTranslation(wCols, pColNew, pCol);
	}
}
CLayer* CPegDoc::LayersSelUsingPoint(const CPnt& pt) const
{
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);

	if (pSeg != 0)
	{
		for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
		{
			CLayer* pLayer = m_layers.GetAt(i);
			if (pLayer->Find(pSeg))
			{
				return (pLayer);
			}
		}
	}
	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);

		if (pLayer->SelSegUsingPoint(pt) != 0)
		{
			return (pLayer);
		}
	}
	return 0;
}
int CPegDoc::RemoveEmptyNotes()
{
	int iCount = 0;

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		iCount += pLayer->RemoveEmptyNotes();
	}

#pragma tasMSG(TODO remove empty notes from blocks)

	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
	}
	return (iCount);
}
int CPegDoc::RemoveEmptySegs()
{
	int iCount = 0;

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		iCount += pLayer->RemoveEmptySegs();
	}

#pragma tasMSG(TODO remove empty segs from blocks)

	CString strKey;
	CBlock* pBlock;

	POSITION pos = m_blks.GetStartPosition();
	while (pos != NULL)
	{
		m_blks.GetNextAssoc(pos, strKey, pBlock);
	}
	return (iCount);
}
void CPegDoc::WorkLayerAddTail(CSeg* pSeg)
{
	m_pLayerWork->AddTail(pSeg);
	detsegs.AddTail(pSeg);

	app.StatusLineDisplay(WorkCnt);
	SetModifiedFlag();
}
void CPegDoc::WorkLayerAddTail(CSegs* pSegs)
{
	m_pLayerWork->AddTail(pSegs);
	detsegs.AddTail(pSegs);

	app.StatusLineDisplay(WorkCnt);
	SetModifiedFlag();
}
// Locates the layer containing a segment and removes it.
// The segment itself is not deleted.
CLayer* CPegDoc::AnyLayerRemove(CSeg* pSeg)
{
	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (pLayer->IsHot() || pLayer->IsWarm())
		{
			if (pLayer->Remove(pSeg) != 0)
			{
				app.StatusLineDisplay(WorkCnt);
				SetModifiedFlag();

				return pLayer;
			}
		}
	}
	return 0;
}
CSeg* CPegDoc::WorkLayerGetTail() const
{
	POSITION pos = m_pLayerWork->GetTailPosition();
	return ((CSeg*)(pos != 0 ? m_pLayerWork->GetPrev(pos) : 0));
}
void CPegDoc::SetOpenFile(WORD wFileType, const CString& strFileName)
{
	int n = 0;
	for (n = strFileName.GetLength() - 1; n > 0; n--)
	{
		if (strFileName[n] == '\\')
		{
			n++;
			break;
		}
	}

	CString strItem = "&File:" + strFileName.Mid(n);

	app.ModifyMenu(0, strItem);

	if (wFileType == FILE_TYPE_NONE)
		WorkLayerSet(LayersGetAt(0));

	m_wOpenFileType = wFileType;
}

void CPegDoc::TracingFuse(CString& layerName) const
{
	CLayer* layer = LayersGet(layerName);
	if (layer != 0)
	{
		char* title = new char[MAX_PATH];
		GetFileTitle(layerName, title, MAX_PATH);
		char* context = nullptr;
		strtok_s(title, ".", &context);
		layerName = title;
		delete [] title;

		layer->ClrTracingFlg();
		layer->ClrStateFlg();
		layer->SetStateFlg(CLayer::STATE_FLG_RESIDENT | CLayer::STATE_FLG_INTERNAL);
		layer->SetStateCold();

		layer->SetName(layerName);
	}
}
bool CPegDoc::TracingLoadLayer(const CString& strPathName, CLayer* pLayer) const
{
	WORD wFileType = CPegApp::GetFileTypeFromPath(strPathName);
	if (!FileTypeIsTracing(wFileType))
		return false;

	if (pLayer == 0)
		return false;

	bool bFileOpen = false;

	if (wFileType == FILE_TYPE_TRACING)
	{
		CFileTracing ft;
		bFileOpen = ft.OpenForRead(strPathName);
		if (bFileOpen)
		{
			ft.ReadHeader();
			ft.ReadSegs(pLayer);
		}
	}
	else
	{
		CFileJob fj;
		bFileOpen = fj.OpenForRead(strPathName);
		if (bFileOpen)
		{
			fj.ReadHeader();
			fj.ReadSegs(pLayer);
		}
	}

	return (bFileOpen);
}
bool CPegDoc::TracingMap(const CString& strPathName)
{
	WORD wFileType = CPegApp::GetFileTypeFromPath(strPathName);
	if (!FileTypeIsTracing(wFileType))
		return false;

	bool bFileOpen = false;

	CLayer* pLayer = LayersGet(strPathName);

	if (pLayer != 0)
	{
		if (pLayer->IsOpened())
			msgWarning(IDS_MSG_CLOSE_TRACING_FIRST, strPathName);
		else
			bFileOpen = true;
	}
	else
	{
		pLayer = new CLayer(strPathName, CLayer::STATE_FLG_COLD);

		bFileOpen = TracingLoadLayer(strPathName, pLayer);

		if (bFileOpen)
			LayersAdd(pLayer);
		else
			delete pLayer;
	}
	if (bFileOpen)
	{
		pLayer->SetTracingFlg(CLayer::TRACING_FLG_MAPPED);
		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
	}
	return (bFileOpen);
}
bool CPegDoc::TracingOpen(const CString& strFileName)
{
	// Opens tracing file.

	CLayer* pLayer = 0;

	int iLayId = LayersLookup(strFileName);

	if (iLayId > 0)
	{	// already loaded
		pLayer = LayersGetAt(iLayId);
		pLayer->ClrStateFlg(CLayer::STATE_FLG_RESIDENT);
	}
	else
	{	// create a new layer and append all the segments in the segment file.

		pLayer = new CLayer(strFileName, CLayer::STATE_FLG_HOT | CLayer::STATE_FLG_WARM);
		LayersAdd(pLayer);

		TracingLoadLayer(strFileName, pLayer);

		detsegs.AddTail(pLayer);
	}
	pLayer->SetTracingFlg(CLayer::TRACING_FLG_OPENED);

	SetOpenFile(FILE_TYPE_TRACING, strFileName);
	WorkLayerSet(pLayer);

	UpdateAllViews(NULL, 0L, NULL);

	return true;
}
bool CPegDoc::TracingView(const CString& strPathName)
{
	WORD wFileType = CPegApp::GetFileTypeFromPath(strPathName);
	if (!FileTypeIsTracing(wFileType))
		return false;

	bool bFileOpen = false;

	CLayer* pLayer = LayersGet(strPathName);

	if (pLayer != 0)
	{
		if (pLayer->IsOpened())
			msgWarning(IDS_MSG_CLOSE_TRACING_FIRST, strPathName);
		else
			bFileOpen = true;
	}
	else
	{
		pLayer = new CLayer(strPathName, CLayer::STATE_FLG_COLD);

		bFileOpen = TracingLoadLayer(strPathName, pLayer);

		if (bFileOpen)
			LayersAdd(pLayer);
		else
			delete pLayer;
	}
	if (bFileOpen)
	{
		pLayer->SetTracingFlg(CLayer::TRACING_FLG_VIEWED);
		UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
	}
	return (bFileOpen);
}
void CPegDoc::WorkLayerClear()
{
	AppSetGinCur();
	WorkLayerInit();
}
void CPegDoc::WorkLayerInit()
{
	//m_pLayerWork->Erase();
	m_pLayerWork->RemoveSegs();

	trapsegs.RemoveAll();
	detsegs.RemoveAll();
	detsegs.DetSeg() = 0;
	m_segsDeleted.RemoveSegs();
}
void CPegDoc::WorkLayerSet(CLayer* pLayer)
{
	if (m_pLayerWork == 0)
	{
		// HACK: m_pLayerWork should be created already. OnNew something, somewhere.
		m_pLayerWork = pLayer;
	}
	m_pLayerWork->SetStateWarm();
	m_pLayerWork = pLayer;
	m_pLayerWork->SetStateHot();

	CString strItem;
	GetMenuString(app.GetMenu(), 0, strItem.GetBuffer(128), 128, MF_BYPOSITION);
	strItem.ReleaseBuffer();

	int n = strItem.Find('/');

	if (n == -1)
		n = strItem.GetLength();

	strItem.Truncate(n);
	strItem += "/" + m_pLayerWork->GetName();

	app.ModifyMenu(0, strItem);
}
void CPegDoc::WriteShadowFile()
{
	if (m_wOpenFileType == FILE_TYPE_PEG)
	{
		CString strShadowFilePath(app.GetShadowDir());
		strShadowFilePath += GetTitle();
		int nExt = strShadowFilePath.Find('.');
		if (nExt > 0)
		{
			CFileStatus fs;
			CFile::GetStatus(GetPathName(), fs);

			strShadowFilePath.Truncate(nExt);
			strShadowFilePath += fs.m_mtime.Format("_%Y%m%d%H%M");
			strShadowFilePath += ".peg";

			CFileException e;
			CFilePeg fp;
			if (!fp.Open(strShadowFilePath, CFile::modeWrite, &e))
			{
				fp.Open(strShadowFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive, &e);
				fp.Unload(this);
				msgWarning(IDS_MSG_FILE_SHADOWED_AS, strShadowFilePath);
				return;
			}
			msgWarning(IDS_MSG_SHADOW_FILE_CREATE_FAILURE);
		}
	}
}
// CPegDoc commands

void CPegDoc::OnClearActiveLayers()
{
	AppSetGinCur();
	for (int i = static_cast<int>(m_layers.GetSize()) - 1; i > 0; i--)
	{
		CLayer* pLayer = m_layers.GetAt(i);

		if (pLayer->IsWarm())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			pLayer->RemoveSegs();
		}
	}
}
void CPegDoc::OnClearAllLayers()
{
	AppSetGinCur();

	for (int i = static_cast<int>(m_layers.GetSize()) - 1; i > 0; i--)
	{
		CLayer* pLayer = m_layers.GetAt(i);

		if (pLayer->IsInternal())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			pLayer->RemoveSegs();
		}
	}
	UpdateAllViews(NULL, 0L, NULL);
}
void CPegDoc::OnClearWorkingLayer()
{
	WorkLayerClear();
}
void CPegDoc::OnClearAllTracings()
{
	AppSetGinCur();

	for (int i = static_cast<int>(m_layers.GetSize()) - 1; i > 0; i--)
	{
		CLayer* pLayer = m_layers.GetAt(i);

		if (!pLayer->IsInternal())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			pLayer->RemoveSegs();
		}
	}
	UpdateAllViews(NULL, 0L, NULL);
}
void CPegDoc::OnClearMappedTracings()
{
	AppSetGinCur();
	for (int i = static_cast<int>(m_layers.GetSize()) - 1; i > 0; i--)
	{
		CLayer* pLayer = m_layers.GetAt(i);

		if (pLayer->IsMapped())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);

			if (pLayer->IsResident())
			{
				pLayer->ClrTracingFlg(CLayer::TRACING_FLG_MAPPED);
				pLayer->SetStateOff();
			}
			else
				LayersRemoveAt(i);
		}
	}
}
void CPegDoc::OnClearViewedTracings()
{
	AppSetGinCur();
	for (int i = static_cast<int>(m_layers.GetSize()) - 1; i > 0; i--)
	{
		CLayer* pLayer = m_layers.GetAt(i);

		if (pLayer->IsViewed())
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);

			if (pLayer->IsResident())
			{
				pLayer->ClrTracingFlg(CLayer::TRACING_FLG_VIEWED);
				pLayer->SetStateOff();
			}
			else
				LayersRemoveAt(i);
		}
	}
}
///<summary>Breaks a primitive into a simpler set of primitives.</summary>
void CPegDoc::OnPrimBreak()
{
	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(app.CursorPosGet());
	if (pSeg != 0 && detsegs.DetPrim() != 0)
	{
		CPrim* pPrim = detsegs.DetPrim();

		PENCOLOR nPenColor = pPrim->PenColor();
		PENSTYLE nPenStyle = pPrim->PenStyle();

		if (pPrim->Is(CPrim::Type::Polyline))
		{
			pSeg->FindAndRemovePrim(pPrim);

			CPrimPolyline* pPolyline = static_cast<CPrimPolyline*>(pPrim);

			CPnts pts;
			pPolyline->GetAllPts(pts);

			for (WORD w = 0; w < pts.GetSize() - 1; w++)
				pSeg->AddTail(new CPrimLine(nPenColor, nPenStyle, pts[w], pts[w + 1]));

			if (pPolyline->IsLooped())
				pSeg->AddTail(new CPrimLine(nPenColor, nPenStyle, pts[pts.GetUpperBound()], pts[0]));

			delete pPrim;
			detsegs.DetSeg() = 0;
		}
		else if (pPrim->Is(CPrim::Type::SegRef))
		{
			CPrimSegRef* pSegRef = static_cast<CPrimSegRef*>(pPrim);

			CBlock* pBlock;

			if (BlksLookup(pSegRef->GetName(), pBlock) != 0)
			{
				pSeg->FindAndRemovePrim(pPrim);

				CPnt ptBase = pBlock->GetBasePt();

				CTMat tm = pSegRef->BuildTransformMatrix(ptBase);

				CSeg* pSegT = new CSeg(*pBlock);
				pSegT->Transform(tm);
				pSeg->AddTail(pSegT);

				delete pPrim;
				detsegs.DetSeg() = 0;
			}
		}
		else if (pPrim->Is(CPrim::Type::Insert))
		{

		}
	}
}
void CPegDoc::OnEditSegToWork()
{
	CPnt pt = app.CursorPosGet();

	CLayer* pLayer = LayersSelUsingPoint(pt);

	if (pLayer != 0)
	{
		if (pLayer->IsInternal())
		{
			CSeg* pSeg = pLayer->SelSegUsingPoint(pt);

			if (pSeg != 0)
			{
				pLayer->Remove(pSeg);
				UpdateAllViews(NULL, HINT_SEG_ERASE_SAFE, pSeg);
				WorkLayerAddTail(pSeg);
				UpdateAllViews(NULL, HINT_SEG, pSeg);
			}
		}
	}
}
void CPegDoc::OnFileQuery()
{
	CPnt pt = app.CursorPosGet();

	CLayer* pLayer = LayersSelUsingPoint(pt);

	if (pLayer != 0)
	{
		CPoint point;
		::GetCursorPos(&point);

		m_strIdentifiedLayerName = pLayer->GetName();

		int iMenu = (pLayer->IsInternal()) ? IDR_LAYER : IDR_TRACING;

		HMENU hMenu = ::LoadMenu(app.GetInstance(), MAKEINTRESOURCE(iMenu));
		CMenu* pMenuSub = CMenu::FromHandle(::GetSubMenu(hMenu, 0));

		pMenuSub->ModifyMenu(0, MF_BYPOSITION | MF_STRING, 0, m_strIdentifiedLayerName);

		if (iMenu == IDR_LAYER)
		{
			pMenuSub->CheckMenuItem(ID_LAYER_WORK, MF_BYCOMMAND | pLayer->IsHot() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_LAYER_ACTIVE, MF_BYCOMMAND | pLayer->IsWarm() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_LAYER_STATIC, MF_BYCOMMAND | pLayer->IsCold() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_LAYER_HIDDEN, MF_BYCOMMAND | pLayer->IsOff() ? MF_CHECKED : MF_UNCHECKED);
		}
		else
		{
			pMenuSub->CheckMenuItem(ID_TRACING_OPEN, MF_BYCOMMAND | pLayer->IsOpened() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_TRACING_MAP, MF_BYCOMMAND | pLayer->IsMapped() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_TRACING_VIEW, MF_BYCOMMAND | pLayer->IsViewed() ? MF_CHECKED : MF_UNCHECKED);
			pMenuSub->CheckMenuItem(ID_TRACING_CLOAK, MF_BYCOMMAND | pLayer->IsOff() ? MF_CHECKED : MF_UNCHECKED);
		}
		pMenuSub->TrackPopupMenu(0, point.x, point.y, CWnd::FromHandle(app.GetSafeHwnd()), 0);
		::DestroyMenu(hMenu);
	}
}
void CPegDoc::OnLayerActive()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);

	if (pLayer == 0)
	{
	}
	else
	{
		if (pLayer->IsHot())
			msgWarning(IDS_MSG_LAYER_NO_ACTIVE, m_strIdentifiedLayerName);
		else
		{
			pLayer->SetStateWarm();
			UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
		}
	}
}
void CPegDoc::OnLayerStatic()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);

	if (pLayer != 0)
	{
		if (pLayer->IsHot())
			msgWarning(IDS_MSG_LAYER_NO_STATIC, m_strIdentifiedLayerName);
		else
		{
			pLayer->SetStateCold();
			UpdateAllViews(NULL, HINT_LAYER_SAFE, pLayer);
		}
	}
}
void CPegDoc::OnLayerHidden()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);

	if (pLayer != 0)
	{
		if (pLayer->IsHot())
			msgWarning(IDS_MSG_LAYER_NO_HIDDEN, m_strIdentifiedLayerName);
		else
		{
			UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
			pLayer->SetStateOff();
		}
	}
}
void CPegDoc::OnLayerMelt()
{
	LayerMelt(m_strIdentifiedLayerName);
}
void CPegDoc::OnLayerWork()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);

	WorkLayerSet(pLayer);
}
void CPegDoc::OnTracingMap()
{
	TracingMap(m_strIdentifiedLayerName);
}
void CPegDoc::OnTracingView()
{
	TracingView(m_strIdentifiedLayerName);
}
void CPegDoc::OnTracingCloak()
{
	CLayer* pLayer = LayersGet(m_strIdentifiedLayerName);

	if (pLayer->IsOpened())
	{
		CFileJob fj;
		if (!fj.OpenForWrite(m_strIdentifiedLayerName))
			return;
		fj.WriteHeader();
		fj.WriteSegs(pLayer);
		SetOpenFile(FILE_TYPE_NONE, "0");
	}
	UpdateAllViews(NULL, HINT_LAYER_ERASE, pLayer);
	pLayer->SetStateOff();
}
void CPegDoc::OnTracingFuse()
{
	TracingFuse(m_strIdentifiedLayerName);
}
void CPegDoc::OnTracingOpen()
{
	TracingOpen(m_strIdentifiedLayerName);
}
///<summary>
///All layers are made active with the exception of the current working layer.
///This is a warm state meaning the layer is displayed using hot color set, is detectable, 
///and may have its segments modified or deleted. No new segments are added to an active layer. 
///Zero or more layers may be active.
///</summary>
void CPegDoc::OnLayersActiveAll()
{
	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (!pLayer->IsHot())
			pLayer->SetStateWarm();
	}
	UpdateAllViews(NULL, 0L, NULL);
}
void CPegDoc::OnLayersStaticAll()
{
	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (!pLayer->IsHot())
			pLayer->SetStateCold();
	}
	UpdateAllViews(NULL, 0L, NULL);
}
void CPegDoc::OnLayersRemoveEmpty()
{
	LayersRemoveEmpty();
}
void CPegDoc::OnSegInsert()
{
	DeletedSegsRestore();
}
void CPegDoc::OnPensRemoveUnusedStyles()
{
	PenStylesRemoveUnused();
}
void CPegDoc::OnBlocksLoad()
{
	CFileDialog dlg(TRUE, "blk", "*.blk");
	dlg.m_ofn.lpstrTitle = "Load Blocks";

	if (dlg.DoModal() == IDOK)
	{
		if ((dlg.m_ofn.Flags & OFN_EXTENSIONDIFFERENT) == 0)
		{
			CFileBlock fb;
			fb.ReadFile(dlg.GetPathName(), m_blks);
		}
		else
			msgWarning(IDS_MSG_FILE_TYPE_ERROR);
	}
}
void CPegDoc::OnBlocksRemoveUnused()
{
	BlksRemoveUnused();
}
void CPegDoc::OnBlocksUnload()
{
	CFileDialog dlg(FALSE, "blk", "*.blk");
	dlg.m_ofn.lpstrTitle = "Unload Blocks As";

	if (dlg.DoModal() == IDOK)
	{
		if ((dlg.m_ofn.Flags & OFN_EXTENSIONDIFFERENT) == 0)
		{
			CFileBlock fb;
			fb.WriteFile(dlg.GetPathName(), m_blks);
		}
	}
}
///<summary>The current view is copied to the clipboard as an enhanced metafile.</summary>
void CPegDoc::OnEditImageToClipboard()
{
	CPegView* pView = CPegView::GetActiveView();

	HDC hdcEMF = ::CreateEnhMetaFile(0, 0, 0, 0);
	LayersDisplayAll(pView, CDC::FromHandle(hdcEMF));
	HENHMETAFILE hemf = ::CloseEnhMetaFile(hdcEMF);

	::OpenClipboard(NULL);
	::EmptyClipboard();
	::SetClipboardData(CF_ENHMETAFILE, hemf);
	::CloseClipboard();
}
///<summary>
///Pastes clipboard to drawing. Only pegsegs format handled and no translation is performed.
///</summary>
void CPegDoc::OnEditTrace()
{
	if (::OpenClipboard(NULL))
	{
		char sBuf[16];

		UINT nClipboardFormat;
		UINT nFormat = 0;

		while ((nClipboardFormat = EnumClipboardFormats(nFormat)) != 0)
		{
			GetClipboardFormatName(nClipboardFormat, sBuf, 16);

			if (strcmp(sBuf, "PegSegs") == 0)
			{
				HGLOBAL hGbl = GetClipboardData(nClipboardFormat);
				if (hGbl != 0)
				{
					CMemFile mf;
					CVec vTrns;

					LPCSTR lpBuffer = (LPCSTR)GlobalLock(hGbl);
					DWORD dwSizeOfBuffer = *((DWORD*)lpBuffer);
					mf.Write(lpBuffer, UINT(dwSizeOfBuffer));
					GlobalUnlock(hGbl);

					mf.Seek(96, CFile::begin);
					mfGetAll(mf, vTrns);

					break;
				}
			}
			nFormat = nClipboardFormat;
		}
		CloseClipboard();
	}
	else

		msgWarning(IDS_MSG_CLIPBOARD_LOCKED);
}
void CPegDoc::OnEditTrapDelete()
{
	trapsegs.DeleteSegs();
	UpdateAllViews(NULL, 0L, NULL);
	OnEditTrapQuit();
}
///<summary>Initializes current trap and all trap component lists.</summary>
void CPegDoc::OnEditTrapQuit()
{
	UpdateAllViews(NULL, HINT_SEGS_SAFE, &trapsegs);
	trapsegs.RemoveAll();
	app.StatusLineDisplay(TrapCnt);
}
void CPegDoc::OnEditTrapCopy()
{
	CPegView* pView = CPegView::GetActiveView();
	DoEditTrapCopy(pView);
}
void CPegDoc::OnEditTrapCut()
{
	CPegView* pView = CPegView::GetActiveView();
	DoEditTrapCopy(pView);
	trapsegs.DeleteSegs();
	UpdateAllViews(NULL, 0L, NULL);
}
///<summary>
///Pastes clipboard to drawing. If the clipboard has the PegSegs format, all other formats are ignored.
///</summary>
void CPegDoc::OnEditTrapPaste()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	if (::OpenClipboard(NULL))
	{
		UINT nClipboardFormat = app.GetClipboardFormatPegSegs();

		if (IsClipboardFormatAvailable(nClipboardFormat))
		{
			if (::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_PASTE_POSITION), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetPasteLoc)) > 0)
			{
				HGLOBAL hGbl = GetClipboardData(nClipboardFormat);
				if (hGbl != 0)
				{
					CPnt ptMin;

					CPnt ptPvt(app.CursorPosGet());
					trapsegs.PvtPt() = ptPvt;

					LPCSTR lpBuffer = (LPCSTR)GlobalLock(hGbl);

					DWORD dwSizeOfBuffer = *((DWORD*)lpBuffer);

					CMemFile mf;
					mf.Write(lpBuffer, UINT(dwSizeOfBuffer));

					mf.Seek(sizeof(DWORD), CFile::begin);
					ptMin.Read(mf);

					CVec vTrns(ptMin, ptPvt);

					GlobalUnlock(hGbl);

					mf.Seek(96, CFile::begin);
					mfGetAll(mf, vTrns);
				}
			}
		}
		else if (IsClipboardFormatAvailable(CF_TEXT))
		{
			HGLOBAL hGbl = GetClipboardData(CF_TEXT);

			LPSTR lpText = new char[GlobalSize(hGbl)];

			LPCSTR lpBuffer = (LPCSTR)GlobalLock(hGbl);
			lstrcpy(lpText, lpBuffer);
			GlobalUnlock(hGbl);

			AddTextBlock(lpText);

			delete [] lpText;
		}
		CloseClipboard();
	}
	else
		msgWarning(IDS_MSG_CLIPBOARD_LOCKED);
}
///<summary>Adds all segments in the work layer to the trap.</summary>
void CPegDoc::OnEditTrapWork()
{
	if (trapsegs.Identify())
	{
		UpdateAllViews(NULL, HINT_SEGS_SAFE, &trapsegs);
	}
	trapsegs.RemoveAll();
	trapsegs.AddTail(WorkLayerGet());
	UpdateAllViews(NULL, HINT_SEGS_SAFE_TRAP, &trapsegs);
	app.StatusLineDisplay(TrapCnt);
}
///<summary>Add all segments in all work and active layers to the trap.</summary>
void CPegDoc::OnEditTrapWorkAndActive()
{
	trapsegs.RemoveAll();

	for (int i = 0; i < static_cast<int>(m_layers.GetSize()); i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (pLayer->IsHot() || pLayer->IsWarm())
		{
			trapsegs.AddTail(pLayer);
		}
	}
	UpdateAllViews(NULL, HINT_SEGS_SAFE_TRAP, &trapsegs);
	app.StatusLineDisplay(TrapCnt);
}
void CPegDoc::OnTrapCommandsHighlight()
{
	m_bTrapHighlight = !m_bTrapHighlight;

	app.CheckMenuItem(ID_TRAPCOMMANDS_HIGHLIGHT, MF_BYCOMMAND | (m_bTrapHighlight ? MF_CHECKED : MF_UNCHECKED));
	trapsegs.SetIdentify(m_bTrapHighlight);

	LPARAM lHint = m_bTrapHighlight ? HINT_SEGS_SAFE_TRAP : HINT_SEGS_SAFE;
	UpdateAllViews(NULL, lHint, &trapsegs);
}
void CPegDoc::OnTrapCommandsCompress()
{
	trapsegs.Compress();
}
void CPegDoc::OnTrapCommandsExpand()
{
	trapsegs.Expand();
}
void CPegDoc::OnTrapCommandsInvert()
{
	int iTblSize = static_cast<int>(m_layers.GetSize());
	for (int i = 0; i < iTblSize; i++)
	{
		CLayer* pLayer = m_layers.GetAt(i);
		if (pLayer->IsHot() || pLayer->IsWarm())
		{
			POSITION pos = pLayer->GetHeadPosition();
			while (pos != 0)
			{
				CSeg* pSeg = pLayer->GetNext(pos);
				POSITION posSeg = trapsegs.Find(pSeg);
				if (posSeg != 0)
					trapsegs.RemoveAt(posSeg);
				else
					trapsegs.AddTail(pSeg);
			}
		}
	}
	UpdateAllViews(NULL, 0L, NULL);
}
void CPegDoc::OnTrapCommandsSquare()
{
	trapsegs.Square();
}
void CPegDoc::OnTrapCommandsQuery()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_EDIT_TRAPCOMMANDS_QUERY), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcEditTrap_CommandsQuery));
}
void CPegDoc::OnTrapCommandsFilter()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_TRAP_FILTER), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcTrapFilter));
}
void CPegDoc::OnTrapCommandsBlock()
{
	if (trapsegs.GetCount() == 0) { return; }

	CBlock* block;
	WORD w = BlksSize();
	std::string blockName;

	do
	{
		std::stringstream ss;
		ss << "_" << std::setfill('0') << std::setw(3) << ++w;
		blockName = ss.str();
	} while (BlksLookup(blockName.c_str(), block));

	block = new CBlock;

	POSITION pos = trapsegs.GetHeadPosition();
	while (pos != 0)
	{
		CSeg* segment = trapsegs.GetNext(pos);

		CSeg* pSeg2 = new CSeg(*segment);

		block->AddTail(pSeg2);

		pSeg2->RemoveAll();

		delete pSeg2;
	}
	block->SetBasePt(trapsegs.PvtPt());
	BlksSetAt(CString(blockName.c_str()), block);
}
void CPegDoc::OnTrapCommandsUnblock()
{
	trapsegs.BreakSegRefs();
}
void CPegDoc::OnSetupPenColor()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_COLOR), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupPenColor));
	app.StatusLineDisplay(Pen);
}
void CPegDoc::OnSetupPenStyle()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_PENSTYLE), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupPenStyle));
	app.StatusLineDisplay(Line);
}
void CPegDoc::OnSetupFillHollow()
{
	pstate.SetPolygonIntStyle(POLYGON_HOLLOW);
}
void CPegDoc::OnSetupFillSolid()
{
	pstate.SetPolygonIntStyle(POLYGON_SOLID);
}
void CPegDoc::OnSetupFillPattern()
{ }
void CPegDoc::OnSetupFillHatch()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_HATCH), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupHatch));
}
void CPegDoc::OnSetupNote()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_SETUP_NOTE), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcSetupNote));
}
void CPegDoc::OnSegBreak()
{
	detsegs.BreakPolylines();
	detsegs.BreakSegRefs();
}
///<summary>
/// Searches for closest detectible segment.  If found, segment is removed
/// from all general segment lists and added to deleted segment list.
/// Notes: The segment resources are not freed.
///</summary>
void CPegDoc::OnSegDelete()
{
	CPnt pt = app.CursorPosGet();

	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);

	if (pSeg != 0)
	{
		CPegView* pView = CPegView::GetActiveView();
		CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

		int iPrimState = pstate.Save();

		AnyLayerRemove(pSeg);
		detsegs.Remove(pSeg);
		if (trapsegs.Remove(pSeg) != NULL)
			app.StatusLineDisplay(TrapCnt);

		UpdateAllViews(NULL, HINT_SEG_ERASE_SAFE, pSeg);
		pstate.Restore(pDC, iPrimState);
		DeletedSegsAddTail(pSeg);
		msgInformation(IDS_SEG_DEL_TO_RESTORE);
	}
}
void CPegDoc::OnSegDeleteLast()
{
	detsegs.DelLast();
}
///<summary>Exchanges the first and last segments on the deleted segment list.</summary>
void CPegDoc::OnSegExchange()
{
	if (!DeletedSegsIsEmpty())
	{
		CSeg* pSegTail = DeletedSegsRemoveTail();
		CSeg* pSegHead = DeletedSegsRemoveHead();
		DeletedSegsAddTail(pSegHead);
		DeletedSegsAddHead(pSegTail);
	}
}
void CPegDoc::OnPrimSnapToEndPoint()
{
	CPegView* pView = CPegView::GetActiveView();

	CPnt4 ptView(app.CursorPosGet(), 1.);
	pView->ModelViewTransform(ptView);

	if (detsegs.IsSegEngaged())
	{
		CPrim* pPrim = detsegs.DetPrim();

		if (pPrim->PvtOnCtrlPt(pView, ptView))
		{
			CPnt ptEng = detsegs.DetPt();
			pPrim->DisRep(ptEng);
			app.CursorPosSet(ptEng);
			return;
		}
		// Did not pivot on engaged primitive
		if (pPrim->IsPtACtrlPt(pView, ptView))
			// still on the control point of the engaged primitive
			// need to keep pick from re-identifying it
			CSeg::IgnorePrim() = pPrim;
	}
	if (detsegs.SelSegAndPrimAtCtrlPt(ptView) != 0)
	{
		CPnt ptEng = detsegs.DetPt();
		detsegs.DetPrim()->DisRep(ptEng);
		app.CursorPosSet(ptEng);
	}
	CSeg::IgnorePrim() = 0;
}
///<summary>Positions the cursor at a "control" point on the current engaged segment.</summary>
void CPegDoc::OnPrimGotoCenterPoint()
{
	if (detsegs.IsSegEngaged())
	{
		CPnt pt = detsegs.DetPrim()->GetCtrlPt();
		app.CursorPosSet(pt);
	}
}
///<summary>
/// Searches for closest detectible primitive.  
/// If found, primitive is lifted from its segment, inserted into a new segment 
/// which is added to deleted segment list. The primitive resources are not freed.
///</summary>
void CPegDoc::OnPrimDelete()
{
	DoPrimDelete(app.CursorPosGet());
}
///<summary>Picks a primative and modifies its attributes to current settings.</summary>
void CPegDoc::OnPrimModifyAttributes()
{
	CPnt pt = app.CursorPosGet();

	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);

	if (pSeg != 0)
	{
		detsegs.DetPrim()->ModifyState();
		UpdateAllViews(NULL, HINT_PRIM_SAFE, detsegs.DetPrim());
	}
}
void CPegDoc::OnSetupSavePoint()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_HOME_POINT_EDIT), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcHomePointSet));
}
void CPegDoc::OnSetupGotoPoint()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_HOME_POINT_GO), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcHomePointGo));
}
void CPegDoc::OnSetupOptionsDraw()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_DRAW_OPTIONS), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcDrawOptions));
	app.StatusLineDisplay();
}
void CPegDoc::OnFileManage()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();
	::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_FILE_MANAGE), pWnd->GetSafeHwnd(), reinterpret_cast<DLGPROC>(DlgProcFileManage));
}
void CPegDoc::OnFileTracing()
{
	static DWORD nFilterIndex = 1;

	char szFilter[256];
	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_TRACINGS, szFilter, sizeof(szFilter));

	OPENFILENAME	of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	of.lpstrFilter = szFilter;
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Tracing File";
	of.Flags = OFN_EXPLORER | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	of.lpstrDefExt = "tra";
	of.lpfnHook = reinterpret_cast<LPOFNHOOKPROC>(OFNHookProcFileTracing);
	of.lpTemplateName = MAKEINTRESOURCE(IDD_TRACING_EX);

	if (GetOpenFileName(&of))
	{
		nFilterIndex = of.nFilterIndex;

		TracingOpen(of.lpstrFile);
	}

	delete [] of.lpstrFile;
}
void CPegDoc::OnMaintenanceRemoveEmptyNotes()
{
	int numOfNotesRemoved = RemoveEmptyNotes();
	int numOfSegmentsRemoved = RemoveEmptySegs();
	std::string paneText = std::to_string(numOfNotesRemoved) + " notes were removed resulting in " + std::to_string(numOfSegmentsRemoved) + " empty segments which were also removed.";
	msgSetPaneText(paneText);
}
void CPegDoc::OnMaintenanceRemoveEmptySegments()
{
	int numOfSegmentsRemoved = RemoveEmptySegs();
	std::string paneText = std::to_string(numOfSegmentsRemoved) + " were removed.";
	msgSetPaneText(paneText);
}
void CPegDoc::OnPensEditColors()
{
	app.PenColorsChoose();
}
void CPegDoc::OnPensLoadColors()
{
	char szFilter[256];
	::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_PENCOLORS, szFilter, sizeof(szFilter));

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = 0;
	of.hInstance = app.GetInstance();
	of.lpstrFilter = szFilter;
	of.lpstrFile = new char[MAX_PATH];
	of.lpstrFile[0] = 0;
	of.nMaxFile = MAX_PATH;
	of.lpstrTitle = "Load Pen Colors";
	of.lpstrInitialDir = app.m_strAppPath;
	of.Flags = OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
	of.lpstrDefExt = "txt";

	if (GetOpenFileName(&of))
	{
		if ((of.Flags & OFN_EXTENSIONDIFFERENT) == 0)
		{
			app.PenColorsLoad(of.lpstrFile);
			app.SetBackGround(crHotCols[0]);
			UpdateAllViews(NULL, 0L, NULL);
		}
		else
			msgWarning(IDS_MSG_FILE_TYPE_ERROR);
	}
	delete [] of.lpstrFile;
}
void CPegDoc::OnPensTranslate()
{
	CStdioFile fl;

	if (fl.Open(app.m_strAppPath + "\\Pens\\xlate.txt", CFile::modeRead | CFile::typeText))
	{
		char pBuf[128];
		WORD wCols = 0;

		while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0)
			wCols++;

		if (wCols > 0)
		{
			PENCOLOR* pColNew = new PENCOLOR[wCols];
			PENCOLOR* pCol = new PENCOLOR[wCols];

			WORD w = 0;

			fl.SeekToBegin();

			while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0)
			{
				char* context = nullptr;
				pCol[w] = PENCOLOR(atoi(strtok_s(pBuf, ",", &context)));
				pColNew[w++] = PENCOLOR(atoi(strtok_s(0, "\n", &context)));
			}
			CPegDoc::GetDoc()->PenTranslation(wCols, pColNew, pCol);

			delete [] pColNew;
			delete [] pCol;
		}
	}
	UpdateAllViews(NULL, 0L, NULL);
}
void CPegDoc::OnFile()
{
	CWnd* pWnd = AfxGetApp()->GetMainWnd();

	CPoint pnt(8, 8);

	pWnd->ClientToScreen(&pnt);
	HMENU hFilePopupMenu = app.GetSubMenu(0);
	::TrackPopupMenu(hFilePopupMenu, TPM_LEFTALIGN, pnt.x, pnt.y, 0, pWnd->GetSafeHwnd(), 0);
}
void CPegDoc::OnPrimExtractNum()
{
	CPnt pt = app.CursorPosGet();

	CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(pt);
	if (pSeg == 0) return;

	CPrim* pPrim = detsegs.DetPrim();

	CString strChr;

	if (pPrim->Is(CPrim::Type::Text))
	{
		strChr = static_cast<CPrimText*>(pPrim)->Text();
	}
	else if (pPrim->Is(CPrim::Type::Dim))
	{
		strChr = static_cast<CPrimDim*>(pPrim)->Text();
	}
	else
		return;

	int iTokId = 0;
	long lDef;
	int iTyp;
	double dVal[32];
	size_t bufferSize = sizeof(dVal);

	try
	{
		lex::Parse(strChr);
		lex::EvalTokenStream((char*)0, &iTokId, &lDef, &iTyp, (void*)dVal, bufferSize);

		if (iTyp != lex::TOK_LENGTH_OPERAND)
		{
			lex::ConvertValTyp(iTyp, lex::TOK_REAL, &lDef, dVal);
		}
		std::stringstream ss;
		ss << std::fixed << std::setprecision(4) << std::setw(10) << dVal[0] << " was extracted from drawing";
		std::string str = ss.str();
		msgSetPaneText(str);
		gbl_dExtNum = dVal[0];
		dde::PostAdvise(dde::ExtNumInfo);
	}
	catch (char* szMessage)
	{
		::MessageBox(0, szMessage, 0, MB_ICONWARNING | MB_OK);
	}
	return;
}
void CPegDoc::OnPrimExtractStr()
{
	CPnt pt = app.CursorPosGet();

	if (detsegs.SelSegAndPrimUsingPoint(pt) != 0)
	{
		CPrim* pPrim = detsegs.DetPrim();

		CString strChr;

		if (pPrim->Is(CPrim::Type::Text))
		{
			strChr = static_cast<CPrimText*>(pPrim)->Text();
		}
		else if (pPrim->Is(CPrim::Type::Dim))
		{
			strChr = static_cast<CPrimDim*>(pPrim)->Text();
		}
		else
		{
			return;
		}
		strcpy_s(gbl_szExtStr, sizeof(gbl_szExtStr), strChr.GetString());

		strChr += " was extracted from drawing";
		msgInformation(strChr);
		dde::PostAdvise(dde::ExtStrInfo);
	}
	return;
}
// Returns a pointer to the currently active document.
CPegDoc* CPegDoc::GetDoc(void)
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd);
	if (pFrame == NULL)
		return NULL;

	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->MDIGetActive();

	return (pChild == NULL) ? NULL : (CPegDoc*)pChild->GetActiveDocument();
}
void CPegDoc::OnFileSave()
{
	WriteShadowFile();
	CDocument::OnFileSave();
}
