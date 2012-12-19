#include "./stdafx.h"
#include "./Graphics.h"

Graphics::Graphics(void)
{
	m_pD3D = NULL;
	m_pDevice = NULL;;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

Graphics::~Graphics(void)
{
	if( m_pDevice)
	{
		m_pDevice->Release( );
		m_pDevice = NULL;
	}

	if( m_pD3D)
	{
		m_pD3D->Release( );
		m_pD3D = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT
Graphics::Create(
       HWND hwnd,
       int width,
       int height,
       bool windowed
	   )
{
	//create Direct3D object
	if( NULL == ( m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) )
	{
		MessageBox( NULL,"D3D creation failed!","InitD3D",MB_OK);
		E_FAIL;
	}

	// Get the current desktop display mode
	D3DDISPLAYMODE dispMode;
    if( FAILED( m_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &dispMode )))
	{
		MessageBox( NULL,"Get display mode failed!","InitD3D",MB_OK);
		E_FAIL;
	}
	
    // Set up the structure used to create the D3DDevice
    ZeroMemory( &m_presentParameters, sizeof( D3DPRESENT_PARAMETERS) );
    m_presentParameters.Windowed = windowed;
    m_presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_presentParameters.EnableAutoDepthStencil = TRUE;
	m_presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	m_presentParameters.BackBufferFormat = dispMode.Format;

	if(FAILED( m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_presentParameters, &m_pDevice)))
	{
		MessageBox( NULL,"Creation of device failed!","InitD3D",MB_OK);
		E_FAIL;
	}

	return S_OK;
}

// --------------------------------------------------------------
// check for lost device
// returns S_OK if OK, D3DERR_DEVICENOTRESET if device need to be reset or E_FAIL
// --------------------------------------------------------------
HRESULT Graphics::CheckDevice(void)
{
	HRESULT res= m_pDevice->TestCooperativeLevel();
	switch( res)
	{
	case D3DERR_DEVICELOST: 
		return E_FAIL;
	case D3DERR_DEVICENOTRESET:
		if( FAILED( m_pDevice->Reset(&m_presentParameters)))
		{
			MessageBox( NULL,"Reset() failed!","CheckDevice()",MB_OK);
			return E_FAIL;
		}
		break;
	}
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Graphics::Resize( int width, int height)
{
	if( m_pDevice)
	{
		// init projection matrix
		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH( &proj, 0.785398f, ( float)width / ( float)height, 1.0f, 2000.0f);
		m_pDevice->SetTransform ( D3DTS_PROJECTION, &proj);
	}
}