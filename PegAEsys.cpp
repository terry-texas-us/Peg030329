#include "stdafx.h"

#include <afxcoll.h>
#include <afxdisp.h>
#include <afxdlgs.h>
#include <afxmsg_.h>
#include <afxres.h>
#include <afxstr.h>
#include <atltypes.h>
#include <ShlObj_core.h>
#include <tchar.h>

#include <direct.h>
#include <string.h>

#include <cmath>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>

#include "ChildFrm.h"
#include "MainFrm.h"
#include "PegAEsys.h"
#include "PegAEsysDoc.h"
#include "PegAEsysView.h"

#include "CharCellDef.h"
#include "Dde.h"
#include "Directory.h"
#include "DlgProcEditOps.h"
#include "FontDef.h"
#if ODA_FUNCTIONALITY
#include "FileOpenDWG.h"
#endif
#include "Grid.h"
#include "Messages.h"
#include "ModelTransform.h"
#include "OpenGL.h"
#include "PenStyle.h"
#include "Pnt.h"
#include "Pnt4.h"
#include "Polygon.h"
#include "Prim.h"
#include "PrimPolygon.h"
#include "PrimState.h"
#include "Seg.h"
#include "Segs.h"
#include "SegsDet.h"
#include "SegsTrap.h"
#include "SubProcAnnotate.h"
#include "SubProcCut.h"
#include "SubProcDimension.h"
#include "SubProcDraw.h"
#include "SubProcDraw2.h"
#include "SubProcEdit.h"
#include "SubProcEditPrimitive.h"
#include "SubProcEditSegment.h"
#include "SubProcFixup.h"
#include "SubProcLPD.h"
#include "SubProcMendPrim.h"
#include "SubProcNodal.h"
#include "SubProcPower.h"
#include "SubProcPipe.h"
#include "SubProcRLPD.h"
#include "SubProcTrapAdd.h"
#include "SubProcTrapRemove.h"
#include "TMat.h"
#include "UnitsString.h"
#include "UserAxis.h"

INT_PTR CALLBACK DlgProcModeLetter(HWND, UINT, WPARAM, LPARAM) noexcept;
INT_PTR CALLBACK DlgProcModeRevise(HWND, UINT, WPARAM, LPARAM) noexcept;

LRESULT CALLBACK WndProcKeyPlan(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProcPreview(HWND, UINT, WPARAM, LPARAM);

namespace hatch {
double dXAxRefVecScal;
double dYAxRefVecScal;
double dOffAng;
int iTableOffset[64];
float fTableValue[1536];
}  // namespace hatch
double gbl_dExtNum = 0.;
char gbl_szExtStr[128]{};

double dPWids[] = {0.,   0.0075, 0.015, 0.02,   0.03, 0.0075, 0.015, 0.0225,
                   0.03, 0.0075, 0.015, 0.0225, 0.03, 0.0075, 0.015, 0.0225};

#include "PegColors.h"

COLORREF* pColTbl = crHotCols;

CPrimState pstate;
CModelTransform mspace;
CSegsTrap trapsegs;
CSegsDet detsegs;
CTMat tmEditSeg;

extern DWORD dwMendPrimPtId;
extern CPrim* pPrimToMend;
extern CPrim* pPrimToMendCopy;
extern CPnt ptMendPrimBeg;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPegApp

// CPegApp construction

CPegApp::CPegApp() {
  SetShadowDir("PegAEsys Shadow Files");

  m_bEditCFImage = false;
  m_bEditCFSegments = true;
  m_bEditCFText = true;
  m_bViewStateInfo = true;  // View state info within the view
  m_bViewModeInfo = false;  // View mode info within the view
  m_bTrapModeAddSegments = true;
  m_nClipboardFormatPegSegs = 0;
  m_dEngLen = 0.;
  m_dEngAngZ = 0.;
  m_dDimLen = 0.;
  m_dDimAngZ = 0.;
  m_dScale = 1.;
  m_iUnitsPrec = 8;
  m_eUnits = Inches;
  m_pStrokeFontDef = 0;
}

// The one and only CPegApp object

CPegApp app;

// CPegApp initialization

BOOL CPegApp::InitInstance() {
  // InitCommonControls() is required on Windows XP if an application
  // manifest specifies use of ComCtl32.dll version 6 or later to enable
  // visual styles.  Otherwise, any window creation will fail.
  InitCommonControls();

  CWinApp::InitInstance();

  // Initialize OLE libraries
  if (!AfxOleInit()) {
    AfxMessageBox(IDP_OLE_INIT_FAILED);
    return FALSE;
  }
  AfxEnableControlContainer();
  // Standard initialization
  // If you are not using these features and wish to reduce the size
  // of your final executable, you should remove from the following
  // the specific initialization routines you do not need
  // Set the registry key under which our settings are stored
  SetRegistryKey(_T("Fanning, Fanning & Associates"));
  LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
  // Register the application's document templates.  Document templates
  //  serve as the connection between documents, frame windows and views
  CMultiDocTemplate* pDocTemplate;
  pDocTemplate = new CMultiDocTemplate(IDR_PEGAESYSTYPE, RUNTIME_CLASS(CPegDoc),
                                       RUNTIME_CLASS(CChildFrame),  // custom Peg child frame
                                       RUNTIME_CLASS(CPegView));
  AddDocTemplate(pDocTemplate);
  // create main Peg Frame window
  CMainFrame* pMainFrame = new CMainFrame;
  if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) return FALSE;
  m_pMainWnd = pMainFrame;
  // call DragAcceptFiles only if there's a suffix
  //  In an Peg app, this should occur immediately after setting m_pMainWnd
  // Enable drag/drop open
  m_pMainWnd->DragAcceptFiles();

  // Parse command line for standard shell commands, DDE, file open
  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);
  // Dispatch commands specified on the command line.  Will return FALSE if
  // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
  if (!ProcessShellCommand(cmdInfo)) return FALSE;

  // Get app path by stripping quotes and program name from command line
  m_strAppPath = ::GetCommandLine();
  Path_UnquoteSpaces(m_strAppPath);

  m_hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
