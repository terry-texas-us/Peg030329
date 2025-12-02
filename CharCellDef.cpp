#include "stdafx.h"

#include "PegAEsysView.h"

#include "CharCellDef.h"
#include "SafeMath.h"
#include "Vec.h"

CCharCellDef::CCharCellDef(const CCharCellDef& fd)
{
    m_dChrHgt = fd.m_dChrHgt;
    m_dChrExpFac = fd.m_dChrExpFac;
    m_dTextRotAng = fd.m_dTextRotAng;
    m_dChrSlantAng = fd.m_dChrSlantAng;
}
CCharCellDef& CCharCellDef::operator=(const CCharCellDef& fd)
{
    m_dChrHgt = fd.m_dChrHgt;
    m_dChrExpFac = fd.m_dChrExpFac;
    m_dTextRotAng = fd.m_dTextRotAng;
    m_dChrSlantAng = fd.m_dChrSlantAng;

    return (*this);
}
///<summary>Produces the reference system vectors for a single charater cell.</summary>
void CharCellDef_EncdRefSys(const CCharCellDef& ccd, CVec& vRefX, CVec& vRefY)
{
    CPegView* pView = CPegView::GetActiveView();

    CVec vNorm = pView->ModelViewGetDirection();

    vRefY = pView->ModelViewGetVwUp();
    vRefY.RotAboutArbAx(vNorm, ccd.m_dTextRotAng);

    vRefX = vRefY;
    vRefX.RotAboutArbAx(vNorm, -HALF_PI);
    vRefY.RotAboutArbAx(vNorm, ccd.m_dChrSlantAng);
    vRefX *= .6 * ccd.m_dChrHgt * ccd.m_dChrExpFac;
    vRefY *= ccd.m_dChrHgt;
}