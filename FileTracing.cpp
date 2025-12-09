#include "stdafx.h"

#include "FilePeg.h"
#include "FileTracing.h"
#include "Layer.h"
#include "Messages.h"
#include "Seg.h"

// SECTION_HEADER sentinel						WORD 0x0101
//		{0 or more key-value pairs}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_SEGMENTS sentinel					WORD 0x0104
//		Number of segments						WORD
//		{0 or more segment definitions}
// SECTION_END sentinel							WORD 0x01ff

bool CFileTracing::OpenForRead(const CString& strPathName) {
  CFileException e;
  CFile::Open(strPathName, CFile::modeRead | CFile::shareDenyNone, &e);

  if (e.m_cause == CFileException::sharingViolation) {
    msgWarning(IDS_MSG_FILE_LOCKED, strPathName);
    return false;
  }
  return true;
}
bool CFileTracing::OpenForWrite(const CString& strPathName) {
  if (CFile::Open(strPathName, CFile::modeWrite | CFile::modeCreate) == 0) {
    msgWarning(IDS_MSG_TRACING_WRITE_FAILURE, strPathName);
    return false;
  }
  return true;
}
void CFileTracing::ReadHeader() {
  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_HEADER)
    throw "Exception CFileTracing: Expecting sentinel SECTION_HEADER.";

  // 	with addition of info here will loop key-value pairs till SECTION_END sentinel

  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_END) throw "Exception CFileTracing: Expecting sentinel SECTION_END.";
}
bool CFileTracing::ReadSegs(CLayer* pLayer) {
  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_SEGMENTS)
    throw "Exception CFileTracing: Expecting sentinel SECTION_SEGMENTS.";

  CPrim* pPrim;

  WORD wSegCount = FilePeg_ReadWord(*this);

  for (WORD wSeg = 0; wSeg < wSegCount; wSeg++) {
    int iPrims = FilePeg_ReadWord(*this);

    CSeg* pSeg = new CSeg;

    for (int i = 0; i < iPrims; i++) {
      FilePeg_ReadPrim(*this, pPrim);
      pSeg->AddTail(pPrim);
    }
    pLayer->AddTail(pSeg);
  }
  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_END) throw "Exception CFileTracing: Expecting sentinel SECTION_END.";

  return true;
}
void CFileTracing::WriteHeader() {
  FilePeg_WriteWord(*this, CFilePeg::SECTION_HEADER);

  FilePeg_WriteWord(*this, CFilePeg::SECTION_END);
}
void CFileTracing::WriteSegs(CLayer* pLayer) {
  FilePeg_WriteWord(*this, CFilePeg::SECTION_SEGMENTS);

  FilePeg_WriteWord(*this, static_cast<WORD>(pLayer->GetCount()));

  POSITION pos = pLayer->GetHeadPosition();
  while (pos != 0) {
    CSeg* pSeg = pLayer->GetNext(pos);
    pSeg->Write(*this);
  }
  FilePeg_WriteWord(*this, CFilePeg::SECTION_END);
  msgInformation(IDS_MSG_TRACING_SAVE_SUCCESS, pLayer->GetName());
}
