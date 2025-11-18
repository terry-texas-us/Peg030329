#pragma once

class CModelView : public CAbstractView
{
private:
	double			m_dUMin;
	double			m_dVMin;
	double			m_dUMax;
	double			m_dVMax; 
	CTMat			m_tmView;
	CTMat			m_tmViewInverse;

public: // Constructors and destructor
	CModelView();
	CModelView(const CModelView& src);

	~CModelView() {}

public: // Operators
	CModelView& operator=(const CModelView& src);

public: // Methods
	void	AdjustWnd(double dAspectRatio);
	void	BuildTransformMatrix();
	void	DoTransform(CPnt4s& pta) const;
	void	DoTransform(CPnt4& pt) const;
	void	DoTransform(int iPts, CPnt4* pt) const;
	void	DoTransform(CVec4& v) const;
	
	CTMat	GetMatrix() const {return m_tmView;}
	CTMat	GetMatrixInverse() const {return m_tmViewInverse;}
	double	GetUExt() const {return m_dUMax - m_dUMin;}
	double	GetUMax() const {return m_dUMax;}
	double	GetUMin() const {return m_dUMin;}
	double	GetVExt() const {return m_dVMax - m_dVMin;}
	double	GetVMax() const {return m_dVMax;}
	double	GetVMin() const {return m_dVMin;}
	
	void	Initialize(double dAspectRatio);
	void	LoadIdentity() {m_tmView.Identity();}
	void	SetCenteredWnd(double dAspectRatio, double dUExt, double dVExt);
	void	SetMatrix(const CTMat& tm) {m_tmView = tm;}
	void	SetWnd(double dUMin, double dVMin, double dUMax, double dVMax);

	void	RotateNorm(const CVec& vNorm);
	void	RotateZ(double dSinAng, double dCosAng);
	void	Scale(const CVec& v);
	void	Translate(const CVec& v);
};

typedef CList<CModelView> CModelViews;
