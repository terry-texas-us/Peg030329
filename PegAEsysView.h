#pragma once

#include <Windows.h>

#include <afx.h>
#include <afxwin.h>

#include <atltypes.h>

#include <string>

#include "ModelView.h"
#include "Pnt.h"
#include "Pnt4.h"
#include "TMat.h"
#include "Vec.h"
#include "Vec4.h"
#include "Viewport.h"

class CPegDoc;

class CPegView : public CView {
 protected:  // create from serialization only
  CPegView();
  DECLARE_DYNCREATE(CPegView)

  // Explicitly delete copy constructor and assignment operator to fix C4625 and C4626 warnings
  CPegView(const CPegView&) = delete;
  CPegView& operator=(const CPegView&) = delete;

  // Attributes
 public:
  bool m_bViewPenWidths;
  bool m_bViewWireframe;

 protected:
  bool m_bViewBackgroundImage;
  bool m_bViewOdometer;
  bool m_bViewRendered;
  bool m_bViewTrueTypeFonts;
  CBitmap m_bmBackgroundImage;
  CPalette m_palBackgroundImage;

  CViewport m_vpActive;
  CViewports m_vps;

  CModelView m_mvActive;
  CModelViews m_mvs;

  CVec m_vRelPos;
  bool m_bPlot;
  double m_dPlotScaleFactor;

 public:
  CModelView m_mvOver;
  CModelView m_mvLast;

 public:
  CPegDoc* GetDocument() const;

  // Operations
 public:
  // Overrides
 public:
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

