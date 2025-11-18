#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

// SECTION_HEADER sentinel						WORD 0x0101
//		{0 or more key-value pairs}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_TABLES sentinel						WORD 0x0102
//		TABLE_VPORT sentinel					WORD 0x0201
//			{0 or more viewport definitions}	WORD
//		TABLE_END sentinel						WORD 0x02ff
//		TABLE_LTYPE sentinel					WORD 0x0202
//			Number of linetypes					WORD
//			{0 or more linetype definitions}
//		TABLE_END sentinel						WORD 0x02ff
//		TABLE_LAYER sentinel					WORD 0x0203
//			Number of layers (resident only)	WORD
//			{0 or more layer definitions}
//		TABLE_END sentinel						WORD 0x02ff
// SECTION_END sentinel							WORD 0x01ff
// SECTION_BLOCKS sentinel						WORD 0x0103
//		Number of blocks						WORD
//		{0 or more block definitions}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_SEGMENTS sentinel					WORD 0x0104
//		Number of segments						WORD
//		{0 or more segment definitions}
// SECTION_END sentinel							WORD 0x01ff

// Layer definition
//		Layers
//			Name								string
//			Tracing flags						WORD
//			State								WORD
//			Layer pen color						WORD
//			Layer pen style						string
//		Tracings
//			?? nothing

// Line style definition
//		Name									string
//		Flags (always 0)						WORD
//		Description								string
//		Definition length						WORD		
//		Pattern length							double
//		if (definition length > 0)
//			1 or more dash length				double...

// Block definition
//		Number of primitives					WORD
//		Name									string
//		Flags									WORD
//		Base point								Pnt
//		{0 or more primitive definitions}

// Segment definition
//		Number of primitives					WORD
//		{0 or more primitive definitions}

// Primitive definition
//		Mark primitive
//			PRIM_MARK type code					WORD 0x0100
//			Pen color							WORD
//			Mark style							WORD
//			Point								Pnt
//			Number of data values				WORD
//			{0 or more data values}
//		Insert primitive
//			PRIM_INSERT type code				WORD 0x0101
//			Pen color							WORD
//			Pen style							WORD
//			Insertion point						Pnt
//			Local reference x-axis				Vec
//			Local reference y-axis				Vec
//			Local reference z-axis				Vec
//			Number of columns					WORD
//			Number of rows						WORD
//			Column spacing						double
//			Row spacing							double
//		SegRef primitive
//			PRIM_SEGR type code					WORD 0x0102
//			Pen color							WORD
//			Pen style							WORD
//			Segment name						string
//			Insertion point						Pnt
//			Local normal vector					Vec
//			Scale factors(x, y, z)				Vec
//			Rotatation							double
//			Number of columns					WORD
//			Number of rows						WORD
//			Column spacing						double
//			Row spacing							double
//		Line primitive
//			PRIM_LINE type code					WORD 0x0200
//			Pen color							WORD
//			Pen style							WORD
//			Line								Ln
//		Polygon primitive
//			PRIM_POLYGON type code				WORD 0x0400
//			Pen color							WORD
//			Polygon style						WORD
//			Polygon Style Index					WORD
//			Number of verticies					WORD
//			Hatch origin						Pnt
//			Hatch/pattern reference x-axis		Vec
//			Hatch/pattern reference y-axis		Vec
//			{0 or more points}	
//		Arc primitive
//			PRIM_ARC type code					WORD 0x1003
//			Pen color							WORD
//			Pen style							WORD
//			Center point						Pnt
//			Major axis							Vec
//			Minor axis							Vec
//			Sweep angle							double
//		BSpline primitive
//			PRIM_BSPLINE type code				WORD 0x2000
//			Pen color							WORD
//			Pen style							WORD
//			Number of control points			WORD
//			{0 or more control points}
//		CSpline primitive
//			PRIM_CSPLINE type code				WORD 0x2001
//			Pen color							WORD
//			Pen style							WORD
//			m_wPtsS								WORD
//			Number of control points			WORD
//			End condition Type					WORD
//			Begin point tangent vector			Vec
//			End point tangent vector			Vec
//			{0 or more control points}
//		Polyline primitive (not ready for io)
//			PRIM_POLYLINE type code				WORD 0x2002
//			Pen color							WORD
//			Pen style							WORD
//			Number of points					WORD
//			{0 or more points}			
//		Text primitive
//			PRIM_TEXT type code					WORD 0x4000
//			Pen color							WORD
//			Pen style							WORD
//			Text precision						WORD
//			Text font name						string
//			Text path							WORD
//			Horizontal alignment				WORD
//			Vertical alignment					WORD
//			Character spacing					double
//			Insertion point						Pnt
//			Local reference x-axis				Vec
//			Local reference y-axis				Vec
//			Text								string
//		Tag primitive
//			PRIM_TAG type code					WORD 0x4100
//			Pen color							WORD
//			Pen style							WORD
//			Point								Pnt
//		Dim primitive 
//			(could be a problem. primdim not overloaded. 
//			 only its base primtext data has been streamed.
//			 has this always been so on tra/peg files.)

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
			if (strName.Find('.') == - 1)
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

	if(FilePeg_ReadWord(*this) != TABLE_END)
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
			FilePeg_WriteWord(*this, (WORD) pLayer->GetCount());
		
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

	switch (wEntityTyp)
	{
		case CPrim::PRIM_MARK:
			pPrim = new CPrimMark;
			break;
		case CPrim::PRIM_INSERT:
			pPrim = new CPrimInsert;
			break;
		case CPrim::PRIM_SEGREF:
			pPrim = new CPrimSegRef;
			break;
		case CPrim::PRIM_LINE:
			pPrim = new CPrimLine;
			break;
		case CPrim::PRIM_POLYGON:
			pPrim = new CPrimPolygon;
			break;
		case CPrim::PRIM_ARC:
			pPrim = new CPrimArc;
			break;
		case CPrim::PRIM_BSPLINE:
			pPrim = new CPrimBSpline;
			break;
		case CPrim::PRIM_CSPLINE:
			pPrim = new CPrimCSpline;
			break;
		case CPrim::PRIM_POLYLINE:
			pPrim = new CPrimPolyline;
			break;
		case CPrim::PRIM_TEXT:
			pPrim = new CPrimText;
			break;
		case CPrim::PRIM_TAG:
			pPrim = new CPrimTag;
			break;
		case CPrim::PRIM_DIM:
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
	fl.Write(str, (UINT) str.GetLength());
	fl.Write("\t", 1);
}
