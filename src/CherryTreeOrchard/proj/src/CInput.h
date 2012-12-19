#pragma once

#include <dinput.h>

// class taking care of input
class CInput
{
private:
	LPDIRECTINPUT8 m_dev;
	LPDIRECTINPUTDEVICE8 m_keyb, m_mouse;

	char m_keyBuf[ 256];
	
	DIMOUSESTATE m_mouseState;

	bool m_sysPressed;
	
	//	private functions
	bool InitDirectInput( void);
	bool InitKeyboard( void);
	bool InitMouse( void);

public:
	CInput( );		// def ctor
	~CInput( );	

	bool	update( void);			
	bool	isKeyPressed( int);		// checks if key is pressed

	void	resetMouse( void);

	inline int		getXChange( )	{	return m_mouseState.lX;	}
	inline int		getYChange( )	{	return m_mouseState.lY;	}	
	
	//		used to avoid multiple actions when a key is pressed for a longer time
	//		to actions like increase recursionLevel ...
	inline bool	getSysPressed( ) {		return m_sysPressed;		}
	inline void	setSysPressed(	bool	state)	{		m_sysPressed = state;	}
};