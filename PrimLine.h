#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>
#include <afxwin.h>

#include "Line.h"
#include "Pnt.h"
#include "Pnt4.h"
#include "Prim.h"
#include "Seg.h"
#include "Segs.h"
#include "TMat.h"
#include "Vec.h"

class CPegView;

class CPrimLine : public CPrim {
 private:
  CLine m_ln;

 public:  // Constructors and destructor
  CPrimLine() {}
  CPrimLine(char*, int iVer);
#if ODA_FUNCTIONALITY
  CPrimLine(PAD_ENT);
#endif
  CPrimLine(const CPnt& pt0, const CPnt& pt1);
  CPrimLine(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CLine& ln);
  CPrimLine(PENCOLOR nPenColor, PENSTYLE nPenStyle, const CPnt& pt0, const CPnt& pt1);

  CPrimLine(const CPrimLine& src);

  ~CPrimLine() {};

 public:  // Operators
  const CPrimLine& operator=(const CPrimLine& src);

 public:  //	Methods - absolute virtuals
  void AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const;
  void Assign(CPrim* pPrim) { *this = *static_cast<CPrimLine*>(pPrim); }
  CPrim*& Copy(CPrim*&) const;
  void Display(CPegView* pView, CDC* pDC) const;
  void DisRep(const CPnt&) const;
  void FormatExtra(CString& str) const;
  void FormatGeometry(CString& str) const;
  void GetAllPts(CPnts& pts);
  CPnt GetCtrlPt() const { return m_ln.ProjPtAlong(.5); }
  void GetExtents(CPnt&, CPnt&, const CTMat&) const;
  CPnt GoToNxtCtrlPt() const;
  bool Is(CPrim::Type type) const { return type == CPrim::Type::Line; }
  bool IsInView(CPegView* pView) const;
  bool IsPtACtrlPt(CPegView* pView, const CPnt4&) const;
  void Read(CFile&);
  CPnt SelAtCtrlPt(CPegView* pView, const CPnt4&) const;
  bool SelUsingLine(CPegView* pView, const CLine& ln, CPnts& ptsInt);
  bool SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&);
  bool SelUsingRect(CPegView* pView, const CPnt&, const CPnt&);
  void Transform(const CTMat& tm) {
    SetPt0(tm * Pt0());
    SetPt1(tm * Pt1());
  }
  void Translate(const CVec& v) { m_ln += v; }
  void TranslateUsingMask(const CVec&, const DWORD);
  bool Write(CFile&) const;
  void Write(CFile&, char*) const;
#if ODA_FUNCTIONALITY
  bool Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

 public:  // Methods - virtuals
  void CutAt2Pts(CPnt*, CSegs*, CSegs*);
  void CutAtPt(const CPnt&, CSeg*);
  int IsWithinArea(const CPnt&, const CPnt&, CPnt*);

 public:  // Methods
  double GetAngAboutZAx() const { return m_ln.GetAngAboutZAx(); }
  void GetLine(CLine& ln) const { ln = m_ln; }
  void GetPts(CPnt& ptBeg, CPnt& ptEnd) const {
    ptBeg = m_ln[0];
    ptEnd = m_ln[1];
  }
  const CPnt& Pt0() const { return m_ln[0]; }
  const CPnt& Pt1() const { return m_ln[1]; }
  const CLine& Ln() const { return m_ln; }
  double Length() const { return (m_ln.Length()); }
  CPnt ProjPt(const CPnt& pt) const { return (m_ln.ProjPt(pt)); }
  double RelOfPt(const CPnt& pt) const;
  void SetPt0(const CPnt& pt) { m_ln[0] = pt; }
  void SetPt1(const CPnt& pt) { m_ln[1] = pt; }
  void Square();
};
