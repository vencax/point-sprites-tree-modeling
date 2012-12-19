// pokView.cpp : implementation of the CpokView class
//

#include "./Stdafx.h"
#include "./Pok.h"


#include "./Graphics.h"
#include "./PokDoc.h"
#include "./PokView.h"

#include <Mmsystem.h>
#include "./TextureDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FPSTIMQUANTUM 5

	static UINT indicators[] =
{	
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	AFX_IDS_IDLEMESSAGE,
};

// CpokView

IMPLEMENT_DYNCREATE(CpokView, CView)

BEGIN_MESSAGE_MAP(CpokView, CView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_BN_CLICKED( IDOK, OnApplyChanges)
	ON_BN_CLICKED( IDC_CHECKWIREFRAME, SetWireFrame)
	ON_BN_CLICKED( IDC_CHECKLIGHTNING, SetLightning)
	
	ON_BN_CLICKED( IDC_BUTTONLOADTEXTURES, LoadTextures)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CpokView::CpokView()
{

	//init member variables
	m_directx			= NULL;
	m_device			= NULL;	

	m_cInput			= NULL;
	m_cCam			= NULL;			

	m_pPropDiag		= NULL;

	m_bCameraIsMoving = false;
	m_bInitialized = false;
	m_bWiredFrame = false;
	m_bLightning = true;
	
	milisecondsElapsed = 0;
	FPS = 0.0f;
	frameRenderTime = 0.0;
	m_framesDrawn = 0;

	counting = 0;
}

CpokView::~CpokView()
{
	//	cleenup everithing
	if( m_cCam)
		delete m_cCam;
	if( m_cInput)
		delete m_cInput;	

	m_axis->Release( );

	if( m_device)
	{
		m_device->Release( );
		m_device = NULL;
	}

	if( m_directx)
	{
		m_directx->Release();
		m_directx = NULL;
	}

	if( m_pPropDiag)
		delete m_pPropDiag;

}



// CChildView message handlers

void CpokView::OnDraw( CDC*) 
{	
	
	// start FPS timer
	Starttiming( );

	m_device->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(70,70,70), 
		1.0f, 0 );
    
	// Begin the scene
	m_device->BeginScene();

			// draw axis
			m_device->SetFVF( POINT_SPRITES_CUSTOMVERTEX_FVF);
			m_device->SetRenderState( D3DRS_LIGHTING, false);
			m_device->SetStreamSource( 0, m_axis, 0, sizeof( PS_VERTEX));
			if( FAILED( m_device->DrawPrimitive( D3DPT_LINELIST, 0, 3) ) )
			{
				return;
			}

			// draw the Natural Object
			(*m_scene)->Draw( m_bLightning);
			 
	m_device->EndScene();

	m_framesDrawn++;

	// stop the timer
	Endtiming( );	
			 
	// Present the backbuffer contents to the display
	m_device->Present( NULL, NULL, NULL, NULL );
	
	// Do not call CWnd::OnPaint() for painting messages
}

// --------------------------------------------------------------
//	create Direct3D object and device
// --------------------------------------------------------------

bool	CpokView::InitDirectX( void)
{
	//create Direct3D object
	if((m_directx = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		return false;
	}

	// Get the current desktop display mode
	D3DDISPLAYMODE dispMode;
    if( FAILED( m_directx->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &dispMode )))
	{
		return false;
	}
	
    // Set up the structure used to create the D3DDevice
    ZeroMemory( &m_presentParameters, sizeof(m_presentParameters) );
    m_presentParameters.Windowed = TRUE;
    m_presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_presentParameters.EnableAutoDepthStencil = TRUE;
	m_presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	m_presentParameters.BackBufferFormat = dispMode.Format;

	if(FAILED( m_directx->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->GetSafeHwnd( ), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_presentParameters, &m_device)))
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------
// set projection matrix, render states and texture stage
// --------------------------------------------------------------
void CpokView::InitScene(void)
{
	// init projection matrix	
	D3DXMATRIX					m_matProjection;
	CRect rect;
	this->GetWindowRect( &rect);
	D3DXMatrixPerspectiveFovLH( &m_matProjection, 0.785398f, 800.0f / 600.0f, 1.0f, 1000.0f);
	m_device->SetTransform(D3DTS_PROJECTION,&m_matProjection);

	m_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	//m_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
	//m_device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	//m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );


	// color key transparency
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	m_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	m_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);


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

	// constructions
	m_cInput		= new CInput( );
	m_cCam		= new TCam( m_device, m_cInput);					

	// init lights
	SetupLights( );

	//	camera reset
	m_cCam->Reset( );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------
// check for lost device
// --------------------------------------------------------------
bool CpokView::CheckDevice(void)
{
switch(m_device->TestCooperativeLevel())
 {
 case D3DERR_DEVICELOST: return false;
 case D3DERR_DEVICENOTRESET:
  {
  if( FAILED( m_device->Reset(&m_presentParameters)))
   {
   MessageBox( _T("Reset() failed!"), _T("CheckDevice()"), MB_OK);
   return false;
   }

  InitScene();

  return true;
  }
 default: return true;
 }
}

///////////////////////////////////////////////////////////////////////////////////

void	CpokView::SetupLights( void)
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


