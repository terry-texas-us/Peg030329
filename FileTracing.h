#pragma once

class CLayer;

class CFileTracing : public CFile
{
public:
	CFileTracing() {};
	
	virtual ~CFileTracing() {}

	bool	OpenForRead(const CString& strPathName);
	bool	OpenForWrite(const CString& strPathName);
	bool	ReadSegs(CLayer* pLayer);
	void	ReadHeader();
	void	WriteSegs(CLayer* pLayer);
	void	WriteHeader();
};