#if !GL_FUNCTIONALITY
  ::EnableMenuItem(m_hMenu, ID_VIEW_LIGHTING, MF_BYCOMMAND | MF_GRAYED);
  ::EnableMenuItem(m_hMenu, ID_VIEW_RENDERED, MF_BYCOMMAND | MF_GRAYED);
  ::EnableMenuItem(m_hMenu, ID_VIEW_WIREFRAME, MF_BYCOMMAND | MF_GRAYED);
#endif

  WNDCLASS wc{};

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProcKeyPlan;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_hInstance;
  wc.hIcon = 0;
  wc.hCursor = ::LoadCursor(0, IDC_CROSS);
  wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = 0;
  wc.lpszClassName = "View";

  if (!::RegisterClass(&wc)) return (FALSE);

  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProcPreview;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = m_hInstance;
  wc.hIcon = 0;
  wc.hCursor = ::LoadCursor(0, IDC_CROSS);
  wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = 0;
  wc.lpszClassName = "Preview";

  if (!::RegisterClass(&wc)) return (FALSE);

  HatchesLoad(m_strAppPath + "\\..\\..\\res\\Hatches\\DefaultSet.txt");
  PenWidthsLoad(m_strAppPath + "\\..\\..\\res\\Pens\\Widths.txt");
  PenColorsLoad(m_strAppPath + "\\..\\..\\res\\Pens\\Colors\\Default.txt");
  PenStylesLoad(m_strAppPath + "\\..\\..\\res\\Pens\\LineTypes.txt");
  StrokeFontLoad("");

  CPegView* pView = CPegView::GetActiveView();
  CDC* pDC = pView->GetDC();

  InitGbls(m_pMainWnd->GetDC());

  SetBackGround(crHotCols[0]);

  pView->SetDeviceWidthInPixels(pDC->GetDeviceCaps(HORZRES));
  pView->SetDeviceHeightInPixels(pDC->GetDeviceCaps(VERTRES));
  pView->SetDeviceWidthInInches(double(pDC->GetDeviceCaps(HORZSIZE)) / 25.4);
  pView->SetDeviceHeightInInches(double(pDC->GetDeviceCaps(VERTSIZE)) / 25.4);

  opengl::Initialize(pDC);

  app.LineFontCreate();

  SetWindowMode(ID_MODE_DRAW);

  // The main window has been initialized, so show and update it
  m_pMainWnd->ShowWindow(m_nCmdShow);
  m_pMainWnd->UpdateWindow();

  // Initialize for using DDEML
  dde::Setup(m_hInstance);

  // This is the private data format used to pass segs from one instance of peg to another
  m_nClipboardFormatPegSegs = RegisterClipboardFormat("PegSegs");

  return TRUE;
}

BEGIN_MESSAGE_MAP(CPegApp, CWinApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
ON_COMMAND(ID_FILE_RUN, OnFileRun)
ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
ON_COMMAND(ID_MODE_ANNOTATE, OnModeAnnotate)
ON_COMMAND(ID_MODE_CUT, OnModeCut)
ON_COMMAND(ID_MODE_DIMENSION, OnModeDimension)
ON_COMMAND(ID_MODE_DRAW, OnModeDraw)
ON_COMMAND(ID_MODE_DRAW2, OnModeDraw2)
ON_COMMAND(ID_MODE_EDIT, OnModeEdit)
ON_COMMAND(ID_MODE_FIXUP, OnModeFixup)
ON_COMMAND(ID_MODE_LETTER, OnModeLetter)
ON_COMMAND(ID_MODE_LPD, OnModeLPD)
ON_COMMAND(ID_MODE_NODAL, OnModeNodal)
ON_COMMAND(ID_MODE_PIPE, OnModePipe)
ON_COMMAND(ID_MODE_POWER, OnModePower)
ON_COMMAND(ID_MODE_PRIM_EDIT, OnModePrimEdit)
ON_COMMAND(ID_MODE_PRIM_MEND, OnModePrimMend)
ON_COMMAND(ID_MODE_REVISE, OnModeRevise)
ON_COMMAND(ID_MODE_RLPD, OnModeRLPD)
ON_COMMAND(ID_MODE_SEGEDIT, OnModeSegEdit)
ON_COMMAND(ID_MODE_TRAP, OnModeTrap)
ON_COMMAND(ID_TRAPCOMMANDS_ADDSEGMENTS, OnTrapCommandsAddSegments)
ON_COMMAND(ID_VIEW_MODEINFORMATION, OnViewModeInformation)
ON_COMMAND(ID_VIEW_STATEINFORMATION, OnViewStateInformation)
#pragma warning(push)
#pragma warning(disable : 4191)
ON_UPDATE_COMMAND_UI(ID_MODE_ANNOTATE, OnUpdateModeAnnotate)
ON_UPDATE_COMMAND_UI(ID_MODE_CUT, OnUpdateModeCut)
ON_UPDATE_COMMAND_UI(ID_MODE_DIMENSION, OnUpdateModeDimension)
ON_UPDATE_COMMAND_UI(ID_MODE_DRAW, OnUpdateModeDraw)
ON_UPDATE_COMMAND_UI(ID_MODE_DRAW2, OnUpdateModeDraw2)
ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, OnUpdateModeEdit)
ON_UPDATE_COMMAND_UI(ID_MODE_FIXUP, OnUpdateModeFixup)
ON_UPDATE_COMMAND_UI(ID_MODE_LPD, OnUpdateModeLpd)
ON_UPDATE_COMMAND_UI(ID_MODE_NODAL, OnUpdateModeNodal)
ON_UPDATE_COMMAND_UI(ID_MODE_PIPE, OnUpdateModePipe)
ON_UPDATE_COMMAND_UI(ID_MODE_POWER, OnUpdateModePower)
ON_UPDATE_COMMAND_UI(ID_MODE_RLPD, OnUpdateModeRlpd)
ON_UPDATE_COMMAND_UI(ID_MODE_TRAP, OnUpdateModeTrap)
ON_UPDATE_COMMAND_UI(ID_VIEW_MODEINFORMATION, OnUpdateViewModeinformation)
ON_UPDATE_COMMAND_UI(ID_VIEW_STATEINFORMATION, OnUpdateViewStateinformation)
#pragma warning(pop)
END_MESSAGE_MAP()

