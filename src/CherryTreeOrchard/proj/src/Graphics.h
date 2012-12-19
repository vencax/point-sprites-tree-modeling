#pragma once

#include <d3dx9.h>	// DirectX9

class Graphics
{
public:
 Graphics();
 virtual ~Graphics();
  
  HRESULT Create(
       HWND hwnd,
       int width,
       int height,
       bool windowed); 

  HRESULT CheckDevice(void);
  void	Resize( int, int);

  inline LPDIRECT3DDEVICE9 GetDevice( void)	{	return m_pDevice;	}

protected:
	LPDIRECT3D9       m_pD3D;
	LPDIRECT3DDEVICE9 m_pDevice;  
	D3DPRESENT_PARAMETERS m_presentParameters;
};
