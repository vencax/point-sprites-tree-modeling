//cursorStack.cpp 

#include "./CursorStack.h"

#include "../CVert.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// default ctor - creates first cursor on stack and first base for branches
CursorStack::CursorStack( LSystem *ls, LPDIRECT3DDEVICE9 d) : m_pLSys( ls), m_dev( d)
{
	// init members
	for( int i= 0; i< NUMBER_OF_PS_TEXTURES; i++)
	{
		m_uNextPSIndex[ i]= 0;
		m_PSIndexOff[ i] = 0;
	}

	m_uNextIndex= 0;
	m_bodyVertexOff =0;
	m_bodyIndexOff = 0;		

	m_numOfBranchesOnLevel = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor
CursorStack::~CursorStack()
{
	Reset( );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generate i-th level of tree
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool
CursorStack::IncreaseLevel( CNO *tree)
{
	int level = tree->GetNumOfActiveLvls( )-1 + 1;

	// get beginning of i-th level chain
	CBranch *b = tree->GetLevelHeads( level)->GetChainPointers( )->NextOnLevel;	

	if( ! b)
		return true;

	// init current chain ends to point at head
	m_curBODYChainEnd = tree->GetLevelHeads( level);
	for( int i= 0; i< NUMBER_OF_PS_TEXTURES; i++)
	{
		m_curPSChainEnds[ i] = tree->GetLevelHeads( level);
	}

	// pop last old head in tree->m_simplBrStack
	tree->GetCurEndSiplifiedStack( ).pop( );	// remove old head
	// and push head of newly increasing( generated) level
	tree->GetCurEndSiplifiedStack( ).push( tree->GetLevelHeads( level) );

	// check if we should create next level
	bool genNextLevel = m_pLSys->ContinueGenerating( level);

	// create buddy branch
	D3DXMATRIX m;
	CBranch *buddy = new CBranch( m, m, '0', '0', '0', '0', '0', 0);
	m_curLevelBranch = buddy;

	// prepare offsets for nodes to point at right location in data
	m_bodyVertexOff = ( unsigned int)tree->GetBodyVertices( ).size( );
	m_bodyIndexOff = ( unsigned int)tree->GetBodyIndeces( ).size( );
	// initial offset for PS will not by set because we solve ryght offsets in appedDataNewData procedure	

	// while not end of current level branchChain
	while( b)
	{
		if( ! GenerateBranch( b, tree, genNextLevel) )
			return false;
		b = b->GetChainPointers( )->NextOnLevel;
	}

	// use buddy to retrieve begin of next level, risen up after this level generation. 
	// To connect new created leve to next level head
	if( genNextLevel)
	{
		tree->GetLevelHeads( level+1)->GetChainPointers( )->NextOnLevel = buddy->GetChainPointers( )->NextOnLevel;
		tree->GetLevelHeads( level+1)->GetChainPointers( )->NextSimplified = buddy->GetChainPointers( )->NextSimplified;
	}
	// delete buddy
	delete buddy;

	// calculate number of LoD operations in one tick
	// only for 1 & 2 levels. On 0 level is only 1 branch to be passed to LoD Op
	if( level < NUM_LVLS-1)
	{
		tree->SetLoDSteps( ( char)( ( m_numOfBranchesOnLevel / 5) + 1), level+1);
	}

	// apend generated data to tree
	this->AppendNewDataToTree( tree, level);

	// create buffers
	if( ! CreateBuffers( tree, level) )
		return false;

	// increase # od active levels
	tree->IncNumOfActiveLvls( );

	// reset cursor stack
	Reset( );

	return true;
}

bool
CursorStack::DecreaseLevel( CNO *tree)
{
	int level = tree->GetNumOfActiveLvls( )-1;

	// dispose last active level
	tree->DisposeLevel( level);

	// create buffers
	if( ! CreateBuffers( tree, level) )
		return false;

	// reset GetCurEndSiplifiedStack that has now contain all branches of
	// currently disposesing level cos' they are all simplified
	  // remove all
	while( tree->GetCurEndSiplifiedStack( ).size( ) )
        tree->GetCurEndSiplifiedStack( ).pop( );	
	// and insert head of one level lower
	tree->GetCurEndSiplifiedStack( ).push( tree->GetLevelHeads( level-1) );

	// reset body drawing pointer in current active level head witch being disposed
	// because it will not be draw as enhanced any more until next increasing level
	tree->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw = NULL;

	// decrease active levels
	tree->DecNumOfActiveLvls( );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// generate new Branch's vertices and indeces
bool 
CursorStack::GenerateBranch( CBranch* b, CNO* tree, bool genNextLevel)
{
	// RAND MUTEX LOCK
	
	// init rand gen.
	srand( b->GetSeed( ) );

	// LSYS MUTEX LOCK

	string s = m_pLSys->getNRewrites( 
		m_pLSys->getRecursionLevel( b->GetLevel( ) ), 
		b->GetNonTerminal( )
		);

	// LSYS MUTEX UNLOCK

	// push initial cursor into the stack with default values
	m_stack.push( new Cursor(
		b->GetLenght( ), 
		b->GetThickness( ), 
		b->GetDefAngle( )
		) );

	// InsertNewBase - initial
	InsertFirstBase( );

	// inspect each character in the string
	for( unsigned i = 0; i<s.length();i++)
	{ 
		// and call the appropriate function
		switch(s[i])
		{

		//	basic orientaion commands
		case 'F':		// 'draws a line
			//	move cursor
			m_stack.top( )->GoForward( );
			break;

		case 'f':
			m_stack.top( )->GoForward( );
			break;

		case '[':
			// draw actual segment
			DrawSegment( );
			//	save current state into the stack
			m_stack.push( new Cursor(	 m_stack.top( ) ) );
			break;

		case ']':
			// draw actual segment
			DrawSegment( );
			// restore the state only deleting the top
			delete m_stack.top( );
			m_stack.pop( );
			break;

		case '&':	// pitch down around left vector( X-axis)
				// draw actual segment
				DrawSegment( );
				m_stack.top( )->pitch( -GetParametr( i, s) );				
				break;

		case '^':	// pitch up around left vector( X-axis)
				// draw actual segment
				DrawSegment( );
				m_stack.top( )->pitch( GetParametr( i, s));
				break;

		case '<':	// turn left around up vector( Y-axis)
				m_stack.top( )->yaw( GetParametr( i, s) );
				break;

		case '>':		// turn right around up vector( Y-axis)
				m_stack.top( )->yaw( -GetParametr( i, s) );
				break;

		case '-':	// roll left (counter clockwise) around forward vector( Z-axis)
				// draw actual segment
				DrawSegment( );
				m_stack.top( )->roll( GetParametr( i, s) );
				break;

		case '+':		// roll right (clockwise) around forward vector( Z-axis)
				// draw actual segment
				DrawSegment( );
				m_stack.top( )->roll( -GetParametr( i, s) );
				break;

		//	spiecial orientaion commands
		case '|':			//  turn 180 deg around up vector
				m_stack.top( )->yaw( 180.0f);
				break;

		case '%':		// 	roll 180 deg around forward vector
				m_stack.top( )->roll( 180.0f);
				break;

		case '$':		// 	Yaw until Horizontal
				m_stack.top( )->YawUntilHorizontal( );
				break;

		case '~':		// 	yaw/pitch/roll in a random direction
				// not implemented because it produces 
				break;

		//case 't':			// 	Pitch down( correction for gravity )
		//		break;

		case 'L':		// 	record point spritePS( leaf)
				AddPS( );
				break;

		//	Inc/Dec commands
		case '"':		// 	     increment length with 1.1   
				if( s[i+1] == '(')
					m_stack.top( )->ChangeLength( ReadVal( i, s) );	// multiply with read value
				else
					m_stack.top( )->ChangeLength( 1.1f);
				break;

		case '\'':		// 	     decrement length with 0.9     
				if( s[i+1] == '(')
					m_stack.top( )->ChangeLength( ReadVal( i, s) );	// multiply with read value
				else
					m_stack.top( )->ChangeLength( 0.9f);
				break;

		case ';':		// 	     increment angle with 1.1 
				if( s[i+1] == '(')
					m_stack.top( )->ChangeDefaultAngle( ReadVal( i, s) );	// multiply with read value
				else
					m_stack.top( )->ChangeDefaultAngle( 1.1f);
				break;

		case ':':		// 	     decrement angle with 0.9 
				if( s[i+1] == '(')
					m_stack.top( )->ChangeDefaultAngle( ReadVal( i, s) );	// multiply with read value
				else
					m_stack.top( )->ChangeDefaultAngle( 0.9f);
				break;

		case '?':		// 	     increment thickness with 1.4
				if( s[i+1] == '(')
					m_stack.top( )->ChangeThickness( ReadVal( i, s) );	// multiply with read value
				else
					m_stack.top( )->ChangeThickness( 1.4f);
				break;

		case '!':		// 	     decrement thickness with 0.7
				if( s[i+1] == '(')
					m_stack.top( )->ChangeThickness( ReadVal( i, s) );	// multiply with read value
				else
					m_stack.top( )->ChangeThickness( 0.7f);
				break;

		
		default:	// if it is left side of rule and we are not still in big depth of recursion, insert new Branch
			// here we read shared LSystem. But only read, so we dont lock it by mutex

			/////////////////////////////////////
			// move cursor with current segment lenght
			/////////////////////////////////////
			Cursor *c = m_stack.top( );
			D3DXMATRIX tmp;
			// if there is segment lenght unresolved by DrawSegment
			if( c->m_segmentLength)
			{
					// first move m_matrix with m_segmentLength
				D3DXMatrixMultiply( &( c->m_matrix), D3DXMatrixTranslation( &tmp, 0.0f, c->m_segmentLength, 0.0f), &( c->m_matrix) );
			}

			//////////////////////////////////////
			// add curr height
			//////////////////////////////////////		
			//D3DXVECTOR3 v = D3DXVECTOR3( 0, 0, 0);
			//// project it with current
			//D3DXVec3TransformCoord( &v, &v, &( c->m_matrix) );
			//// and push it into stack
			//m_centerHeights.push( v.y);

			///////////////////////////////////////
			// and if there should be created next level, do it
			///////////////////////////////////////
			if( m_pLSys->IsItLeftSideOfARule( s[ i]) && genNextLevel )
			{
				
				CBranch *newBrother = new CBranch( c->m_matrix,
																		b->GetWorldMatrix( ),
																		s[ i], 
																		c->m_length, 
																		c->m_thickness,																		
																		c->m_defAngle, 
																		b->GetLevel( )+1,
																		( unsigned int)rand( ) );

				//////////////////////////////////////////////
				// solve center, use tree's m_centerDistances array
				//////////////////////////////////////////////
				// make translate matrix already multiplied with toWorld matrix
				float distToCenter = tree->GetCenterDistances( )[ newBrother->GetLevel( )];
				D3DXMatrixTranslation( &( newBrother->GetCenterMatrix( ) ), 0, distToCenter, 0);
				D3DXMatrixMultiply( &newBrother->GetCenterMatrix( ), &newBrother->GetCenterMatrix( ), &newBrother->GetWorldMatrix( ) );


				m_numOfBranchesOnLevel++;

				/////////////////////////////////
				// connect chains
				////////////////////////////////
				// connect it into level chain
				m_curLevelBranch->GetChainPointers( )->NextOnLevel = newBrother;

				// also build simlified chain because theese new branches will appear as simplified
				m_curLevelBranch->GetChainPointers( )->NextSimplified = newBrother;

				// skip on new brother
				m_curLevelBranch = newBrother;

				// and set him as simplified
				newBrother->SetStatus( CBranch::SIMPLYFIED);

				// append newBrother to parent b
				b->AppendChild( newBrother);

			#ifdef MYSTATS
				tree->stats.totalSysData += sizeof( CBranch);
			#endif
			}
			break;
		}

	}	

	// UNLOCK RAND MUTEX

	// draw the last segment
	DrawSegment( );

	// delete the first cursor
	if( m_stack.size( ) == 1)
	{
		delete m_stack.top( );
		m_stack.pop( );
	}
	else {
		MessageBox( 0, "Cursor Stack err!!", "err", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	//////////////////////// now create sIBNode struct for drawing body or PointSprites /////////////////////////////////////////////////

	CBranch::sIBNode *ibNode;
	CBranch::sPSNode *PSNode;

	///////////////////////////////////
	// solve body
	///////////////////////////////////
	if( m_uNextIndex > NUM_OF_VERT_ON)
	{
		// create and fill new node
		ibNode = new CBranch::sIBNode;
		ibNode->pToVert = m_bodyVertexOff;
		ibNode->pToIndx = m_bodyIndexOff;
		ibNode->count	=	m_uNextIndex;		// count of vertices

		// check if  there is already allocated, occasionaly delete it
		if( b->GetBodyNode( ) )
			delete b->GetBodyNode( );
		
		// set it to branch
		b->SetBodyNode( ibNode);

		// connect it to body drawing chain
		m_curBODYChainEnd->GetChainPointers( )->NextSimplified = b;
		m_curBODYChainEnd = b;	// new is old			
		// and insert b into CurEndSiplifiedStack
		tree->GetCurEndSiplifiedStack( ).push( b);

		// update holding variables
		m_bodyVertexOff += m_uNextIndex;
		m_bodyIndexOff += ( m_uNextIndex - NUM_OF_VERT_ON) * 6;	// # of trangles * 3 (3 indeces for 1 triangle)

		// reset next index
		m_uNextIndex = 0;

		#ifdef MYSTATS
			// update stats
			tree->stats.BodyNodesData += sizeof( CBranch::sIBNode);
			tree->stats.totalSysData += sizeof( CBranch::sIBNode) + sizeof( CBranch*);	// pointer in tree containers
		#endif
	}
	else
	{
		// only base vertices. DrawSegment ended due to smal diameter drawn segments
		// reset next index
		m_uNextIndex = 0;

		// remove last NUM_OF_VERT_ON vertices from container
		for( int i= 0; i < NUM_OF_VERT_ON; i++)
		{
			m_body.pop_back( );
		}
	}

	///////////////////////////////////
	// now solve point sprites indeces. We move each PS to random container so each PS will have
	///////////////////////////////////
	// random texture
	for( int i= 0; i< NUMBER_OF_PS_TEXTURES; i++)
	{		
		if( m_uNextPSIndex[ i])
		{
			// create and fill new node
			PSNode = new CBranch::sPSNode;
			PSNode->startVertex = m_PSIndexOff[ i];
			PSNode->count	=	m_uNextPSIndex[ i];

			// check if  there is already allocated, occasionaly delete it
			if( b->GetPSNode( i) )
				delete b->GetPSNode( i);

			// set it to branch
			b->SetPSNode( PSNode, i);

			// connect it to body drawing chain
			m_curPSChainEnds[ i]->GetChainPointers( )->NextInPSDraw[ i] = b;
			m_curPSChainEnds[ i] = b;	// new is old			

			// update offset into vertex & index buffer( they have the same values) 1:1ratio
			m_PSIndexOff[ i] += m_uNextPSIndex[ i];

			// reset next index
			m_uNextPSIndex[ i] = 0;

		#ifdef MYSTATS
			// update stats
			tree->stats.PSNodesData += sizeof( CBranch::sPSNode);
			tree->stats.totalSysData += sizeof( CBranch::sPSNode) + sizeof( CBranch*);	// pointer in tree containers
		#endif
		}

	}

	b->SetStatus( CBranch::SIMPLYFIED);

	return true;
}

void
CursorStack::AppendNewDataToTree( CNO *tree, int level)
{
	// pointers to container data
	CNO::VECTORS::pointer PSdata, PSsource;
	CNO::BODYVECTORS::pointer bodyData, bodySource;
	CNO::VECTORINDECES::pointer indexData, indexSource;

	size_t size= 0;
	size_t offset = 0;

	//////////////////////////////////////////////////////
	// Body vertices
	//////////////////////////////////////////////////////
	// add space for new vertices
	offset = tree->GetBodyVertices( ).size( );
	tree->GetBodyVertices( ).resize( offset + m_body.size( ) );

	// set where data of i-th level begins ( current offset)
	tree->SetLevelBodyDataBeginsAt( ( unsigned int)offset, level);

	// than copy them into
	bodySource = &m_body[ 0];	// source pointer
	bodyData = &tree->GetBodyVertices( )[ offset];	// dest pointer
	size = m_body.size( ) * sizeof( BODY_VERTEX);		// amount items to copy
	// copy data
	memcpy( bodyData, bodySource, size);

	//////////////////////////////////////////////////////
	// Body indeces
	//////////////////////////////////////////////////////
	// add space for new vertices
	offset = tree->GetBodyIndeces( ).size( );
	tree->GetBodyIndeces( ).resize( offset + m_cIndeces.size( ) );

	// set where data of i-th level begins ( current offset)
	tree->SetLevelBodyIndexDataBeginsAt( ( unsigned int)offset, level);

	// than copy them into
	indexSource = &m_cIndeces[ 0];	// source pointer
	indexData = &tree->GetBodyIndeces( )[ offset];	// dest pointer
	size = m_cIndeces.size( ) * sizeof( unsigned short);		// amount bytes to copy
	// copy data
	memcpy( indexData, indexSource, size);
	
	//////////////////////////////////////////////////////
	// PS vertices
	//////////////////////////////////////////////////////
	unsigned int pom = 0;

	// prepare size - size of all PS containers
	size = 0;
	for( int i= 0; i< NUMBER_OF_PS_TEXTURES; i++)
		size += m_pointSprites[ i].size( );

	offset = tree->GetPS( ).size( );
    // add space for new vertices
	tree->GetPS( ).resize( offset + size);

	// set where data of i-th level begins ( current offset)
	tree->SetLevelPSDataBeginsAt( ( unsigned int)offset, level);

	CBranch *tmp;
	// than copy them into
	// copy data from all PS containers to the vertex buffer. consecutive 0-1-2-3-....
	for( int i= 0; i< NUMBER_OF_PS_TEXTURES; i++)
	{
		if( m_pointSprites[ i].size( ) )
		{
			// get vector buffer
			PSsource = &m_pointSprites[ i][ 0];	
			PSdata = &tree->GetPS( )[ offset];
			
			size = m_pointSprites[ i].size( )  * sizeof( D3DXVECTOR3);

			// copy data
			memcpy( PSdata, PSsource, size);		

			// get i-th currently generated chain and update its nodes
			tmp = tree->GetLevelHeads( level)->GetChainPointers( )->NextInPSDraw[ i];
			while( tmp)
			{
				// update and
				tmp->GetPSNode( i)->startVertex += ( unsigned int)offset;
				// skip on next
				tmp = tmp->GetChainPointers( )->NextInPSDraw[ i];
			}

			// update offset with current i-th PS container size
			offset += m_pointSprites[ i].size( );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool
CursorStack::CreateBuffers( CNO *tree, int level)
{

	LPDIRECT3DVERTEXBUFFER9 vertBuf;
	LPDIRECT3DINDEXBUFFER9 IndexBuffer;

	size_t size= 0;

	///////////////////////////////////// 
	// BODY vertex buffer
	/////////////////////////////////////
	// discard old
	if( tree->GetBodyVertexBuf( ) )
		tree->GetBodyVertexBuf( )->Release( );

	//	create vertex buffer
	size = sizeof( BODY_VERTEX) * tree->GetBodyVertices( ).size( );
	if( FAILED( m_dev->CreateVertexBuffer( ( unsigned int)size, D3DUSAGE_WRITEONLY, CUSTOM_BODY_VERTEX_FVF, D3DPOOL_MANAGED, &vertBuf, NULL) ) )
	{
		MessageBox( 0, "Creation of VertexBuffer failed", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	
	BODY_VERTEX* vertArray;
	vertBuf->Lock( 0, 0, ( void**)&vertArray, 0);

	// get vector buffer
	CNO::BODYVECTORS::pointer bodyData = &tree->GetBodyVertices( )[ 0];
	// copy data
	memcpy( vertArray, bodyData, size);

	vertBuf->Unlock( );
	///////////////////////////////////////////////////////////////////////
	//char *rr = new char[ m_body.size( ) * sizeof( BODY_VERTEX) * 150];
	//char pomc[ 100];
	//int off = 0;
	//int pomoff = 0;
	//for( int i= 0; i < m_body.size( ); i++)
	//{
	//	sprintf( pomc, "(%f, %f, %f) Norm( %f, %f, %f) Tex( %f, %f); ", vertArray[ i].pos.x, vertArray[ i].pos.y, vertArray[ i].pos.z, vertArray[ i].normal.x, vertArray[ i].normal.y, vertArray[ i].normal.z, vertArray[ i].tu, vertArray[ i].tv);
	//	while( pomc[ pomoff])
	//		rr[ off++] = pomc[ pomoff++];
	//	pomoff = 0;
	//}
	//rr[ off] = 0;
	//delete [] rr;
	//MessageBox( 0, ib, "IB", MB_OK);
	///////////////////////////////////////////////////////////////////////

	// set tree vert buf
	tree->SetBodyVertBuf( vertBuf);

#ifdef MYSTATS
	// set stats
	tree->stats.bodyData += ( unsigned int)size;
	tree->stats.numOfVertices += ( unsigned int)m_body.size( );
	tree->stats.totalGPUData += ( unsigned int)size;	
#endif


	///////////////////////////////////// 
	// BODY index buffer buffer 
	/////////////////////////////////////	
	// discard old
	if( tree->GetIndexBuf( ) )
		tree->GetIndexBuf( )->Release( );

	// new indexbuffer size is whole index container
	size = sizeof( unsigned short) * ( unsigned int)tree->GetBodyIndeces( ).size( );

	// create it
	if( FAILED( m_dev->CreateIndexBuffer( ( unsigned int)size	, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &IndexBuffer, NULL) ) )
	{
		MessageBox( 0, "Creation of IndexBuffer failed", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	unsigned short *iBuf;
	// lock it
	IndexBuffer->Lock( 0, 0, ( void**)&iBuf, 0 );
	// copy data
	CNO::VECTORINDECES::pointer bodyIndexData = &tree->GetBodyIndeces( )[ 0];		
	memcpy( iBuf, bodyIndexData, size );		// copy to IB to right location determined by offset
	
	///////////////////////////////////////////////////////////////////////
	//char *ib = new char[ size * 5];
	//off = 0;
	//pomoff = 0;
	//for( int i= 0; i < size; i++)
	//{
	//	sprintf( pomc, "%i", iBuf[ i]);
	//	while( pomc[ pomoff])
	//		ib[ off++] = pomc[ pomoff++];
	//	pomoff = 0;
	//	ib[ off++] = ',';
	//	ib[ off++] = ' ';
	//}
	//ib[ off] = 0;
	//delete [] ib;
	//MessageBox( 0, ib, "IB", MB_OK);
	///////////////////////////////////////////////////////////////////////
	IndexBuffer->Unlock( );
	// set tree index buffer
	tree->SetIndexBuf( IndexBuffer);

#ifdef MYSTATS
	// set stats
	tree->stats.bodyData += ( unsigned int)size;
	tree->stats.totalGPUData += ( unsigned int)size;
#endif


	/////////////////////////////////////
	// Point Sprites vertex buffer 
	/////////////////////////////////////
	// discard old
	if( tree->GetPSVertexBuf( ) )
		tree->GetPSVertexBuf( )->Release( );

	// size for PS vertexbuffer is whole PS contaner
	size = tree->GetPS( ).size( ) * sizeof( D3DXVECTOR3);
	//	create vertex buffer
	if( FAILED( m_dev->CreateVertexBuffer( ( unsigned int)size, D3DUSAGE_WRITEONLY, CLEAR_POINT_SPRITES_FVF, D3DPOOL_MANAGED, &vertBuf, NULL) ) )
	{
		MessageBox( 0, "Creation of VertexBuffer failed", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	
	D3DXVECTOR3* PSArray;
	vertBuf->Lock( 0, 0, ( void**)&PSArray, 0);

	// get PS
	CNO::VECTORS::pointer PSdata = &tree->GetPS( )[ 0];
	// anmd copy it into VB
	memcpy( PSArray, PSdata, size);		

	vertBuf->Unlock( );

	// set tree vert buf
	tree->SetPSVertBuf( vertBuf);

#ifdef MYSTATS
	// set stats
	tree->stats.PSData += ( unsigned int)size;
	tree->stats.numOfPS += ( unsigned int)size / sizeof( D3DXVECTOR3);
	tree->stats.totalGPUData += ( unsigned int)size;
#endif

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CursorStack::DrawSegment( void)
{
	BODY_VERTEX  bodyVertexPom;

	Cursor *c = m_stack.top( );

	// check if there is what to draw
	if( c->m_segmentLength == 0)
		return;

	float radius;
	D3DXMATRIX rotMatrix;
	D3DXMATRIX backYawMatrix;

	// solve right texture vertical coords. Switching between 0 and other value accord. segmentLenght
	float tv;
	if( c->lastVertTexCoordZero)		// if last was 0.0
	{
		tv = c->m_segmentLength;		// current is segmentLenght		
		c->lastVertTexCoordZero = ! c->lastVertTexCoordZero;	// and switch flag
	}
	else		// and vice versa
	{
		tv = 0.0f;
		c->lastVertTexCoordZero = ! c->lastVertTexCoordZero;
	}
 
	// we save actual begin of segment
	D3DXVECTOR3 m_segmentBeginPosition = c->m_segmentBeginPosition;

	// then we move of segment lenght ( skip)
	c->SkipForward( );

	// check if radius is not too small
	if( c->m_thickness < MIN_RADIUS)
		return;
	
// /////////// making rotation matrix around segment axis ////////////////////////////////////////////////////////////
	// actual segment end is begin position after moving
	D3DXVECTOR3 segmentEndPosition = c->m_segmentBeginPosition;

	// so lets make rotation matrix to rotate around the rotation axis ( segmentEnd - segmentBegin)
	D3DXMatrixRotationAxis(	&rotMatrix,
													&( segmentEndPosition - m_segmentBeginPosition), 
													D3DXToRadian( 360/NUM_OF_VERT_ON) );

	// back Yawmatrix
	D3DXMatrixRotationAxis(	&backYawMatrix,
													&( segmentEndPosition - m_segmentBeginPosition), 
													D3DXToRadian( -c->m_yaw) );

//  ////////////  now lets compute first vertex of top of segment	////////////////////////////////////////////////////
	D3DXVECTOR3 normal = D3DXVECTOR3( 1, 0, 1);

	// project it with normalMatrix to get well oriented vector
	D3DXVec3TransformCoord( &normal, &normal, &( c->m_normalMatrix) );

	// normalize it so we have well oriented normalized vector of the base of segment
	D3DXVec3Normalize( &normal, &normal);

	radius = c->m_thickness/2;
	D3DXVECTOR3 topVertex = D3DXVECTOR3( radius, 0, radius);

	// now transform it with m_normalMatrix to get right oriented vector
	D3DXVec3TransformCoord( &topVertex, &topVertex, &( c->m_normalMatrix) );	

	// tranform normal and topVertex with backYaw matrix matrix
	D3DXVec3TransformCoord( &normal, &normal, &backYawMatrix );
	D3DXVec3TransformCoord( &topVertex, &topVertex, &backYawMatrix );

	// texture coords
	float tu= 0.0f;

// insert new strip ///////////////////////

	bodyVertexPom.pos = topVertex+segmentEndPosition;
	bodyVertexPom.normal = normal;
	bodyVertexPom.tu = tu;
	bodyVertexPom.tv = tv;

	m_body.push_back( bodyVertexPom);
	
	// Now we only rotate normal, baseVertex, topVertex with rotation matrix to get other points
	for( int j = 1; j < NUM_OF_VERT_ON; j++)
	{
		// rotate normal with matRotAxis matrix
		D3DXVec3TransformCoord( &normal, &normal, &rotMatrix );
		D3DXVec3TransformCoord( &topVertex, &topVertex, &rotMatrix );
		tu += 1.0f / (float)NUM_OF_VERT_ON;

			// prepare the vertex
		bodyVertexPom.pos = topVertex+segmentEndPosition;
		bodyVertexPom.normal = normal;
		bodyVertexPom.tu = tu;
		bodyVertexPom.tv = tv;

			// and add it
		m_body.push_back( bodyVertexPom);
	}

	unsigned int *indeces = c->m_lastIndeces;

// ok, now we add indeces to make trialgle list
	// right sequence : 0, 4, 1, 5, 2, 6, 3, 7
	for( int i= 0; i < NUM_OF_VERT_ON-1; i++)
	{
		// first triangle
		m_cIndeces.push_back( indeces[ i] );
		m_cIndeces.push_back( m_uNextIndex + i );		
		m_cIndeces.push_back( m_uNextIndex + i + 1 );

		// second triangle
		m_cIndeces.push_back( indeces[ i] );
		m_cIndeces.push_back( m_uNextIndex + i + 1 );	
		m_cIndeces.push_back( indeces[ i+1] );
	}
	// close the strip
	m_cIndeces.push_back( m_uNextIndex + NUM_OF_VERT_ON-1 );
	m_cIndeces.push_back( m_uNextIndex );
	m_cIndeces.push_back( indeces[ NUM_OF_VERT_ON-1] );
	
	m_cIndeces.push_back( m_uNextIndex );
	m_cIndeces.push_back( indeces[ 0] );
	m_cIndeces.push_back( indeces[ NUM_OF_VERT_ON-1] );
	
// make actual indeces last indeces
	for( int i= 0; i < NUM_OF_VERT_ON; i++)
	{
		indeces[ i] = m_uNextIndex+ i;
	}

	// increase next index cos we added NUM_OF_VERT_ON new vectors
	m_uNextIndex += NUM_OF_VERT_ON;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CursorStack::AddPS( void)
{
	D3DXVECTOR3 pomVect = D3DXVECTOR3( 0, 0, 0);
	
	// trasform it using matrix from top of the cursors that transforms it to Branch model space
	// so drawing of PS of each branch must be precendent by setting apropriate world matrix
	D3DXVec3TransformCoord( &pomVect, &pomVect, &( m_stack.top( )->m_matrix) );

	// add it into random index container	
	int index= ( rand( ) % NUMBER_OF_PS_TEXTURES);

	m_pointSprites[ index].push_back( pomVect);
	m_uNextPSIndex[ index]++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
reads floating point value from string s on initial position i
i is changed by this function
if reading is successfull i points on the char just after closing ')'
*/
float
CursorStack::ReadVal( unsigned int &i, string &s)
{
	float val = 0;			// value of the func
	int afterFP = 0;		// # numbers after .
	bool after = false;
    int sign = 1;			// sign
	
	i += 2;		// skip to firs char after (

	//  is ther s '-' at the beginning?
	switch( s[i])
	{
	case '-':
		sign = -1;
		i++;
		break;
	case '+':
		i++;
		break;
	}

	while( ( i < s.size( ) ) && ( s[ i] != ')') )
	{
		if( ( s[ i] >= '0') && ( s[ i] <= '9') )		// if the char is digit
		{
			val = ( val * 10) + ( s[ i] - '0');
			// if we are after float point . inc afterFP by 1
			if( after)
				afterFP++;
			i++;
		}
		if( s[ i] == '.')
		{
			after = true;
			i++;
		}
	}

	if( afterFP > 0)
		return sign * val / ( 10 * afterFP);
	else
		return sign * val;	
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UTIL, if there is parametr for current command return it, else return default

float
CursorStack::GetParametr( unsigned int &i, string &s)
{
	if( s[i+1] == '(')
		return ReadVal( i, s);
	else
		return m_stack.top( )->m_defAngle;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// resets this object
void
CursorStack::Reset( void)
{
	// reset members & containers
	for( int i= 0; i< NUMBER_OF_PS_TEXTURES; i++)
	{
		m_pointSprites[ i].clear( );
		m_uNextPSIndex[ i]= 0;
		m_PSIndexOff[ i] = 0;
	}
	m_body.clear( );
	m_cIndeces.clear( );	
	m_uNextIndex= 0;
	m_bodyVertexOff =0;
	m_bodyIndexOff = 0;	

	m_numOfBranchesOnLevel = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CursorStack::InsertFirstBase( void)
{
	Cursor *c = m_stack.top( );
	float radius;
	BODY_VERTEX bodyVertexPom;

	// now we compute first base of segment
	// init normal (1, 0, 1) at begining
	D3DXVECTOR3 normal = D3DXVECTOR3( 1, 0, 1);

	// normalize it so we have well oriented normalized vector of the base of segment
	D3DXVec3Normalize( &normal, &normal);

	// now lets compute first vertex of base of segment (thick, 0, thick at the begin)
	// compute radius = thickness/2
	radius = c->m_thickness/2;
	D3DXVECTOR3 baseVertex = D3DXVECTOR3( radius, 0, radius);

	// prepare rotation matrix( rotat around y-axis at the beginning)
	D3DXMATRIX rotMatrix;
	D3DXMatrixRotationY( &rotMatrix, D3DXToRadian( 360/NUM_OF_VERT_ON ) );

	// texture coords
	float tu= 0.0f;

	c->lastVertTexCoordZero = true;

		// prepare vector
	bodyVertexPom.pos = baseVertex;
	bodyVertexPom.normal = normal;
	bodyVertexPom.tu = tu;
	bodyVertexPom.tv = 0.0;
		// and add it
	m_body.push_back( bodyVertexPom);
	
// Now we only rotate normal, baseVertex, topVertex with rotation matrix to get other points
	for( int j = 1; j < NUM_OF_VERT_ON; j++)
	{
		// rotate normal with matRotAxis matrix
		D3DXVec3TransformCoord( &normal, &normal, &rotMatrix );
		D3DXVec3TransformCoord( &baseVertex, &baseVertex, &rotMatrix );
		tu += 1.0f / (float)NUM_OF_VERT_ON;

		// prepare vertex
		bodyVertexPom.pos = baseVertex;
		bodyVertexPom.normal = normal;
		bodyVertexPom.tu = tu;
		bodyVertexPom.tv = 0.0;

		// and add it
		m_body.push_back( bodyVertexPom);
	}

	// set last indeces
	for( int j = 0; j < NUM_OF_VERT_ON; j++)
	{
		c->m_lastIndeces[ j] = m_uNextIndex + j;
	}

	m_uNextIndex += NUM_OF_VERT_ON;	// increase next index cos we added NUM_OF_VERT_ON new vectors

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CursorStack::InitCenterDistances( CNO *tree)
{
	float initLenght = tree->GetLevelHeads( 0)->GetChainPointers( )->NextOnLevel->GetLenght( );
	int recLevel;
	float currDist = 0;

	for( int i= 0; i< NUM_LVLS; i++)
	{
		recLevel = m_pLSys->getRecursionLevel( i);

		// i use -1 because suppose that in each branching is shortening of segment
		tree->GetCenterDistances( )[ i] = ( recLevel - 1) * initLenght;

		// simulate decreasing of segment leght
		for( int j= 0; j< recLevel; j++)
			initLenght *= 0.9f;
	}

	// add to level 1 distance of level 2. Because we want the simplified branches centers
	// to get better visual perf. They are too much centralized around level 0 center
	tree->GetCenterDistances( )[ 1] += 0.5f * tree->GetCenterDistances( )[ 2];

	// add to 0th as much initLenghts as F in axiom
	initLenght = tree->GetLevelHeads( 0)->GetChainPointers( )->NextOnLevel->GetLenght( );
	tree->GetCenterDistances( )[ 0] += initLenght * m_pLSys->GetNumOfFInAxiom( );

}

////////////////////////////////////////////////////////////////////////////////////

float
CursorStack::CalculateCenerPointHeight( void)
{
	//// return prumer from center points
	//float v = m_centerHeights.top( );
	//int n = 1;

	//// pop back
	//m_centerHeights.pop( );

	//while( m_centerHeights.size( ) )
	//{
	//	v = v + m_centerHeights.top( );
	//	n++;
	//	m_centerHeights.pop( );
	//}

	//// divide with n
	//v = v / n;

	//return v;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
void
CursorStack::DispatchCommand( CNO *tree)
{
	/*CNO::ECommands comm = tree->GetCommandStack( ).top( );

	switch( comm)
	{
		case CNO::ECommands::FORCE_ENHNC_LVL0 :
			
			break;

		case CNO::ECommands::FORCE_SIMPL_LVL0 :
			tree->ForceSimplify( tree);
			tree->SetCurrState( IDLE);
			break;

		case CNO::ECommands::START_SIMPLIFY_LVL0 :
			
			break;

		case CNO::ECommands::START_ENHANCE_LVL0 :

			break;

		case CNO::ECommands::FORCE_ENHNC_LVL1:
			this->ForceEnhance( tree);
			tree->SetCurrState( IDLE);
			break;

		case CNO::ECommands::FORCE_SIMPL_LVL1:
			this->ForceSimplify( tree);
			tree->SetCurrState( IDLE);
			break;

		case CNO::ECommands::START_SIMPLIFY_LVL1:	

			break;

		case CNO::ECommands::START_ENHANCE_LVL1:
			
			break;

		case CNO::ECommands::FORCE_ENHNC_LVL2 :
			this->ForceEnhance( tree);
			break;

		case CNO::ECommands::FORCE_SIMPL_LVL2 :	
			this->ForceSimplify( tree);
			break;

		case CNO::ECommands::START_SIMPLIFY_LVL2 :		

			break;

		case CNO::ECommands::START_ENHANCE_LVL2 :
			if( tree->GetNumOfActiveLvls( ) != 1)
				MessageBox( 0, "NE", "NEE", MB_OK);

			break;
	}*/
}