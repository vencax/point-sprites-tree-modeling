#pragma once

#include <vector>

#include "../CVert.h"

#define NUMBER_OF_PS_TEXTURES 10

#define NUM_OF_SIMPLFD_TEXTRS2 10	// num of textures used in dravin' simpl. branches on 2nd level
#define NUM_OF_SIMPLFD_TEXTRS1 10	//  ... on 1st level
#define NUM_OF_SIMPLFD_TEXTRS0 5		//  ... on 0th level ( tree is onlu one PointSprite)

using namespace std;

class CBranch
{
public:
	/////////////////////////////////////////////////////////////////////////////////////////////
	///  typedefs 
	/////////////////////////////////////////////////////////////////////////////////////////////

	// index buffer node struct contains info used in drawing of branch
	typedef struct ibnode_ {
		unsigned int pToVert;		//	BaseVertexIndex - pointer to vertex buffer
		unsigned int pToIndx;		//	Start Index - pointer to index buffer
		unsigned short count;	//	# vertices used in drawing - num vertices
	}	sIBNode;
	
	// index buffer node struct contains info used in drawing of branch body
	typedef struct psnode_ {
		unsigned int startVertex;		//	BaseVertexIndex - pointer to vertex buffer
		unsigned short count;	//	# vertices used in drawing - num vertices
	}	sPSNode;

	// typedefs
	typedef vector< CBranch *> CHILDREN;
	typedef enum status_ {
		NOTGENERATED,
		ENHANCED,
		SIMPLYFIED,
		SIMPLIFIED_BUT_NOT_DRAWN,
	}	STATUS;

	typedef	struct sChainPointers_	{
		CBranch *NextOnLevel;
		CBranch *NextInBodyDraw;
		CBranch *NextSimplified;
		CBranch *NextInPSDraw[ NUMBER_OF_PS_TEXTURES];
	}	TChainPointers;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// ctors & dests
	/////////////////////////////////////////////////////////////////////////////////////////////
	CBranch( );
	CBranch( D3DXMATRIX&, D3DXMATRIX&, char, float, float, float, char, unsigned int);		// mat, nonterminal, lenght, thick, defAng, level, seed
	~CBranch( );

	/////////////////////////////////////////////////////////////////////////////////////////////
	// private fields and public properties
	/////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	// SEED
	////////////////////////////////////////
private:	unsigned int m_seed;
public:	inline unsigned int GetSeed( void)	{ return m_seed;	}
public:	inline void SetSeed( unsigned int s)	{ m_seed = s;	}

	//////////////////////////////
	// MODEL MATRX
	// matrix storing position and orienation to origin of model
	//////////////////////////////
private:	D3DXMATRIX	m_modelMatrix;
public:	inline D3DXMATRIX &GetModelMatrix( void)	{ return m_modelMatrix;	}

private:	D3DXMATRIX	m_toCenterMatrix;
public:	inline D3DXMATRIX &GetCenterMatrix( void)		{ return m_toCenterMatrix;	}
public:	inline void	SetCenterMatrix( D3DXMATRIX &m)	{ m_toCenterMatrix = m;	}
	
	//////////////////////////////
	// WORLD MATRX
	// precalculate position and orienation to origin of the world
	//////////////////////////////
private:	D3DXMATRIX	m_worldMatrix;
public:	inline D3DXMATRIX &GetWorldMatrix( void)	{ return m_worldMatrix;	}
public:	inline void	SetWorldMatrix( D3DXMATRIX &m)	{ m_worldMatrix = m;	}

	///////////////////////////////////////////////
	// atributes needed to regenerate
	///////////////////////////////////////////////
	// LEGHT
	//////////////////////////
private:	float	m_length;
public:	inline float GetLenght( void)	{ return m_length;	}
public:	inline void SetLenght( float l)	{ m_length = l;	}

	////////////////////////////
	// THICKNESS
	/////////////////////////////
private:	float	m_thickness;	
public:	inline float GetThickness( void)	{ return m_thickness;	}
public:	inline void SetThickness( float c)	{ m_thickness = c;	}

	////////////////////////////
	// DEFAULT ANGLE
	////////////////////////////
private:	float	m_defAngle;
public:	inline float GetDefAngle( void)	{ return m_defAngle;	}
public:	inline void SetDefAngle( float a)	{ m_defAngle = a;	}

	////////////////////////////
	// NON TERMINAL
	// non terminal char used in generation of this branch
	////////////////////////////
private:	char m_initNonTerminal;	
public:	inline char GetNonTerminal( void)	{ return m_initNonTerminal;	}
public:	inline void SetNonTerminal( char n)	{ m_initNonTerminal = n;	}

	/////////////////////////////
	// level of this branch
	/////////////////////////////
private:	char m_level;
public:	inline char GetLevel( void)	{ return m_level;	}
public:	inline void SetLevel( char l)	{ m_level = l;	}

	/////////////////////////////
	// Status
	/////////////////////////////
private:	STATUS		m_status;
public:	inline STATUS GetStatus( void)		{	return m_status;	}
public:	inline void SetStatus( STATUS s )	{	m_status = s;	}
	
	
private:	CHILDREN	m_children;		// pointers to descendant branches
public:	inline  CHILDREN& GetChildren( void)	{ return m_children;	}

	/////////////////////////////////
	// NODES
	/////////////////////////////////
	// index buffer node for BODY
private: sIBNode  *m_bodyIBN;
public: inline sIBNode  *GetBodyNode( void)		{	return m_bodyIBN;	}
public: inline void	SetBodyNode( sIBNode  *val)	{	m_bodyIBN = val;	}

	// index buffer nodes for different types of leaves
private: sPSNode *m_PSNodes[ NUMBER_OF_PS_TEXTURES];
public: inline sPSNode  *GetPSNode( int index)	{	return m_PSNodes[ index];	}
public: inline void	SetPSNode( sPSNode  *val, int index)	{	m_PSNodes[ index] = val;	}

	// pointers to right and left brother( brothers are linked as a bidirectional chain)
private: TChainPointers m_chainPointers;
public: inline TChainPointers *GetChainPointers( void)	{	return &m_chainPointers;	}

private: int m_simplifiedTextureIndex;
public: inline int GetSimplTexIndx( void)	{	return m_simplifiedTextureIndex;	}

	///////////////////////////////////////////// 
	// Methods
	///////////////////////////////////////////// 

public:	inline void AppendChild( CBranch *ch)	{	m_children.push_back( ch);	}
public:	void	Release( void);		// deletes everything except self


	
};