// CPegApp message handlers

void CPegApp::OnEditCfImage() {
  m_bEditCFImage = !m_bEditCFImage;
  app.CheckMenuItem(ID_EDIT_CF_IMAGE, static_cast<UINT>(MF_BYCOMMAND | (m_bEditCFImage ? MF_CHECKED : MF_UNCHECKED)));
}
void CPegApp::OnEditCfSegments() {
  m_bEditCFSegments = !m_bEditCFSegments;
  app.CheckMenuItem(ID_EDIT_CF_SEGMENTS,
                    static_cast<UINT>(MF_BYCOMMAND | (m_bEditCFSegments ? MF_CHECKED : MF_UNCHECKED)));
}
void CPegApp::OnEditCfText() {
  m_bEditCFText = !m_bEditCFText;
  app.CheckMenuItem(ID_EDIT_CF_TEXT, static_cast<UINT>(MF_BYCOMMAND | (m_bEditCFText ? MF_CHECKED : MF_UNCHECKED)));
}
// Subclasses the main window to Trap mode operations.
void CPegApp::OnModeTrap() { app.SetWindowMode(m_bTrapModeAddSegments ? ID_MODE_TRAP : ID_MODE_TRAPR); }
// Subclasses the main window to Draw mode operations.
void CPegApp::OnModeDraw() { SetWindowMode(ID_MODE_DRAW); }
void CPegApp::OnModeLPD() { SetWindowMode(ID_MODE_LPD); }
void CPegApp::OnModeAnnotate() { SetWindowMode(ID_MODE_ANNOTATE); }
void CPegApp::OnModeDimension() { SetWindowMode(ID_MODE_DIMENSION); }
void CPegApp::OnModeDraw2() { SetWindowMode(ID_MODE_DRAW2); }
void CPegApp::OnModeEdit() { SetWindowMode(ID_MODE_EDIT); }
void CPegApp::OnModeCut() { SetWindowMode(ID_MODE_CUT); }
void CPegApp::OnModeNodal() {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  nodal::SegList = new CSegs;
  nodal::UPL = new CObList;
  nodal::PrimLis = new CObList;
  nodal::bAdd = true;

  CPnts pts;
  short nMarkStyle = pstate.MarkStyle();
  pstate.SetMarkStyle(11);

  POSITION pos = pDoc->WorkLayerGetHeadPosition();
  while (pos != 0) {
    CSeg* pSeg = pDoc->WorkLayerGetNext(pos);

    POSITION posPrim = pSeg->GetHeadPosition();
    while (posPrim != 0) {
      CPrim* pPrim = pSeg->GetNext(posPrim);

      pPrim->GetAllPts(pts);
      // TODO display pts
    }
  }
  pstate.SetMarkStyle(nMarkStyle);
  SetWindowMode(ID_MODE_NODAL);
}
void CPegApp::OnModeFixup() { SetWindowMode(ID_MODE_FIXUP); }
void CPegApp::OnModeLetter() {
#pragma tasMSG(TODO : OnModeLetter - Using app main window as parent of dialog)
  ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ADD_NOTE), GetSafeHwnd(), DlgProcModeLetter);
}
void CPegApp::OnModePipe() { SetWindowMode(ID_MODE_PIPE); }
void CPegApp::OnModePower() { SetWindowMode(ID_MODE_POWER); }
void CPegApp::OnModeRLPD() { SetWindowMode(ID_MODE_RLPD); }

