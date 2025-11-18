#pragma once

class CPrim;
class CPrimLine;
class CPrimText;

class CSegsDet : public CSegs
{
private:
	CPnt	m_ptDet;
	double	m_dPicApertSiz;
	CSeg*	m_pDetSeg;
	CPrim*	m_pDetPrim;
public:
	CSegsDet();
	~CSegsDet() {}

	void		AddTailSegsInActiveView(CPegView* pView, CLayer* pLayer);
	void		DelLast();
	CSeg*&		DetSeg() {return m_pDetSeg;}
	CPrim*&		DetPrim() {return m_pDetPrim;}
	CPnt&		DetPt() {return m_ptDet;}
	bool		IsSegEngaged() const {return m_pDetSeg != 0;}
	CSeg*		SelLineUsingPoint(CPegView* pView, const CPnt& pt);
	CSeg*		SelSegAndPrimUsingPoint(const CPnt&);
	CSeg*		SelSegAndPrimAtCtrlPt(const CPnt4& pt);
	CPrimText*	SelTextUsingPoint(CPegView* pView, const CPnt& pt);
	double&		PicApertSiz() {return m_dPicApertSiz;}
};

extern CSegsDet	detsegs;
