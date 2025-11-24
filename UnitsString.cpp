#include "stdafx.h"

#include "PegAEsys.h"

#include "Lex.h"

///<summary>Finds the greatest common divisor of arbitrary integers.</summary>
// Returns: First number if second number is zero, greatest 
//			common divisor otherwise.
// Parameters:	aiNmb1	first number
//				aiNmb2	second number
int UnitsString_GCD(int aiNmb1, int aiNmb2)	   
{
	int iRetVal = abs(aiNmb1);
	int iNmb = abs(aiNmb2);
	while (iNmb != 0)
	{
		int iRem = iRetVal % iNmb;
		iRetVal = iNmb;
		iNmb = iRem;
	}
	return (iRetVal);
}

std::string UnitsString_FormatAngle(const double angle, int minWidth, int precision)
{
	if (!std::isfinite(angle)) 
	{ // Angle is NaN or infinite
		return "Invalid angle";
	}
	if (precision < 0) { precision = 0; }
	if (minWidth < 0) { minWidth = 0; }

	std::stringstream ss;
	ss << std::left << std::setprecision(precision) << std::setw(0);
	ss << (angle / RADIAN);
	return ss.str();
}

void UnitsString_FormatLength(char* buffer, size_t bufferSize, EUnits units, double adVal, int precision, int aiScal)
{
	char numberBuffer[16];

	// Reduce length to number of scaled inches
	double dLen = adVal * app.GetScale();			
	
	if (units == Architectural)
	{
		// Determine how many whole feet and inches
		int iNmbFt = int(dLen / 12.);
		int iNmbIn = abs(int(fmod(dLen, 12.)));
		int iUnitsPrec = app.GetUnitsPrec();
		int iNum = int(fabs(fmod(dLen, 1.)) * (double) (iUnitsPrec) + .5);	// Numerator of fractional component of inches
		
		if (iNum == iUnitsPrec)
		{	// Tricky wholes
			if (iNmbIn == 11)
			{
				iNmbFt++;
				iNmbIn = 0;
			}
			else
				iNmbIn++;
			
			iNum = 0;
		}
		buffer[0] = '\0';
		if (iNmbFt == 0 && dLen < 0.)
		{
			buffer[0] = '-';
			buffer[1] = '\0';
		}
		_itoa_s(iNmbFt, numberBuffer, sizeof(numberBuffer), 10);
		strcat_s(buffer, bufferSize, numberBuffer);
		strcat_s(buffer, bufferSize, "'");
		_itoa_s(iNmbIn, numberBuffer, sizeof(numberBuffer), 10);
		strcat_s(buffer, bufferSize, numberBuffer);
		if (iNum > 0)				// Significant decimal inch component
		{
			strcat_s(buffer, bufferSize, "^/");
			int	iGrtComDivisor = UnitsString_GCD(iNum, iUnitsPrec);
			iNum = iNum / iGrtComDivisor;
			int iDen = iUnitsPrec / iGrtComDivisor; // Add fractional component of inches
			_itoa_s(iNum, numberBuffer, sizeof(numberBuffer), 10);
			strcat_s(buffer, bufferSize, numberBuffer);
			strcat_s(buffer, bufferSize, "/");
			_itoa_s(iDen, numberBuffer, sizeof(numberBuffer), 10);
			strcat_s(buffer, bufferSize, numberBuffer);
			strcat_s(buffer, bufferSize, "^");
		}
		strcat_s(buffer, bufferSize, "\"");
	}
	else if (units == Engineering)
	{
		int iScal = aiScal;
		if (fabs(dLen) >= 1.)
		{
			iScal = aiScal - int(log10(fabs(dLen))) - 1;
		}
		if (iScal >= 0)
		{											 
			double dScalFac = pow(10., (double) iScal);
			
			dLen = fabs(dLen) + .5 /  dScalFac; 				// Round it
			buffer[0] = ' ';
			buffer[1] = '\0';
			_itoa_s(int(dLen / 12.), numberBuffer, sizeof(numberBuffer), 10);
			strcat_s(buffer, bufferSize, numberBuffer);
			dLen = fmod(dLen, 12.);
			if (adVal < 0.)
			{
				buffer[0] = '-';
			}
			strcat_s(buffer, bufferSize, "'");
			_itoa_s(int(dLen), numberBuffer, sizeof(numberBuffer), 10);
			strcat_s(buffer, bufferSize, numberBuffer);
			strcat_s(buffer, bufferSize, ".");
			dLen = fmod(dLen, 1.) * dScalFac;
			_itoa_s(int(dLen), numberBuffer, sizeof(numberBuffer), 10);
			strcat_s(buffer, bufferSize, numberBuffer);
			strcat_s(buffer, bufferSize, "\"");
		}	
		else
			buffer[0] = '\0';

	}
	else
	{
		char format[24];

		strcpy_s(format, sizeof(format), "%");
		_itoa_s(precision, numberBuffer, sizeof(numberBuffer), 10);
		strcat_s(format, sizeof(format), numberBuffer);
		strcat_s(format, sizeof(format), ".");
		_itoa_s(aiScal, numberBuffer, sizeof(numberBuffer), 10);
		strcat_s(format, sizeof(format), numberBuffer);
		strcat_s(format, sizeof(format), "f");

		if (units == Feet)
		{
			strcat_s(format, sizeof(format), "'");
			sprintf_s(buffer, bufferSize, format, dLen / 12.);
		}
		else if (units == Inches)
		{
			strcat_s(format, sizeof(format), "\"");
			sprintf_s(buffer, bufferSize, format, dLen);
		}
		else if (units == Meters)
		{
			strcat_s(format, sizeof(format), "m");
			sprintf_s(buffer, bufferSize, format, dLen * 0.0254);
		}
		else if (units == Millimeters)
		{
			strcat_s(format, sizeof(format), "mm");
			sprintf_s(buffer, bufferSize, format, dLen * 25.4);
		}
		else if (units == Centimeters)
		{
			strcat_s(format, sizeof(format), "cm");
			sprintf_s(buffer, bufferSize, format, dLen * 2.54);
		}
		else if (units == Decimeters)
		{
			strcat_s(format, sizeof(format), "dm");
			sprintf_s(buffer, bufferSize, format, dLen * 0.254);
		}
		else if (units == Kilometers)
		{
			strcat_s(format, sizeof(format), "km");
			sprintf_s(buffer, bufferSize, format, dLen * 0.0000254);
		}
		else
			buffer[0] = '\0';
	}
}