void CPegApp::OnModePrimEdit() {
  app.SetEditSegBeg(app.CursorPosGet());

  CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(app.GetEditSegBeg());

  if (pSeg != 0) {
    SetWindowMode(ID_MODE_PRIM_EDIT);
    AppSetGinCur(pSeg, detsegs.DetPrim());
  }
}
void CPegApp::OnModeSegEdit() {
  app.SetEditSegBeg(app.CursorPosGet());

  CSeg* pSeg = detsegs.SelSegAndPrimUsingPoint(app.GetEditSegBeg());

  if (pSeg != 0) {
    SetWindowMode(ID_MODE_SEG_EDIT);
    AppSetGinCur(pSeg);
  }
}
void CPegApp::OnModeRevise() {
#pragma tasMSG(TODO : OnModeRevise - Using app main window as parent of dialog)
  ::DialogBox(app.GetInstance(), MAKEINTRESOURCE(IDD_ADD_NOTE), GetSafeHwnd(), DlgProcModeRevise);
}
void CPegApp::OnModePrimMend() {
  CPegView* pView = CPegView::GetActiveView();

  CPnt pt = app.CursorPosGet();
  CPnt4 ptView(pt, 1.);
  pView->ModelViewTransform(ptView);

  pPrimToMend = 0;

  if (detsegs.IsSegEngaged()) {  // Segment is currently engaged, see if cursor is on a control point
    CPnt ptDet;

    CPrim* pPrim = detsegs.DetPrim();

    CPrimPolygon::EdgeToEvaluate() = CPrimPolygon::Edge();

    if (pPrim->SelUsingPoint(pView, ptView, detsegs.PicApertSiz(),
                             ptDet)) {  // Cursor is close enough to engaged primitive to use it first
      pPrimToMend = pPrim;
    }
  }
  if (pPrimToMend == 0) {  // No engaged segment, or engaged primitive to far from cursor
    if (detsegs.SelSegAndPrimUsingPoint(ptMendPrimBeg) != 0) {  // Segment successfully engaged
      pPrimToMend = detsegs.DetPrim();
    }
  }
  ptMendPrimBeg = pt;

  if (pPrimToMend != 0) {
    pPrimToMend->Copy(pPrimToMendCopy);
    ptMendPrimBeg = pPrimToMend->SelAtCtrlPt(pView, ptView);
    dwMendPrimPtId = 1U << static_cast<DWORD>(CPrim::CtrlPt());

    SetWindowMode(ID_MODE_PRIM_MEND);
  }
}
void CPegApp::OnTrapCommandsAddSegments() {
  m_bTrapModeAddSegments = !m_bTrapModeAddSegments;
  app.CheckMenuItem(ID_TRAPCOMMANDS_ADDSEGMENTS, (m_bTrapModeAddSegments ? MF_CHECKED : MF_UNCHECKED));

  if (m_iModeId == ID_MODE_TRAP)
    app.SetWindowMode(ID_MODE_TRAPR);
  else if (m_iModeId == ID_MODE_TRAPR)
    app.SetWindowMode(ID_MODE_TRAP);
}
void CPegApp::OnFileRun() {
  char szFilter[256]{};
  ::LoadString(app.GetInstance(), IDS_OPENFILE_FILTER_APPS, szFilter, sizeof(szFilter));

  CFileDialog dlg(TRUE, "exe", "*.exe", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter);
  dlg.m_ofn.lpstrTitle = "Run Application";

  if (dlg.DoModal() == IDOK) {
    CString strFile = dlg.GetFileName();

    STARTUPINFO StartupInfo{sizeof(StartupInfo)};  // Initialize startup information

    StartupInfo.dwFlags = STARTF_USESHOWWINDOW;  // Enable the wShowWindow flag
    StartupInfo.wShowWindow = SW_SHOW;

    PROCESS_INFORMATION processInfo{0};  // Will receive process and thread info

    // Call CreateProcess
    BOOL success =
        CreateProcess(NULL, (LPTSTR)strFile.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &processInfo);

    if (!success) {
      // Handle error, e.g., via GetLastError()
      return;
    }

    // Optionally wait for the process to complete (WinExec does not wait by default)
    // WaitForSingleObject(pi.hProcess, INFINITE);

    // Clean up handles
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    return;
  }
}
void CPegApp::OnHelpContents() { ::WinHelp(app.GetSafeHwnd(), "peg.hlp", HELP_CONTENTS, 0L); }
void CPegApp::PenWidthsLoad(const CString& fileName) {
  std::ifstream file(fileName);
  if (!file.is_open()) { return; }

  std::string line;
  while (std::getline(file, line)) {
    std::size_t equalPosition{line.find('=')};
    if (equalPosition == std::string::npos) { continue; }  // skip lines without '='

    try {
      std::string idString{line.substr(0, equalPosition)};
      int iId = std::stoi(idString);

      std::string valueString{line.substr(equalPosition + 1)};
      std::size_t commaPosition = valueString.find(',');
      if (commaPosition != std::string::npos) { valueString = valueString.substr(0, commaPosition); }

      double value = std::stod(valueString);
      const int arraySize = static_cast<int>(sizeof(dPWids) / sizeof(dPWids[0]));

      if (iId >= 0 && iId < arraySize) { dPWids[iId] = value; }
    } catch (const std::exception&) {
      continue;  // skip misunderstood lines
    }
  }
}
WORD CPegApp::GetFileTypeFromPath(const CString& strPathName) {
  int nExt = strPathName.ReverseFind('.');

  if (nExt != -1) {
    CString strExt = strPathName.Mid(nExt);

    if (strExt.CompareNoCase(".blk") == 0)
      return (CPegDoc::FILE_TYPE_BLOCK);
    else if (strExt.CompareNoCase(".dwg") == 0)
      return (CPegDoc::FILE_TYPE_DWG);
    else if (strExt.CompareNoCase(".dxf") == 0)
      return (CPegDoc::FILE_TYPE_DXF);
    else if (strExt.CompareNoCase(".ffa") == 0)
      return (CPegDoc::FILE_TYPE_FFA);
    else if (strExt.CompareNoCase(".peg") == 0)
      return (CPegDoc::FILE_TYPE_PEG);
    else if (strExt.CompareNoCase(".tra") == 0)
      return (CPegDoc::FILE_TYPE_TRACING);
    else if (strExt.CompareNoCase(".jb1") == 0)
      return (CPegDoc::FILE_TYPE_JOB);
    else if (strExt.CompareNoCase(".bmp") == 0)
      return (CPegDoc::FILE_TYPE_BMP);
  }
  return (CPegDoc::FILE_TYPE_NONE);
}
void CPegApp::SetShadowDir(const std::string& strDir) {
  TCHAR szPath[MAX_PATH]{};

  if (SUCCEEDED(SHGetSpecialFolderPath(m_pMainWnd->GetSafeHwnd(), szPath, CSIDL_PERSONAL, TRUE))) {
    m_strShadowDir = szPath;
  } else {
    m_strShadowDir.clear();
  }
  m_strShadowDir += "\\" + strDir + "\\";

  _mkdir(m_strShadowDir.c_str());
}
CPnt CPegApp::CursorPosGet() {
  CPegView* pView = CPegView::GetActiveView();
  if (pView == NULL) return ORIGIN;

  CPoint pntCurPos;

  ::GetCursorPos(&pntCurPos);
  pView->ScreenToClient(&pntCurPos);

  CPnt pt(double(pntCurPos.x), double(pntCurPos.y), m_ptCursorPosDev[2]);
  if (pt != m_ptCursorPosDev) {
    m_ptCursorPosDev = pt;
    m_ptCursorPosWorld = m_ptCursorPosDev;

    pView->DoProjectionInverse(m_ptCursorPosWorld);

    m_ptCursorPosWorld = pView->ModelViewGetMatrixInverse() * m_ptCursorPosWorld;
    m_ptCursorPosWorld = grid::UserSnapPt(m_ptCursorPosWorld);
  }
  return (m_ptCursorPosWorld);
}
///<summary> Positions cursor at targeted position.</summary>
void CPegApp::CursorPosSet(const CPnt& pt) {
  CPegView* pView = CPegView::GetActiveView();

  CPnt4 ptView(pt, 1);

  pView->ModelViewTransform(ptView);

  if (!Pnt4_IsInView(ptView)) {  // Redefine the view so targeted position becomes center
    pView->ModelViewSetTarget(pt);
    pView->ModelViewSetEye(pView->ModelViewGetDirection());

    m_pMainWnd->InvalidateRect(NULL, TRUE);

    ptView = pt;
    pView->ModelViewTransform(ptView);
  }
  // Move the cursor to specified position.
  CPoint pntCurPos = pView->DoProjection(ptView);
  m_ptCursorPosDev(pntCurPos.x, pntCurPos.y, ptView[2] / ptView[3]);
  m_ptCursorPosWorld = pt;

  pView->ClientToScreen(&pntCurPos);
  ::SetCursorPos(pntCurPos.x, pntCurPos.y);
}
// Loads the hatch table.
void CPegApp::HatchesLoad(const CString& strFileName) {
  CFileException e;
  CStdioFile fl;

  if (!fl.Open(strFileName, CFile::modeRead | CFile::typeText, &e)) return;

  char szLn[128]{};
  double dTotStrsLen{0.};
  int iNmbEnts{0};
  int iNmbStrsId{0};

  char szValDel[] = ",\0";
  int iHatId{0};
  int iNmbHatLns{0};
  int iTblId{0};

  while (fl.ReadString(szLn, sizeof(szLn) - 1) != 0) {
    if (*szLn == '!')  // New Hatch index
    {
      if (iHatId != 0) hatch::fTableValue[hatch::iTableOffset[iHatId]] = float(iNmbHatLns);
      hatch::iTableOffset[++iHatId] = iTblId++;
      iNmbHatLns = 0;
    } else {
      iNmbStrsId = iTblId;
      iTblId += 2;
      iNmbEnts = 0;
      dTotStrsLen = 0.;
      char* context = nullptr;
      char* pTok = strtok_s(szLn, szValDel, &context);
      while (pTok != 0) {
        hatch::fTableValue[iTblId] = float(atof(pTok));
        iNmbEnts++;
        if (iNmbEnts >= 6) dTotStrsLen = dTotStrsLen + hatch::fTableValue[iTblId];
        iTblId++;
        pTok = strtok_s(0, szValDel, &context);
      }
      hatch::fTableValue[iNmbStrsId++] = float(iNmbEnts - 5);
      hatch::fTableValue[iNmbStrsId] = float(dTotStrsLen);
      iNmbHatLns++;
    }
  }
}
CPnt CPegApp::HomePointGet(int i) const {
  if (i >= 0 && i < 9) return (m_ptHomePoint[i]);

  return (ORIGIN);
}
void CPegApp::HomePointSave(int i, const CPnt& pt) {
  if (i >= 0 && i < 9) m_ptHomePoint[i] = pt;
}
//Initializes all peg global sections to their default (startup) values.
void CPegApp::InitGbls(CDC* pDC) {
  pstate.SetPolygonIntStyle(POLYGON_HOLLOW);
  pstate.SetPolygonIntStyleId(1);

  hatch::dXAxRefVecScal = .1;
  hatch::dYAxRefVecScal = .1;
  hatch::dOffAng = 0.;

  dlgproceditops::SetMirrorScale(-1, 1., 1.);

  dlgproceditops::SetRotOrd(0, 1, 2);
  dlgproceditops::SetRotAng(0., 0., 45.);
  dlgproceditops::SetScale(2., 2., 2.);

  CCharCellDef ccd;
  pstate.SetCharCellDef(ccd);

  CFontDef fd;
  pstate.SetFontDef(pDC, fd);

  app.SetScale(96.);
  app.SetUnits(Engineering);
  app.SetUnitsPrec(8);
  app.SetDimLen(.125);
  app.SetDimAngZ(45.);

  grid::Init();
  UserAxisInit();

  AppSetGinCur();
  pstate.SetPen(pDC, 1, 1);
  pstate.SetMarkStyle(1);
}
// Creates the font used for all overlaid text display.
// This is primarily the mode line and status line.
// Should be called at start-up and every time window is resized.
void CPegApp::LineFontCreate() {
  LineFontRelease();

  m_pFontApp = new CFont;
  m_pFontApp->CreateStockObject(ANSI_VAR_FONT);
}

