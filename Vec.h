#pragma once

#include "afx.h" // for CObject (base class) and CFile
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string>

class CPnt;
class CLine;
class CTMat;
class CVec4;

typedef double	Vecd[3];

class CVec : public CObject
{

private:

	double m_d[3];

public: // constructors and destructor

	CVec() { m_d[0] = m_d[1] = m_d[2] = 0.; }
	CVec(const double dX, const double dY, const double dZ);
	CVec(const CVec& v) { m_d[0] = v.m_d[0]; m_d[1] = v.m_d[1]; m_d[2] = v.m_d[2]; }

	CVec(const CPnt& pt0, const CPnt& pt1);
	CVec(const CLine& ln);
	CVec(const CVec4& v);

	~CVec() { }

public: // operators

	CVec& operator=(const CVec& v);
	void operator+=(const CVec& v);
	void operator-=(const CVec& v);
	void operator*=(const double t);
	void operator/=(const double t);

	double& operator[](int i) { return m_d[i]; }
	const double& operator[](int i) const { return m_d[i]; }

	void operator()(const double dX, const double dY, const double dZ);

	CVec operator-() const { return CVec(-m_d[0], -m_d[1], -m_d[2]); }

	bool operator==(const CVec& v) const { return (Identical(v)); }
	bool operator!=(const CVec& v) const { return (!Identical(v)); }

	CVec operator+(const CVec& v) const;
	CVec operator-(const CVec& v) const;
	double operator*(const CVec& v) const;
	CVec operator*(double t) const;

	CVec operator^(const CVec& v) const;

public: // Methods

	bool	Collinear(const CVec& v, double dTol = FLT_EPSILON) const;
	void	Get(Vecd vd) const { vd[0] = m_d[0]; vd[1] = m_d[1]; vd[2] = m_d[2]; }

	/// @brief Converts the vector's three-dimensional coordinates (x, y, z stored in m_d[0], m_d[1], and m_d[2]) into a formatted string using fixed-point notation with the specified decimal precision (defaulting to 6), applies the minimum field width (defaulting to 0), separates the coordinates with semicolons, and appends a tab character at the end for output.
	/// @param precision Formatting precision (default 6). Typically controls the number of digits after the decimal point for floating-point values or the general formatting precision.
	/// @param minWidth Minimum field width (default 0). If the formatted result is shorter than this width, it will be padded (commonly with spaces) to reach minWidth.
	/// @return A std::string containing the formatted representation of the vector.
	std::string	ToStdString(int precision = 6, int minWidth = 0) const;
	bool	Identical(const CVec& v, double tol = DBL_EPSILON) const;
	bool	IsNull(double dTol = DBL_EPSILON) const;
	double	Length() const { return (sqrt(SquaredLength())); }
	void	Normalize();
	void	Read(CFile&);
	void	RotAboutArbAx(const CVec&, double dAng);
	void	RotAboutZAx(double dSinAng, double dCosAng);
	double	SquaredLength() const;
	void	Write(CFile&) const;

public: // Friends

	friend CPnt operator+(const CPnt& pt, const CVec& v);
	friend CPnt operator-(const CPnt& pt, const CVec& v);
	friend CVec operator*(const double d, const CVec& v);
	friend CVec operator/(const CVec& v, const double d);
	friend CVec Max(const CVec& vA, const CVec& vB);
	friend CVec Min(const CVec& vA, const CVec& vB);
	friend double Vec_Angle_xy(const CVec& v);
	friend double Vec_Length_xy(const CVec& v);

	friend CVec operator*(const CTMat& tm, const CVec& v);
};

const CVec XDIR(1., 0., 0.);
const CVec YDIR(0., 1., 0.);
const CVec ZDIR(0., 0., 1.);

// Constructors

inline CVec::CVec(const double dX, const double dY, const double dZ)
{
	m_d[0] = dX;
	m_d[1] = dY;
	m_d[2] = dZ;
}

// Operators

inline CVec& CVec::operator=(const CVec& v)
{
	m_d[0] = v.m_d[0];
	m_d[1] = v.m_d[1];
	m_d[2] = v.m_d[2];

	return *this;
}

inline void CVec::operator+=(const CVec& v)
{
	m_d[0] += v.m_d[0];
	m_d[1] += v.m_d[1];
	m_d[2] += v.m_d[2];
}

inline void CVec::operator-=(const CVec& v)
{
	m_d[0] -= v.m_d[0];
	m_d[1] -= v.m_d[1];
	m_d[2] -= v.m_d[2];
}

inline void CVec::operator*=(const double t)
{
	m_d[0] *= t;
	m_d[1] *= t;
	m_d[2] *= t;
}

inline void CVec::operator/=(const double t)
{
	m_d[0] /= t;
	m_d[1] /= t;
	m_d[2] /= t;
}

inline void CVec::operator()(const double dX, const double dY, const double dZ)
{
	m_d[0] = dX;
	m_d[1] = dY;
	m_d[2] = dZ;
}

inline CVec CVec::operator+(const CVec& v) const
{
	return CVec(m_d[0] + v.m_d[0], m_d[1] + v.m_d[1], m_d[2] + v.m_d[2]);
}

inline CVec CVec::operator-(const CVec& v) const
{
	return CVec(m_d[0] - v.m_d[0], m_d[1] - v.m_d[1], m_d[2] - v.m_d[2]);
}

inline double CVec::operator*(const CVec& v) const
{
	return (m_d[0] * v.m_d[0] + m_d[1] * v.m_d[1] + m_d[2] * v.m_d[2]);
}

inline CVec CVec::operator*(double t) const
{
	return CVec(m_d[0] * t, m_d[1] * t, m_d[2] * t);
}

inline double CVec::SquaredLength() const
{
	return (m_d[0] * m_d[0] + m_d[1] * m_d[1] + m_d[2] * m_d[2]);
}

// friends

inline CVec operator*(const double t, const CVec& v)
{
	return (v * t);
}

inline CVec operator/(const CVec& v, const double d)
{
	return CVec(v.m_d[0] / d, v.m_d[1] / d, v.m_d[2] / d);
}

inline CVec Max(const CVec& vA, const CVec& vB)
{
	return CVec(std::max(vA.m_d[0], vB.m_d[0]), std::max(vA.m_d[1], vB.m_d[1]), std::max(vA.m_d[2], vB.m_d[2]));
}

inline CVec Min(const CVec& vA, const CVec& vB)
{
	return CVec(std::min(vA.m_d[0], vB.m_d[0]), std::min(vA.m_d[1], vB.m_d[1]), std::min(vA.m_d[2], vB.m_d[2]));
}
