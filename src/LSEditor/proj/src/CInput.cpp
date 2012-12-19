
#include "cinput.h"

CInput::CInput( )
{	
	m_dev = NULL;
	m_keyb = NULL;
	m_mouse = NULL;

	// initialize DIRECTINPUT8 object
	if(! InitDirectInput())
	{
		return;
	}
	else if(! InitKeyboard() || ! InitMouse())
	{
		if( m_dev != NULL)
		{
			m_dev->Release();
			m_dev = NULL;
		}
		return;
	}

	// update
	update( );
}

// -----------------------------------------
CInput::~CInput( )
{
	//		unallocate everything
	if(m_keyb != NULL)
	{
		m_keyb->Unacquire();
		m_keyb->Release();
		m_keyb = NULL;
	}

	if( m_mouse != NULL)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = NULL;
	}

	if( m_dev != NULL)
	{
		m_dev->Release();
		m_dev = NULL;
	}
}

// -----------------------------------------
bool CInput::InitDirectInput(void)
{
	if( FAILED( DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&m_dev,NULL)))
	{
		MessageBox( NULL,"DirectInput8Create() failed!","InitDirectInput()", MB_OK);
		return false;
	}

	return true;
}

// -----------------------------------------
bool CInput::InitKeyboard(void)
{
	if( FAILED( m_dev->CreateDevice( GUID_SysKeyboard, &m_keyb, NULL)))
	{
		MessageBox( NULL, "CreateDevice() failed!", "InitKeyboard()", MB_OK);
		return false;
	}

	if( FAILED( m_keyb->SetDataFormat( &c_dfDIKeyboard)))
	{
		MessageBox( NULL, "SetDataFormat() failed!", "InitKeyboard()", MB_OK);
		return false;
	}

	if( FAILED( m_keyb->SetCooperativeLevel( NULL, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		MessageBox( NULL, "SetCooperativeLevel() failed!", "InitKeyboard()", MB_OK);
		return false;
	}

	if( FAILED( m_keyb->Acquire()))
	{
		MessageBox( NULL, "Acquire() failed!", "InitKeyboard()", MB_OK);
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////
// because GetDeviceState retrieves relative state to the alst call so i call it 
// at the beginning of flyby mode

void	CInput::resetMouse( void)
{
	if(m_mouse->GetDeviceState(sizeof(m_mouseState),(LPVOID)&m_mouseState) == DIERR_INPUTLOST)
	{
		m_mouse->Acquire();
	}
}

// -----------------------------------------
bool	CInput::update( void)
{
	HRESULT hRet;
	if ( m_keyb )
		m_keyb->Acquire(); 
	hRet = m_keyb->GetDeviceState( sizeof( m_keyBuf),(LPVOID)&m_keyBuf);
	if( FAILED( hRet))
		return false;

	if(m_mouse->GetDeviceState(sizeof(m_mouseState),(LPVOID)&m_mouseState) == DIERR_INPUTLOST)
	{
		m_mouse->Acquire();
	}

	return true;
 }

// -----------------------------------------
bool CInput::InitMouse(void)
{
	DIDEVCAPS MouseCapabilities;

	if( FAILED( m_dev->CreateDevice( GUID_SysMouse, &m_mouse, NULL)))
	{
		MessageBox( NULL, "CreateDevice() failed!", "InitMouse()", MB_OK);
		return false;
	}

	if( FAILED( m_mouse->SetDataFormat( &c_dfDIMouse)))
	{
		MessageBox( NULL, "SetDataFormat() failed!", "InitMouse()", MB_OK);
		return false;
	}

	//m_mouse->SetProperty( 
	if( FAILED( m_mouse->SetCooperativeLevel( NULL, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		MessageBox( NULL, "SetCooperativeLevel() failed!", "InitMouse()", MB_OK);
		return false;
	}

	if( FAILED( m_mouse->Acquire()))
	{
		MessageBox( NULL, "Acquire() failed!", "InitMouse()", MB_OK);
		return false;
	}

	MouseCapabilities.dwSize = sizeof(MouseCapabilities);
	m_mouse->GetCapabilities( &MouseCapabilities);

	if( !( MouseCapabilities.dwFlags & DIDC_ATTACHED))
	{
		MessageBox( NULL, "Mouse is currently not attached!", "InitMouse()", MB_OK);
		return false;
	}

	return true;
}//

// -----------------------------------------
bool	CInput::isKeyPressed( int Key)
{
	if( m_keyBuf[Key] & 0x80)
	{
		return true;
	}

return false;
}
// -----------------------------------------