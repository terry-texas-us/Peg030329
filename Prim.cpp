#include "stdafx.h"

#include "FilePeg.h"
#include "PrimState.h"

PENCOLOR	CPrim::mS_nLayerPenColor = 1;
PENSTYLE	CPrim::mS_nLayerPenStyle = 1;
PENSTYLE	CPrim::mS_nSpecPenStyle = 0;
PENCOLOR	CPrim::mS_nSpecPenColor = 0;
short		CPrim::mS_nSpecPolygonStyle = - 1;

WORD		CPrim::mS_wCtrlPt = USHRT_MAX;
double		CPrim::mS_dRel = 0.;
double		CPrim::mS_dPicApertSiz = .02;

std::string CPrim::StdFormatPenColor() const
{
	std::string str;	
	if (m_nPenColor == PENCOLOR_BYLAYER)
	{
		str = "ByLayer";
	}
	else if (m_nPenColor == PENCOLOR_BYBLOCK)
		str = "ByBlock";
	else 
	{
		str = std::to_string(m_nPenColor);
	}
	return str;
}

std::string CPrim::StdFormatPenStyle() const
{
	std::string str;
	if (m_nPenStyle == PENSTYLE_BYLAYER)
	{
		str = "ByLayer";
	}

	else if (m_nPenStyle == PENSTYLE_BYBLOCK)
	{
		str = "ByBlock";
	}
	else
	{
		str = std::to_string(m_nPenStyle);
	}
	return str;
}

PENCOLOR CPrim::LogicalPenColor() const
{
	PENCOLOR nPenColor = mS_nSpecPenColor == 0 ? m_nPenColor : mS_nSpecPenColor;

	if (nPenColor == PENCOLOR_BYLAYER) 
		nPenColor = mS_nLayerPenColor;
	else if (nPenColor == PENCOLOR_BYBLOCK)
		nPenColor = 7;

	return (nPenColor);
}
PENSTYLE CPrim::LogicalPenStyle() const
{
	PENSTYLE nPenStyle = mS_nSpecPenStyle == 0 ? m_nPenStyle : mS_nSpecPenStyle;
	
	if (nPenStyle == PENSTYLE_BYLAYER) 
		nPenStyle = mS_nLayerPenStyle;
	else if (nPenStyle == PENSTYLE_BYBLOCK) 
		nPenStyle = 1;
	
	return (nPenStyle);
}
void CPrim::ModifyState() 
{
	m_nPenColor = pstate.PenColor();
	m_nPenStyle = pstate.PenStyle();
}
void CPrim::Read(CFile& fl)
{
	FilePeg_ReadShort(fl, m_nPenColor);
	FilePeg_ReadShort(fl, m_nPenStyle);
}
CVec Prim_ComputeArbitraryAxis(const CVec& vNorm)
{
	// Compute a not so arbitrary axis for AutoCAD entities
	
	const double e = 1. / 64;
	
	if ((fabs(vNorm[0]) < e) && (fabs(vNorm[1]) < e))
		return (YDIR ^ vNorm);
	else
		return (ZDIR ^ vNorm);
}