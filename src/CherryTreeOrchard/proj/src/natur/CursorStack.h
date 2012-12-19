
// CursorStack keeps track of all the cursors and allows them to be saved
// to allow branching in the LSystem
#pragma once

#include <d3dx9.h>	// DirectX9
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <stdlib.h>	// rand( )

#include "./Cursor.h"
#include "./LSystem.h"		// for connection with lsystem
#include "./MyNaturalObj.h"
#include "./Branch.h"

//#define RATIO 1
//#define INITIAL_RADIUS 1
//#define INITIAL_LENGHT 1

using namespace std;

class CursorStack {

public:
	
	CursorStack( LSystem *, LPDIRECT3DDEVICE9);
	~CursorStack( );
	
	/////////////////////////////////////////////  
	/// typedefs 
	/////////////////////////////////////////////

		// used in generation to descend in BranchTree by layers
	typedef queue< CBranch*> TBRANCH_QUEUE;
	typedef stack< Cursor*>	TCURSOR_STACK;

	/////////////////////////////////////////////  
	// methods 
	/////////////////////////////////////////////
public:	bool IncreaseLevel( CNO *);			// Generate i-th level of tree
public:	bool DecreaseLevel( CNO *);

			void DispatchCommand( CNO *);

			void	InitCenterDistances( CNO *);

private:	bool GenerateBranch( CBranch*, CNO*, bool);		// generate new branch according its properties
private:	bool CreateBuffers( CNO *, int);					// creates all vertex and index buffers of i-th level
private:	void Reset( void);		// resets this object to be used by another Branch

private:
	// stack that holds the Cursors in the stack - reset after branch generation
	TCURSOR_STACK m_stack;		

	//  vector of body points & normals	
	CNO::BODYVECTORS	m_body;								
	//  vector for indeces of actual branch
	CNO::VECTORINDECES m_cIndeces;		

	CNO::VECTORS	m_pointSprites[ NUMBER_OF_PS_TEXTURES];					//	vectors of point sprites

	LSystem *m_pLSys;
	LPDIRECT3DDEVICE9 m_dev;

	unsigned int m_numOfBranchesOnLevel;

	unsigned int m_uNextIndex;		//  - reset after branch generation
	unsigned int m_uNextPSIndex[ NUMBER_OF_PS_TEXTURES];	//  - reset after branch generation	
	
	unsigned int m_bodyVertexOff;			// offset in m_body
	unsigned int m_bodyIndexOff;				// offset in m_cIndeces

	//stack< float> m_centerHeights;

	// offset in indeces of each type PS
	unsigned int m_PSIndexOff[ NUMBER_OF_PS_TEXTURES];

	// support pointers for building BranchChain while generation of level
	CBranch *m_curLevelBranch;

	// support pointer for building chain of branches to draw body & PS
	CBranch *m_curBODYChainEnd;
	CBranch *m_curPSChainEnds[ NUMBER_OF_PS_TEXTURES];	


	// utils
	// reads float value in () from string. Modifies the int. After successful reading set the int poiting just at closing ')'
	float ReadVal( unsigned int &, string &);	
	float GetParametr( unsigned int &, string &);
	void AddPS( void);
	void DrawSegment( void);
	void InsertFirstBase( void);

	float CalculateCenerPointHeight( void);		// not used
	void	AppendNewDataToTree( CNO *, int);
};
