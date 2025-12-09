#pragma once

#include "Vec.h"  // for CVec

/// @brief Represents character cell properties used for text rendering: character height, width expansion factor, text rotation, and slant angle.
class CCharCellDef {
 private:
  double m_dChrHgt;       // height of character cell
  double m_dChrExpFac;    // expansion factor applied to character cell width
  double m_dTextRotAng;   // rotation applied to the character cell
  double m_dChrSlantAng;  // rotation applied to the vertical component of the character cell

 public:
  CCharCellDef() : m_dChrHgt(0.1), m_dChrExpFac(1.0), m_dTextRotAng(0.0), m_dChrSlantAng(0.0) {}
  CCharCellDef(double dTextRotAng, double dChrSlantAng, double dChrExpFac, double dChrHgt)
      : m_dChrHgt(dChrHgt), m_dChrExpFac(dChrExpFac), m_dTextRotAng(dTextRotAng), m_dChrSlantAng(dChrSlantAng) {}

  CCharCellDef(const CCharCellDef& ccd);

  CCharCellDef& operator=(const CCharCellDef& ccd);

  double ChrExpFacGet() const { return (m_dChrExpFac); }
  void ChrExpFacSet(double d) { m_dChrExpFac = d; }
  double ChrHgtGet() const { return (m_dChrHgt); }
  void ChrHgtSet(double d) { m_dChrHgt = d; }
  double ChrSlantAngGet() const { return (m_dChrSlantAng); }
  void ChrSlantAngSet(double d) { m_dChrSlantAng = d; }
  double TextRotAngGet() const { return (m_dTextRotAng); }
  void TextRotAngSet(double d) { m_dTextRotAng = d; }

  friend void CharCellDef_EncdRefSys(const CCharCellDef& fd, CVec& vRefX, CVec& vRefY);
};

void CharCellDef_EncdRefSys(const CCharCellDef& ccd, CVec& vXAxRef, CVec& vYAxRef);
