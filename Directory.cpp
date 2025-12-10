#include "stdafx.h"

#include <afxstr.h>

#include <corecrt.h>
#include <io.h>

#include <cstdlib>

void Path_UnquoteSpaces(CString& strPathName) {
  if (strPathName.IsEmpty()) return;

  TCHAR* pPathName = new TCHAR[MAX_PATH];
  _tcscpy_s(pPathName, MAX_PATH, strPathName);
  size_t n = _tcslen(pPathName) - 1;
  while (n != 0 && pPathName[n] != '\\') { n--; }
  pPathName[n] = 0;
  strPathName = &pPathName[1];
  delete[] pPathName;
}

void Directory_ExamineFile(TCHAR* oldfile, TCHAR* newfile) {
  // the default file examine -- if the file isn't pathed to the right place,
  // then search the ACAD environment variable for the oldfile */

  const int kExistenceOnly = 0;

  TCHAR holdch;
  TCHAR testpath[256]{};

  static TCHAR pathchar = '\\';
  static TCHAR oldpathchar = '/';

  // replace path characters, if present, with proper ones for platform
  for (int i = 0; i < int(_tcslen(oldfile)); i++) {
    if (oldfile[i] == oldpathchar) oldfile[i] = pathchar;
  }
  if (_tcschr(oldfile, pathchar) != nullptr) {  // file has some degree of path designation
    if (_access(oldfile, kExistenceOnly) == 0) {
      _tcscpy_s(newfile, sizeof(newfile), oldfile);
      return;
    }
    // strip the path
    TCHAR* pLast = _tcsrchr(oldfile, pathchar);
    _tcscpy_s(oldfile, sizeof(oldfile), ++pLast);
  }
  _tcscpy_s(newfile, sizeof(newfile), oldfile);

  if (_access(oldfile, kExistenceOnly) == 0) {  // its in current diretory
    return;
  } else {
    if (_tcschr(oldfile, ':') != nullptr) {  // path with drive id
      return;
    }
    TCHAR* envptr = nullptr;
    size_t len;
    errno_t err = _dupenv_s(&envptr, &len, _T("ACAD"));

    if (err != 0 || envptr == nullptr) {  // no ACAD environment to search
      return;
    }
    TCHAR* cptr = envptr;
    do {
      while (*cptr != ';' && *cptr != 0) { cptr++; }
      holdch = *cptr; /* grab terminating character */
      *cptr = 0;
      _tcscpy_s(testpath, sizeof(testpath), envptr);
      *cptr = holdch; /* put it back */
      int iTest = (int)_tcslen(testpath);
      if (testpath[iTest - 1] != pathchar) { /* append / or \ */
        testpath[iTest] = pathchar;
        testpath[iTest + 1] = 0;
      }
      _tcscat_s(testpath, sizeof(testpath), oldfile);

      if (!_access(testpath, 0)) { /* true if found */
        _tcscpy_s(newfile, sizeof(newfile), testpath);
        return;
      }
      cptr++;
      envptr = cptr;
    } while (holdch == ';'); /* terminator is 0 for end of env string */
  }
}
