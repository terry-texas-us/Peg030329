#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

#include "FilePeg.h"
#include "Layer.h"
#include "Messages.h"
#include "PenStyle.h"
#include "PrimArc.h"
#include "PrimBSpline.h"
#include "PrimCSpline.h"
#include "PrimDim.h"
#include "PrimInsert.h"
#include "PrimLine.h"
#include "PrimMark.h"
#include "PrimPolygon.h"
#include "PrimPolyline.h"
#include "PrimSegRef.h"
#include "PrimTag.h"
#include "PrimText.h"

void CFilePeg::Load(CPegDoc* pDoc)
{
	ReadHeaderSection(pDoc);
	ReadTablesSection(pDoc);
	ReadBlocksSection(pDoc);
	ReadEntitiesSection(pDoc);
}
void CFilePeg::ReadBlocksSection(CPegDoc* pDoc)
{
	if (FilePeg_ReadWord(*this) != SECTION_BLOCKS)
		throw "Exception CFilePeg: Expecting sentinel SECTION_BLOCKS.";

	CPrim* pPrim;
	CString strName;
	char szXRefPathNam[32];

	WORD wTblSize = FilePeg_ReadWord(*this);

	for (WORD w = 0; w < wTblSize; w++)
	{
		WORD wPrimCount = FilePeg_ReadWord(*this);

		FilePeg_ReadString(*this, strName);
		WORD wBlkTypFlgs = FilePeg_ReadWord(*this);
		CPnt pt;
		pt.Read(*this);

		CBlock* pBlock = new CBlock(wBlkTypFlgs, pt, szXRefPathNam);
		pDoc->BlksSetAt(strName, pBlock);

		for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
		{
			FilePeg_ReadPrim(*this, pPrim);
			pBlock->AddTail(pPrim);
		}
	}
	if (FilePeg_ReadWord(*this) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}
void CFilePeg::ReadEntitiesSection(CPegDoc* pDoc)
{
	if (FilePeg_ReadWord(*this) != SECTION_SEGMENTS)
		throw "Exception CFilePeg: Expecting sentinel SECTION_SEGMENTS.";

	CPrim* pPrim;

	WORD wTblSize = FilePeg_ReadWord(*this);

	for (WORD w = 0; w < wTblSize; w++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(w);

		if (pLayer == 0)
			return;

		WORD wSegCount = FilePeg_ReadWord(*this);

		if (pLayer->IsInternal())
		{
			for (WORD wSeg = 0; wSeg < wSegCount; wSeg++)
			{
				CSeg* pSeg = new CSeg;
				WORD wPrimCount = FilePeg_ReadWord(*this);
				for (WORD wPrim = 0; wPrim < wPrimCount; wPrim++)
				{
					FilePeg_ReadPrim(*this, pPrim);
					pSeg->AddTail(pPrim);
				}
				pLayer->AddTail(pSeg);
			}
		}
		else
			pDoc->TracingLoadLayer(pLayer->GetName(), pLayer);
	}
	if (FilePeg_ReadWord(*this) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}
void CFilePeg::ReadHeaderSection(CPegDoc*)
{
	if (FilePeg_ReadWord(*this) != SECTION_HEADER)
		throw "Exception CFilePeg: Expecting sentinel SECTION_HEADER.";

	// 	with addition of info here will loop key-value pairs till SECTION_END sentinel

	if (FilePeg_ReadWord(*this) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}
void CFilePeg::ReadLayerTable(CPegDoc* pDoc)
{
	if (FilePeg_ReadWord(*this) != TABLE_LAYER)
		throw "Exception CFilePeg: Expecting sentinel TABLE_LAYER.";

	CString strName;
	CString strPenStyleName;

	WORD wTblSize = FilePeg_ReadWord(*this);

	for (WORD w = 0; w < wTblSize; w++)
	{
		FilePeg_ReadString(*this, strName);
		WORD wTracingFlgs = FilePeg_ReadWord(*this);
		WORD wStateFlgs = FilePeg_ReadWord(*this);

		wStateFlgs |= CLayer::STATE_FLG_RESIDENT;

		if ((wStateFlgs & CLayer::STATE_FLG_INTERNAL) != CLayer::STATE_FLG_INTERNAL)
		{
			// HACK is this correct
			if (strName.Find('.') == -1)
				strName += ".jb1";
		}
		PENCOLOR nPenColor = PENCOLOR(FilePeg_ReadWord(*this));
		FilePeg_ReadString(*this, strPenStyleName);

		if (pDoc->LayersLookup(strName) < 0)
		{
			CLayer* pLayer = new CLayer(strName, wStateFlgs);

			pLayer->SetTracingFlg(wTracingFlgs);
			pLayer->SetPenColor(nPenColor);
			pLayer->SetPenStyleName(strPenStyleName);
			pDoc->LayersAdd(pLayer);

			// UNDONE: access to undefined m_pLayerWork
			if (pLayer->IsHot())
				pDoc->WorkLayerSet(pLayer);
		}
	}
	if (FilePeg_ReadWord(*this) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}
void CFilePeg::ReadLnTypsTable(CPegDoc* pDoc)
{
	if (FilePeg_ReadWord(*this) != TABLE_LTYPE)
		throw "Exception CFilePeg: Expecting sentinel TABLE_LTYPE.";

	CString strName;
	CString strDescr;
	WORD wDefLen;
	WORD iStdFlgs;
	double* dDash = new double[32];

	double dPatternLen;

	WORD wTblSize = FilePeg_ReadWord(*this);

	for (WORD w = 0; w < wTblSize; w++)
	{
		FilePeg_ReadString(*this, strName);
		// standard flag values 	??
		iStdFlgs = FilePeg_ReadWord(*this);
		FilePeg_ReadString(*this, strDescr);

		wDefLen = FilePeg_ReadWord(*this);
		FilePeg_ReadDouble(*this, dPatternLen);

		for (WORD wDash = 0; wDash < wDefLen; wDash++)
			FilePeg_ReadDouble(*this, dDash[wDash]);

		if (pDoc->PenStylesLookup(strName) == PENSTYLE_LookupErr)
			pDoc->PenStylesAdd(new CPenStyle(strName, strDescr, wDefLen, dDash));
	}
	delete [] dDash;

	if (FilePeg_ReadWord(*this) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}
void CFilePeg::ReadTablesSection(CPegDoc* pDoc)
{
	if (FilePeg_ReadWord(*this) != SECTION_TABLES)
		throw "Exception CFilePeg: Expecting sentinel SECTION_TABLES.";

	ReadVPortTable(pDoc);
	ReadLnTypsTable(pDoc);
	ReadLayerTable(pDoc);

	if (FilePeg_ReadWord(*this) != SECTION_END)
		throw "Exception CFilePeg: Expecting sentinel SECTION_END.";
}
void CFilePeg::ReadVPortTable(CPegDoc*)
{
	if (FilePeg_ReadWord(*this) != TABLE_VPORT)
		throw "Exception CFilePeg: Expecting sentinel TABLE_VPORT.";

	FilePeg_ReadWord(*this);

	if (FilePeg_ReadWord(*this) != TABLE_END)
		throw "Exception CFilePeg: Expecting sentinel TABLE_END.";
}
void CFilePeg::Unload(CPegDoc* pDoc)
{
	CFile::SetLength(0);
	CFile::SeekToBegin();

	WriteHeaderSection(pDoc);
	WriteTablesSection(pDoc);
	WriteBlocksSection(pDoc);
	WriteEntitiesSection(pDoc);
	FilePeg_WriteString(*this, "EOF");

	CFile::Flush();
}
void CFilePeg::WriteBlocksSection(CPegDoc* pDoc)
{
	FilePeg_WriteWord(*this, SECTION_BLOCKS);

	WORD wTblSize = pDoc->BlksSize();
	FilePeg_WriteWord(*this, wTblSize);

	CString strKey;
	CBlock* pBlock;

	POSITION pos = pDoc->BlksGetStartPosition();
	while (pos != 0)
	{
		pDoc->BlksGetNextAssoc(pos, strKey, pBlock);

		ULONGLONG dwCountPosition = CFile::GetPosition();
		FilePeg_WriteWord(*this, 0);
		WORD iPrimCount = 0;

		FilePeg_WriteString(*this, strKey);
		FilePeg_WriteWord(*this, pBlock->GetBlkTypFlgs());
		pBlock->GetBasePt().Write(*this);

#pragma tasMSG(TODO if block is an xref add a pathname)

		POSITION posPrim = pBlock->GetHeadPosition();
		while (posPrim != 0)
		{
			CPrim* pPrim = pBlock->GetNext(posPrim);
			if (pPrim->Write(*this))
				iPrimCount++;
		}
		ULONGLONG dwPosition = CFile::GetPosition();
		CFile::Seek(dwCountPosition, CFile::begin);
		FilePeg_WriteWord(*this, iPrimCount);
		CFile::Seek(dwPosition, CFile::begin);
	}

	FilePeg_WriteWord(*this, SECTION_END);
}
void CFilePeg::WriteEntitiesSection(CPegDoc* pDoc)
{
	FilePeg_WriteWord(*this, SECTION_SEGMENTS);

	int iTblSize = pDoc->LayersGetSize();
	FilePeg_WriteWord(*this, WORD(iTblSize));

	for (int i = 0; i < iTblSize; i++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(i);
		if (pLayer->IsInternal())
		{
			FilePeg_WriteWord(*this, (WORD)pLayer->GetCount());

			POSITION pos = pLayer->GetHeadPosition();
			while (pos != 0)
			{
				CSeg* pSeg = pLayer->GetNext(pos);
				pSeg->Write(*this);
			}
		}
		else
			FilePeg_WriteWord(*this, 0);
	}
	FilePeg_WriteWord(*this, SECTION_END);
}
void CFilePeg::WriteHeaderSection(CPegDoc*)
{
	FilePeg_WriteWord(*this, SECTION_HEADER);

	// header variable items go here

	FilePeg_WriteWord(*this, SECTION_END);
}
void CFilePeg::WriteLayerTable(CPegDoc* pDoc)
{
	int iTblSize = pDoc->LayersGetSize();

	FilePeg_WriteWord(*this, TABLE_LAYER);

	ULONGLONG dwTblSizePosition = CFile::GetPosition();
	FilePeg_WriteWord(*this, WORD(iTblSize));

	for (int i = 0; i < pDoc->LayersGetSize(); i++)
	{
		CLayer* pLayer = pDoc->LayersGetAt(i);
		if (pLayer->IsResident())
		{
			FilePeg_WriteString(*this, pLayer->GetName());
			FilePeg_WriteWord(*this, pLayer->GetTracingFlgs());
			FilePeg_WriteWord(*this, pLayer->GetStateFlgs());
			FilePeg_WriteWord(*this, pLayer->PenColor());
			FilePeg_WriteString(*this, pLayer->GetPenStyleName());
		}
		else
			iTblSize--;
	}
	FilePeg_WriteWord(*this, TABLE_END);

	if (iTblSize != pDoc->LayersGetSize())
	{
		ULONGLONG dwPosition = CFile::GetPosition();
		CFile::Seek(dwTblSizePosition, CFile::begin);
		FilePeg_WriteWord(*this, WORD(iTblSize));
		CFile::Seek(dwPosition, CFile::begin);
	}
}
void CFilePeg::WritePenStyleTable(CPegDoc* pDoc)
{
	WORD wTblSize = WORD(pDoc->PenStylesGetSize());

	FilePeg_WriteWord(*this, TABLE_LTYPE);
	FilePeg_WriteWord(*this, wTblSize);

	for (int i = 0; i < wTblSize; i++)
	{
		CPenStyle* pPenStyle = pDoc->PenStylesGetAt(i);

		FilePeg_WriteString(*this, pPenStyle->GetName());
		// standard flag values 	??
		FilePeg_WriteWord(*this, 0);
		FilePeg_WriteString(*this, pPenStyle->GetDescription());

		WORD wDefLen = pPenStyle->GetDefLen();
		FilePeg_WriteWord(*this, wDefLen);
		double dPatternLen = pPenStyle->GetPatternLen();
		FilePeg_WriteDouble(*this, dPatternLen);

		if (wDefLen > 0)
		{
			double* dDash = new double[wDefLen];
			pPenStyle->GetDashLen(dDash);
			for (WORD w = 0; w < wDefLen; w++)
				FilePeg_WriteDouble(*this, dDash[w]);

			delete [] dDash;
		}
	}
	FilePeg_WriteWord(*this, TABLE_END);
}
void CFilePeg::WriteTablesSection(CPegDoc* pDoc)
{
	FilePeg_WriteWord(*this, SECTION_TABLES);

	WriteVPortTable(pDoc);
	WritePenStyleTable(pDoc);
	WriteLayerTable(pDoc);
	FilePeg_WriteWord(*this, SECTION_END);
}
void CFilePeg::WriteVPortTable(CPegDoc*)
{
	FilePeg_WriteWord(*this, TABLE_VPORT);
	FilePeg_WriteWord(*this, 0);
	FilePeg_WriteWord(*this, TABLE_END);
}


bool FilePeg_ReadPrim(CFile& fl, CPrim*& pPrim)
{
	WORD wEntityTyp = FilePeg_ReadWord(fl);

	switch (static_cast<CPrim::Type>(wEntityTyp))
	{
	case CPrim::Type::Mark:
		pPrim = new CPrimMark;
		break;
	case CPrim::Type::Insert:
		pPrim = new CPrimInsert;
		break;
	case CPrim::Type::SegRef:
		pPrim = new CPrimSegRef;
		break;
	case CPrim::Type::Line:
		pPrim = new CPrimLine;
		break;
	case CPrim::Type::Polygon:
		pPrim = new CPrimPolygon;
		break;
	case CPrim::Type::Arc:
		pPrim = new CPrimArc;
		break;
	case CPrim::Type::BSpline:
		pPrim = new CPrimBSpline;
		break;
	case CPrim::Type::CSpline:
		pPrim = new CPrimCSpline;
		break;
	case CPrim::Type::Polyline:
		pPrim = new CPrimPolyline;
		break;
	case CPrim::Type::Text:
		pPrim = new CPrimText;
		break;
	case CPrim::Type::Tag:
		pPrim = new CPrimTag;
		break;
	case CPrim::Type::Dim:
		pPrim = new CPrimDim;
		break;
	default:
		msgWarning(IDS_MSG_BAD_PRIM_TYPE);
		return false;
	}
	pPrim->Read(fl);
	return true;
}
void FilePeg_ReadString(CFile& fl, CString& str)
{
	str.Empty();
	char c;
	while (fl.Read(&c, 1) == 1)
	{
		if (c == '\t') return;
		str += c;
	}
}
void FilePeg_WriteString(CFile& fl, const CString& str)
{
	fl.Write(str, (UINT)str.GetLength());
	fl.Write("\t", 1);
}
