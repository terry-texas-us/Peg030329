#include "stdafx.h"

#include <corecrt.h>
#include <string.h>

#include <cstdlib>

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "CharCellDef.h"
#include "ddeCmds.h"
#include "FileJob.h"
#include "FontDef.h"
#include "Layer.h"
#include "Pnt.h"
#include "Prim.h"
#include "PrimState.h"
#include "PrimText.h"
#include "RefSys.h"
#include "Seg.h"
#include "SegsDet.h"
#include "SegsTrap.h"
#include "Text.h"
#include "UnitsString.h"
#include "Vec.h"

using namespace dde;

///<summary>Sets the Text Height.</summary>

bool dde::ExecNoteHT(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[32]{};
  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);
  CCharCellDef ccd;
  pstate.GetCharCellDef(ccd);
  ccd.ChrHgtSet(atof(szBuf));
  pstate.SetCharCellDef(ccd);
  return true;
}
///<summary>Sets the Fill.</summary>
bool dde::ExecFill(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[8]{};
  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);
  pstate.SetPolygonIntStyleId(static_cast<short>(_ttoi(szBuf)));
  return true;
}
///<summary>Sets the Scale.</summary>
bool dde::ExecScale(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[32]{};
  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);
  app.SetScale(atof(szBuf));
  app.StatusLineDisplay(Scale);
  return true;
}
///<summary>Sets the Diamond Length.</summary>
bool dde::ExecDL(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[32]{};
  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);
  app.SetDimLen(UnitsString_ParseLength(app.GetUnits(), szBuf));
  app.StatusLineDisplay(DimLen);
  return true;
}
///<summary>Sets the Diamond Angle.</summary>
bool dde::ExecDA(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[32]{};
  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);
  app.SetDimAngZ(atof(szBuf));
  app.StatusLineDisplay(DimAng);
  return true;
}
///<summary></summary>
bool dde::ExecTracingBlank(PTOPICINFO, LPSTR, UINT, UINT uiNargs, LPSTR* ppArgs) {
  int iPathKey = static_cast<int>(uiNargs - 1);

  CPegDoc* pDoc = CPegDoc::GetDoc();
  pDoc->LayerBlank(ppArgs[iPathKey]);

  return true;
}
///<summary>Maps a tracing file.</summary>
bool dde::ExecTracingMap(PTOPICINFO, LPSTR, UINT, UINT uiNargs, LPSTR* ppArgs) {
  int iPathKey = static_cast<int>(uiNargs - 1);

  CPegDoc* pDoc = CPegDoc::GetDoc();
  pDoc->TracingMap(ppArgs[iPathKey]);

  return true;
}
///<summary>Opens a tracing file.</summary>
bool dde::ExecTracingOpen(PTOPICINFO, LPSTR, UINT, UINT uiNargs, LPSTR* ppArgs) {
  int iPathKey = static_cast<int>(uiNargs - 1);

  CPegDoc* pDoc = CPegDoc::GetDoc();
  pDoc->TracingOpen(ppArgs[iPathKey]);

  return true;
}
///<summary>Views a tracing file.</summary>
bool dde::ExecTracingView(PTOPICINFO, LPSTR, UINT, UINT uiNargs, LPSTR* ppArgs) {
  int iPathKey = static_cast<int>(uiNargs - 1);

  CPegDoc* pDoc = CPegDoc::GetDoc();
  pDoc->TracingView(ppArgs[iPathKey]);

  return true;
}
///<summary>Gets a tracing file.</summary>
bool dde::ExecFileGet(PTOPICINFO, LPSTR, UINT, UINT uiNargs, LPSTR* ppArgs) {
  int iPathKey = static_cast<int>(uiNargs - 1);

  CPegDoc* pDoc = CPegDoc::GetDoc();

  // Reference .. set up translation vector
  CPnt ptPvt(app.CursorPosGet());
  trapsegs.PvtPt() = ptPvt;
  CVec vTrns(ORIGIN, ptPvt);

  CFileJob fj;

  if (fj.OpenForRead(ppArgs[iPathKey])) {
    CLayer* pLayer = pDoc->WorkLayerGet();

    fj.ReadHeader();
    fj.ReadSegs(pLayer);
    detsegs.AddTail(pLayer);

    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE, &trapsegs);

    trapsegs.RemoveAll();
    trapsegs.AddTail(pLayer);
    trapsegs.Translate(vTrns);

    pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEGS_SAFE_TRAP, &trapsegs);
  }
  return true;
}
///<summary>Set the position of the cursor.</summary>
bool dde::ExecGotoPoint(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[8]{};

  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);

  int iStakeId = _ttoi(szBuf);
  app.CursorPosSet(app.HomePointGet(iStakeId));

  return true;
}
///<summary>Sets the pen color.</summary>
bool dde::ExecPen(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[8]{};

  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);

  pstate.SetPenColor(PENCOLOR(_ttoi(szBuf)));
  app.StatusLineDisplay(Pen);

  return true;
}
///<summary>Sets the pen style</summary>
bool dde::ExecLine(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[8]{};

  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);

  pstate.SetPenStyle(PENSTYLE(_ttoi(szBuf)));
  app.StatusLineDisplay(Line);

  return true;
}
///<summary>Adds a note the drawing at the current cursor position.</summary>
bool dde::ExecNote(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  CPegDoc* pDoc = CPegDoc::GetDoc();

  CPnt ptPvt = app.CursorPosGet();

  CFontDef fd;
  pstate.GetFontDef(fd);
  CCharCellDef ccd;
  pstate.GetCharCellDef(ccd);

  CRefSys rs(ptPvt, ccd);

  CSeg* pSeg = new CSeg(new CPrimText(fd, rs, ppArgs[0]));
  pDoc->WorkLayerAddTail(pSeg);
  pDoc->UpdateAllViews(nullptr, CPegDoc::HINT_SEG, pSeg);

  ptPvt = text_GetNewLinePos(fd, rs, 1., 0);
  app.CursorPosSet(ptPvt);

  return true;
}
///<summary>Posts message to force key driven action.</summary>
bool dde::ExecSend(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  HWND hWndTarget = GetFocus();
  TCHAR* pIdx = (LPSTR)ppArgs[0];
  while (*pIdx != 0) {
    if (*pIdx == '{') {
      *pIdx++;
      int iVkValue = _ttoi(pIdx);
      ::PostMessage(hWndTarget, WM_KEYDOWN, static_cast<WPARAM>(iVkValue), 0L);
      while ((*pIdx != 0) && (*pIdx != '}')) pIdx++;
    } else {
      ::PostMessage(hWndTarget, WM_CHAR, static_cast<WPARAM>(*pIdx), 0L);
      pIdx++;
    }
  }
  return true;
}
///<summary>Sets a home point.</summary>
bool dde::ExecSetPoint(PTOPICINFO, LPSTR, UINT, UINT, LPSTR* ppArgs) {
  TCHAR szBuf[8]{};

  _tcsncpy_s(szBuf, sizeof(szBuf), ppArgs[0], _TRUNCATE);

  int iStakeId = _ttoi(szBuf);

  app.HomePointSave(iStakeId, app.CursorPosGet());

  return true;
}
