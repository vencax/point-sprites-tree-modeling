#pragma once

#include <atlimage.h>
#include <string>

using namespace std;

// CTextureDialog dialog

class CTextureDialog : public CDialog
{
	DECLARE_DYNAMIC(CTextureDialog)

public:
	CTextureDialog(CWnd* pParent = NULL, string* flnms = NULL);   // standard constructor
	virtual ~CTextureDialog();

// Dialog Data
	enum { IDD = IDD_TEXTURES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStatictex1();

private:
	string *m_aFilenames;

	CImage m_aImages[ NUMBER_OF_PS_TEXTURES];

	void LoadData( void);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnStnClickedStatictex2();
	afx_msg void OnStnClickedStatictex3();
	afx_msg void OnStnClickedStatictex4();
	afx_msg void OnStnClickedStatictex5();
	afx_msg void OnStnClickedStatictex6();
	afx_msg void OnStnClickedStatictex7();
	afx_msg void OnStnClickedStatictex8();
	afx_msg void OnStnClickedStatictex9();
	afx_msg void OnStnClickedStatictex10();
};
