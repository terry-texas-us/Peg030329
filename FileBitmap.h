#pragma once

#include <afxwin.h>

#include "PixelMapRGB.h"

class CFileBitmap: public CFile
{

private:
    int		m_nRows{0};		// dimensions of the bitmap
    int		m_nCols{0};
    CRGB* m_rgbPixel{nullptr};

public:
    CFileBitmap() { }
    CFileBitmap(const CString& strPathName);

    ~CFileBitmap() { }

    bool Load(const CString& strPathName, CBitmap& bm, CPalette& pal);
};
