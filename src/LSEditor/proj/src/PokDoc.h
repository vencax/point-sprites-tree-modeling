// pokDoc.h : interface of the CpokDoc class
//
#pragma once

// neccessary inclusions
#include "./MyPropDiag.h"
#include "./Camera.h"

class CpokDoc : public CDocument
{
protected: // create from serialization only
	CpokDoc();
	DECLARE_DYNCREATE(CpokDoc)

private:
	//	3D device 
	LPDIRECT3DDEVICE9		m_pDev;
	// camera
	TCam	*m_pCam;
	// pointer to MyPropDiag
	CMyPropDiag *m_pPropDIag;
	CWnd *m_memDiag;


// Attributes
public:		

	CScene	*m_pScene;

	// attach functions
	inline	void	AttachCam( TCam * c)	{	m_pCam = c;		}
	inline void	AttachDev( LPDIRECT3DDEVICE9 d)	{ m_pDev = d; m_pScene = new CScene( d, m_pCam);	}
	inline void  AttachDaig( CMyPropDiag* d)		{	m_pPropDIag = d;	}
	inline void	AttachMemDiag( CWnd *w)	{	m_memDiag = w;	}

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CpokDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


