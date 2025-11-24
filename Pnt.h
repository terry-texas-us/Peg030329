#pragma once

#include "Vec.h"

class CPnt4;

typedef CArray<CPnt, const CPnt&> CPnts;

class CPnt : public CObject
{

private:
	
	double m_d[3];

public:	// Constructors and destructor

	CPnt() {m_d[0] = m_d[1] = m_d[2] = 0.;}
	CPnt(const double dX, const double dY, const double dZ) {m_d[0] = dX; m_d[1] = dY; m_d[2] = dZ;}
	CPnt(const CPnt& pt) {m_d[0] = pt.m_d[0]; m_d[1] = pt.m_d[1]; m_d[2] = pt.m_d[2];}
	
	CPnt(const CPnt4& pt);

	~CPnt() {}

public: // Operators

	CPnt&	operator=(const CPnt& pt);
	
	void	operator+=(const CVec& v) {m_d[0] += v[0]; m_d[1] += v[1]; m_d[2] += v[2];}
	void	operator-=(const CVec& v) {m_d[0] -= v[0]; m_d[1] -= v[1]; m_d[2] -= v[2];}
	void	operator*=(const double t) {m_d[0] *= t; m_d[1] *= t; m_d[2] *= t;}
	void	operator/=(const double t) {m_d[0] /= t; m_d[1] /= t; m_d[2] /= t;}
	
	double& operator[](int i) {return m_d[i];}
	const	double& operator[](int i) const {return m_d[i];}	

	void	operator()(const double dX, const double dY, const double dZ) {m_d[0] = dX; m_d[1] = dY; m_d[2] = dZ;}
	
	CPnt	operator*(double t) const {return CPnt(m_d[0] * t, m_d[1] * t, m_d[2] * t);}
	CPnt	operator/(double t) const {return CPnt(m_d[0] / t, m_d[1] / t, m_d[2] / t);}
	
public: // Methods

	void	Get(Vecd vd) const {vd[0] = m_d[0]; vd[1] = m_d[1]; vd[2] = m_d[2];}
	
	/// @brief Converts the point's three-dimensional coordinates (x, y, z stored in m_d[0], m_d[1], and m_d[2]) into a formatted string using fixed-point notation with the specified decimal precision (defaulting to 6), applies the minimum field width (defaulting to 0), separates the coordinates with semicolons, and appends a tab character at the end for output.
	/// @param precision Formatting precision (default 6). Typically controls the number of digits after the decimal point for floating-point values or the general formatting precision.
	/// @param minWidth Minimum field width (default 0). If the formatted result is shorter than this width, it will be padded (commonly with spaces) to reach minWidth.
	/// @return A std::string containing the formatted representation of the point.
	std::string ToStdString(int precision = 6, int minWidth = 0) const;
	void	Read(CFile&);
	void	Write(CFile&) const;

public: // Friends

	friend CPnt	operator+(const CPnt& pt, const CVec& v);	
	friend CVec operator-(const CPnt& ptA, const CPnt& ptB);
	friend CPnt	operator-(const CPnt& pt, const CVec& v);
	friend CPnt operator*(const CTMat& tm, const CPnt& pt);

	friend bool operator==(const CPnt& ptA, const CPnt& ptB);
	friend bool	operator!=(const CPnt& ptA, const CPnt& ptB);
	
	friend CPnt Max(const CPnt& ptA, const CPnt& ptB);
	friend CPnt Mid(const CPnt& ptA, const CPnt& ptB);
	friend CPnt Min(const CPnt& ptA, const CPnt& ptB);
	
	friend double Pnt_DistanceTo_xy(const CPnt&, const CPnt&);
	friend CPnt Pnt_RotateAboutPt_xy(CPnt, const CPnt&, const double);
	friend CPnt Pnt_ProjPtTo(const CPnt&, const CPnt&, const double);
	friend CPnt Pnt_ProjPtTo_xy(const CPnt&, const CPnt&, const double);
//	virtual void Dump(CDumpContext& dc) const;
};

bool	p2DPtInWnd(const CPnt& pt, CPnt* ptWnd); 
int		p2DRelOfPtToWnd(const CPnt& pt, const CPnt& ptLL, const CPnt& ptUR);
bool	pFndCPGivRadAnd4Pts(double, CPnt, CPnt, CPnt, CPnt, CPnt*);
CPnt	Pnt_RotAboutArbPtAndAx(const CPnt&, const CPnt&, const CVec&, double);

const CPnt ORIGIN(0., 0., 0.);
