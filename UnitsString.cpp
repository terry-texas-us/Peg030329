#include "stdafx.h"

#include <iomanip>
#include <sstream>

#include "PegAEsys.h"

#include "Lex.h"
#include "SafeMath.h"
#include "UnitsString.h"

int UnitsString_GCD(int aiNmb1, int aiNmb2) {
  int iRetVal = abs(aiNmb1);
  int iNmb = abs(aiNmb2);
  while (iNmb != 0) {
    int iRem = iRetVal % iNmb;
    iRetVal = iNmb;
    iNmb = iRem;
  }
  return (iRetVal);
}

std::string UnitsString_FormatAngle(const double angle, int minWidth, int precision) {
  if (!std::isfinite(angle)) {  // Angle is NaN or infinite
    return _T("Invalid angle");
  }
  if (precision < 0) { precision = 0; }
  if (minWidth < 0) { minWidth = 0; }

  std::stringstream ss;
  ss << std::left << std::setprecision(precision) << std::setw(0);
  ss << (angle / RADIAN);
  return ss.str();
}

void UnitsString_FormatLength(TCHAR* buffer, size_t bufferSize, EUnits units, double length, int minWidth,
                              int precision) {
  if (units == Architectural) {
    _tcscpy_s(buffer, bufferSize, UnitsString_FormatAsArchitectural(length).c_str());
  } else if (units == Engineering) {
    _tcscpy_s(buffer, bufferSize, UnitsString_FormatAsEngineering(length, precision).c_str());
  } else {
    _tcscpy_s(buffer, bufferSize, UnitsString_FormatAsSimple(length, minWidth, precision, units).c_str());
  }
}

std::string UnitsString_FormatAsArchitectural(double length) {
  length *= app.GetScale();

  // Determine how many whole feet and inches
  int feet = int(length / 12.);
  int inches = abs(int(fmod(length, 12.)));
  int unitsPrecision = app.GetUnitsPrec();
  int fractionNumerator = int(fabs(fmod(length, 1.)) * static_cast<double>(unitsPrecision) +
                              0.5);  // Numerator of fractional component of inches

  if (fractionNumerator == unitsPrecision) {
    if (inches == 11) {  // Rounding up the fractional part rolls over to next inch
      feet++;
      inches = 0;
    } else {
      inches++;
    }
    fractionNumerator = 0;
  }
  std::stringstream ss;
  if (feet == 0 && length < 0.) { ss << '-'; }
  ss << feet << _T("'") << inches;
  if (fractionNumerator > 0) {  //
    int greatestCommonDivisor = UnitsString_GCD(fractionNumerator, unitsPrecision);
    fractionNumerator = fractionNumerator / greatestCommonDivisor;
    int fractionDenominator = unitsPrecision / greatestCommonDivisor;  // Add fractional component of inches
    ss << _T("^/") << fractionNumerator << _T("/") << fractionDenominator << _T("^");
  }
  ss << _T("\"");
  return ss.str();
}

std::string UnitsString_FormatAsEngineering(double length, int precision) {
  length *= app.GetScale();

  std::stringstream ss;
  if (fabs(length) >= 1.) { precision = precision - int(log10(fabs(length))) - 1; }
  if (precision >= 0) {
    double scaleFactor = pow(10., static_cast<double>(precision));

    length = fabs(length) + 0.5 / scaleFactor;  // Round it
    if (length < 0.) { ss << '-'; }

    ss << int(length / 12.) << _T("'");
    length = fmod(length, 12.);
    ss << int(length) << _T(".");
    length = fmod(length, 1.) * scaleFactor;
    ss << int(length) << _T("\"");
  }
  return ss.str();
}

std::string UnitsString_FormatAsSimple(double length, int minWidth, int precision, EUnits units) {
  length *= app.GetScale();

  std::stringstream ss;
  ss << std::fixed << std::left << std::setprecision(precision) << std::setw(minWidth);

  if (units == Feet) {
    ss << (length / 12.) << _T("'");
  } else if (units == Inches) {
    ss << length << _T("\"");
  } else if (units == Meters) {
    ss << (length * 0.0254) << _T("m");
  } else if (units == Millimeters) {
    ss << (length * 25.4) << _T("mm");
  } else if (units == Centimeters) {
    ss << (length * 2.54) << _T("cm");
  } else if (units == Decimeters) {
    ss << (length * 0.254) << _T("dm");
  } else if (units == Kilometers) {
    ss << (length * 0.0000254) << _T("km");
  } else {
    ss << length;
  }
  return ss.str();
}

double UnitsString_ParseLength(TCHAR* aszLen) {
  TCHAR* szEndPtr;

  double dRetVal = strtod(aszLen, &szEndPtr);

  switch (toupper((int)szEndPtr[0])) {
    case '\'':                                     // Feet and maybe inches
      dRetVal *= 12.;                              // Reduce to inches
      dRetVal += strtod(&szEndPtr[1], &szEndPtr);  // Begin scan for inches at character following foot delimeter
      break;

    case 'M':
      if (toupper((int)szEndPtr[1]) == 'M') {
        dRetVal *= 0.03937007874015748;
      } else {
        dRetVal *= 39.37007874015748;
}
      break;

    case 'C':
      dRetVal *= 0.3937007874015748;
      break;

    case 'D':
      dRetVal *= 3.937007874015748;
      break;

    case 'K':
      dRetVal *= 39370.07874015748;
  }
  return (dRetVal / app.GetScale());
}

double UnitsString_ParseLength(EUnits eUnits, TCHAR* aszLen) {  // Convert length expression to double value.
  try {
    int iTokId = 0;
    long lDef;
    int iTyp;
    double dVal[32];
    size_t bufferSize = sizeof(dVal);

    lex::Parse(aszLen);
    lex::EvalTokenStream(nullptr, &iTokId, &lDef, &iTyp, (void*)dVal, bufferSize);

    if (iTyp == lex::TOK_LENGTH_OPERAND) {
      return (dVal[0]);
    } else {
      lex::ConvertValTyp(iTyp, lex::TOK_REAL, &lDef, dVal);

      switch (eUnits) {
        case Architectural:
        case Engineering:
        case Feet:
          dVal[0] *= 12.;
          break;
        case Inches:
          break;
        case Meters:
          dVal[0] *= 39.37007874015748;
          break;
        case Millimeters:
          dVal[0] *= 0.03937007874015748;
          break;
        case Centimeters:
          dVal[0] *= 0.3937007874015748;
          break;
        case Decimeters:
          dVal[0] *= 3.937007874015748;
          break;
        case Kilometers:
          dVal[0] *= 39370.07874015748;
      }
      dVal[0] /= app.GetScale();
    }
    return (dVal[0]);
  } catch (TCHAR* szMessage) {
    ::MessageBox(nullptr, szMessage, nullptr, MB_ICONWARNING | MB_OK);
    return (0.);
  }
}
