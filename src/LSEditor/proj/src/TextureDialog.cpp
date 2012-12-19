// TextureDialog.cpp : implementation file
//

#include "./Stdafx.h"
#include "./Pok.h"
#include "./natur/Branch.h"

#include "./Texturedialog.h"


// CTextureDialog dialog

IMPLEMENT_DYNAMIC(CTextureDialog, CDialog)
CTextureDialog::CTextureDialog(CWnd* pParent /*=NULL*/, string *fls)
	: CDialog(CTextureDialog::IDD, pParent)
	, m_aFilenames( fls)
{
}

CTextureDialog::~CTextureDialog()
{
}

void CTextureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTextureDialog, CDialog)
	ON_STN_CLICKED(IDC_STATICTEX1, OnStnClickedStatictex1)
	ON_STN_CLICKED(IDC_STATICTEX2, OnStnClickedStatictex2)
	ON_STN_CLICKED(IDC_STATICTEX3, OnStnClickedStatictex3)
	ON_STN_CLICKED(IDC_STATICTEX4, OnStnClickedStatictex4)
	ON_STN_CLICKED(IDC_STATICTEX5, OnStnClickedStatictex5)
	ON_STN_CLICKED(IDC_STATICTEX6, OnStnClickedStatictex6)
	ON_STN_CLICKED(IDC_STATICTEX7, OnStnClickedStatictex7)
	ON_STN_CLICKED(IDC_STATICTEX8, OnStnClickedStatictex8)
	ON_STN_CLICKED(IDC_STATICTEX9, OnStnClickedStatictex9)
	ON_STN_CLICKED(IDC_STATICTEX10, OnStnClickedStatictex10)
END_MESSAGE_MAP()


// CTextureDialog message handlers

void CTextureDialog::OnStnClickedStatictex1()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 0].Destroy( );
		if( S_OK != m_aImages[ 0].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX1);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 0]);
		m_aFilenames[ 0] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::LoadData( void)
{
	CStatic *frame;

	for( int i = 0; i < NUMBER_OF_PS_TEXTURES; i++)
	{				
		if( S_OK != m_aImages[ i].Load( m_aFilenames[ i].c_str( ) ) )
			return;
		frame = ( CStatic*)GetDlgItem( IDC_STATICTEX1+i);
		frame->SetBitmap( ( HBITMAP)m_aImages[ i]);
	}
}


BOOL CTextureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// load data from document 
	LoadData( );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTextureDialog::OnStnClickedStatictex2()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 1].Destroy( );
		if( S_OK != m_aImages[ 1].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX2);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 1]);
		m_aFilenames[ 1] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex3()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 2].Destroy( );
		if( S_OK != m_aImages[ 2].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX3);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 2]);
		m_aFilenames[ 2] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex4()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 3].Destroy( );
		if( S_OK != m_aImages[ 3].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX4);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 3]);
		m_aFilenames[ 3] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex5()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 4].Destroy( );
		if( S_OK != m_aImages[ 4].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX5);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 4]);
		m_aFilenames[ 4] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex6()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 5].Destroy( );
		if( S_OK != m_aImages[ 5].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX6);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 5]);
		m_aFilenames[ 5] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex7()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 6].Destroy( );
		if( S_OK != m_aImages[ 6].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX7);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 6]);
		m_aFilenames[ 6] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex8()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 7].Destroy( );
		if( S_OK != m_aImages[ 7].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX8);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 7]);
		m_aFilenames[ 7] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex9()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 8].Destroy( );
		if( S_OK != m_aImages[ 8].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX9);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 8]);
		m_aFilenames[ 8] = diag.GetPathName( ).GetBuffer( );
	}
}

void CTextureDialog::OnStnClickedStatictex10()
{
	CFileDialog diag( 0, 0, 0, 0, _T("Bitmaps (*.bmp)|*.bmp|PNGs (*.png)|*.png|JPEGs(*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||"), this, 0);
	if( diag.DoModal( ) == IDOK)
	{
		m_aImages[ 9].Destroy( );
		if( S_OK != m_aImages[ 9].Load( diag.GetFileName( ) ) )
			return;

		CStatic *frame = ( CStatic*)GetDlgItem( IDC_STATICTEX10);
		frame->SetBitmap( ( HBITMAP)m_aImages[ 9]);
		m_aFilenames[ 9] = diag.GetPathName( ).GetBuffer( );
	}
}
