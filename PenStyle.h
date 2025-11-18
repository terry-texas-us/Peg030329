#pragma once

class CPenStyle : public CObject
{
private:
	CString		m_strName;		// linetype name
								// standard flag values (not implemented)
	CString		m_strDesc; 		// descriptive text for linetype
	WORD		m_wLens;		// number of dash length items
	double*		m_dLen; 		// pointer to zero or more dash lengths

public:
	CPenStyle();
	CPenStyle(const CString& strName, const CString& strDesc, WORD, double*);

	CPenStyle(const CPenStyle& penstyle);
	
	~CPenStyle();
	
	CPenStyle& operator=(const CPenStyle& penstyle);
	
	WORD GetDefLen() const {return m_wLens;}
	void GetDashLen(double* dDash) const {for (int i = 0; i < m_wLens; i++) dDash[i] = m_dLen[i];}
	CString GetDescription() const {return m_strDesc;}
	CString GetName() const {return m_strName;}
	double GetPatternLen() const;
};

typedef CTypedPtrArray<CObArray, CPenStyle*> CPenStyles;
