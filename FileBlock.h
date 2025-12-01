#pragma once

#include <afx.h>
#include <afxstr.h>

#include "Block.h"

class CFileBlock: public CFile
{
public:
    CFileBlock() { }
    CFileBlock(const CString& strPathName);

    CFileBlock(const CFileBlock&) = delete;
    CFileBlock& operator=(const CFileBlock&) = delete;

    virtual ~CFileBlock() { }

    void	ReadFile(const CString&, CBlocks& blks);
    void	ReadBlocks(CBlocks& blks);
    void	ReadHeader();
    void	WriteBlock(const CString& strName, CBlock* pBlock);
    void	WriteBlocks(CBlocks& blks);
    void	WriteFile(const CString& strPathName, CBlocks& blks);
    void	WriteHeader();
};
