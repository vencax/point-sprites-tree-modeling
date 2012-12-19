// pok.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "pok.h"
#include "MyPropDiag.h"
#include "MainFrm.h"

#include "pokDoc.h"
#include "pokView.h"
#include ".\pok.h"
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CpokApp

BEGIN_MESSAGE_MAP(CpokApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CpokApp construction

CpokApp::CpokApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CpokApp object

CpokApp theApp;



// CpokApp initialization

BOOL CpokApp::InitInstance()
{
// InitCommonControls() is required on Windows XP if an application
// manifest specifies use of ComCtl32.dll version 6 or later to enable
// visual styles.  Otherwise, any window creation will fail.
InitCommonControls();

CWinApp::InitInstance();

// Initialize OLE libraries
if (!AfxOleInit())
{
	AfxMessageBox(IDP_OLE_INIT_FAILED);
	return FALSE;
}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CpokDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CpokView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdithelp();
	afx_msg void OnEnUpdateEdithelp();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_EN_CHANGE(IDC_EDITHELP, OnEnChangeEdithelp)
	ON_EN_UPDATE(IDC_EDITHELP, OnEnUpdateEdithelp)
END_MESSAGE_MAP()

// App command to run the dialog
void CpokApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CpokApp message handlers


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString c;				// string to store help

	// load help.dat containing help
	#define MAXL 1024 * 2
	ifstream f;
	char buf[ MAXL];

	f.open( "help.dat");

	if( ! f.good() )
		AfxMessageBox( _T("Opening help.dat failed!"), MB_OK | MB_ICONEXCLAMATION, NULL);

	while( ! f.fail() )
	{
		f.getline( buf, MAXL+1);
		c += buf;
		c+= "\r\n";
	}

	CEdit *e = ( CEdit *)GetDlgItem( IDC_EDITHELP);
	e->SetWindowText( c);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnEnChangeEdithelp()
{
	// do nothing
}

void CAboutDlg::OnEnUpdateEdithelp()
{
	AfxMessageBox( _T("Nehrab se mi v tom!"), MB_OK);
}
