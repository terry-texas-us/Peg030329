#include "stdafx.h"
#include <io.h>

void Path_UnquoteSpaces(CString& strPathName)
{
	if (strPathName.IsEmpty())
		return;

	char* pPathName = new char[MAX_PATH];
	strcpy_s(pPathName, MAX_PATH, strPathName);
	size_t n = strlen(pPathName) - 1;
	while (n != 0 && pPathName[n] != '\\') { n--; }
	pPathName[n] = 0;
	strPathName = &pPathName[1];
	delete [] pPathName;
}

void Directory_ExamineFile(char* oldfile, char* newfile)
{
	// the default file examine -- if the file isn't pathed to the right place,
	// then search the ACAD environment variable for the oldfile */
  
	const int kExistenceOnly = 0;
	
	char holdch;
	char testpath[256];
	  
	static char pathchar='\\';
	static char oldpathchar='/';

	// replace path characters, if present, with proper ones for platform
	for (int i = 0; i < int(strlen(oldfile)); i++)
	{
		if (oldfile[i] == oldpathchar) oldfile[i] = pathchar;
	}
	if (strchr(oldfile, pathchar) != 0) 
	{	// file has some degree of path designation
		if (_access(oldfile, kExistenceOnly) == 0) 
		{
			strcpy_s(newfile, sizeof(newfile), oldfile);
			return;
		}
		// strip the path
		char* pLast = strrchr(oldfile, pathchar);
		strcpy_s(oldfile, sizeof(oldfile), ++pLast);
	}
	strcpy_s(newfile, sizeof(newfile), oldfile);

	if (_access(oldfile, kExistenceOnly) == 0) 
	{	// its in current diretory
		return;
	}
	else 
	{
		if (strchr(oldfile, ':') != 0) 
		{	// path with drive id
			return;
		}
		char* envptr = nullptr;
		size_t len;
		errno_t err = _dupenv_s(&envptr, &len, "ACAD");

	    if (err != 0 || envptr == nullptr) 
		{	// no ACAD environment to search
			return;
		}
		char* cptr = envptr;
		do 
		{
			while (*cptr != ';' && *cptr != 0) 
			{
				cptr++;
			}
			holdch = *cptr;             /* grab terminating character */
			*cptr = 0;
			strcpy_s(testpath, sizeof(testpath), envptr);
			*cptr = holdch;               /* put it back */
			int iTest = (int) strlen(testpath);
			if (testpath[iTest - 1] != pathchar) 
			{  /* append / or \ */
				testpath[iTest] = pathchar;
				testpath[iTest + 1] = 0;
			}
			strcat_s(testpath, sizeof(testpath), oldfile);

			if (!_access(testpath, 0)) 
			{  /* true if found */
				strcpy_s(newfile, sizeof(newfile), testpath);
				return;
			}
			cptr++;
			envptr = cptr;
		} 
		while (holdch == ';');        /* terminator is 0 for end of env string */
	}
}