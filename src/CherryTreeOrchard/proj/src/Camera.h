#pragma once

#include "./CInput.h"

class TCam
{
	public:
	TCam( LPDIRECT3DDEVICE9 d, CInput* i) : m_pDevice( d), m_pInput( i) {	Reset( );	};	//  constructor

	// methods
	void Reset( );				// resets the camera and sets the device
	void Update( );			// updates attribs of camera and sets the device

public:	inline D3DXVECTOR3 &GetPos( void)	{	return m_pos;	}

private:
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_lookAt;
	D3DXVECTOR3 m_up;

	float		m_speed;			// degree/pixel

	CInput							*m_pInput;
	LPDIRECT3DDEVICE9	m_pDevice;
};