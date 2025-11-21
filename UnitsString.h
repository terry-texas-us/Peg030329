#pragma once

#include "PegAEsys.h"

#include "StringExtra.h"

void	UnitsString_FormatAngle(char*, double, int = 6, int = 2);

/// <summary>
/// Formats a length value into a string based on the specified units.
/// </summary>
/// <param name="aszBuf">Output buffer to store the formatted length string.</param>
/// <param name="bufferSize">Size of the output buffer.</param>
/// <param name="units">Enumeration specifying the units (e.g., Architectural, Engineering, Feet, etc.).</param>
/// <param name="adVal">Length value to format.</param>
/// <param name="aiPrec">Precision for formatting.</param>
/// <param name="aiScal">Scale for formatting.</param>
/// <remarks>
/// Architectural units formatted as follows:
///	<feet>'<inches>.<fraction numerator>/<fraction denominator>"
/// Engineering units formatted as follows:
///	<feet>'<inches>.<decimal inches>"
/// All other units formatted using floating decimal.
/// </remarks>
void	UnitsString_FormatLength(char* aszBuf, size_t bufferSize, EUnits units, double adVal, int aiPrec, int aiScal);
int		UnitsString_GCD(int aiNmb1, int aiNmb2);	   
double  UnitsString_ParseLength(char*);
double	UnitsString_ParseLength(EUnits, char*);
