#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxstr.h>
#include <afxwin.h>

#include "Line.h"
#include "Pnt.h"
#include "Pnt4.h"
#include "Prim.h"
#include "TMat.h"
#include "Vec.h"

class CPegView;

class CPrimPolygon : public CPrim {
 private:
  short m_nIntStyle;
  short m_nIntStyleId;
  WORD m_wPts;
  CPnt m_ptOrig;
  CVec m_vPosXAx;
  CVec m_vPosYAx;
  CPnt* m_Pt;

 public:  // Constructors and destructor
  CPrimPolygon();
  CPrimPolygon(char*, int iVer);
#if ODA_FUNCTIONALITY
  CPrimPolygon(AD_DB_HANDLE, PAD_ENT_HDR, PAD_ENT);
#endif
  CPrimPolygon(WORD, CPnt*);
  CPrimPolygon(size_t numberOfPoints, CPnt origin, const CVec& xAxis, const CVec& yAxis, const CPnt* points);

  CPrimPolygon(const CPrimPolygon& src);

 public:  // Operators
  const CPrimPolygon& operator=(const CPrimPolygon&);

  ~CPrimPolygon() override;

 public:  // Methods - absolute virtuals
  void AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const override;
  void Assign(CPrim* pPrim) override { *this = *static_cast<CPrimPolygon*>(pPrim); }
  CPrim*& Copy(CPrim*&) const override;
  void Display(CPegView* pView, CDC* pDC) const override;
  void DisRep(const CPnt&) const override;
  void FormatExtra(CString& str) const override;
  void FormatGeometry(CString& str) const override;
  void GetAllPts(CPnts& pts) override;
  CPnt GetCtrlPt() const override;
  void GetExtents(CPnt&, CPnt&, const CTMat&) const override;
  CPnt GoToNxtCtrlPt() const override;
  bool Is(CPrim::Type type) const override { return type == CPrim::Type::Polygon; }
  bool IsPtACtrlPt(CPegView* pView, const CPnt4&) const override;
  bool IsInView(CPegView* pView) const override;
  void Read(CFile&) override;
  CPnt SelAtCtrlPt(CPegView* pView, const CPnt4&) const override;
  bool SelUsingLine(CPegView*, const CLine&, CPnts&) override { return false; }
  bool SelUsingPoint(CPegView* pView, const CPnt4&, double, CPnt&) override;
  bool SelUsingRect(CPegView* pView, const CPnt&, const CPnt&) override;
  void Transform(const CTMat&) override;
  void Translate(const CVec&) override;
  void TranslateUsingMask(const CVec&, const DWORD) override;
  bool Write(CFile&) const override;
  void Write(CFile&, char*) const override;
#if ODA_FUNCTIONALITY
  bool Write(AD_DB_HANDLE, AD_VMADDR, PAD_ENT_HDR, PAD_ENT);
#endif

  CString FormatIntStyle() const;
  const short& IntStyle() const { return (m_nIntStyle); }
  const short& IntStyleId() const { return (m_nIntStyleId); }
  CPnt GetPt(int i) const { return (m_Pt[i]); }
  int GetPts() { return (m_wPts); }
  void ModifyState() override;
  bool PvtOnCtrlPt(CPegView* pView, const CPnt4&) const override;
  void SetIntStyle(const short n) { m_nIntStyle = n; }
  void SetIntStyleId(const short n) { m_nIntStyleId = n; }
  void SetHatRefVecs(double, double, double);

 private:
  WORD SwingVertex() const;

 private:
  static WORD mS_wEdgeToEvaluate;
  static WORD mS_wEdge;
  static WORD mS_wPivotVertex;

 public:
  static WORD& EdgeToEvaluate() { return mS_wEdgeToEvaluate; }
  static WORD& Edge() { return mS_wEdge; }
};
