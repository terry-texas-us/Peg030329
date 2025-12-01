#pragma once

#include <afx.h>

#include <afxtempl.h> // for CList
#include <atltypes.h>

#include "Pnt.h" // for CPnt
#include "Pnt4.h" // for CPnt4, CPnt4s

class CViewport: public CObject
{
private:
    CPnt	m_ptCenter;				    // center of the viewport
    int		m_iDeviceHeightInPixels{};
    double	m_dDeviceHeightInInches{};
    int		m_iDeviceWidthInPixels{};
    double  m_dDeviceWidthInInches{};
    double	m_dHeight{};				// height of the viewport
    double	m_dWidth{};				    // width of the viewport

public: // Constructors and destructors

    CViewport() { }
    CViewport(int iWidth, int iHeight) { Set(iWidth, iHeight); }
#if ODA_FUNCTIONALITY
    CViewport(PAD_VPENT);
#endif
    CViewport(const CViewport& src);

    ~CViewport() { };

    CViewport& operator=(const CViewport& src);

public: // Methods
    double	GetAspectRatio() const { return m_dHeight / m_dWidth; }
    CPnt	GetCenter() const { return m_ptCenter; }
    double	GetHeight() const { return m_dHeight; }
    double	GetWidthInInches() const { return m_dWidth / (m_iDeviceWidthInPixels / m_dDeviceWidthInInches); }
    double	GetHeightInInches() const { return m_dHeight / (m_iDeviceHeightInPixels / m_dDeviceHeightInInches); }
    double	GetWidth() const { return m_dWidth; }

    CPoint	DoProjection(const CPnt4& pt) const;
    void	DoProjection(CPoint* pnt, int iPts, CPnt4* pt) const;
    void	DoProjection(CPoint* pnt, CPnt4s& pta) const;
    void	DoProjectionInverse(CPnt& pt) const;

    void	Set(double dWidth, double dHeight);
    void	SetDeviceHeightInInches(double dHeight) { m_dDeviceHeightInInches = dHeight; }
    void	SetDeviceHeightInPixels(int iHeight) { m_iDeviceHeightInPixels = iHeight; }
    void	SetDeviceWidthInInches(double dWidth) { m_dDeviceWidthInInches = dWidth; }
    void	SetDeviceWidthInPixels(int iWidth) { m_iDeviceWidthInPixels = iWidth; }
    void	SetHeight(double dHeight) { m_dHeight = dHeight; }
    void	SetWidth(double dWidth) { m_dWidth = dWidth; }
};
typedef CList<CViewport> CViewports;
