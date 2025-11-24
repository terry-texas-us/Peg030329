#include "stdafx.h"

#include "PegAEsysDoc.h"
#include "Messages.h"

CPrimInsert::CPrimInsert()
{
	m_pt = ORIGIN;
	m_vX = XDIR;
	m_vY = YDIR;
	m_vZ = ZDIR;
	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}	
CPrimInsert::CPrimInsert(const CString& strName, const CPnt& pt)
{
	m_strName = strName;
	m_pt = pt;
	m_vX = XDIR;
	m_vY = YDIR;
	m_vZ = ZDIR;
	m_wColCnt = 1;
	m_wRowCnt = 1;
	m_dColSpac = 0.;
	m_dRowSpac = 0.;
}	
CPrimInsert::CPrimInsert(const CPrimInsert& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vX = src.m_vX;
	m_vY = src.m_vY;
	m_vZ = src.m_vZ;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;
}
const CPrimInsert& CPrimInsert::operator=(const CPrimInsert& src)
{
	m_strName = src.m_strName;
	m_pt = src.m_pt;
	m_vX = src.m_vX;
	m_vY = src.m_vY;
	m_vZ = src.m_vZ;
	m_wColCnt = src.m_wColCnt;
	m_wRowCnt = src.m_wRowCnt;
	m_dColSpac = src.m_dColSpac;
	m_dRowSpac = src.m_dRowSpac;

	return (*this);
}

void CPrimInsert::AddToTreeViewControl(HWND hTree, HTREEITEM hParent) const
{
	tvAddItem(hTree, hParent, "<Insert>", (CObject*) this);
}
CTMat CPrimInsert::BuildTransformMatrix(const CPnt& ptBase) const
{
	CTMat tm(m_pt, m_vX, m_vY);
	tm.Translate(ptBase);
	tm.Inverse();
	return tm;
}

CPrim*& CPrimInsert::Copy(CPrim*& pPrim) const
{
	pPrim = new CPrimInsert(*this);
	return (pPrim);
}

void CPrimInsert::Display(CPegView* pView, CDC* pDC) const
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
void CPrimInsert::DisRep(const CPnt&) const 
{
	std::string str = "<Insert>";
	str += " Color: " + StdFormatPenColor();
	str += " Style: " + StdFormatPenStyle();
	msgInformation(str.c_str());
}
void CPrimInsert::FormatExtra(CString& str) const
{
	std::string extra;
	extra = "Color;" + StdFormatPenColor() + "\t";
	extra += "Style;" + StdFormatPenStyle();
	str = extra.c_str();
}
void CPrimInsert::FormatGeometry(CString& str) const
{
	std::string geometry;
	geometry = "Insertion Point;" + m_pt.ToStdString();
	geometry += "X Axis;" + m_vX.ToStdString();
	geometry += "Y Axis;" + m_vY.ToStdString();
	geometry += "Z Axis;" + m_vZ.ToStdString();
	str = geometry.c_str();
}
CPnt CPrimInsert::GetCtrlPt() const 
{
	CPnt pt;
	pt = m_pt;
	return (pt);
}

void CPrimInsert::GetExtents(CPnt& ptMin, CPnt& ptMax, const CTMat& tm) const
{
	CBlock* pBlock;
	
	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return;}

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tmIns = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tmIns);

	pBlock->GetExtents(ptMin, ptMax, tm);

	mspace.Return();
}

bool CPrimInsert::IsInView(CPegView* pView) const
{
	// Test whether an instance of a block is wholly or partially within the current view volume.
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return false;}

	CPnt ptBase = pBlock->GetBasePt();

	CTMat tm = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	bool bInView = pBlock->IsInView(pView);
	
	mspace.Return();
	return (bInView);
}
CPnt CPrimInsert::SelAtCtrlPt(CPegView* pView, const CPnt4& ptPic) const 
{
	mS_wCtrlPt = USHRT_MAX;
	CPnt ptCtrl;

	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return ptCtrl;}

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
bool CPrimInsert::SelUsingRect(CPegView* pView, const CPnt& pt1, const CPnt& pt2)
{
	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return false;}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	bool bResult = pBlock->SelUsingRect(pView, pt1, pt2);
	
	mspace.Return();
	return (bResult);
}

///<summary>Evaluates whether a point lies within tolerance specified of block.</summary>
bool CPrimInsert::SelUsingPoint(CPegView* pView, const CPnt4& pt, double dtol, CPnt& ptProj)
{
	bool bResult = false;

	CBlock* pBlock;

	if (CPegDoc::GetDoc()->BlksLookup(m_strName, pBlock) == 0) {return (bResult);}

	CPnt ptBase = pBlock->GetBasePt();
	
	CTMat tm = BuildTransformMatrix(ptBase);
	
	mspace.InvokeNew();
	mspace.SetLocalTM(tm);
	
	POSITION posPrim = pBlock->GetHeadPosition();
	while (posPrim != 0)
	{
		if ((pBlock->GetNext(posPrim))->SelUsingPoint(pView, pt, dtol, ptProj))
			{bResult = true; break;}
	}
	mspace.Return();
	return (bResult);
}	 

void CPrimInsert::Read(CFile& fl)
{
	CPrim::Read(fl);
	
	FilePeg_ReadString(fl, m_strName);
	m_pt.Read(fl);
	m_vX.Read(fl);
	m_vY.Read(fl);
	m_vZ.Read(fl);
	FilePeg_ReadWord(fl, m_wColCnt);
	FilePeg_ReadWord(fl, m_wRowCnt);
	FilePeg_ReadDouble(fl, m_dColSpac);
	FilePeg_ReadDouble(fl, m_dRowSpac);
}
void CPrimInsert::Transform(const CTMat& tm)
{
	m_pt = tm * m_pt;
	m_vX = tm * m_vX;
	m_vY = tm * m_vY;
	m_vZ = tm * m_vZ;
}

void CPrimInsert::TranslateUsingMask(const CVec& v, DWORD dwMask)
{
	if (dwMask != 0)
	{
		m_pt += v;
	}
}
bool CPrimInsert::Write(CFile& fl) const
{
	FilePeg_WriteWord(fl, PRIM_INSERT);
	FilePeg_WriteWord(fl, m_nPenColor);
	FilePeg_WriteWord(fl, m_nPenStyle);
	FilePeg_WriteString(fl, m_strName); 
	m_pt.Write(fl);
	m_vX.Write(fl);
	m_vY.Write(fl);
	m_vZ.Write(fl);
	FilePeg_WriteWord(fl, m_wColCnt);
	FilePeg_WriteWord(fl, m_wRowCnt);
	FilePeg_WriteDouble(fl, m_dColSpac);
	FilePeg_WriteDouble(fl, m_dRowSpac);

	return true;
}