///////////////////////////////////////////////////////////////////////////////////


void CpokView::OnTimer(UINT nIDEvent)
{	
	
	// update input and camera
	m_cInput->update( );
	m_cCam->Update( );

	if( counting > 5)
	{
		( *m_scene)->Update( );
		counting = 0;
	}

	// do animation
	( *m_scene)->Animate( );





	// recall draw function of this window
	RedrawWindow( );

	counting ++;

	CView::OnTimer(nIDEvent);
}


///////////////////////////////////////////////////////////////////////////////////


BOOL CpokView::OnEraseBkgnd(CDC* pDC)
{
	// do nothing
	return true;
}


///////////////////////////////////////////////////////////////////////////////////


void CpokView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// do the initialization only at the begining
	if( !m_bInitialized)
	{
		CpokDoc* doc = ( CpokDoc*)GetDocument();

		// create directX
		if( ! InitDirectX( ))		
			return;

		// init the scene
		this->InitScene( );

		// attach Camera and device
		doc->AttachCam( m_cCam);
		doc->AttachDev( m_device);

		// set pointers to LSystem and NaturalObject
		m_pLSys		=	doc->m_pScene->GetLSystem( );
		m_scene	=		&doc->m_pScene;

		// create statusbar
		if (!m_statusBar.Create(AfxGetApp( )->GetMainWnd( )) ||
			!m_statusBar.SetIndicators(indicators,
			sizeof(indicators)/sizeof(UINT)))
		{
			TRACE0("Failed to create status bar\n");
			return;      // fail to create
		}

		// create properties dialog
		m_pPropDiag = new CMyPropDiag( this, &doc->m_pScene);
		if( m_pPropDiag->Create( AfxGetApp( )->GetMainWnd( ), IDD_PROPERTIES, CBRS_LEFT, 1111) == false)
		{
			TRACE0("Failed to create properties dialog bar\n");
			return;      // fail to create
		}

		// init dialog properties
		// init Controls
		m_pPropDiag ->SetControlsAccordingLSystem( false);

		// attach DialogBar to Document
		doc->AttachDaig( m_pPropDiag);

		// create memory diag
		CDialogBar *memDiag = new CDialogBar( );
		if( memDiag->Create( AfxGetApp( )->GetMainWnd( ), IDD_MEMSTATS, CBRS_BOTTOM, 1111) == false)
		{
			TRACE0("Failed to create properties dialog bar\n");
			return;      // fail to create
		}
		doc->AttachMemDiag( ( CWnd*)memDiag);

		// attach mem dialog to properties dialog
		m_pPropDiag->AttachMemDiag( ( CWnd*)memDiag);

		// set m_bInitialized to true
		m_bInitialized = true;
	}
}


///////////////////////////////////////////////////////////////////////////////////


void CpokView::OnApplyChanges()
{
	// apply changes made in edit boxes to lsystem and regenerate the lsystem
	m_pPropDiag->ApplyChanges( );
}

void CpokView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_SPACE)
	{
		if( m_bCameraIsMoving)
		{
			// set flag to false
			m_bCameraIsMoving = false;

			//change caption in status bar
			//CWnd *button = m_pPropDiag->GetDlgItem( IDC_STARTCAMERA);
			//button->SetWindowText( "StartCamera");

			// stop camera 
			KillTimer( m_uTimer);

		} else {
			// set flag to true
			m_bCameraIsMoving = true;

			// reset input
			m_cInput->resetMouse( );

			//change caption in status bar
			//CWnd *button = m_pPropDiag->GetDlgItem( IDC_STARTCAMERA);
			//button->SetWindowText( "StopCamera");

			// start camera 
			m_uTimer = SetTimer( 1, 50, 0);
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CpokView::SetWireFrame( void)
{
	if( m_bWiredFrame)
	{
		m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_bWiredFrame = ! m_bWiredFrame;		
	} else {
		m_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		m_bWiredFrame = ! m_bWiredFrame;
	}
	this->RedrawWindow( );
}

// message handler to SetLightning button
void CpokView::SetLightning( void)
{
	m_bLightning = ! m_bLightning;
	this->RedrawWindow( );
}


// message handler to LoadTextures button
void CpokView::LoadTextures( void)
{
	CTextureDialog diag( this, ( ( CpokDoc*)GetDocument( ))->m_pScene->GetFileNames( ) );
	if( IDOK == diag.DoModal( ))
	{
		( ( CpokDoc*)GetDocument())->m_pScene->GenerateTextures( );
	}
}

void CpokView::Starttiming()
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	QueryPerformanceCounter((LARGE_INTEGER *)&t1);
}

void CpokView::Endtiming()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&t2);
	milisecondsElapsed +=(double)(t2-t1)/(double)(freq);
	
		frameRenderTime = ( float)milisecondsElapsed;
		// calculate FPS
		FPS = 1 / frameRenderTime;
		// reset
		milisecondsElapsed = 0;
		m_framesDrawn = 0;

		// set statusbar
		char s[ 64];
		sprintf( s, "FPS: %.1f, actual render time: %.3f s, dist: %d", FPS, frameRenderTime, ( *m_scene)->GetTreeDist( ));
		m_statusBar.SetPaneText( 0, s, 1);

}
