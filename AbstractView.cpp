#include "stdafx.h"

CAbstractView::CAbstractView()
{
	m_nViewMode = 0;
	m_nRenderMode = 0;
	m_sUcsOrthoViewType = 1;
	
	m_ptUCSOrigin = ORIGIN;
	m_vUCSXAxis = XDIR;
	m_vUCSYAxis = YDIR;
	m_dElevation = 0.;

	m_ptEye = CPnt(0., 0., 60.);
	m_ptTarget = ORIGIN;
	m_vDirection = ZDIR;
	m_vVwUp = YDIR;
	m_dViewTwist = 0.;
	m_dHeight = 1.;
	m_dWidth = 1.;
	m_dCenter[0] = .5;
	m_dCenter[1] = .5;
	
	m_dLensLength = 60.;

	m_dNearClipDistance = 20.;
	m_dFarClipDistance = 100.;
}
CAbstractView::CAbstractView(const CAbstractView& src)
{
	m_nViewMode = src.m_nViewMode; 
	m_nRenderMode = src.m_nRenderMode;
	
	m_sUcsOrthoViewType = src.m_sUcsOrthoViewType;
	m_ptUCSOrigin = src.m_ptUCSOrigin;
	m_vUCSXAxis = src.m_vUCSXAxis;
	m_vUCSYAxis = src.m_vUCSYAxis;
	m_dElevation = src.m_dElevation;
	
	m_ptEye = src.m_ptEye;
	m_ptTarget = src.m_ptTarget;
	m_vDirection = src.m_vDirection;
	m_vVwUp = src.m_vVwUp;
	m_dViewTwist = src.m_dViewTwist;
	m_dHeight = src.m_dHeight;
	m_dWidth = src.m_dWidth;
	m_dCenter[0] = src.m_dCenter[0]; m_dCenter[1] = src.m_dCenter[1];
	m_dLensLength = src.m_dLensLength;
	m_dNearClipDistance = src.m_dNearClipDistance;
	m_dFarClipDistance = src.m_dFarClipDistance;
}
CAbstractView::CAbstractView(PAD_VPENT pVP)
{
	m_ptUCSOrigin = CPnt(pVP->ucsorg[0], pVP->ucsorg[1], pVP->ucsorg[2]);
	m_vUCSXAxis = CVec(pVP->ucsxdir[0], pVP->ucsxdir[1], pVP->ucsxdir[2]); 
	m_vUCSYAxis = CVec(pVP->ucsydir[0], pVP->ucsydir[1], pVP->ucsydir[2]); 
	m_sUcsOrthoViewType = pVP->ucsorthoviewtype;
	m_dElevation = pVP->ucselevation;
	
	m_ptTarget = CPnt(pVP->viewtarget[0], pVP->viewtarget[1], pVP->viewtarget[2]);
	m_vDirection = CVec(pVP->viewdir[0], pVP->viewdir[1], pVP->viewdir[2]);
	
	m_dViewTwist = pVP->viewtwist;
	m_dHeight = pVP->viewheight;
	m_dWidth = pVP->viewheight * pVP->width / pVP->height;	
	m_dCenter[0] = pVP->viewcenter[0];
	m_dCenter[1] = pVP->viewcenter[1];
	m_dLensLength = pVP->viewlenslength;
	m_dNearClipDistance = -1000.;//pVP->viewfrontclip;
	m_dFarClipDistance = 1000.;//pVP->viewbackclip;

	m_ptEye = m_ptTarget + m_vDirection * m_dLensLength;
	m_vVwUp(0., 1., 0.);
}
CAbstractView& CAbstractView::operator=(const CAbstractView& src)
{
	m_nViewMode = src.m_nViewMode; 
	m_nRenderMode = src.m_nRenderMode;
	
	m_sUcsOrthoViewType = src.m_sUcsOrthoViewType;
	m_ptUCSOrigin = src.m_ptUCSOrigin;
	m_vUCSXAxis = src.m_vUCSXAxis;
	m_vUCSYAxis = src.m_vUCSYAxis;
	m_dElevation = src.m_dElevation;
	
	m_ptEye = src.m_ptEye;
	m_ptTarget = src.m_ptTarget;
	m_vDirection = src.m_vDirection;
	m_vVwUp = src.m_vVwUp;
	m_dViewTwist = src.m_dViewTwist;
	m_dHeight = src.m_dHeight;
	m_dWidth = src.m_dWidth;
	m_dCenter[0] = src.m_dCenter[0]; m_dCenter[1] = src.m_dCenter[1];
	m_dLensLength = src.m_dLensLength;
	m_dNearClipDistance = src.m_dNearClipDistance;
	m_dFarClipDistance = src.m_dFarClipDistance;

	return *this;
}
void CAbstractView::LookAt(const CPnt& ptEye, const CPnt& ptTarget, const CVec& vVwUp) 
{
	m_ptEye = ptEye;
	m_ptTarget = ptTarget;
	SetVwUp(vVwUp);
}
void CAbstractView::SetDirection(const CVec& v)
{
	if (v.Length() > DBL_EPSILON)
	{
		m_vDirection = v;
		m_vDirection.Normalize();
	}
}
void CAbstractView::SetEye(const CVec& v) 
{
	m_ptEye = m_ptTarget + (v * m_dLensLength);
}
void CAbstractView::SetFarClippingEnabled(bool bEnabled)
{
	if (bEnabled) 
	{
		m_nViewMode |= AV_FARCLIPPING;
	}
	else 
	{
		m_nViewMode &= ~AV_FARCLIPPING;
	}
}
void CAbstractView::SetNearClippingEnabled(bool bEnabled)
{
	if (bEnabled) 
	{
		m_nViewMode |= AV_NEARCLIPPING;
	}
	else 
	{
		m_nViewMode &= ~AV_NEARCLIPPING;
	}
}
void CAbstractView::SetPerspectiveEnabled(bool bEnabled)
{
	if (bEnabled) 
	{
		m_nViewMode |= AV_PERSPECTIVE;
	}
	else 
	{
		m_nViewMode &= ~AV_PERSPECTIVE;
	}
}
void CAbstractView::SetVwUp(const CVec& v)
{
	if (v.Length() > DBL_EPSILON)
	{
		m_vVwUp = v;
		m_vVwUp.Normalize();
	}
}