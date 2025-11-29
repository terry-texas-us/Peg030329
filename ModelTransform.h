#pragma once

#include <afxwin.h> // for CObject (base class)

#include <afxcoll.h> // for CObList

#include "Pnt.h" // for CPnt
#include "Pnt4.h" // for CPnt4
#include "TMat.h" // for CTMat
#include "Vec.h" // for CVec

class CModelTransform : public CObject
{

private:

	WORD	m_wDepth;
	CTMat*	m_p_tmCompositeModeling;

	CObList m_TMList;	

public:
	
	CModelTransform();
	~CModelTransform();

	// Explicitly delete copy constructor and assignment operator to avoid C4625 and C4626
	CModelTransform(const CModelTransform&) = delete;
	CModelTransform& operator=(const CModelTransform&) = delete;

	void	InvokeNew();
	void	Return();
	void	Transform(CPnt& pt) const;
	void	Transform(CPnt4& pt) const;
	void	Transform(CVec4& v) const;
	void	Transform(CVec& v) const;
	void	SetLocalTM(const CTMat& tm);

};

extern CModelTransform mspace;