 protected:
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);

  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

  // Implementation
 public:
  virtual ~CPegView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

 protected:
  void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

 public:
  void BackgroundImageDisplay(CDC* pDC);
  CVec GetRelPos() const { return m_vRelPos; }
  bool GetViewTrueTypeFonts() const { return m_bViewTrueTypeFonts; }
  void DisplayOdometer();
  void DoCameraRotate(int iDir);

  /// @brief Parses a command string and posts keyboard messages to the view's window. Plain characters are posted as WM_CHAR messages; sequences of decimal digits enclosed in `{` and `}` are parsed and posted as WM_KEYDOWN with the numeric key code.
  /// @param customCommand Input sequence containing literal characters and optional key codes in braces (for example: `a{13}b`). Characters not inside braces are sent as WM_CHAR; digits immediately following `{` are parsed as a decimal key code and sent as WM_KEYDOWN. Empty brace groups generate no message.
  void DoCustomMouseClick(const std::string& customCommand) const;
  void DoWindowPan(double dRatio);
  void DoWindowPan0(int iDir);

  void ModelViewAdjustWnd(double dAspectRatio) { m_mvActive.AdjustWnd(dAspectRatio); }
  void ModelViewAdjustWnd(double&, double&, double&, double&, double);
  void ModelViewGetActive(CModelView& v) const { v = m_mvActive; }
  CVec ModelViewGetDirection() { return m_mvActive.GetDirection(); }
  CPnt ModelViewGetEye() { return m_mvActive.GetEye(); }
  double ModelViewGetFarClipDistance() { return m_mvActive.GetFarClipDistance(); }
  double ModelViewGetLensLength() { return m_mvActive.GetLensLength(); }
  CTMat ModelViewGetMatrix() { return m_mvActive.GetMatrix(); }
  CTMat ModelViewGetMatrixInverse() { return m_mvActive.GetMatrixInverse(); }
  double ModelViewGetNearClipDistance() { return m_mvActive.GetNearClipDistance(); }
  CPnt ModelViewGetTarget() { return m_mvActive.GetTarget(); }
  double ModelViewGetUExt() { return m_mvActive.GetUExt(); }
  double ModelViewGetUMax() { return m_mvActive.GetUMax(); }
  double ModelViewGetUMin() { return m_mvActive.GetUMin(); }
  double ModelViewGetVExt() { return m_mvActive.GetVExt(); }
  double ModelViewGetVMax() { return m_mvActive.GetVMax(); }
  double ModelViewGetVMin() { return m_mvActive.GetVMin(); }
  CVec ModelViewGetVwUp() { return m_mvActive.GetVwUp(); }
  void ModelViewInitialize() { m_mvActive.Initialize(GetAspectRatio()); }
  void ModelViewLookAt(const CPnt& ptEye, const CPnt& ptTarget, const CVec& vVwUp) {
    m_mvActive.LookAt(ptEye, ptTarget, vVwUp);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewPopActive();
  void ModelViewPushActive();
  void ModelViewTransform(CPnt4& pt) { m_mvActive.DoTransform(pt); }
  void ModelViewTransform(int iPts, CPnt4* pt) { m_mvActive.DoTransform(iPts, pt); }
  void ModelViewTransform(CPnt4s& pta) { m_mvActive.DoTransform(pta); }
  void ModelViewTransform(CVec4& v) { m_mvActive.DoTransform(v); }
  void ModelViewSetActive(const CModelView& v) {
    m_mvActive = v;
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetCenteredWnd(double dUExt = 0., double dVExt = 0.) {
    m_mvActive.SetCenteredWnd(GetAspectRatio(), dUExt, dVExt);
  }
  void ModelViewSetDirection(const CVec& v) {
    m_mvActive.SetDirection(v);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetEye(const CPnt& pt) {
    m_mvActive.SetEye(pt);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetEye(const CVec& v) {
    m_mvActive.SetEye(v);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetFarClipDistance(double d) {
    m_mvActive.SetFarClipDistance(d);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetLensLength(double d) {
    m_mvActive.SetLensLength(d);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetMatrix(const CTMat& tm) { m_mvActive.SetMatrix(tm); }
  void ModelViewSetNearClipDistance(double d) {
    m_mvActive.SetNearClipDistance(d);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetPerspectiveEnabled(bool b) {
    m_mvActive.SetPerspectiveEnabled(b);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetTarget(const CPnt& pt) {
    m_mvActive.SetTarget(pt);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetVwUp(const CVec& v) {
    m_mvActive.SetVwUp(v);
    m_mvActive.BuildTransformMatrix();
  }
  void ModelViewSetWnd(double dUMin, double dVMin, double dUMax, double dVMax) {
    m_mvActive.SetWnd(dUMin, dVMin, dUMax, dVMax);
  }

  /// @brief Determines the number of pages for 1 to 1 print
  UINT NumPages(CDC* conext, double scaleFactor, UINT& horizontalPages, UINT& verticalPages) const;

  CPnt OverGetTarget() const { return m_mvOver.GetTarget(); }
  double OverGetUExt() const { return m_mvOver.GetUExt(); }
  double OverGetUMin() const { return m_mvOver.GetUMin(); }
  double OverGetVExt() const { return m_mvOver.GetVExt(); }
  double OverGetVMin() const { return m_mvOver.GetVMin(); }

  CPoint DoProjection(const CPnt4& pt) { return m_vpActive.DoProjection(pt); }
  void DoProjection(CPoint* pnt, int iPts, CPnt4* pt) { m_vpActive.DoProjection(pnt, iPts, pt); }
  void DoProjection(CPoint* pnt, CPnt4s& pta) { m_vpActive.DoProjection(pnt, pta); }
  void DoProjectionInverse(CPnt& pt) { m_vpActive.DoProjectionInverse(pt); }
  double GetAspectRatio() { return m_vpActive.GetAspectRatio(); }
  double GetHeightInInches() { return m_vpActive.GetHeightInInches(); }
  double GetWidthInInches() { return m_vpActive.GetWidthInInches(); }
  void ViewportPopActive();
  void ViewportPushActive();
  void ViewportSet(int iWidth, int iHeight) { m_vpActive.Set(iWidth, iHeight); }
  void SetDeviceHeightInInches(double dHeight) { m_vpActive.SetDeviceHeightInInches(dHeight); }
  void SetDeviceHeightInPixels(int iHeight) { m_vpActive.SetDeviceHeightInPixels(iHeight); }
  void SetDeviceWidthInInches(double dWidth) { m_vpActive.SetDeviceWidthInInches(dWidth); }
  void SetDeviceWidthInPixels(int iWidth) { m_vpActive.SetDeviceWidthInPixels(iWidth); }

  // Generated message map functions
 protected:
 public:
  afx_msg void OnBackgroundImageLoad();
  afx_msg void OnBackgroundImageRemove();
  afx_msg void OnMouseMove(UINT flags, CPoint point);
  afx_msg void OnLButtonDown(UINT flags, CPoint point);

  /// @brief  It handles the release of the left mouse button, with special behavior if the Shift key is held down during the release.
  /// @param  flags  The flags associated with the mouse event.
  /// @param  point  The position of the mouse cursor at the time of the event.
  afx_msg void OnLButtonUp(UINT flags, CPoint point);
  afx_msg void OnRButtonDown(UINT flags, CPoint point);
  afx_msg void OnRButtonUp(UINT flags, CPoint point);
  afx_msg void OnSetFocus(CWnd* pWndOld);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSetupScale();
  afx_msg void On3dViewsAxonometric();
  afx_msg void On3dViewsBack();
  afx_msg void On3dViewsBottom();
  afx_msg void On3dViewsFront();
  afx_msg void On3dViewsIsometric();
  afx_msg void On3dViewsLeft();
  afx_msg void On3dViewsOblique();
  afx_msg void On3dViewsPerspective();
  afx_msg void On3dViewsRight();
  afx_msg void On3dViewsTop();
  afx_msg void OnViewBackgroundImage();
  afx_msg void OnViewTrueTypeFonts();
  afx_msg void OnViewPenWidths();
  afx_msg void OnViewOdometer();
  afx_msg void OnViewRefresh();
  afx_msg void OnViewZoom();
  afx_msg void OnViewWireframe();
  afx_msg void OnViewRendered();
  afx_msg void OnViewParameters();
  afx_msg void OnViewSolid();
  afx_msg void OnViewLighting();
  afx_msg void OnWindowNormal();
  afx_msg void OnWindowBest();
  afx_msg void OnWindowLast();
  afx_msg void OnWindowSheet();
  afx_msg void OnWindowZoomIn();
  afx_msg void OnWindowZoomOut();
  afx_msg void OnWindowPan();
  afx_msg void OnWindowPanLeft();
  afx_msg void OnWindowPanRight();
  afx_msg void OnWindowPanUp();
  afx_msg void OnWindowPanDown();
  afx_msg void OnCameraRotateLeft();
  afx_msg void OnCameraRotateRight();
  afx_msg void OnCameraRotateUp();
  afx_msg void OnCameraRotateDown();
  afx_msg void OnViewWindow();
  afx_msg void OnSetupDimLength();
  afx_msg void OnSetupDimAngle();
  afx_msg void OnSetupUnits();
  afx_msg void OnSetupConstraintsAxis();
  afx_msg void OnSetupConstraintsGrid();
  afx_msg void OnCursorDefault();
  afx_msg void OnCursorSymbol();
  afx_msg void OnCursorCrosshair();
  afx_msg void OnSetupMouseButtons();
  afx_msg void OnRelativeMovesEngDown();
  afx_msg void OnRelativeMovesEngDownRotate();
  afx_msg void OnRelativeMovesEngIn();
  afx_msg void OnRelativeMovesEngOut();
  afx_msg void OnRelativeMovesEngLeft();
  afx_msg void OnRelativeMovesEngLeftRotate();
  afx_msg void OnRelativeMovesEngRight();
  afx_msg void OnRelativeMovesEngRightRotate();
  afx_msg void OnRelativeMovesEngUp();
  afx_msg void OnRelativeMovesEngUpRotate();
  afx_msg void OnRelativeMovesRight();
  afx_msg void OnRelativeMovesUp();
  afx_msg void OnRelativeMovesLeft();
  afx_msg void OnRelativeMovesDown();
  afx_msg void OnRelativeMovesIn();
  afx_msg void OnRelativeMovesOut();
  afx_msg void OnRelativeMovesRightRotate();
  afx_msg void OnRelativeMovesUpRotate();
  afx_msg void OnRelativeMovesLeftRotate();
  afx_msg void OnRelativeMovesDownRotate();
  afx_msg void OnPrimSnapTo();
  afx_msg void OnPrimPerpJump();
  afx_msg void OnHelpKey();
  // Returns a pointer to the currently active view.
  static CPegView* GetActiveView(void);
  afx_msg void OnUpdateViewOdometer(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewTrueTypeFonts(CCmdUI* pCmdUI);

 protected:
  DECLARE_MESSAGE_MAP()
 public:
  afx_msg void OnUpdateViewBackgroundImage(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBackgroundimageLoad(CCmdUI* pCmdUI);
  afx_msg void OnUpdateBackgroundimageRemove(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewRendered(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewWireframe(CCmdUI* pCmdUI);
  afx_msg void OnUpdateViewPenwidths(CCmdUI* pCmdUI);
  afx_msg void OnFilePlotHalf();
  afx_msg void OnFilePlotFull();
  afx_msg void OnFilePlotQuarter();
  afx_msg void OnFilePrint();
};
#ifndef _DEBUG  // debug version in PegView.cpp
inline CPegDoc* CPegView::GetDocument() const { return reinterpret_cast<CPegDoc*>(m_pDocument); }
#endif