double UnitsString_ParseLength(char* aszLen)
{
	char*	szEndPtr;
	
	double dRetVal = strtod(aszLen, &szEndPtr);
	
	switch (toupper((int) szEndPtr[0]))
	{
		case '\'':												// Feet and maybe inches
			dRetVal *= 12.; 										// Reduce to inches
			dRetVal += strtod(&szEndPtr[1], &szEndPtr); 			// Begin scan for inches at character following foot delimeter
			break;
	
		case 'M': 
			if (toupper((int) szEndPtr[1]) == 'M')
				dRetVal *= .03937007874015748;
			else
				dRetVal *= 39.37007874015748;
			break;
	
		case 'C':
			dRetVal *= .3937007874015748;
			break;
			 
		case 'D':
			dRetVal *= 3.937007874015748;
			break;
		
		case 'K':
			dRetVal *= 39370.07874015748;
	
	}
	return (dRetVal / app.GetScale());
}

double UnitsString_ParseLength(EUnits eUnits, char* aszLen)
{	// Convert length expression to double value.
	try
	{
		int iTokId = 0;
		long lDef;
		int iTyp;
		double dVal[32];
		size_t bufferSize = sizeof(dVal);

		lex::Parse(aszLen);
		lex::EvalTokenStream((char*) 0, &iTokId, &lDef, &iTyp, (void*) dVal, bufferSize);
		
		if (iTyp == lex::TOK_LENGTH_OPERAND)
			return (dVal[0]);
		else
		{
			lex::ConvertValTyp(iTyp, lex::TOK_REAL, &lDef, dVal);
			
			switch (eUnits)
			{
				case Architectural:
				case Engineering:
				case Feet:
					dVal[0] *= 12.;
					break;

				case Meters:
					dVal[0] *= 39.37007874015748;
					break;

				case Millimeters:
					dVal[0] *= .03937007874015748;
					break;

				case Centimeters:
					dVal[0] *= .3937007874015748;
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
	}
	catch(char* szMessage)
	{
		::MessageBox(0, szMessage, 0, MB_ICONWARNING | MB_OK);
		return (0.);
	}
}

