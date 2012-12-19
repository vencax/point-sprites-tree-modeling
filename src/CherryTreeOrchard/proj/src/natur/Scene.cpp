

#include "../Stdafx.h"

#include <fstream>

#include <time.h>

#include "./Scene.h"

#define NUM_OF_TREES	10

CScene::CScene( LPDIRECT3DDEVICE9 d, TCam *cam) : 
m_device( d),
m_pos( D3DXVECTOR3( 0, 0, 0) ),
m_pCamera( cam)
{
	// rand generator
	srand( (unsigned)time( NULL ) );
	
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

	m_axis = NULL;
	m_ground = NULL;
	m_groundTexture = NULL;

	//////////////////////////////
	// create ground texture
	/////////////////////////////
	if( FAILED( D3DXCreateTextureFromFile( m_device, "./data/textures/grass.jpg", &m_groundTexture) ) )
	{
		MessageBox( 0, "Creation of ground texture failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	/////////////////////////////
	// FPS
	////////////////////////////
	m_FPS = 0.0f;

	//////////////////////////////
	// init path directory
	/////////////////////////////
	this->InitDirMap( );

	//////////////////////////////
	// add the trees
	/////////////////////////////
	this->AddTrees( );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

	if( m_axis)
	{
		m_axis->Release( );
		m_axis = NULL;
	}

	if( m_ground)
	{
		m_ground->Release( );
		m_ground = NULL;
	}

	if( m_groundTexture)
	{
		m_groundTexture->Release( );
		m_groundTexture = NULL;
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CScene::Draw( void)
{
	m_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(70,70,70), 
		1.0f, 0 );
    
	// Begin the scene
	m_device->BeginScene();

		// draw axis
		m_device->SetFVF( POINT_SPRITES_CUSTOMVERTEX_FVF);
		m_device->SetRenderState( D3DRS_LIGHTING, false);
		m_device->SetStreamSource( 0, m_axis, 0, sizeof( PS_VERTEX));
		m_device->DrawPrimitive( D3DPT_LINELIST, 0, 3);

		// draw ground
		m_device->SetTexture( 0, m_groundTexture);
		m_device->SetFVF( CUSTOM_BODY_VERTEX_FVF);
		//m_device->SetRenderState( D3DRS_LIGHTING, true);
		m_device->SetStreamSource( 0, m_ground, 0, sizeof( BODY_VERTEX));
		m_device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

		// draw the trees in the scene
		// draw all Kinds
		for( CScene::TKindMap::iterator it = this->m_kindMap.begin( );
			it != this->m_kindMap.end( );
			it++)
			it->second->Draw( );	
			 
	m_device->EndScene();
			 
			 
	// Present the backbuffer contents to the display
	m_device->Present( NULL, NULL, NULL, NULL );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
CScene::AddTree( string name, D3DXVECTOR3 &pos)
{
	// try to finf if rule c is already in map
	TKindMap::iterator it = m_kindMap.find( name);

	CKind* kind;

	////////////////////////////////////
	// get path to resources of tree named name from dirMap
	////////////////////////////////////
	TDirectoryMap::iterator path = m_dirMap.find( name);

	if( path == m_dirMap.end( ) )
		return false;

	if( it == m_kindMap.end( ) )		// there is not, so add
	{
		// create new CKind
		kind = new CKind( m_device, m_supportVB, path->second);

		// insert new rule into map
		m_kindMap.insert( TKindMap::value_type( name, kind) );
	}
	else // there already is, so try to add into array
	{
		kind = it->second;
	}

	// insert new tree into Kind
	kind->AddTree( pos, m_pCamera->GetPos( ));

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CScene::InitScene( void)
{
	/////////////////////////////
	// init render states
	/////////////////////////////
	m_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	//m_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
	//m_device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	//m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	//////////////////////////////
	// color key transparency
	//////////////////////////////
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	m_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	m_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	////////////////////////////////
	// init axes
	////////////////////////////////
	PS_VERTEX * pVert;
	//	create vertex buffer for drawing axis
	m_device->CreateVertexBuffer( sizeof( PS_VERTEX) * 6, D3DUSAGE_WRITEONLY, POINT_SPRITES_CUSTOMVERTEX_FVF, D3DPOOL_MANAGED, &m_axis, NULL);
	m_axis->Lock( 0, 0, ( void**)&pVert, 0);
	pVert[ 0].vect = D3DXVECTOR3( 100, 0, 0);
	pVert[ 0].dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0);
	pVert[ 1].vect = D3DXVECTOR3( 0, 0, 0);
	pVert[ 1].dwColor = D3DCOLOR_ARGB( 255, 255, 0, 0);
	pVert[ 2].vect = D3DXVECTOR3( 0, 100, 0);
	pVert[ 2].dwColor = D3DCOLOR_ARGB( 255, 0, 255, 0);
	pVert[ 3].vect = D3DXVECTOR3( 0, 0, 0);
	pVert[ 3].dwColor = D3DCOLOR_ARGB( 255, 0, 255, 0);
	pVert[ 4].vect = D3DXVECTOR3( 0, 0, 100);
	pVert[ 4].dwColor = D3DCOLOR_ARGB( 255, 0, 0, 255);
	pVert[ 5].vect = D3DXVECTOR3( 0, 0, 0);
	pVert[ 5].dwColor = D3DCOLOR_ARGB( 255, 0, 0, 255);
	m_axis->Unlock( );

	///////////////////////////////////////
	// create vertex buffer for ground
	///////////////////////////////////////	
	BODY_VERTEX * pgVert;
	m_device->CreateVertexBuffer( sizeof( BODY_VERTEX) * 4, D3DUSAGE_WRITEONLY, CUSTOM_BODY_VERTEX_FVF, D3DPOOL_MANAGED, &m_ground, NULL);
	m_ground->Lock( 0, 0, ( void**)&pgVert, 0);
	pgVert[ 0].normal = D3DXVECTOR3( 0, 1, 0);
	pgVert[ 0].pos = D3DXVECTOR3( -1000.f, 0, -1000);
	pgVert[ 0].tu = 0.0f;
	pgVert[ 0].tv = 0.0f;

	pgVert[ 1].normal = D3DXVECTOR3( 0, 1, 0);
	pgVert[ 1].pos = D3DXVECTOR3( 1000.f, 0, -1000);
	pgVert[ 1].tu = 100.0f;
	pgVert[ 1].tv = 0.0f;

	pgVert[ 2].normal = D3DXVECTOR3( 0, 1, 0);
	pgVert[ 2].pos = D3DXVECTOR3( -1000.f, 0, 1000);
	pgVert[ 2].tu = 0.0f;
	pgVert[ 2].tv = 100.0f;

	pgVert[ 3].normal = D3DXVECTOR3( 0, 1, 0);
	pgVert[ 3].pos = D3DXVECTOR3( 1000.f, 0, 1000);
	pgVert[ 3].tu = 100.0f;
	pgVert[ 3].tv = 100.0f;
	m_ground->Unlock( );

	// init lights
	SetupLights( );

	//	camera reset
	m_pCamera->Reset( );

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CScene::SetupLights( void)
{
	// Set up a material. 
    D3DMATERIAL9 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 0.4f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 0.2f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	mtrl.Specular.r = mtrl.Specular.b = mtrl.Specular.g  = mtrl.Specular.a = 1.0f;
	m_device->SetMaterial( &mtrl );
	
	
	// Set up 3 lights for even lighting
    D3DXVECTOR3 vecDir,v2,v3;
    D3DLIGHT9 light, l2, l3;
	
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = light.Specular.r  =1.0f;
    light.Diffuse.g  = light.Specular.g  =1.0f;
    light.Diffuse.b  = light.Specular.b  =1.0f;
    vecDir = D3DXVECTOR3(1.0f,
		1.0f,
		1.0f );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;
    m_device->SetLight( 0, &light );
    m_device->LightEnable( 0, TRUE );
	
	ZeroMemory( &l2, sizeof(D3DLIGHT9) );
    l2.Type       = D3DLIGHT_DIRECTIONAL;
    l2.Diffuse.r  = l2.Specular.r  =0.5f;
    l2.Diffuse.g  = l2.Specular.g  =0.5f;
    l2.Diffuse.b  = l2.Specular.b  =0.5f;
    v2 = D3DXVECTOR3(1.0f,
		0.0f,
		0.0f );
	
    D3DXVec3Normalize( (D3DXVECTOR3*)&l2.Direction, &v2 );
    l2.Range       = 1000.0f;
    m_device->SetLight( 1, &l2 );
    m_device->LightEnable( 1, TRUE );
	
	ZeroMemory( &l3, sizeof(D3DLIGHT9) );
    l3.Type       = D3DLIGHT_DIRECTIONAL;
    l3.Diffuse.r  = l3.Specular.r  =0.25f;
    l3.Diffuse.g  = l3.Specular.g  =0.25f;
    l3.Diffuse.b  = l3.Specular.b  =0.25f;
    v3 = D3DXVECTOR3(0.0f,
		1.0f,
		-0.5f );
	
    D3DXVec3Normalize( (D3DXVECTOR3*)&l3.Direction, &v3 );
    l3.Range       = 1000.0f;
    m_device->SetLight( 2, &l3 );
    m_device->LightEnable( 2, TRUE );
	
	// turn on a little ambient light
    m_device->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CScene::Update( void)
{
	if( m_kindMap.size( ) == 0)
		return;

	CKind *kind;
	for( TKindMap::iterator it = m_kindMap.begin( ); it != m_kindMap.end( );	it++)
	{
		kind = it->second;
		kind->Update( m_pCamera->GetPos( ) );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CScene::Animate( void)
{
	if( m_kindMap.size( ) == 0)
		return;

	CKind *kind;
	for( TKindMap::iterator it = m_kindMap.begin( ); it != m_kindMap.end( );	it++)
	{
		kind = it->second;
		kind->TickUpdate( m_pCamera->GetPos( ));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CScene::Starttiming()
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	QueryPerformanceCounter((LARGE_INTEGER *)&t1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CScene::Endtiming()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&t2);
	double secondsElapsed =(double)(t2-t1)/(double)(freq);
	
	// calculate FPS
	m_FPS = ( float)( 1 / secondsElapsed);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// reads entire file dir.dat, where are paths to each tree
bool
CScene::InitDirMap( void)
{
	ifstream f;

	string name, path;
	char tmp1[ 100], tmp2[ 100];

	// try to open the file
	f.open( "dir.dat");
	if( ! f.good() )
		return false;

	// read pairs
	f.getline( tmp1, 100);
	f.getline( tmp2, 100);

	while( ! f.fail() )
	{
		name = tmp1;
		path = tmp2;

		m_dirMap.insert( TDirectoryMap::value_type( name, path) );		

		f.getline( tmp1, 100);
		f.getline( tmp2, 100);
	}

	f.close( );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// add many trees into the scene to randomly generadet positions
void
CScene::AddTrees( void)
{
	D3DXVECTOR3 pos;

	srand( (unsigned)time( NULL ) );

	for( int i=0; i< NUM_OF_TREES; i++)
	{
		for( int j=0; j< NUM_OF_TREES; j++)
		{
			// prepare position
			//pos.x = ( float)( -1000 + ( rand( ) % 2000) );
			//pos.y = ( float)( -1000 + ( rand( ) % 2000) );
			pos.x = ( float)( ( i * 12) - ( NUM_OF_TREES / 2 * 12) );
			pos.z = ( float)( ( j * 12) - ( NUM_OF_TREES / 2 * 12) );
			pos.y = 0.0f;

			this->AddTree( "cherryTree", pos);
		}
	}
}