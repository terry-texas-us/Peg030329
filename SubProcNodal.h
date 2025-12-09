#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxcoll.h>

#include "Pnt.h"
#include "Vec.h"

class CPrim;
class CSeg;
class CSegs;

LRESULT CALLBACK SubProcNodal(HWND, UINT, WPARAM, LPARAM) noexcept;

#define btest(m, p) ((bool)(((((DWORD)m) >> ((int)p)) & 1UL) == 1 ? true : false))
#define ibset(m, p) ((DWORD)(((DWORD)m) | (1UL << ((int)p))))
#define ibclr(m, p) ((DWORD)(((DWORD)m) & ~(1UL << ((int)p))))

class CNodalPrim : public CObject {
 public:
  const CPrim* pPrim;
  DWORD dwMask;

 public:
  CNodalPrim() noexcept : CObject(), pPrim(nullptr), dwMask(0) {}
  CNodalPrim(const CNodalPrim&) = delete;
  CNodalPrim& operator=(const CNodalPrim&) = delete;

  CNodalPrim(CNodalPrim&&) = delete;
  CNodalPrim& operator=(CNodalPrim&&) = delete;
};

class CUniqPt : public CObject {
 public:
  int iCnt;
  CPnt pt;

 public:
  CUniqPt() noexcept : CObject(), iCnt(0), pt() {}
  CUniqPt(const CUniqPt&) = delete;
  CUniqPt& operator=(const CUniqPt&) = delete;
  CUniqPt(CUniqPt&&) = delete;
  CUniqPt& operator=(CUniqPt&&) = delete;
};

namespace nodal {
extern bool bAdd;
extern CObList* UPL;
extern CSegs* SegList;
extern CObList* PrimLis;

void AddByArea(CPnt, CPnt);
void AddByPt(const CPnt&);
void AddEngPrim();
void AddPrimBit(bool, const CPrim*, int);
void AddPrimByPt(const CPnt);
void CopyPrims(const CVec&);
void GenFilAreas(const CVec&);
void GenVecs(const CVec&);
DWORD GetPrimMask(CPrim*);
void Translate(const CVec&);
void UpdLis(bool, CSeg*, CPrim*, DWORD, int, CPnt);
}  // namespace nodal
