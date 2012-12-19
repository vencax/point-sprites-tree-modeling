#include "../Stdafx.h"
#include "../../res/resource.h"

#include <direct.h>			// to retrieve cur dir

#include "./Scene.h"

CScene::CScene( LPDIRECT3DDEVICE9 d, TCam *cam) : 
m_device( d),
m_pos( D3DXVECTOR3( 0, 0, 0) ),
m_pCamera( cam)
{
			// retrieve current directory
	char buffer[_MAX_PATH];   
	if( _getcwd( buffer, _MAX_PATH ) == NULL )
	{
		MessageBox( 0, "Cant get current Dirr", "ERROR", MB_OK);
		return;
	}
	currentDirectory = buffer;

	// create support vertex buffer with one vertex( 0, 0, 0) within
	// to draw simplified branches in all Kinds
	if( FAILED( m_device->CreateVertexBuffer( sizeof( D3DXVECTOR3), D3DUSAGE_WRITEONLY, CLEAR_POINT_SPRITES_FVF, D3DPOOL_MANAGED, &m_supportVB, NULL) ) )
	{
		MessageBox( 0, "Creation of support VertexBuffer failed", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	// copy 1 D3DXVECTOR3( 0, 0, 0)
	D3DXVECTOR3* PSArray;
	m_supportVB->Lock( 0, 0, ( void**)&PSArray, 0);
	PSArray[ 0] = D3DXVECTOR3( 0, 0, 0);
	m_supportVB->Unlock( );

	
	CKind *k = new CKind( m_device, m_supportVB, currentDirectory);
	string s = "jojojo";
	m_kindMap.insert( TKindMap::value_type( s, k) );		
}

CScene::~CScene( void)
{
	// delete all Kinds
	for( CScene::TKindMap::iterator it = this->m_kindMap.begin( );
		it != this->m_kindMap.end( );
		it++)
		delete it->second;
	// and clear the map
	m_kindMap.clear( );

	// release supportVB
	if( m_supportVB)
	{
		m_supportVB->Release( );
		m_supportVB = NULL;
	}
}

void
CScene::Draw( bool light)
{
	// draw all Kinds
	for( CScene::TKindMap::iterator it = this->m_kindMap.begin( );
		it != this->m_kindMap.end( );
		it++)
		it->second->Draw( light);	
}

void
CScene::AddTree( string name, D3DXVECTOR3 &pos)
{
	// try to finf if rule c is already in map
	TKindMap::iterator it = m_kindMap.find( name);

	CKind* kind;

	if( it == m_kindMap.end( ) )		// there is not, so add
	{
		// create new CKind
		kind = new CKind( m_device, m_supportVB, currentDirectory);

		// insert new rule into map
		m_kindMap.insert( TKindMap::value_type( name, kind) );
	}
	else // there already is, so try to add into array
	{
		kind = it->second;
	}

	// insert new tree into Kind
	kind->AddTree( pos, m_pCamera->GetPos( ));
}

/////////////////////////////////////////////////////////////////////////////////////
// Redirections to one songle Kind
/////////////////////////////////////////////////////////////////////////////////////
void
CScene::GenerateTree( )
{
	if( m_kindMap.size( ) == 0)
		return;

	CKind *kind = m_kindMap.begin( )->second;
	kind->AddTree( m_pos, m_pCamera->GetPos( ) );
}

void
CScene::Update( void)
{
	if( m_kindMap.size( ) == 0)
		return;

	CKind *kind = m_kindMap.begin( )->second;
	kind->Update( m_pCamera->GetPos( ) );
}

int
CScene::GetTreeDist( void)
{
	if( m_kindMap.size( ) == 0)
		return 0;

	CKind *kind = m_kindMap.begin( )->second;
	CNO *tree = kind->GetTree( );
	if( ! tree)
		return 0;

	D3DXVECTOR3 treePos = tree->GetPosition( );

	return ( int)D3DXVec3Length( &( treePos - m_pCamera->GetPos( ) ) );
}

void
CScene::Animate( void)
{
	if( m_kindMap.size( ) == 0)
		return;

	CKind *kind = m_kindMap.begin( )->second;
	kind->TickUpdate( m_pCamera->GetPos( ));
}

void
CScene::ResetTree( )
{
	if( m_kindMap.size( ) == 0)
		return;
	CKind *kind = m_kindMap.begin( )->second;
	kind->DeleteTree( );
}

LSystem *
CScene::GetLSystem( void)
{

	if( m_kindMap.size( ) == 0)
		return NULL;

	CKind *kind = m_kindMap.begin( )->second;
	return kind->GetLSystem( );
}

bool
CScene::GenerateTextures( void)
{
	CKind *kind = m_kindMap.begin( )->second;
	return kind->GenerateTextures( );
}
string *
CScene::GetFileNames( void)
{
	CKind *kind = m_kindMap.begin( )->second;
	return kind->GetFileNames( );
}

void
CScene::Reset( void)
{
	CKind *kind = m_kindMap.begin( )->second;

	delete kind;
	kind = new CKind( m_device, m_supportVB, currentDirectory);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CScene::UpdateMemStatsWin( CWnd *w)
{
	char buf[ 1024];
	char pom[3][ 32];
	
	CKind *kind = m_kindMap.begin( )->second;
	MyStats stats = kind->GetTree( )->stats;
	CWnd *c;

	// system mem
	ToRightUnits( stats.totalSysData, pom[0]);
	ToRightUnits( stats.PSNodesData, pom[1]);
	ToRightUnits( stats.BodyNodesData, pom[2]);

	c= w->GetDlgItem( IDC_SYSMEM);
	sprintf( buf, "Total: %s;  PSNodes: %s;  BodyNodes: %s", pom[0], pom[1], pom[2]);
	c->SetWindowText( buf);

	// GPU mem
	ToRightUnits( stats.totalGPUData, pom[0]);
	ToRightUnits( stats.PSData, pom[1]);
	ToRightUnits( stats.bodyData, pom[2]);

	c= w->GetDlgItem( IDC_GPUMEM);
	sprintf( buf, "Total: %s;  PSData( %i PSs): %s;  BodyData( %i Vertices): %s", pom[0], stats.numOfPS, pom[1], stats.numOfVertices, pom[2]);
	c->SetWindowText( buf);
	
}

void
CScene::ToRightUnits( unsigned int what, char *out)
{
	float f;
	char *unit = "B";
	if( what < 1024)
	{
		sprintf( out, "%i %s", what, unit);
		return;
	}

	unit = "kB";
	f = ( float)what / 1024.0f;
	if( f < 1024.0f)
	{
		sprintf( out, "%.3f %s", f, unit);
		return;
	}

	unit = "MB";
	f = f / 1024.0f;
	if( f < 1024.0f)
	{
		sprintf( out, "%.3f %s", f, unit);
		return;
	}

}