
#include <d3dx9.h>	// DirectX9
#include "./CInput.h"
#include "./Camera.h"

// -------------------------------------
void TCam::Reset( )
{
	m_speed = .75f;

	m_pos = D3DXVECTOR3( 200.0f, 0.0f, 0.0f);
	m_lookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f);

	m_up = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
	Update( );
}

// -------------------------------------
void TCam::Update( )
{	
	
	D3DXVECTOR3 vDirection;
	bool computed = false;

//	solve moving
	//	get normalized direction vector
	D3DXVec3Normalize(&vDirection,&(m_lookAt - m_pos));

	// forward
	if( m_pInput->isKeyPressed(DIK_W))
	{	
		m_pos += vDirection * m_speed;
		m_lookAt += vDirection * m_speed;
	}

	// backward
	if( m_pInput->isKeyPressed(DIK_S))
	{
		m_pos -= vDirection * m_speed;
		m_lookAt -= vDirection * m_speed;
	}

	// left
	if( m_pInput->isKeyPressed(DIK_A))
	{
		// compute cross product of direction and up vector
		D3DXVec3Cross(&vDirection,&vDirection,&m_up);
		D3DXVec3Normalize(&vDirection,&vDirection);
		computed = true;
	
		m_pos += vDirection * m_speed;
		m_lookAt += vDirection * m_speed;
	}

	// right
	if( m_pInput->isKeyPressed(DIK_D))
	{
		if( ! computed)
		{
			D3DXVec3Cross(&vDirection,&vDirection,&m_up);
			D3DXVec3Normalize(&vDirection,&vDirection);
			computed = true;
		}	
		m_pos -= vDirection * m_speed;
		m_lookAt -= vDirection * m_speed;

	}

	
	// solve looking
	D3DXVECTOR3 rotAxis;		// axis used for horizontal rotation
	D3DXMATRIX		matRotAxis, 
							matRotZ;

	D3DXVec3Normalize(&vDirection,&(m_lookAt - m_pos));
	D3DXVec3Cross(&rotAxis,&vDirection,&m_up);
	D3DXVec3Normalize(&rotAxis,&rotAxis);

	D3DXMatrixRotationAxis(&matRotAxis,&rotAxis, (float)m_pInput->getYChange( ) / -360);
	D3DXMatrixRotationY(&matRotZ, (float)m_pInput->getXChange( ) / 360);
	
	//	transform vectors
	D3DXVec3TransformCoord(&vDirection,&vDirection,&(matRotAxis * matRotZ));
	D3DXVec3TransformCoord(&m_up,&m_up,&(matRotAxis * matRotZ));
	//	update look at vector
	m_lookAt = vDirection + m_pos;
    
    // Update the view matrix
	D3DXMATRIX mView;
    D3DXMatrixLookAtLH( &mView, &m_pos, &m_lookAt, &m_up );

	// set the device
	m_pDevice->SetTransform(D3DTS_VIEW,&mView);	
}