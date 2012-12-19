#pragma once

#include <vector>
#include <string>

#include <d3dx9.h>	// DirectX9

#include "./LSystem.h"
#include "./MyNaturalObj.h"
#include "./Branch.h"

#include "./CursorStack.h"

#define DIFF 10		// amount of random difference of initial values

// definitions of LoD borders
#define LVL2_BORDER 45
#define LVL2ANNULUS_BORDER 55

#define LVL1_BORDER 65
#define LVL1ANNULUS_BORDER 70

#define LVL0_BORDER 100
#define LVL0ANNULUS_BORDER 5000

using namespace std;

class CKind
{
public:
	CKind( LPDIRECT3DDEVICE9, LPDIRECT3DVERTEXBUFFER9, string &);
	~CKind( void);

	// typedefs ///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// vector of all trees in the forest
	typedef vector< CNO*> FOREST;	

// methods ///////////////////////////////////////////////////////////////////////////////////////////////////////////

	void AddTree( D3DXVECTOR3 &, D3DXVECTOR3 &);	// adds a new tree to spec. position
	void Draw( void);

	// gets
	inline string *GetFileNames( void)	{ return m_pTextureFilenames;	}
	inline LSystem *GetLSystem( void)	{ return m_LSystem;	}


	bool GenerateTextures( string &);


	void Update( D3DXVECTOR3 &);	// check if there is any simplify or enhance operations neccessery
	void TickUpdate( D3DXVECTOR3 &);	// called each frame to do actual animation

// private members //////////////////////////////////////////////////////////////////////////////////////////////

private:
	CursorStack *m_cursorStack;
	LSystem	*m_LSystem;		//  LSystem

	LPDIRECT3DDEVICE9 m_dev;

	// textures
	LPDIRECT3DTEXTURE9 *m_textures[ 4];

	LPDIRECT3DTEXTURE9 m_crustTexture;

	CKind::FOREST	 m_forest;

	// pointer to supportive vertext buffer containing one vertex( 0, 0, 0) for draving
	// simplified branches
private: LPDIRECT3DVERTEXBUFFER9	m_SuppVertBuf;

	// filenames of loaded textures
	string m_pTextureFilenames[ NUMBER_OF_PS_TEXTURES];

	// utils
	bool	DrawPS( CNO *);
	bool	DrawSiplifiedBranches( CNO *);
	bool	DrawCrown( CNO *);
	// Helper function to stuff a FLOAT into a DWORD argument
	inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

	void EnablePointSprites( int);
	void DisablePointSprites( void);
	void SetPSSize( int);

	// inits tree data( num of active levels in accordance to its position from cam
	void InitTree( CNO *, D3DXVECTOR3 &);

};