void CPegApp::LineFontRelease() {
  if (m_pFontApp != nullptr) {
    m_pFontApp->DeleteObject();
    delete m_pFontApp;
    m_pFontApp = nullptr;
  }
}
#pragma tasMSG(TODO : Move PenStylesLoad to CPegDoc)
///<summary>Loads the PenStyle table.</summary>
void CPegApp::PenStylesLoad(const CString& strFileName) {
  CPegDoc* pDoc = CPegDoc::GetDoc();
  if (pDoc == NULL) return;

  CStdioFile fl;

  // TODO check for existance of table  - crashes on 99 otherwise ands no pen dlg box.

  if (fl.Open(strFileName, CFile::modeRead | CFile::typeText)) {
    CString strDescription;
    CString strName;
    char pBuf[128]{};

    WORD wLensMax = 8;
    double* pLen = new double[wLensMax];
    char* context = nullptr;
    while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0) {
      int iId = atoi(strtok_s(pBuf, "=", &context));
      strName = strtok_s(0, ",", &context);
      strDescription = strtok_s(0, "\n", &context);
      fl.ReadString(pBuf, sizeof(pBuf) - 1);

      WORD wLens = WORD(atoi(strtok_s(pBuf, ",\n", &context)));

      if (wLens > wLensMax) {
        delete[] pLen;
        pLen = new double[wLens];
        wLensMax = wLens;
      }
      for (WORD w = 0; w < wLens; w++) pLen[w] = atof(strtok_s(0, ",\n", &context));

      pDoc->PenStylesInsert(iId, new CPenStyle(strName, strDescription, wLens, pLen));
    }
    delete[] pLen;
  }
}
void CPegApp::ModifyMenu(UINT uPos, LPCTSTR lpNewItem) {
  if (m_pMainWnd) {
    HMENU hMenu = ::GetSubMenu(m_hMenu, static_cast<int>(uPos));
    ::ModifyMenu(m_hMenu, uPos, MF_BYPOSITION | MF_STRING | MF_POPUP, UINT_PTR(hMenu), lpNewItem);

    m_pMainWnd->DrawMenuBar();
  }
}
void CPegApp::PenColorsChoose() {
  CHOOSECOLOR cc{};

  cc.lStructSize = sizeof(CHOOSECOLOR);

  cc.rgbResult = crHotCols[pstate.PenColor()];
  cc.lpCustColors = crHotCols;
  cc.Flags = CC_FULLOPEN | CC_RGBINIT | CC_SOLIDCOLOR;
  ::ChooseColor(&cc);

  cc.rgbResult = crWarmCols[pstate.PenColor()];
  cc.lpCustColors = crWarmCols;
  ::ChooseColor(&cc);
  SetBackGround(crHotCols[0]);

  CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}
