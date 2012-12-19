#pragma once

#include "./natur/Scene.h"
#include "./natur/LSystem.h"

// CMyPropDiag
class CMyPropDiag : public CDialogBar
{
	DECLARE_DYNAMIC(CMyPropDiag)

public:
	CMyPropDiag( CWnd *, CScene **);
	virtual ~CMyPropDiag();

	void SetControlsAccordingLSystem( bool);

	//  fired when user clickes the button 'Apply changes'
	void ApplyChanges( void);
	void AttachMemDiag( CWnd *w)	{	m_memStats = w;}

protected:
	DECLARE_MESSAGE_MAP()

private:
	CWnd *m_pView;
	CWnd *m_memStats;
	CScene **m_pScene;


	bool popedUp;
	void OnInit( void);

public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
};


