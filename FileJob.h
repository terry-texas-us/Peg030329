#pragma once

#include "Job File Format.md"

#include <afx.h>
#include <afxstr.h>

#include "Prim.h"

class CLayer;
class CSeg;

class CFileJob: public CFile
{
private:
	int   m_iVersion{0};
	char* m_PrimBuf{nullptr};

public:
	CFileJob() { m_PrimBuf = new char[CPrim::BUFFER_SIZE]; }

	virtual ~CFileJob() { delete [] m_PrimBuf; }

	CFileJob(const CFileJob&) = delete;
	CFileJob& operator=(const CFileJob&) = delete;

	bool	OpenForRead(const CString& strPathName);
	bool	OpenForWrite(const CString& strPathName);
	void	ReadHeader();
	void	ReadSegs(CLayer* pLayer);
	int		ReleaseVersion();
	void	WriteHeader();
	void	WriteSeg(CSeg* pSeg);
	void	WriteSegs(CLayer* pLayer);
};

bool filejob_GetNextPrim(CFile& fl, int iVersion, char* pBuf, CPrim*& pPrim);
bool filejob_GetNextSeg(CFile& fl, int iVersion, char*, CSeg*& pSeg);
bool filejob_IsValidPrimitive(const short nType);