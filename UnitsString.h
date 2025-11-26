#pragma once

#include "PegAEsys.h"

#include <string>

/// @brief Formats an angle value into a string using the specified minimum field width and decimal precision.
/// @param angle The angle value to format.
/// @param minWidth Minimum total field width for the resulting string; padded if necessary. Default is 0.
/// @param precision Number of digits to display after the decimal point. Default is 6.
/// @return A std::string containing the formatted angle.
std::string	UnitsString_FormatAngle(const double angle, int minWidth = 0, int precision = 6);

/// @brief Formats a length value into a textual representation according to the specified units and writes it into a caller-provided buffer.
/// @param aszBuf Pointer to the output buffer where the formatted string will be written (null-terminated).
/// @param bufferSize Size of the output buffer in bytes/characters; used to prevent buffer overruns.
/// @param units EUnits value specifying the unit system or unit type to use for formatting.
/// @param adVal The numeric length value to format.
/// @param minWidth Minimum field width for the formatted output; the result will be padded if shorter than this (default 0).
/// @param precision Number of fractional digits to include in the formatted output (default 4).
void UnitsString_FormatLength(char* aszBuf, size_t bufferSize, EUnits units, double adVal, int minWidth = 0, int precision = 4);

/// @brief Formats a length value as an architectural-style string (feet, inches, and fractional inches).
/// @param length The length value to format, expressed in the library's base unit (commonly inches or feet — refer to the library/application settings).
/// @return A std::string containing the length in architectural notation (e.g., 5'-7 3/8, representing feet, inches, and fractional inches).
std::string UnitsString_FormatAsArchitectural(double length);

/// @brief Formats a given length (in inches) into an engineering-style string representation (e.g., feet'inches.decimal"), handling scaling, rounding, and precision adjustments.
/// @param length The numeric value to format.
/// @param precision The number of digits to display after the decimal point; determines rounding of the formatted value.
/// @return A std::string containing the value formatted in engineering notation with the specified precision.
std::string UnitsString_FormatAsEngineering(double length, int precision);

/// @brief Formats a length value as a simple string based on the specified units. 
/// @param length
/// @param minWidth 
/// @param precision 
/// @param units 
/// @return
std::string	UnitsString_FormatAsSimple(double length, int minWidth, int precision, EUnits units);

/// @brief Calculates the greatest common divisor (GCD) of two integers.
/// @param aiNmb1 The first integer.
/// @param aiNmb2 The second integer.
/// @return The non-negative greatest common divisor of the two arguments. If one argument is zero, returns the absolute value of the other; if both are zero, returns 0.
int	UnitsString_GCD(int aiNmb1, int aiNmb2);	   

/// @brief 
/// @param  
/// @return 
double UnitsString_ParseLength(char*);

/// @brief 
/// @param  
/// @param  
/// @return 
double UnitsString_ParseLength(EUnits, char*);
