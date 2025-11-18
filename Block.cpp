#include "stdafx.h"

CBlock::CBlock(WORD wBlkTypFlgs, const CPnt& ptBase)
{
	m_wBlkTypFlgs = wBlkTypFlgs;
	m_ptBase = ptBase;
}
CBlock::CBlock(WORD wBlkTypFlgs, const CPnt& ptBase, const CString& strXRefPathName)
{
	m_wBlkTypFlgs = wBlkTypFlgs;
	m_ptBase = ptBase;
	m_strXRefPathName = strXRefPathName;
}
