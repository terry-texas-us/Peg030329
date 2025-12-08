#pragma once

#include <string>

#include "PegAEsys.h"

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

/// @brief Creates a simple, human-readable string representation of a length value using the specified units.
/// @param length The length value to format.
/// @param minWidth Minimum width of the resulting string; the output is padded if necessary to reach this width.
/// @param precision Number of digits to display after the decimal point (controls rounding).
/// @param units Enumeration value specifying the units to use for formatting (affects the unit label and any conversion applied).
/// @return A formatted string representing the length with the chosen units, respecting the specified precision and minimum width.
std::string	UnitsString_FormatAsSimple(double length, int minWidth, int precision, EUnits units);

/// @brief Calculates the greatest common divisor (GCD) of two integers.
/// @param aiNmb1 The first integer.
/// @param aiNmb2 The second integer.
/// @return The non-negative greatest common divisor of the two arguments. If one argument is zero, returns the absolute value of the other; if both are zero, returns 0.
int	UnitsString_GCD(int aiNmb1, int aiNmb2);

/// @brief Parses a length string with an optional unit suffix and returns the length converted to the application's internal scale.
/// @param length Pointer to a null-terminated C string containing a numeric length optionally followed by a unit suffix. Recognized suffixes (case-insensitive): ' (feet, may be followed by inches), mm (millimeters), m (meters), cm (centimeters), d (decimeters), k (kilometers). If no recognized suffix is present, the numeric value is treated as inches. The pointer must be valid for reading; the function does not modify the string.
/// @return The parsed length converted to the application's internal units: the value converted to inches according to the suffix (or treated as inches if none) and then divided by app.GetScale().
double UnitsString_ParseLength(char* length);

/// @brief Parses a length expression string and returns its value converted to the application's internal length units (inches adjusted by app.GetScale()).
/// @param eUnits The unit system of the input length expression. Determines the conversion factor applied. Supported values include: Architectural, Engineering, Feet, Inches, Meters, Millimeters, Centimeters, Decimeters, and Kilometers.
/// @param length A null-terminated C string containing the length expression to parse. The string is processed by the lexer/evaluator functions (lex::Parse, lex::EvalTokenStream).
/// @return The parsed length as a double expressed in the application's internal units (the function converts input units to inches and then divides by app.GetScale()). On parse/evaluation error the function displays a message box and returns 0.0.
double UnitsString_ParseLength(EUnits, char* length);
