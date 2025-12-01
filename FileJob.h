#pragma once

// Detailed documentation for .jb1 (job files) at "Job File Format.md"

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

	bool	OpenForRead(const CString& pathName);
	bool	OpenForWrite(const CString& pathName);
	void	ReadHeader();
	void	ReadSegs(CLayer* layer);
	int		ReleaseVersion();
	void	WriteHeader();
	void	WriteSeg(CSeg* segment);
	void	WriteSegs(CLayer* layer);
};

bool filejob_GetNextPrim(CFile& file, int version, char* buffer, CPrim*& primitive);
bool filejob_GetNextSeg(CFile& file, int iVersion, char*, CSeg*& segment);
bool filejob_IsValidPrimitive(const short type);