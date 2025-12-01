#pragma once

#include <afx.h>
#include <afxwin.h>

#include <afxstr.h>

#include "PixelMapRGB.h"

class CFileBitmap: public CFile
{

private:
    int   m_nRows{0};		// dimensions of the bitmap
    int	  m_nCols{0};
    CRGB* m_rgbPixel{nullptr};

public:
    CFileBitmap() { }
    CFileBitmap(const CString& strPathName);

    CFileBitmap(const CFileBitmap&) = delete;
    CFileBitmap& operator=(const CFileBitmap&) = delete;

    ~CFileBitmap() { }

    bool Load(const CString& strPathName, CBitmap& bm, CPalette& pal);
};
