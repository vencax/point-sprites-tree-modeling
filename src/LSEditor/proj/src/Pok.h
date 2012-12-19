// pok.h : main header file for the pok application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "../res/Resource.h"       // main symbols


// CpokApp:
// See pok.cpp for the implementation of this class
//

class CpokApp : public CWinApp
{
public:
	CpokApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CpokApp theApp;