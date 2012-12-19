// MyPropDiag.cpp : implementation file
//
#include <string>

#include "./Stdafx.h"
#include "../res/Resource.h"
#include "./MyPropDiag.h"
#include "./MyExcept.h"
#include ".\mypropdiag.h"

// CMyPropDiag

IMPLEMENT_DYNAMIC(CMyPropDiag, CDialogBar)

CMyPropDiag::CMyPropDiag( CWnd *view, CScene **scene)
	: m_pView( view)
	, m_pScene( scene)
{
	popedUp= false;
}

CMyPropDiag::~CMyPropDiag()
{
}


BEGIN_MESSAGE_MAP(CMyPropDiag, CDialogBar)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// CMyPropDiag message handlers

void CMyPropDiag::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSpinButtonCtrl* spin;
	
	// rec. level 0
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_RC0SPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{

		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			(*m_pScene)->GetLSystem( )->setRecursionLevel( spin->GetPos( ), 0);
			(*m_pScene)->GetLSystem( )->RecalculateActiveLvls( );

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update MemStatsWindow
			(*m_pScene)->UpdateMemStatsWin( m_memStats);

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// rec. level 1
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_RC1SPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			(*m_pScene)->GetLSystem( )->setRecursionLevel( spin->GetPos( ), 1);
			(*m_pScene)->GetLSystem( )->RecalculateActiveLvls( );

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

						// update MemStatsWindow
			(*m_pScene)->UpdateMemStatsWin( m_memStats);

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// rec. level 2
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_RC2SPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			(*m_pScene)->GetLSystem( )->setRecursionLevel( spin->GetPos( ), 2);
			(*m_pScene)->GetLSystem( )->RecalculateActiveLvls( );

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

						// update MemStatsWindow
			(*m_pScene)->UpdateMemStatsWin( m_memStats);

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// PS Size 0
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS0SPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			float size = ( float)spin->GetPos( ) / 10.0f;		// in dm = 1/10 m
			(*m_pScene)->GetLSystem( )->SetPSSize( 0, size);

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// PS Size 1
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS1SPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			float size = ( float)spin->GetPos( ) / 10.0f;
			(*m_pScene)->GetLSystem( )->SetPSSize( 1, size);

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// PS Size 2
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS2SPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			float size = ( float)spin->GetPos( ) / 10.0f;
			(*m_pScene)->GetLSystem( )->SetPSSize( 2, size);

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// PS Size 3
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS3SPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			float size = ( float)spin->GetPos( ) / 10.0f;
			(*m_pScene)->GetLSystem( )->SetPSSize( 3, size);

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// Def angle
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_DASPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			(*m_pScene)->GetLSystem( )->setDefaultAngle( ( char)spin->GetPos( ) );

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// Def lenght
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_DLSPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			(*m_pScene)->GetLSystem( )->SetDefaultLenght( ( char)spin->GetPos( ) );

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	// Def Thick
	spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_DTSPIN);
	if( pScrollBar == ( CScrollBar* )spin )
	{
		if( ! popedUp)
		{
			popedUp = ! popedUp;
			// change LSystem
			(*m_pScene)->GetLSystem( )->setDefaultThick( ( char)spin->GetPos( ) );

			// regenerate Natural Object
			(*m_pScene)->ResetTree( );
			(*m_pScene)->GenerateTree( );

			// update View
			m_pView->RedrawWindow( );
		}
		else
			popedUp = ! popedUp;

		return;
	}

	//CDialogBar::OnVScroll(nSBCode, nPos, pScrollBar);
}

//////////////////////////

