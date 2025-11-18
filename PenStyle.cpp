#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPenStyle::CPenStyle()
{
	m_wLens = 0;
	m_dLen = NULL;
}
CPenStyle::CPenStyle(const CString& strName, const CString& strDesc, WORD wLens, double* dLen)
{
	m_strName = strName;
	m_strDesc = strDesc;
	m_wLens = wLens;
	if (m_wLens == 0)
		m_dLen = 0;
	else
	{
		m_dLen = new double[m_wLens];
		for (int i = 0; i < m_wLens; i++)
		{
			m_dLen[i] = Max(- 99., Min(dLen[i], 99.));
		}
	}
}
CPenStyle::CPenStyle(const CPenStyle& penstyle)
{
	m_strName = penstyle.m_strName;
	m_strDesc = penstyle.m_strDesc;
	m_wLens = penstyle.m_wLens;
	m_dLen = new double[m_wLens];
	for (int i = 0; i < m_wLens; i++)
	{
		m_dLen[i] = penstyle.m_dLen[i];
	}
}
CPenStyle::~CPenStyle()
{
	delete [] m_dLen;
}
CPenStyle& CPenStyle::operator=(const CPenStyle& penstyle)
{
	m_strName = penstyle.m_strName;
	m_strDesc = penstyle.m_strDesc;
	m_wLens = penstyle.m_wLens;
	m_dLen = new double[m_wLens];
	for (int i = 0; i < m_wLens; i++)
	{
		m_dLen[i] = penstyle.m_dLen[i];
	}
	return *this;
}
double CPenStyle::GetPatternLen() const
{
	double dLen = 0.;

	for (int i = 0; i < m_wLens; i++) 
		dLen += fabs(m_dLen[i]);
	
	return (dLen);
}