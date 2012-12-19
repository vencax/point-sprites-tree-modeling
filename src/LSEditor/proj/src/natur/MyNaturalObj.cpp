

#include "./Cursor.h"		// for NUM_OF_VERT_ON constant
#include "./LSystem.h"	// for NUM_LVLS constant

#include "./MyNaturalObj.h"

CNO::CNO( unsigned int seed, D3DXVECTOR3 &position) : m_seed( seed), m_position( position)
{
	// init members
	for( int i= 0; i < NUM_LVLS; i++)
	{
		m_LevelHeads[ i].SetNonTerminal( 'H');
		m_LevelHeads[ i].SetLevel( i);
		m_levelBodyDataBeginsAt[ i] = 777;
		m_levelBodyIndexDataBeginsAt[ i] = 777;
		m_levelPSDataBeginsAt[ i] = 777;
	}

	// set all levels buffers and chains to NULL
	m_PSVertBuf = NULL;
	m_bodyVertBuf = NULL;
	m_indexBuffer = NULL;

	// set # of active levels to 0
	m_curActiveLevels = 0;

	m_lastDistance = 0;

	ZeroMemory( m_centerDistances, sizeof( float) * NUM_LVLS);

	m_LoDSteps[ 0] = 1;		// anlu one branch on 0th level so 1 LoD operat

#ifdef MYSTATS
	ResetStats( );
#endif
}

//////////////////////////////////////////////

CNO::~CNO(void)
{
	// dispose and delete all active levels
	for( int i= GetNumOfActiveLvls( )-1; i >= 0; i--)
	{		
		this->DisposeLevel( i);
	}

	// and delete the first level
	this->DeleteLevel( 0);

	// reset stack
	while( this->GetCurEndSiplifiedStack( ).size( ) )
        this->GetCurEndSiplifiedStack( ).pop( );	
}

//////////////////////////////////////////////