void CMyPropDiag::SetControlsAccordingLSystem( bool loadFromLS)
{
	CSpinButtonCtrl *spin;

	if( loadFromLS)
	{

		// set RecLvl 0	
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_RC0SPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->getRecursionLevel( 0)) );
		spin->EnableWindow( true);

		// set RecLvl 1	
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_RC1SPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->getRecursionLevel( 1)) );
		spin->EnableWindow( true);

		// set RecLvl 2	
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_RC2SPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->getRecursionLevel( 2)) );
		spin->EnableWindow( true);




		// set PS Size Level  0	
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS0SPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->GetPSSizes( )[ 0] * 10) );
		spin->EnableWindow( true);

		// set PS Size Level  1	
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS1SPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->GetPSSizes( )[ 1] * 10) );
		spin->EnableWindow( true);

		// set PS Size Level  2	
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS2SPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->GetPSSizes( )[ 2] * 10) );
		spin->EnableWindow( true);

		// set PS Size Level  3
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_PS3SPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->GetPSSizes( )[ 3] * 10) );
		spin->EnableWindow( true);



		// def angle
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_DASPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->getDefaultAngle( ) ) );
		spin->EnableWindow( true);

		// def. lenght
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_DLSPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->GetDefaultLenght( ) ) );
		spin->EnableWindow( true);

		// def thick
		spin = ( CSpinButtonCtrl *)GetDlgItem( IDC_DTSPIN);
		spin->SetPos( (int)( (*m_pScene)->GetLSystem( )->getDefaultThick( ) ) );
		spin->EnableWindow( true);

		/////////////////////////////////////////////////////////////////////
		// edit boxes
		CEdit *edit = ( CEdit*)GetDlgItem( IDC_EDITAXIOM);
		edit->SetWindowText( (*m_pScene)->GetLSystem( )->getAxiom( ).c_str( ) );
		edit->EnableWindow( true);

		edit = ( CEdit*)GetDlgItem( IDC_EDITRULES);
		edit->SetWindowText( (*m_pScene)->GetLSystem( )->getRulesString( ).c_str( ) );
		edit->EnableWindow( true);
	}
	else
	{
		this->OnInit( );
	}

}

void CMyPropDiag::ApplyChanges( void)
{
	CString str;
	CWnd *w;

	// reset rules first
	(*m_pScene)->GetLSystem( )->ResetRules( );

	// set axiom
	w = GetDlgItem( IDC_EDITAXIOM);
	w->GetWindowText( str);
	if( str.IsEmpty( ))
	{
		AfxMessageBox( _T("Axiom cannot be empty"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	string	stri = str.GetBuffer( );
	(*m_pScene)->GetLSystem( )->setAxiom( stri );

	// set rules
	str.Empty( );
	this->GetDlgItemText( IDC_EDITRULES, str);
	if( ! (*m_pScene)->GetLSystem( )->LoadRulesFromText( ( char*)str.GetBuffer( ) ) )
	{
		AfxMessageBox( _T("Loading rules from text failed"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// generate Nat Obj
	(*m_pScene)->ResetTree(  );
	(*m_pScene)->GenerateTree(  );

				// update MemStatsWindow
			(*m_pScene)->UpdateMemStatsWin( m_memStats);
		
	// update the view
	m_pView->RedrawWindow( );
}


void CMyPropDiag::OnInit( void)
{
		CSpinButtonCtrl *spin;
	// set budies for sliders & spins
	// PS0
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_PS0SPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_PS0BUDDY));
	spin->SetBase( 10);
	spin->SetRange( 10, 1000);		// 10 dm - 1000 dm = 1 - 100 m
	spin->EnableWindow( false);

	// PS1
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_PS1SPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_PS1BUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 500);		// 1 dm - 500 dm = 0.1 - 50 m
	spin->EnableWindow( false);

	// PS2
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_PS2SPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_PS2BUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 200);		// 1 dm - 200 dm = 0.1 - 20 m
	spin->EnableWindow( false);

	// PS3
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_PS3SPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_PS3BUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 10);		// 0.1 - 1 m
	spin->EnableWindow( false);

	// RecLvl0
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_RC0SPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_RC0BUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 10);
	spin->EnableWindow( false);

	// RecLvl1
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_RC1SPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_RC1BUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 10);
	spin->EnableWindow( false);

	// RecLvl2
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_RC2SPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_RC2BUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 10);
	spin->EnableWindow( false);

	// def angle
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_DASPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_DABUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 180);
	spin->EnableWindow( false);

	// def lenght
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_DLSPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_DLBUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 10);			// 0.1 - 1 m
	spin->EnableWindow( false);

	// def thick
	spin = ( CSpinButtonCtrl *)this->GetDlgItem( IDC_DTSPIN);
	spin->SetBuddy( this->GetDlgItem( IDC_DTBUDDY));
	spin->SetBase( 10);
	spin->SetRange( 1, 100);		// 0.1 - 1 m
	spin->EnableWindow( false);
}
