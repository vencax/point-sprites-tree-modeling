
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

	/////////////////////////////////////
	// Ctor & destor
	/////////////////////////////////////
	CScene( LPDIRECT3DDEVICE9, TCam *);
	~CScene( );

	/////////////////////////////////////
	// Methods
	/////////////////////////////////////
	void AddTree( string, D3DXVECTOR3 &);

	void Draw( bool light);

	void Update( void);
	void Animate( void);

	// just redirections to one single kind
	void	GenerateTree( );
	void	ResetTree( );
	LSystem *GetLSystem( void);
	bool GenerateTextures( void);
	string *GetFileNames( void);

	void	UpdateMemStatsWin( CWnd*);

	void	Reset( void);

		string name;

		string currentDirectory;

		int GetTreeDist( void);

private:
	TKindMap m_kindMap;	
	LPDIRECT3DDEVICE9 m_device;
	TCam	*m_pCamera;				//	camera

	LPDIRECT3DVERTEXBUFFER9 m_supportVB;

	D3DXVECTOR3 m_pos;
	// utils
    void ToRightUnits( unsigned int, char*);
};