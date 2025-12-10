#include "stdafx.h"

#include "Block.h"
#include "FileBlock.h"
#include "FilePeg.h"

// SECTION_HEADER sentinel						WORD 0x0101
//		{0 or more key-value pairs}
// SECTION_END sentinel							WORD 0x01ff
// SECTION_BLOCKS sentinal						WORD 0x0103
//		Number of blocks						WORD
//		{0 or more block definitions}
// SECTION_END sentinel							WORD 0x01ff

CFileBlock::CFileBlock(const CString& strPathName) { CFile::Open(strPathName, modeReadWrite | shareDenyNone); }
void CFileBlock::ReadBlocks(CBlocks& blks) {
  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_BLOCKS) {
    throw _T("Exception CFileBlock: Expecting sentinel SECTION_BLOCKS.");
}

  CString strName;

  CPrim* pPrim;

  WORD wTblSize = FilePeg_ReadWord(*this);
  for (int i = 0; i < wTblSize; i++) {
    int iPrims = FilePeg_ReadWord(*this);

    FilePeg_ReadString(*this, strName);
    WORD wBlkTypFlgs = FilePeg_ReadWord(*this);

    auto* pBlock = new CBlock(wBlkTypFlgs, ORIGIN);

    for (int j = 0; j < iPrims; j++) {
      FilePeg_ReadPrim(*this, pPrim);
      pBlock->AddTail(pPrim);
    }
    blks[strName] = pBlock;
  }
  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_END) {
    throw _T("Exception CFileBlock: Expecting sentinel SECTION_END.");
}
}
void CFileBlock::ReadFile(const CString& strPathName, CBlocks& blks) {
  CFileException e;

  if (CFile::Open(strPathName, modeRead | shareDenyNone, &e)) {
    ReadHeader();
    ReadBlocks(blks);
  }
}
void CFileBlock::ReadHeader() {
  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_HEADER) {
    throw _T("Exception CFileBlock: Expecting sentinel SECTION_HEADER.");
}

  // 	with addition of info where will loop key-value pairs till SECTION_END sentinel

  if (FilePeg_ReadWord(*this) != CFilePeg::SECTION_END) {
    throw _T("Exception CFileBlock: Expecting sentinel SECTION_END.");
}
}
void CFileBlock::WriteBlock(const CString& strName, CBlock* pBlock) {
  WORD wPrims = 0;

  ULONGLONG dwCountPosition = GetPosition();

  FilePeg_WriteWord(*this, wPrims);
  FilePeg_WriteString(*this, strName);
  FilePeg_WriteWord(*this, pBlock->GetBlkTypFlgs());

  POSITION pos = pBlock->GetHeadPosition();
  while (pos != nullptr) {
    CPrim* pPrim = pBlock->GetNext(pos);
    if (pPrim->Write(*this)) wPrims++;
  }
  ULONGLONG dwPosition = GetPosition();
  Seek(static_cast<LONGLONG>(dwCountPosition), begin);
  FilePeg_WriteWord(*this, wPrims);
  Seek(static_cast<LONGLONG>(dwPosition), begin);
}
void CFileBlock::WriteBlocks(CBlocks& blks) {
  FilePeg_WriteWord(*this, CFilePeg::SECTION_BLOCKS);
  FilePeg_WriteWord(*this, (WORD)blks.GetSize());

  CString strKey;
  CBlock* pBlock;

  POSITION pos = blks.GetStartPosition();
  while (pos != nullptr) {
    blks.GetNextAssoc(pos, strKey, pBlock);
    WriteBlock(strKey, pBlock);
  }
  FilePeg_WriteWord(*this, CFilePeg::SECTION_END);
}
void CFileBlock::WriteFile(const CString& strPathName, CBlocks& blks) {
  CFileException e;

  CFile::Open(strPathName, modeCreate | modeWrite, &e);

  WriteHeader();
  WriteBlocks(blks);
}
void CFileBlock::WriteHeader() {
  FilePeg_WriteWord(*this, CFilePeg::SECTION_HEADER);

  FilePeg_WriteWord(*this, CFilePeg::SECTION_END);
}
