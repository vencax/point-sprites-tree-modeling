// pokDoc.cpp : implementation of the CpokDoc class
//

#include "./Stdafx.h"
#include "./Pok.h"
#include "./MyExcept.h"
#include "./PokDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CpokDoc

IMPLEMENT_DYNCREATE(CpokDoc, CDocument)

BEGIN_MESSAGE_MAP(CpokDoc, CDocument)
END_MESSAGE_MAP()


// CpokDoc construction/destruction

CpokDoc::CpokDoc()
{	
	m_pCam = NULL;
	m_pPropDIag = NULL;
	m_memDiag = NULL;
	m_pScene = NULL;
}

CpokDoc::~CpokDoc()
{
	if( m_pScene)
		delete m_pScene;
	if( m_memDiag)
		delete m_memDiag;
}

BOOL CpokDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CpokDoc serialization

void CpokDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//  storing code
		CString c = _T("#######  Output of LSystem editor program  ########\r\n#\r\n#\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c = _T("# recursion levels\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		for( int i= 0; i < NUM_LVLS; i++)
		{
			c.AppendFormat( "%i-", m_pScene->GetLSystem( )->getRecursionLevel( i) );
		}

		for( int i = 0; i < c.GetLength( )-1; i++)
			ar << c[ i];
			

		c.Empty( );
		c = _T("\r\n# default angle\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c.AppendFormat( "%.3f", m_pScene->GetLSystem( )->getDefaultAngle( ) );
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];	

		c.Empty( );
		c = _T("\r\n# default thickness\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c.AppendFormat( "%.3f", m_pScene->GetLSystem( )->getDefaultThick( ) );
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c = _T("\r\n# default lenght\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c.AppendFormat( "%.3f", m_pScene->GetLSystem( )->GetDefaultLenght( ) );
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c = _T("\r\n# PointSprites Sizes\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c.AppendFormat( "%.3f-%.3f-%.3f-%.3f",
			m_pScene->GetLSystem( )->GetPSSizes( )[ 0],
			m_pScene->GetLSystem( )->GetPSSizes( )[ 1],
			m_pScene->GetLSystem( )->GetPSSizes( )[ 2],
			m_pScene->GetLSystem( )->GetPSSizes( )[ 3]
			);
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c = _T("\r\n#\r\n# axiom:\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];		

		c.Empty( );
		c = m_pScene->GetLSystem( )->getAxiom( ).c_str( );
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];		
		

		c.Empty( );
		c = _T("\r\n# rules:\r\n");
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];

		c.Empty( );
		c = m_pScene->GetLSystem( )->getRulesString( ).c_str( );
		for( int i = 0; i < c.GetLength( ); i++)
			ar << c[ i];
	}
	else
	{
		// reset scene
		m_pScene->Reset( );

		// load the data in the buffer into the LSystem
		if( ! m_pScene->GetLSystem( )->Load( ( char*)( ar.m_strFileName.GetBuffer( ) ) ) )
		{
			AfxMessageBox( _T("Loading of LSystem failed") , MB_OK, 0);
			return;
		}

		// load lSys file name
		m_pScene->name = ar.GetFile( )->GetFilePath( ).GetBuffer( );
		
		// load values into DialogBar
		m_pPropDIag->SetControlsAccordingLSystem( true);

		if( ! m_pScene->GenerateTextures( ) )
		{
			AfxMessageBox( _T("Loading of Textures Failed") , MB_OK, 0);
			return;
		}
		
		// reset camera
		m_pCam->Reset( );

		// generate Nat Obj
		m_pScene->GenerateTree( );

		// update MemStatsWindow
		m_pScene->UpdateMemStatsWin( m_memDiag);
		
		// update the view
		this->UpdateAllViews( NULL);
	}
}




// CpokDoc diagnostics

#ifdef _DEBUG
void CpokDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CpokDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CpokDoc commands
