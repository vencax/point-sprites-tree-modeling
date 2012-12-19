#pragma once

class Graphics
{
public:
 Graphics();
 virtual ~Graphics();
  
  HRESULT create(
       HWND hwnd,
       int width,
       int height,
       bool windowed); 

  LPDIRECT3DDEVICE9 getDevice();

protected:
  LPDIRECT3D9       _d3d8;
  LPDIRECT3DDEVICE9 _device;
};
