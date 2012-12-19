// pokView.h : interface of the CpokView class
//
#pragma once

#include "./Cinput.h"
#include "./CVert.h"
#include "./MyPropDiag.h"


class CpokView : public CView
{
private:
	// pointer to LSystem and NaturalObject got direst from document
	LSystem		*m_pLSys;
	CScene	**m_scene;
	
	bool m_bCameraIsMoving;
	bool m_bInitialized;
	bool m_bWiredFrame;
	bool m_bLightning;

protected: // create from serialization only
	CpokView();
	DECLARE_DYNCREATE(CpokView)

// Attributes
public:

	UINT_PTR m_uTimer;

	void Starttiming();
	void Endtiming();

	float FPS;
	float frameRenderTime;
	double milisecondsElapsed;
	unsigned int m_framesDrawn;
	__int64 t1, t2, freq;

	int counting;
// Operations
public:

// Overrides
	protected:


// Implementation
public:
	virtual ~CpokView();

	bool	InitDirectX( void);
	void	InitScene( void);
	bool	CheckDevice( void);
	void	SetupLights( void);

	// Generated message map functions
protected:
	afx_msg void OnDraw( CDC*);
	DECLARE_MESSAGE_MAP()

	LPDIRECT3DVERTEXBUFFER9		m_axis;	 // direct buffer for axis

public:
	
	CInput		*m_cInput;				//	input object. User input is controlled via DirectInput
	TCam		*m_cCam;				//	camera
	
	// properties dialog bar
	CMyPropDiag	*m_pPropDiag;
	CStatusBar m_statusBar;

	LPDIRECT3D9							m_directx;
	D3DPRESENT_PARAMETERS	m_presentParameters;
	LPDIRECT3DDEVICE9				m_device;

	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnInitialUpdate();
	afx_msg void OnApplyChanges();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void SetWireFrame( void);		// check box handler to set or unset wireFrame fillmode
	afx_msg void SetLightning( void);
	afx_msg void LoadTextures( void);


#ifndef _DEBUG  // debug version in pokView.cpp
inline CpokDoc* CpokView::GetDocument() const
   { return reinterpret_cast<CpokDoc*>(m_pDocument); }
#endif

};

