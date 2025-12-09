#pragma once

#include <afx.h>  // For CFile as base class and CString
#include <afxstr.h>

class CLayer;

class CFileTracing : public CFile {
 public:
  CFileTracing() {};

  CFileTracing(const CFileTracing&) = delete;
  CFileTracing& operator=(const CFileTracing&) = delete;

  virtual ~CFileTracing() {}

  bool OpenForRead(const CString& strPathName);
  bool OpenForWrite(const CString& strPathName);
  bool ReadSegs(CLayer* pLayer);
  void ReadHeader();
  void WriteSegs(CLayer* pLayer);
  void WriteHeader();
};
