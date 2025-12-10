#include "stdafx.h"

#include <afxstr.h>
#include <afxwin.h>

#include "FileBitmap.h"

CFileBitmap::CFileBitmap(const CString& strFileName) {
  CFileException e;
  if (CFile::Open(strFileName, modeRead | shareDenyNone, &e)) {}
}

bool CFileBitmap::Load(const CString& strFileName, CBitmap& bmReference, CPalette& palReference) {
  auto hBitmap = (HBITMAP)::LoadImage(nullptr, strFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
  if (hBitmap == nullptr) return false;

  bmReference.Attach(hBitmap);

  // Return now if device does not support palettes

  CClientDC dc(nullptr);
  if ((dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE) == 0) { return true; }

  DIBSECTION ds{};
  bmReference.GetObject(sizeof(DIBSECTION), &ds);

  int nColors{};

  if (ds.dsBmih.biClrUsed != 0) {
    nColors = static_cast<int>(ds.dsBmih.biClrUsed);
  }

  else {
    nColors = 1 << ds.dsBmih.biBitCount;
  }
  // Create a halftone palette if the DIB section contains more than 256 colors
  if (nColors > 256) {
    palReference.CreateHalftonePalette(&dc);
  } else {  // Create a custom palette from the DIB section's color table
    auto* pRGB = new RGBQUAD[static_cast<size_t>(nColors)];

    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);

    CBitmap* pBitmap = dcMem.SelectObject(&bmReference);
    ::GetDIBColorTable((HDC)dcMem, 0, static_cast<UINT>(nColors), pRGB);
    dcMem.SelectObject(pBitmap);

    UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * (nColors - 1));

    auto* pLogPal = (LOGPALETTE*)new BYTE[nSize];

    pLogPal->palVersion = 0x300;
    pLogPal->palNumEntries = (WORD)nColors;

    for (int i = 0; i < nColors; i++) {
      pLogPal->palPalEntry[i].peRed = pRGB[i].rgbRed;
      pLogPal->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
      pLogPal->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
      pLogPal->palPalEntry[i].peFlags = 0;
    }
    palReference.CreatePalette(pLogPal);

    delete[] pLogPal;
    delete[] pRGB;
  }
  Close();

  return true;
}
