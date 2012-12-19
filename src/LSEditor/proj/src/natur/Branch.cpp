#include "../Stdafx.h"

#include "./Branch.h"

#include "../CVert.h"

CBranch::CBranch( )
{
	ZeroMemory( this, sizeof( *this) );	
}

////////////////////////////////////////////////////////////
/// parametrized ctor : mat, nonterminal, thick, lenght, defAng, level, seed 
CBranch::CBranch( D3DXMATRIX &m, D3DXMATRIX &w, char nonTerm, float ln, float thick, float defA, char lvl, unsigned int seed)
: m_modelMatrix( m), m_worldMatrix( w), m_initNonTerminal( nonTerm), m_thickness( thick), m_length( ln),
m_defAngle( defA), m_level( lvl), m_seed( seed)
{
	m_status = NOTGENERATED;

	// init nodes
	m_bodyIBN = NULL;
	for( int i = 0; i < NUMBER_OF_PS_TEXTURES; i++)
	{
		m_PSNodes[ i] = NULL;
	}

	// generate rnd index in texture pool
	switch( m_level)
	{
		case 0:
				m_simplifiedTextureIndex = rand( ) % NUM_OF_SIMPLFD_TEXTRS0;
				break;	
		case 1:
				m_simplifiedTextureIndex = rand( ) % NUM_OF_SIMPLFD_TEXTRS1;
				break;
		case 2:
				m_simplifiedTextureIndex = rand( ) % NUM_OF_SIMPLFD_TEXTRS2;
				break;
	}

	// multiply model with world matrix
	D3DXMatrixMultiply( &m_worldMatrix, &m_modelMatrix, &m_worldMatrix );

	ZeroMemory( &m_chainPointers, sizeof( TChainPointers) );
}

CBranch::~CBranch(void)
{
	Release( );

	ZeroMemory( this, sizeof( *this) );
}

////////////////////////////////////////////////////////////////////////////////////////
// deletes everything except self
////////////////////////////////////////////////////////////////////////////////////////
void
CBranch::Release( void)
{
	// release nodes
	if( m_bodyIBN)
	{
		delete m_bodyIBN;
		m_bodyIBN = NULL;
	}
	for( int i = 0; i < NUMBER_OF_PS_TEXTURES; i++)
	{
		if( m_PSNodes[ i])
		{
			delete m_PSNodes[ i];
			m_PSNodes[ i] = NULL;
		}
	}

	// clear children container
	m_children.clear( );
}
