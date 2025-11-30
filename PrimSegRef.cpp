#include "stdafx.h"

#include <sstream>

#include "PegAEsysDoc.h"

#include "FilePeg.h"
#include "Messages.h"
#include "ModelTransform.h"
#include "PrimSegRef.h"

CPrimSegRef::CPrimSegRef()
{
	m_pt = ORIGIN;
	m_vZ = ZDIR;

	m_vScale(1., 1., 1.);
	m_dRotation = 0.;

	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}

CPrimSegRef::CPrimSegRef(const CString& strName, const CPnt& pt)
{
	m_strName = strName;
	m_pt = pt;
	m_vZ = ZDIR;
	m_vScale(1., 1., 1.);
	m_dRotation = 0.;
	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}
CPrimSegRef::CPrimSegRef(const CPrimSegRef& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vZ = src.m_vZ;
	m_vScale = src.m_vScale;
	m_dRotation = src.m_dRotation;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;
}

const CPrimSegRef& CPrimSegRef::operator=(const CPrimSegRef& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vZ = src.m_vZ;
	m_vScale = src.m_vScale;
	m_dRotation = src.m_dRotation;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;

	return (*this);
}

void CPrimSegRef::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	CBlock* pBlock;
	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) { return; }

	HTREEITEM hti = tvAddItem(hTree, hParent, "<SegRef>", (CObject*)this);

	((CSeg*)pBlock)->AddPrimsToTreeViewControl(hTree, hti);
}
CTMat CPrimSegRef::BuildTransformMatrix(const CPnt& ptBase) const
{
	CTMat tm1; tm1.Translate(ORIGIN - ptBase);
	CTMat tm2; tm2.Scale(m_vScale);
	CTMat tm3; tm3.RotateZ(sin(m_dRotation), cos(m_dRotation));
	CTMat tm4(ORIGIN, m_vZ);
	CTMat tm5; tm5.Translate(m_pt - ORIGIN);

	return (tm1 * tm2 * tm3 * tm4 * tm5);
}

CPrim*& CPrimSegRef::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimSegRef(*this);
	return (pPrim);
}
void CPrimSegRef::Display(CPegView* pView, CDC* pDC) const
{
	CBlock* pBlock;
	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0)
		return;

	CPnt ptBase = pBlock->GetBasePt();
	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	pBlock->Display(pView, pDC);

	mspace.Return();
}
void CPrimSegRef::DisRep(const CPnt&) const
{
	std::string str = "<SegRef>";
	str += " Color: " + StdFormatPenColor();
	str += " Style: " + StdFormatPenStyle();
	str += " Segment Name: " + m_strName;
	msgSetPaneText(str);
}
void CPrimSegRef::FormatExtra(CString& str) const
{
	std::stringstream ss;

	ss << "Color;" << StdFormatPenColor() << "\t"
		<< "Style;" << StdFormatPenStyle() << "\t"
		<< "Segment Name;" << m_strName.GetString() << "\t"
		<< "Rotation Angle;" << m_dRotation;

	str = ss.str().c_str();
}

void CPrimSegRef::FormatGeometry(CString& str) const
{
	std::stringstream ss;

	ss << "Insertion Point;" << m_pt.ToStdString() << "\t"
		<< "Normal;" << m_vZ.ToStdString() << "\t"
		<< "Scale;" << m_vScale.ToStdString();

	str = ss.str().c_str();
}

CPnt CPrimSegRef::GetCtrlPt() const
{
	CPnt pt;
	pt = m_pt;
	return (pt);
}

void CPrimSegRef::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) { return; }

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tmIns = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tmIns);

	pBlock->GetExtents(ptMin, ptMax, tm);

	mspace.Return();
}

bool CPrimSegRef::IsInView(CPegView* pView) const
{
	// Test whether an instance of a block is wholly or partially within the current view volume.
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) { return false; }

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	bool bInView = pBlock->IsInView(pView);

	mspace.Return();
	return (bInView);
}
CPnt CPrimSegRef::SelAtCtrlPt(CPegView* pView, const CPnt4& ptPic) const
{
	mS_wCtrlPt = USHRT_MAX;
	CPnt ptCtrl;

	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) { return ptCtrl; }

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	POSITION pos = pBlock->GetHeadPosition();
	while (pos != 0)
	{
		CPrim* pPrim = pBlock->GetNext(pos);
		ptCtrl = pPrim->SelAtCtrlPt(pView, ptPic);
		if (mS_wCtrlPt != USHRT_MAX)
		{
			mspace.Transform(ptCtrl);
			break;
		}
	}
	mspace.Return();
	return ptCtrl;
}
bool CPrimSegRef::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) { return false; }

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	bool bResult = pBlock->SelUsingRect(pView, pt1, pt2);

	mspace.Return();
	return (bResult);
}
///<summary>Evaluates whether a point lies within tolerance specified of block.</summary>
bool CPrimSegRef::SelUsingPoint(CPegView* pView, const CPnt4& pt, double dtol, CPnt& ptProj)
{
	bool bResult = false;

	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) { return (bResult); }

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tm = BuildTransformMatrix(ptBase);

	mspace.InvokeNew();
	mspace.SetLocalTM(tm);

	POSITION posPrim = pBlock->GetHeadPosition();
	while (posPrim != 0)
	{
		if ((pBlock->GetNext(posPrim))->SelUsingPoint(pView, pt, dtol, ptProj))
		{
			bResult = true; break;
		}
	}
	mspace.Return();
	return (bResult);
}

void CPrimSegRef::Read(CFile& fl)
{
	CPrim::Read(fl);

	FilePeg_ReadString(fl, m_strName);
	m_pt.Read(fl);
	m_vZ.Read(fl);
	m_vScale.Read(fl);
	FilePeg_ReadDouble(fl, m_dRotation);
	FilePeg_ReadWord(fl, m_wColCnt);
	FilePeg_ReadWord(fl, m_wRowCnt);
	FilePeg_ReadDouble(fl, m_dColSpac);
	FilePeg_ReadDouble(fl, m_dRowSpac);
}
void CPrimSegRef::Transform(const CTMat& tm)
{
	m_pt = tm * m_pt;
	m_vZ = tm * m_vZ;

	if (fabs(m_vZ[0]) <= FLT_EPSILON && fabs(m_vZ[1]) <= FLT_EPSILON)
	{
		m_vScale = tm * m_vScale;
	}
}

void CPrimSegRef::TranslateUsingMask(const CVec& v, DWORD dwMask)
{
	if (dwMask != 0)
	{
		m_pt += v;
	}
}
bool CPrimSegRef::Write(CFile& fl) const
{
	FilePeg_WriteWord(fl, static_cast<WORD>(CPrim::Type::SegRef));
	FilePeg_WriteWord(fl, m_nPenColor);
	FilePeg_WriteWord(fl, m_nPenStyle);
	FilePeg_WriteString(fl, m_strName);
	m_pt.Write(fl);
	m_vZ.Write(fl);
	m_vScale.Write(fl);
	FilePeg_WriteDouble(fl, m_dRotation);
	FilePeg_WriteWord(fl, m_wColCnt);
	FilePeg_WriteWord(fl, m_wRowCnt);
	FilePeg_WriteDouble(fl, m_dColSpac);
	FilePeg_WriteDouble(fl, m_dRowSpac);

	return true;
}
