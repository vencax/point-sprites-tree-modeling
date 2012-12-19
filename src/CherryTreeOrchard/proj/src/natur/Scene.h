
#pragma once

#include <stdlib.h>
#include <string>
#include <map>

#include <d3dx9.h>	// DirectX9

#include "./Kind.h"
#include "../Camera.h"

using namespace std;

class CScene
{
	/////////////////////////////////////
	// typedefs
	/////////////////////////////////////
	public: typedef map< string, CKind* >	TKindMap;		// map of kinds according their names
	public: typedef map< string, string>	TDirectoryMap;	// holds directories where textures are stored for each Kind (string)

	/////////////////////////////////////
	// Ctor & destor
	/////////////////////////////////////
	CScene( LPDIRECT3DDEVICE9, TCam *);
	~CScene( );

	/////////////////////////////////////
	// Methods
	/////////////////////////////////////
	bool AddTree( string, D3DXVECTOR3 &);

	void Draw( void);
	void Update( void);
	void Animate( void);

	void	InitScene( void);
	void	SetupLights( void);


	void	AddTrees( void);

	/////////////////////////////////////////////
	// for FPS
	////////////////////////////////////////////
	void Starttiming();
	void Endtiming();
	inline float GetFPS( void)	{	return m_FPS;	}

private:
	TKindMap m_kindMap;	
	LPDIRECT3DDEVICE9 m_device;
	TCam	*m_pCamera;				//	camera

	TDirectoryMap m_dirMap;

	LPDIRECT3DVERTEXBUFFER9 m_supportVB;
	LPDIRECT3DVERTEXBUFFER9		m_axis;	 // direct buffer for axis
	LPDIRECT3DVERTEXBUFFER9		m_ground;
	LPDIRECT3DTEXTURE9		m_groundTexture;

	D3DXVECTOR3 m_pos;

	bool	InitDirMap( void);

	float m_FPS;
	__int64 t1, t2, freq;
};