bool CNO::Draw( LPDIRECT3DDEVICE9 dev, bool lightningEnabled)
{
	CBranch::sIBNode *node;


	// turn off the light if lightning disabled
	if( lightningEnabled)
		dev->SetRenderState( D3DRS_LIGHTING, true);
	else 
		dev->SetRenderState( D3DRS_LIGHTING, false);

	// set custom vertex
	dev->SetFVF( CUSTOM_BODY_VERTEX_FVF);
	// set vertex buffer
	dev->SetStreamSource( 0, m_bodyVertBuf, 0, sizeof( BODY_VERTEX));
	// set index buffer
	dev->SetIndices( m_indexBuffer);			

	// go through all active levels and their drawing chain and draw.
	for( int i= 0; i < this->GetNumOfActiveLvls( ); i++)
	{
		CBranch *curr = this->GetLevelHeads( i)->GetChainPointers( )->NextInBodyDraw;
		while( curr)
		{	
			// set model matrix
			dev->SetTransform( D3DTS_WORLD, &curr->GetWorldMatrix( ) );

			// draw the branch
			node = curr->GetBodyNode( );

			if( D3D_OK != dev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST
				, node->pToVert
				, 0
				, node->count
				, node->pToIndx
				, ( node->count - NUM_OF_VERT_ON) * 2
				) )
				return false;	

			// step to next
			curr = curr->GetChainPointers( )->NextInBodyDraw;
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Disposes buffers storing data on GPU and delete its children on next level if any
////////////////////////////////////////////////////////////////////////////////////////////
void
CNO::DisposeLevel( int level)
{
	// release vertex buffers
	if( m_bodyVertBuf)
		m_bodyVertBuf->Release( );
	m_bodyVertBuf = NULL;

	if( m_PSVertBuf)
		m_PSVertBuf->Release( );
	m_PSVertBuf = NULL;

	if( m_indexBuffer)
		m_indexBuffer->Release( );
	m_indexBuffer = NULL;

	// delete children
	this->DeleteLevel( level+1);

	if( level < NUM_LVLS-1)
	{
		// reset children containers in currently disposing level
		CBranch *curr = this->GetLevelHeads( level)->GetChainPointers( )->NextOnLevel;
		while( curr)
		{
			curr->GetChildren( ).clear( );
			curr = curr->GetChainPointers( )->NextOnLevel;
		}
	}

	// remove apropriate part from data containers
	BODYVECTORS::iterator bodyBegin;
	bodyBegin = m_bodyVertices.begin( ) + GetLevelBodyDataBeginsAt( level);
	m_bodyVertices.erase( bodyBegin, m_bodyVertices.end( ) );

	this->SetLevelBodyDataBeginsAt( 777, level);

	VECTORINDECES::iterator bodyIndecesBegin;
	bodyIndecesBegin = m_bodyIndeces.begin( ) + GetLevelBodyIndexDataBeginsAt( level);
	m_bodyIndeces.erase( bodyIndecesBegin, m_bodyIndeces.end( ) );

	this->SetLevelBodyIndexDataBeginsAt( 777, level);

	VECTORS::iterator PSBegin;
	PSBegin = m_PS.begin( ) + GetLevelPSDataBeginsAt( level);
	m_PS.erase( PSBegin, m_PS.end( ) );

	this->SetLevelPSDataBeginsAt( 777, level);

	// terminate drawing chains in this level except simplified chain
	this->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw = NULL;
	ZeroMemory( &this->GetLevelHeads( level)->GetChainPointers( )->NextInPSDraw,
		sizeof( CBranch*) * NUMBER_OF_PS_TEXTURES);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Deletes all branches on given level. Go through branchTree level chain
////////////////////////////////////////////////////////////////////////////////////////////
void
CNO::DeleteLevel( int level)
{
	if( level >= NUM_LVLS)
		return;
	
	CBranch *cur, *next;
	
	// first after head
	cur = GetLevelHeads( level)->GetChainPointers( )->NextOnLevel;

	while( cur)
	{
		next = cur->GetChainPointers( )->NextOnLevel;
		delete cur;
		cur = next;
	}

	// set level to NULL
	this->GetLevelHeads( level)->GetChainPointers( )->NextOnLevel = NULL;
	this->GetLevelHeads( level)->GetChainPointers( )->NextSimplified = NULL;
}

////////////////////////////////////////////////////////////////////
// LoD operations
////////////////////////////////////////////////////////////////////
void
CNO::Simplify( void)
{
	int level = this->GetNumOfActiveLvls( )-1;

	CBranch *curSEnd = this->GetCurEndSiplifiedStack( ).top( );
	CBranch *firstEnhanced = this->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw;

	if( ! firstEnhanced)	// there are all branches already simplified
		return;	// so do nothing

	// reconnect head to point at firstEnhanced's successor
	this->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw = \
		firstEnhanced->GetChainPointers( )->NextInBodyDraw;

	// insert firstEnhanced to Simplified chain with pointing curSEnd to firstEnhanced
	curSEnd->GetChainPointers( )->NextSimplified = firstEnhanced;

	// and set next InBodyDraw to NULL in currently simplified
	firstEnhanced->GetChainPointers( )->NextInBodyDraw = NULL;

	// terminate Simplified Chain at firstEnhanced
	firstEnhanced->GetChainPointers( )->NextSimplified = NULL;

	// and set its children NOTDRAW flag
	for( CBranch::CHILDREN::iterator it = firstEnhanced->GetChildren( ).begin( );
		it != firstEnhanced->GetChildren( ).end( );
		it++)
	{
		( *it)->SetStatus( CBranch::SIMPLIFIED_BUT_NOT_DRAWN);
	}

	// set SIMPLIFIED FLAG
	firstEnhanced->SetStatus( CBranch::SIMPLYFIED);

	// push firstEnhanced into GetCurEndSiplifiedStack
	this->GetCurEndSiplifiedStack( ).push( firstEnhanced);
}

void
CNO::Enhance( void)
{	
	// there is no simplified branch if there is only head in the stack
	if( this->GetCurEndSiplifiedStack( ).size( ) == 1)
		return;

	int level = this->GetNumOfActiveLvls( )-1;

	// check if there are every branches already enhanced
	if( ! this->GetLevelHeads( level)->GetChainPointers( )->NextSimplified)
		return;

	// retrieve last simplified branch
	CBranch *curSEnd = this->GetCurEndSiplifiedStack( ).top( );
	// currently being enhanced branch is the first neInBodyDrawChain member of curActiveLevel Head
	CBranch *firstEnhanced = this->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw;

	// make head pointing to curSEnd
	this->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw = curSEnd;

	// connect curSEnd to bodyDraw chain
	curSEnd->GetChainPointers( )->NextInBodyDraw = firstEnhanced;

	// set status
	curSEnd->SetStatus( CBranch::ENHANCED);	

	// and unset its children NOTDRAW flag
	for( CBranch::CHILDREN::iterator it = curSEnd->GetChildren( ).begin( );
		it != curSEnd->GetChildren( ).end( );
		it++)
	{
		( *it)->SetStatus( CBranch::SIMPLYFIED);
	}

	// disconnect curSEnd from NextSimplified Chain in his predecessor 
	// get the predecessor with poping CurEndSiplifiedStack
	this->GetCurEndSiplifiedStack( ).pop( );
	curSEnd = this->GetCurEndSiplifiedStack( ).top( );
	// and terminate
	curSEnd->GetChainPointers( )->NextSimplified = NULL;	
}

void
CNO::ForceSimplify( void)
{
	// simplify until there is no enhanced Branch
	int level = this->GetNumOfActiveLvls( )-1;
	while( this->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw != 0)
		this->Simplify( );
}

void
CNO::ForceEnhance( void)
{
	// enhance until there is no simplified Branch
	int level = this->GetNumOfActiveLvls( )-1;
	while( this->GetLevelHeads( level)->GetChainPointers( )->NextSimplified != 0)
		this->Enhance( );
}