// Loads the color table.
void CPegApp::PenColorsLoad(const CString& strFileName) {
  CStdioFile fl;

  if (fl.Open(strFileName, CFile::modeRead | CFile::typeText)) {
    char pBuf[128]{};
    LPSTR pId, pRed, pGreen, pBlue;

    while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0 && _strnicmp(pBuf, "<Colors>", 8) != 0);

    char* context = nullptr;
    while (fl.ReadString(pBuf, sizeof(pBuf) - 1) != 0 && *pBuf != '<') {
      pId = strtok_s(pBuf, "=", &context);
      pRed = strtok_s(0, ",", &context);
      pGreen = strtok_s(0, ",", &context);
      pBlue = strtok_s(0, ",", &context);
      crHotCols[atoi(pId)] = RGB(atoi(pRed), atoi(pGreen), atoi(pBlue));
      pRed = strtok_s(0, ",", &context);
      pGreen = strtok_s(0, ",", &context);
      pBlue = strtok_s(0, "\n", &context);
      crWarmCols[atoi(pId)] = RGB(atoi(pRed), atoi(pGreen), atoi(pBlue));
    }
  }
}
// Disables rubberbanding.
void CPegApp::RubberBandingDisable() {
  CPegView* pView = CPegView::GetActiveView();
  CDC* pDC = (pView == NULL) ? NULL : pView->GetDC();

  if (m_iGinRubTyp == 0) return;

  int iDrawMode = pstate.SetROP2(pDC, R2_XORPEN);

  if (m_iGinRubTyp == Lines) {
    CPoint pnt[2];

    pnt[0] = m_pntGinStart;
    pnt[1] = m_pntGinEnd;
    pDC->Polyline(pnt, 2);
  } else if (m_iGinRubTyp == Quads) {
    CPoint pnt[5];
    int iPts = 1;

    pnt[0] = m_pntGinStart;
    pnt[1].x = m_pntGinStart.x + (m_pntGinCur.x - m_pntGinEnd.x);
    pnt[1].y = m_pntGinStart.y + (m_pntGinCur.y - m_pntGinEnd.y);

    if (pnt[1].x != m_pntGinStart.x || pnt[1].y != m_pntGinStart.y) {
      pnt[2] = m_pntGinCur;
      pnt[3] = m_pntGinEnd;
      iPts = 4;
    }
    pnt[iPts++] = m_pntGinStart;
    pDC->Polyline(pnt, iPts);
  } else if (m_iGinRubTyp == Circles) {
    CBrush* pBrushOld = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    int iRadiusX = abs(m_pntGinEnd.x - m_pntGinStart.x);
    int iRadiusY = abs(m_pntGinEnd.y - m_pntGinStart.y);
    int iRadius = (int)sqrt((double)(iRadiusX * iRadiusX + iRadiusY * iRadiusY) + .5);
    pDC->Ellipse(m_pntGinStart.x - iRadius, m_pntGinStart.y + iRadius, m_pntGinStart.x + iRadius,
                 m_pntGinStart.y - iRadius);
    pDC->SelectObject(pBrushOld);
  } else if (m_iGinRubTyp == Rectangles) {
    CBrush* pBrushOld = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
    pDC->Rectangle(m_pntGinStart.x, m_pntGinStart.y, m_pntGinEnd.x, m_pntGinEnd.y);
    pDC->SelectObject(pBrushOld);
  }
  pstate.SetROP2(pDC, iDrawMode);
  m_iGinRubTyp = 0;
}
void CPegApp::RubberBandingEnable(ERubs aiRubOp) {
  // Enables rubberbanding.

  m_iGinRubTyp = aiRubOp;

  if (aiRubOp == Quads) m_pntGinCur = m_pntGinEnd;
}
void CPegApp::RubberBandingStartAtEnable(const CPnt& pt, ERubs eRub) {
  CPegView* pView = CPegView::GetActiveView();

  CPnt4 ptView(pt, 1);

  pView->ModelViewTransform(ptView);

  if (Pnt4_IsInView(ptView)) {
    m_ptRubStart = pt;
    m_ptRubEnd = pt;

    m_pntGinStart = pView->DoProjection(ptView);
    m_pntGinEnd = m_pntGinStart;
  }
  RubberBandingEnable(eRub);
}
void CPegApp::SetBackGround(COLORREF cr) {
  CPegView* pView = CPegView::GetActiveView();
  if (pView != NULL) {
    ::SetClassLongPtr(pView->GetSafeHwnd(), GCLP_HBRBACKGROUND, reinterpret_cast<LONG_PTR>(::CreateSolidBrush(cr)));
    CDC* pDC = pView->GetDC();
    pDC->SetBkColor(cr);
  }
}
void CPegApp::SetModeCursor(int iModeId) {
  WORD wResourceId;

  switch (iModeId) {
    case ID_MODE_ANNOTATE:
      wResourceId = ID_MODE_ANNOTATE;
      break;

    case ID_MODE_CUT:
      wResourceId = ID_MODE_CUT;
      break;

    case ID_MODE_DIMENSION:
      wResourceId = ID_MODE_DIMENSION;
      break;

    case ID_MODE_DRAW:
      wResourceId = ID_MODE_DRAW;
      break;

    case ID_MODE_LPD:
    case ID_MODE_PIPE:
    case ID_MODE_POWER:
    case ID_MODE_RLPD:
      wResourceId = IDC_APPS;
      break;

    case ID_MODE_DRAW2:
      wResourceId = ID_MODE_DRAW2;
      break;

    case ID_MODE_EDIT:
      wResourceId = ID_MODE_EDIT;
      break;

    case ID_MODE_FIXUP:
      wResourceId = ID_MODE_FIXUP;
      break;

    case ID_MODE_NODAL:
      wResourceId = ID_MODE_NODAL;
      break;

    case ID_MODE_NODALR:
      wResourceId = ID_MODE_NODALR;
      break;

    case ID_MODE_TRAP:
      wResourceId = ID_MODE_TRAP;
      break;

    case ID_MODE_TRAPR:
      wResourceId = ID_MODE_TRAPR;
      break;

    default:
      SetCursor(LoadCursor(IDC_CROSS));
      return;
  }
  HCURSOR hCursor = LoadCursor(MAKEINTRESOURCE(wResourceId));

  if (hCursor != 0) SetCursor(hCursor);
}
void CPegApp::SetWindowMode(int aiModeId) {
  CFrameWnd* pFrame = (CFrameWnd*)m_pMainWnd;
  HWND hwnd = pFrame->GetSafeHwnd();

  switch (aiModeId) {
    case ID_MODE_DRAW2:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcDraw2));
      break;

    case ID_MODE_ANNOTATE:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcAnnotate));
      m_iPrimModeId = ID_MODE_ANNOTATE;
      break;

    case ID_MODE_CUT:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcCut));
      break;

    case ID_MODE_DIMENSION:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcDimension));
      break;

    case ID_MODE_DRAW:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcDraw));
      m_iPrimModeId = ID_MODE_DRAW;
      break;

    case ID_MODE_EDIT:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcEdit));
      break;

    case ID_MODE_FIXUP:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcFixup));
      break;

    case ID_MODE_LPD:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcLPD));
      break;

    case ID_MODE_RLPD:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcRLPD));
      break;

    case ID_MODE_NODAL:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcNodal));
      break;

    case ID_MODE_PIPE:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcPipe));
      break;

    case ID_MODE_POWER:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcPower));
      break;

    case ID_MODE_PRIM_EDIT:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcEditPrimitive));
      break;

    case ID_MODE_SEG_EDIT:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcEditSegment));
      break;

    case ID_MODE_PRIM_MEND:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcMendPrim));
      break;

    case ID_MODE_TRAP:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcTrapAdd));
      break;

    case ID_MODE_TRAPR:
      ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(SubProcTrapRemove));
  }

  RubberBandingDisable();

  m_iModeId = aiModeId;
  msgInformation(0);
  SetModeCursor(aiModeId);
  StatusLineDisplay();
  ModeLineDisplay();
}
void CPegApp::StatusLineDisplay(EStatusLineItem sli) const {
  if (m_bViewStateInfo) {
    CPegView* pView = CPegView::GetActiveView();

    if (pView == NULL) return;

    CDC* pDC = pView->GetDC();

    CRect rc;
    char szBuf[128]{};

    CFont* pFont = pDC->SelectObject(m_pFontApp);
    UINT nTextAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);
    COLORREF crText = pDC->SetTextColor(AppGetTextCol());
    COLORREF crBk = pDC->SetBkColor(~AppGetTextCol());

    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);

    CRect rcClient;
    pView->GetClientRect(&rcClient);

    if (sli == All || sli == WorkCnt) {  // print num in work
      rc.SetRect(0, rcClient.top, 8 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      std::string str = std::to_string(CPegDoc::GetDoc()->GetHotCount() + CPegDoc::GetDoc()->GetWarmCount()) + "  ";
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    if (sli == All || sli == TrapCnt) {  // print num in trap
      rc.SetRect(10 * tm.tmAveCharWidth, rcClient.top, 19 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      std::string str = " " + std::to_string(trapsegs.GetCount()) + "  ";
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    if (sli == All || sli == Pen) {  // print pen  info
      rc.SetRect(21 * tm.tmAveCharWidth, rcClient.top, 27 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      std::string str = "P " + std::to_string(pstate.PenColor()) + " ";
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    if (sli == All || sli == Line) {  // print line info
      rc.SetRect(29 * tm.tmAveCharWidth, rcClient.top, 35 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      std::string str = "L " + std::to_string(pstate.PenStyle());
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    if (sli == All || sli == TextHeight) {  // print text height
      CCharCellDef ccd;
      pstate.GetCharCellDef(ccd);
      rc.SetRect(37 * tm.tmAveCharWidth, rcClient.top, 47 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      std::stringstream ss;
      ss << "T " << std::fixed << std::setprecision(2) << ccd.ChrHgtGet();
      std::string str = ss.str();
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    if (sli == All || sli == Scale) {  //print scale
      rc.SetRect(49 * tm.tmAveCharWidth, rcClient.top, 59 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      std::stringstream ss;
      ss << "1: " << std::fixed << std::setprecision(2) << GetScale();
      std::string str = ss.str();
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    if (sli == All || sli == WndRatio) {  //print zoom
      rc.SetRect(61 * tm.tmAveCharWidth, rcClient.top, 71 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      double dRatio = pView->GetWidthInInches() / pView->ModelViewGetUExt();
      std::stringstream ss;
      ss << "@ " << std::fixed << std::setprecision(3) << dRatio;
      std::string str = ss.str();
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    if (sli == All || sli == DimLen) {  // print DimLen
      rc.SetRect(73 * tm.tmAveCharWidth, rcClient.top, 90 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      UnitsString_FormatLength(szBuf, sizeof(szBuf), GetUnits(), GetDimLen());
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, szBuf, (UINT)strlen(szBuf), 0);
    }
    if (sli == All || sli == DimAng) {  // print DimAngle
      rc.SetRect(92 * tm.tmAveCharWidth, rcClient.top, 107 * tm.tmAveCharWidth, rcClient.top + tm.tmHeight);
      std::stringstream ss;
      ss << ">> " << std::fixed << std::setprecision(4) << GetDimAngZ();
      std::string str = ss.str();
      pDC->ExtTextOut(rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc, str.c_str(), (UINT)str.length(), 0);
    }
    // restore Device Context to its original state
    pDC->SetBkColor(crBk);
    pDC->SetTextColor(crText);
    pDC->SetTextAlign(nTextAlign);
    pDC->SelectObject(pFont);
  }
}
// size (16384) of object for 96 character
// font set with a maximum of 4096 stokes
void CPegApp::StrokeFontLoad(const CString& strPathName) {
  if (strPathName.GetLength() < 1) {
    int fontlen;
    HRSRC hrsrc;
    LPVOID fontptr;

    // Open binary resources
    hrsrc = FindResource(NULL, MAKEINTRESOURCE(IDR_PEGSTROKEFONT), "STROKEFONT");
    fontlen = static_cast<int>(SizeofResource(NULL, hrsrc));
    m_pStrokeFontDef = new char[static_cast<size_t>(fontlen)];

    fontptr = LockResource(LoadResource(NULL, hrsrc));
    memcpy(m_pStrokeFontDef, fontptr, static_cast<size_t>(fontlen));

  } else {
    HANDLE hFile = CreateFile(strPathName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile != ((HANDLE)-1)) {
      if (SetFilePointer(hFile, 0, 0, FILE_BEGIN) != (DWORD)-1) {
        if (m_pStrokeFontDef == 0) m_pStrokeFontDef = new char[16384];

        DWORD nBytesRead;
        ReadFile(hFile, m_pStrokeFontDef, 16384U, &nBytesRead, 0);
      }
      CloseHandle(hFile);
    }
  }
}
void CPegApp::StrokeFontRelease() {
  // Releases a large character array loaded for stroke fonts.

  if (m_pStrokeFontDef != 0) delete[] m_pStrokeFontDef;
}
COLORREF AppGetTextCol() { return (~(crHotCols[0] | 0xff000000)); }
void CPegApp::OnUpdateModeDraw(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_DRAW); }

void CPegApp::OnUpdateModeAnnotate(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_ANNOTATE); }

void CPegApp::OnUpdateModeTrap(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_TRAP); }

void CPegApp::OnUpdateModeEdit(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_EDIT); }

void CPegApp::OnUpdateModeDraw2(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_DRAW2); }

void CPegApp::OnUpdateModeDimension(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_DIMENSION); }

void CPegApp::OnUpdateModeCut(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_CUT); }

void CPegApp::OnUpdateModeNodal(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_NODAL); }

void CPegApp::OnUpdateModeFixup(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_FIXUP); }

void CPegApp::OnUpdateModeLpd(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_LPD); }

void CPegApp::OnUpdateModePower(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_POWER); }

void CPegApp::OnUpdateModePipe(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_PIPE); }

void CPegApp::OnUpdateModeRlpd(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_iModeId == ID_MODE_RLPD); }

void CPegApp::OnViewModeInformation() {
  m_bViewModeInfo = !m_bViewModeInfo;
  CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}
void CPegApp::OnUpdateViewModeinformation(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_bViewModeInfo); }
void CPegApp::OnViewStateInformation() {
  m_bViewStateInfo = !m_bViewStateInfo;
  CPegDoc::GetDoc()->UpdateAllViews(NULL, 0L, NULL);
}
void CPegApp::OnUpdateViewStateinformation(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_bViewStateInfo); }
