#pragma once

#include <Windows.h>  // for HWND, HTREEITEM

#include <afx.h>
#include <afxwin.h>  // for CDC

#include <afxstr.h>  // for CString

#include "FontDef.h"  // for CFontDef member
#include "Line.h"     // for CLine member
#include "Pnt.h"      // for CPnt
#include "Pnt4.h"     // for CPnt4
#include "Prim.h"     // for CPrim base class, PENSTYLE and PENCOLOR types
#include "RefSys.h"   // for CRefSys member
#include "TMat.h"     // for CTMat
#include "Vec.h"      // for CVec

class CSeg;
class CSegs;
class CPegView;

class CPrimDim : public CPrim {
 private:
  CLine m_ln{};
  PENCOLOR m_nTextPenColor{1};
  CFontDef m_fd{};
  CRefSys m_rs{};
  CString m_strText{};

 public:  // Constructors and destructor
  CPrimDim() {}
  CPrimDim(PENCOLOR penColor, PENSTYLE penStyle, const CLine& line);
  CPrimDim(char*);

  CPrimDim(const CPrimDim& other);

  ~CPrimDim() {}

 public:  // Operators
  const CPrimDim& operator=(const CPrimDim& other);

 public:  // Methods - absolute virtuals
  void AddToTreeViewControl(HWND tree, HTREEITEM parent) const;
  void Assign(CPrim* primitive) { *this = *static_cast<CPrimDim*>(primitive); }
  CPrim*& Copy(CPrim*&) const;
  void Display(CPegView* view, CDC* context) const;
  void DisRep(const CPnt& point) const;
  void FormatExtra(CString& str) const;
  void FormatGeometry(CString& str) const;
  void GetAllPts(CPnts& points);
  CPnt GetCtrlPt() const { return m_ln.ProjPtAlong(0.5); }
  void GetExtents(CPnt&, CPnt&, const CTMat&) const;
  CPnt GoToNxtCtrlPt() const;
  bool Is(CPrim::Type type) const { return type == CPrim::Type::Dim; }
  bool IsInView(CPegView* view) const;
  bool IsPtACtrlPt(CPegView* pView, const CPnt4&) const;
  void Read(CFile& file);
  CPnt SelAtCtrlPt(CPegView* view, const CPnt4&) const;
  bool SelUsingLine(CPegView* view, const CLine& ln, CPnts& ptInt);
  bool SelUsingPoint(CPegView* view, const CPnt4&, double, CPnt&);
  bool SelUsingRect(CPegView* view, const CPnt&, const CPnt&);
  void Transform(const CTMat&);
  void Translate(const CVec&);
  void TranslateUsingMask(const CVec&, const DWORD);
  bool Write(CFile& file) const;
  void Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
  bool Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

 public:  // Methods - virtuals
  void CutAt2Pts(CPnt*, CSegs*, CSegs*);
  void CutAtPt(const CPnt&, CSeg*);
  void ModifyState();

 public:  // Methods
  void GetBoundingBox(CPnts& ptsBox, double dSpacFac) const;
  const CFontDef& FontDef() const { return m_fd; }
  const CLine& Line() const { return m_ln; }
  void GetPts(CPnt& ptBeg, CPnt& ptEnd) const {
    ptBeg = m_ln[0];
    ptEnd = m_ln[1];
  }
  void GetRefSys(CRefSys& rs) const { rs = m_rs; }
  double Length() const { return m_ln.Length(); }
  double RelOfPt(const CPnt& pt) const;
  void SetDefaultNote();
  void SetPt0(const CPnt& pt) { m_ln[0] = pt; }
  void SetPt1(const CPnt& pt) { m_ln[1] = pt; }
  void SetText(const CString& str) { m_strText = str; }
  void SetTextHorAlign(WORD w) { m_fd.TextHorAlignSet(w); }
  void SetTextPenColor(PENCOLOR nPenColor) { m_nTextPenColor = nPenColor; }
  void SetTextVerAlign(WORD w) { m_fd.TextVerAlignSet(w); }
  const CString& Text() const { return m_strText; }
  const PENCOLOR& TextPenColor() const { return m_nTextPenColor; }

 private:
  static WORD mS_wFlags;  // bit 1 clear if dimension selected at note else set if dimension selected at line
};
