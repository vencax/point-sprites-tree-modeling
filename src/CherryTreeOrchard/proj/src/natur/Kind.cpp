
#include <time.h>				// to retrieve time

#include "./Kind.h"


CKind::CKind( LPDIRECT3DDEVICE9 d, LPDIRECT3DVERTEXBUFFER9	suppVB, string &textPath) : 
m_dev( d),
m_SuppVertBuf( suppVB)
{
	// CREATE LSYSTEM
	m_LSystem = new LSystem( );

	// and load appropirate LS file
	char lsFile[ 64];
	sprintf( lsFile, "%slSystem.LS", &textPath[ 0]);
	m_LSystem->Load( lsFile);

	// CREATE CURSORSTACK
	m_cursorStack = new CursorStack( m_LSystem, m_dev);

	//////////////////////////////
	// init textures
	//////////////////////////////
	// level0 details
	m_textures[ 0] = new LPDIRECT3DTEXTURE9[ NUM_OF_SIMPLFD_TEXTRS0];
	ZeroMemory( m_textures[ 0], sizeof( LPDIRECT3DTEXTURE9) * NUM_OF_SIMPLFD_TEXTRS0);
	// level1 details
	m_textures[ 1] = new LPDIRECT3DTEXTURE9[ NUM_OF_SIMPLFD_TEXTRS1];
	ZeroMemory( m_textures[ 1], sizeof( LPDIRECT3DTEXTURE9) * NUM_OF_SIMPLFD_TEXTRS1);
	// level2 details
	m_textures[ 2] = new LPDIRECT3DTEXTURE9[ NUM_OF_SIMPLFD_TEXTRS2];
	ZeroMemory( m_textures[ 2], sizeof( LPDIRECT3DTEXTURE9) * NUM_OF_SIMPLFD_TEXTRS2);
	// leaves
	m_textures[ 3] = new LPDIRECT3DTEXTURE9[ NUMBER_OF_PS_TEXTURES];
	ZeroMemory( m_textures[ 3], sizeof( LPDIRECT3DTEXTURE9) * NUMBER_OF_PS_TEXTURES);
	
	m_crustTexture = NULL;

	// generate textures
	this->GenerateTextures( textPath);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CKind::~CKind(void)
{
	// delete objects
	delete( m_LSystem);
	delete( m_cursorStack);

	// delete all trees in forrest
	for( CKind::FOREST::iterator it= m_forest.begin( ); it != m_forest.end( ); it++)
		delete *it;

	// release the textures

	// level 0
	for( int i = 0; i < NUM_OF_SIMPLFD_TEXTRS0; i++)
	{
		if( m_textures[ 0][ i])
		{
			m_textures[ 0][ i]->Release( );
			m_textures[ 0][ i] = NULL;
		}
	}

	// level 1
	for( int i = 0; i < NUM_OF_SIMPLFD_TEXTRS1; i++)
	{
		if( m_textures[ 1][ i])
		{
			m_textures[ 1][ i]->Release( );
			m_textures[ 1][ i] = NULL;
		}
	}

	// level 2
	for( int i = 0; i < NUM_OF_SIMPLFD_TEXTRS2; i++)
	{
		if( m_textures[ 2][ i])
		{
			m_textures[ 2][ i]->Release( );
			m_textures[ 2][ i] = NULL;
		}
	}

	// leaves
	for( int i = 0; i < NUMBER_OF_PS_TEXTURES; i++)
	{
		if( m_textures[ 3][ i])
		{
			m_textures[ 3][ i]->Release( );
			m_textures[ 3][ i] = NULL;
		}
	}

	// delete allocated pointer for textures
	for( int i = 0; i < 4; i++)
		delete m_textures[ i];

	// crust texture
	if( m_crustTexture)
	{
		m_crustTexture->Release( );
		m_crustTexture = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CKind::TickUpdate( D3DXVECTOR3 &cameraPos)
{
	CNO *tree;
	float diff;
	float curCamDist;
	unsigned int countOps = 0;
	int level;

	for( CKind::FOREST::iterator it= m_forest.begin( ); it != m_forest.end( ); it++)
	{
		tree = ( *it);
		curCamDist = D3DXVec3Length( &cameraPos);

		switch( tree->GetCurrState( ) )
		{
			case CNO::EState::IDLE :
				// do nothing
				break;

			case CNO::EState::ACTION :	// something is happening

				countOps = 0;
				level = tree->GetNumOfActiveLvls( )-1;

				// we solve the direction we move towards tree. If it is positive, we are reciding the tree and
				diff = curCamDist - tree->GetLastDistanceInAnnulus( );

				// simplify operation will be performed
				if( diff > 0)
				{
					// simplify next branches
					while( ( tree->GetLevelHeads( level)->GetChainPointers( )->NextInBodyDraw != 0) && 
						( countOps < ( unsigned int)tree->GetLoDSteps( )[ tree->GetNumOfActiveLvls( )-1] ) )
					{
						tree->Simplify( );
						countOps++;
					}
				}
				else if( diff < 0)
				{
					// enhance next branch
					while( ( tree->GetLevelHeads( level)->GetChainPointers( )->NextSimplified != 0) && 
						( countOps < ( unsigned int)tree->GetLoDSteps( )[ tree->GetNumOfActiveLvls( )-1] ) )
					{
						tree->Enhance( );
						countOps++;
					}					
				}
				break;
		}

		// update lastAnnulusDistance
		tree->SetLastDistanceInAnnulus( curCamDist);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
CKind::Update( D3DXVECTOR3 &cameraPos)
{
	int dist = 0;
	int dir = 0;		// flag if we are crossing the borders from or to the tree ( simplify/enhance)

	CNO *tree;
	//go through whole forest and check distanece of each tree
	for( CKind::FOREST::iterator it= m_forest.begin( ); it != m_forest.end( ); it++)
	{
		tree = ( *it);
		dist = ( int)D3DXVec3Length( &( tree->GetPosition( ) - cameraPos) );

		///////////////////////////////
		// Level 2 circle. Full details
		///////////////////////////////
		if( ( dist <= LVL2_BORDER) )		
		{
			if( ( tree->GetLastDistance( ) <= LVL2_BORDER) )
			{
				// do nothing. Cos we are at the same circle
				continue;
			}
			else
			{							
				// If we are on This circle we must be coming closer. So Force enhance level2 to full details 
				tree->ForceEnhance( );

				// and set status to IDLE
				tree->SetCurrState( CNO::EState::IDLE);

				// update last distance
				tree->SetLastDistance( dist);

				// and skip on the next tree
				continue;
			}
		}


		///////////////////////////////
		// Leve 2 annunlus
		///////////////////////////////
		if( ( dist <= LVL2ANNULUS_BORDER) && ( dist > LVL2_BORDER) )		
		{
			if( ( tree->GetLastDistance( ) <= LVL2ANNULUS_BORDER) &&
				( tree->GetLastDistance( ) > LVL2_BORDER) )
			{
				// do nothing. Cos we are at the same circle
				continue;
			}
			else
			{
				// take apropriate action
				dir = dist - tree->GetLastDistance( );
				
				if( dir > 0)	// we are receding the tree. So start simplify BranchTree level2( full details)
					tree->SetCurrState( CNO::EState::ACTION);
				else	// we are coming closer, so start enhance to full details
				{
					// firstly we have to increase active levels
					m_cursorStack->IncreaseLevel( tree);
			
					tree->SetCurrState( CNO::EState::ACTION);
				}

				// update last distance
				tree->SetLastDistance( dist);

				// and skip on the next tree
				continue;
			}
		}

		///////////////////////////////
		// level 1 detail circle
		///////////////////////////////
		if( ( dist <= LVL1_BORDER) && ( dist > LVL2ANNULUS_BORDER) )		
		{
			if( ( tree->GetLastDistance( ) <= LVL1_BORDER) &&
				( tree->GetLastDistance( ) > LVL2ANNULUS_BORDER) )
			{
				// do nothing. Cos we are at the same circle
				continue;
			}
			else
			{
				// take apropriate action
				dir = dist - tree->GetLastDistance( );
				
				if( dir > 0)	// we are receding the tree. Force level2 to simplify
				{
					tree->ForceSimplify( );

					// release level
					m_cursorStack->DecreaseLevel( tree);
				}
				else	// we are coming closer
					tree->ForceEnhance( );

				// and set status to IDLE
				tree->SetCurrState( CNO::EState::IDLE);

				// update last distance
				tree->SetLastDistance( dist);

				// and skip on the next tree
				continue;
			}
		}

		///////////////////////////////
		// level 1 detail annulus
		///////////////////////////////
		if( ( dist <= LVL1ANNULUS_BORDER) && ( dist > LVL1_BORDER) )		
		{
			if( ( tree->GetLastDistance( ) <= LVL1ANNULUS_BORDER) &&
				( tree->GetLastDistance( ) > LVL1_BORDER) )
			{
				// do nothing. Cos we are at the same circle
				continue;
			}
			else
			{
				// take apropriate action
				dir = dist - tree->GetLastDistance( );

				if( dir > 0)	// we are receding the tree
					tree->SetCurrState( CNO::EState::ACTION);	
				else	// we are coming closer
				{
					// firstly we have to increase active levels
					m_cursorStack->IncreaseLevel( tree);

					tree->SetCurrState( CNO::EState::ACTION);
				}

				// update last distance
				tree->SetLastDistance( dist);

				// and skip on the next tree
				continue;
			}
		}

		///////////////////////////////
		// level 0 detail circle
		///////////////////////////////
		if( ( dist <= LVL0_BORDER) && ( dist > LVL1ANNULUS_BORDER) )		
		{
			if( ( tree->GetLastDistance( ) <= LVL0_BORDER) &&
				( tree->GetLastDistance( ) > LVL1ANNULUS_BORDER) )
			{
				// do nothing. Cos we are at the same circle
				continue;
			}
			else
			{
				// take apropriate action
				dir = dist - tree->GetLastDistance( );
				
				if( dir > 0)	// we are receding the tree
				{
					tree->ForceSimplify( );

					// release level
					m_cursorStack->DecreaseLevel( tree);
					
				}
				else	// we are coming closer
				{
					// dont force enhance, because we are comming from 0th level, that is always enhanced
					// tree->ForceEnhance( );
				}

				// and set status to IDLE
				tree->SetCurrState( CNO::EState::IDLE);

				// update last distance
				tree->SetLastDistance( dist);

				// and skip on the next tree
				continue;
			}
		}

		///////////////////////////////
		// level 0 detail annulus
		///////////////////////////////
		if( ( dist <= LVL0ANNULUS_BORDER) && ( dist > LVL0_BORDER) )		
		{
			if( ( tree->GetLastDistance( ) <= LVL0ANNULUS_BORDER) &&
				( tree->GetLastDistance( ) > LVL0_BORDER) )
			{
				// do nothing. Cos we are at the same circle
				continue;
			}
			else
			{
				// take apropriate action
				dir = dist - tree->GetLastDistance( );
				
				if( dir > 0)	// we are receding the tree
				{
					// draw BODY and even 1 PS representing crown of the tree					
				}
				else	// we are coming closer
				{
					// respawn or something
				}
				
				// and set status to IDLE
				tree->SetCurrState( CNO::EState::IDLE);

				// update last distance
				tree->SetLastDistance( dist);

				// and skip on the next tree
				continue;
			}
		}

		/////////////////////////
		// outer space
		////////////////////////
		if( ( dist > LVL0ANNULUS_BORDER) )		
		{
			if( ( tree->GetLastDistance( ) > LVL0ANNULUS_BORDER) )
			{
				// do nothing. Cos we are at the same circle
				continue;
			}
			else
			{				
				// delete or something

				// update last distance
				tree->SetLastDistance( dist);

				// and skip on the next tree
				continue;
			}
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// creates and adds new tree into forrest on spec. position
// create first branch and generate it. Also generate others branches
void
CKind::AddTree( D3DXVECTOR3 &pos, D3DXVECTOR3 &camPos)
{	
	D3DXMATRIX posMatrix;

	// create new tree with random seed
	CNO *tree = new CNO( rand( ), pos);

	// prepare new tree

	// LOCK RAND MUTEX
	srand( tree->GetSeed( ) );

	// now we generate random initial thickness, lenght, defaultAngle differencies from LS values
	// to make every tree planting with different thickness, defaultAngle, lenght
	// this differencies has to be small to disable to much different exemplares

	float initDefAngle = m_LSystem->getDefaultAngle( ) + ( -DIFF + ( rand( ) % (2*DIFF) ) );
	float initThick = m_LSystem->getDefaultThick( ) + ( 0.005f* ( -DIFF + ( rand( ) % (2*DIFF) ) ) );		// allow 2*DIFF% difference
	float initLenght = m_LSystem->GetDefaultLenght( ) + (  0.01f* ( -DIFF + ( rand( ) % (2*DIFF) ) ) );

	// as first we generate random orientation of the tree
	// to make every tree planting with differen orientation
	float initYaw = D3DXToRadian( ( float)( rand( ) % 359));	// the tree can be rotated anywhere arout y axis	
	float initPitch = D3DXToRadian( ( float)( -DIFF + ( rand( ) % (2*DIFF) ) ));
	float initRoll = D3DXToRadian( ( float)( -DIFF + ( rand( ) % (2*DIFF) ) ));

	// and create init matrix
	D3DXMATRIX init;
	D3DXMatrixRotationYawPitchRoll( &init, initYaw, initPitch, initRoll);

	// combine it with position matrix
	D3DXMatrixTranslation( &posMatrix, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply( &posMatrix, &init, &posMatrix);

	// set it to new tree's root
	tree->SetWorldMatrix( posMatrix);

	///////////////////////////////////////////////////////////////
	// Init 0th level
	///////////////////////////////////////////////////////////////
	D3DXMatrixIdentity( &init);

	// create base tree skelet( first branch) with creation of new CBranch
	CBranch *root = new CBranch( 
		init,
		tree->GetWorldMatrix( ),
		'X', 
		initLenght,
		initThick,
		initDefAngle,
		0,
		( unsigned int)rand( )
		);

	// UNLOCK RAND MUTEX	

	// set root as 0th level
	tree->GetLevelHeads( 0)->GetChainPointers( )->NextOnLevel = root;

	// insert new tree
	m_forest.push_back( tree);
	
#ifdef MYSTATS
	tree->stats.totalSysData += sizeof( CBranch);
#endif

	/////////////////////////////////////////
	// init center distances
	////////////////////////////////////////
	m_cursorStack->InitCenterDistances( tree);

	//////////////////////////////////////////////
	// solve center of 0th branch
	//////////////////////////////////////////////
	// make translate matrix already multiplied with toWorld matrix
	float distToCenter = tree->GetCenterDistances( )[ root->GetLevel( )];
	D3DXMatrixTranslation( &( root->GetCenterMatrix( ) ), 0, distToCenter, 0);
	D3DXMatrixMultiply( &root->GetCenterMatrix( ), &root->GetCenterMatrix( ), &root->GetWorldMatrix( ) );

	// init tree's data
	this->InitTree( tree, camPos);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inits tree data( num of active levels in accordance to its position from cam
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CKind::InitTree( CNO *tree, D3DXVECTOR3 &camPos)
{
	// generate and enhance 0th level. Always
	m_cursorStack->IncreaseLevel( tree);
	tree->ForceEnhance( );
	
	// generate active levels according camera position
	float annulusDist = D3DXVec3Length( &( tree->GetPosition( ) - camPos) );
	int dist = ( int)annulusDist;

	// we have now guarantee that there will be right count of active levels
	// according camera position to begin with and apropriate status
	// we use the same system of intervals as in UpdateProcedure with one difference
	// we omit annuli, because we don't know what is the direction of camera because
	// we dont have last camera position
	// so we rather generate and simplify as much levels as needed in accordance
	// which circle we are

	///////////////////////////////
	// Level 2 circle. Full details. 
	///////////////////////////////
	if( ( dist <= LVL2ANNULUS_BORDER) )		
	{						
		// Generate and force Enhance all levels
		for( int i= 0; i< NUM_LVLS-1; i++)
		{
			m_cursorStack->IncreaseLevel( tree);
			tree->ForceEnhance( );
		}

		// update last distance && distanceInAnnulus
		tree->SetLastDistance( dist);
		tree->SetLastDistanceInAnnulus( annulusDist);
		
		// and set state. according if it is in annulus or circle
		if( dist <= LVL2_BORDER)
			tree->SetCurrState( CNO::EState::IDLE);
		else
			tree->SetCurrState( CNO::EState::ACTION);

		return;
	}

	///////////////////////////////
	// level 1 detail circle
	///////////////////////////////
	if( ( dist <= LVL1ANNULUS_BORDER) )		
	{
		// Generate and force Enhance all levels
		for( int i= 0; i< NUM_LVLS-2; i++)
		{
			m_cursorStack->IncreaseLevel( tree);
			tree->ForceEnhance( );
		}

		// update last distance && distanceInAnnulus
		tree->SetLastDistance( dist);
		tree->SetLastDistanceInAnnulus( annulusDist);

		// and set state. according if it is in annulus or circle
		if( dist <= LVL1_BORDER)
			tree->SetCurrState( CNO::EState::IDLE);
		else
			tree->SetCurrState( CNO::EState::ACTION);

		return;
	}	

	///////////////////////////////
	// 3rd level detail circle
	///////////////////////////////
	if( ( dist <= LVL0ANNULUS_BORDER) )		
	{
		// dont generate anymore, because 0th level is already gen.

		// update last distance && distanceInAnnulus
		tree->SetLastDistance( dist);
		tree->SetLastDistanceInAnnulus( annulusDist);

		// and set state. according if it is in annulus or circle
		if( dist <= LVL0_BORDER)
			tree->SetCurrState( CNO::EState::IDLE);
		else
			// in this case idle as well. Its 0level
			tree->SetCurrState( CNO::EState::IDLE);

		return;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draw all trees in forest
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CKind::Draw( void)
{
	CNO *tree;

	///////////////////////
	// draw bodies
	///////////////////////

	// set crust texture
	if( m_crustTexture)
		m_dev->SetTexture( 0, m_crustTexture);	

	// turn on lighting
		m_dev->SetRenderState( D3DRS_LIGHTING, true);
	
	for( FOREST::iterator it= m_forest.begin( ); it != m_forest.end( ); it++)
	{
		tree = (*it);
		tree->Draw( m_dev);
	}

	for( FOREST::iterator it= m_forest.begin( ); it != m_forest.end( ); it++)
	{
		tree = (*it);

		// if tree is further than level 0 border draw only one Point Sprite representing crown
		if( tree->GetLastDistance( ) > LVL0_BORDER)
		{
			DrawCrown( tree);
		}
		// draw other elements
		else	
		{
			this->DrawPS( tree);
			this->DrawSiplifiedBranches( tree);
		}

	}

	// set ideintity to world matrix
	D3DXMATRIX  mat;
	D3DXMatrixIdentity( &mat);
	m_dev->SetTransform( D3DTS_WORLD, &mat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
CKind::DrawPS( CNO *tree)
{		
	CBranch::sPSNode *node;	

	this->EnablePointSprites( 3);

	// set stream source
	m_dev->SetFVF( CLEAR_POINT_SPRITES_FVF);
	m_dev->SetStreamSource( 0, tree->GetPSVertexBuf( ), 0, sizeof( D3DXVECTOR3) );

	CBranch *curr;
	// draw separately each type of PointSprite in each active levels
	for( int i = 0; i < NUMBER_OF_PS_TEXTURES; i++)
	{
		// set texture
		m_dev->SetTexture( 0, m_textures[ 3][ i] );			

		// go through active levels
		for( int j= 0; j < tree->GetNumOfActiveLvls( ); j++)
		{
			curr = tree->GetLevelHeads( j)->GetChainPointers( )->NextInPSDraw[ i];
			while( curr)
			{
				// set world matrix
				m_dev->SetTransform( D3DTS_WORLD, &curr->GetWorldMatrix( ) );
				
				node= curr->GetPSNode( i);

				if( D3D_OK != m_dev->DrawPrimitive( D3DPT_POINTLIST, node->startVertex, node->count) )
				{
					MessageBox( 0, "Rendering of PS Index buffer failed", "FAILURE", MB_OK);
					return false;
				}
				
				// skip on next
				curr = curr->GetChainPointers( )->NextInPSDraw[ i];
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool
CKind::DrawSiplifiedBranches( CNO *tree)
{
	// go through CurrentActive branch and draw simplified Chain
	int level = tree->GetNumOfActiveLvls( )-1;
		
	// select supportive vertex buffer to draw from
	m_dev->SetFVF( CLEAR_POINT_SPRITES_FVF);
	m_dev->SetStreamSource( 0, m_SuppVertBuf, 0, sizeof( D3DXVECTOR3) );
	
	CBranch *curr;
	curr = tree->GetLevelHeads( level)->GetChainPointers( )->NextSimplified;

	if( curr)
	{
		// set apropriate size
		this->SetPSSize( ( int)curr->GetLevel( ) );
	}

	while( curr)
	{
		// set world matrix
		m_dev->SetTransform( D3DTS_WORLD, &curr->GetCenterMatrix( ) );
		
		// set right texture
		m_dev->SetTexture( 0, m_textures[ level][ curr->GetSimplTexIndx( )] );

		if( D3D_OK != m_dev->DrawPrimitive( D3DPT_POINTLIST, 0, 1) )
		{
			MessageBox( 0, "Rendering of PS Index buffer failed", "FAILURE", MB_OK);
			return false;
		}
				
		// skip on next
		curr = curr->GetChainPointers( )->NextSimplified;
	}

	// and draw one level above curr active level because there are simplified branches
	//  that corresponds to branches of CAL.( their children). 
	// NOT for NUM_LVLS-1 th level( the last level. It has no children
	if( level < NUM_LVLS-1)
	{
		curr = tree->GetLevelHeads( level+1)->GetChainPointers( )->NextSimplified;
		
		if( curr)
		{
			this->SetPSSize( ( int)curr->GetLevel( ) );
		}

		while( curr)
		{
			// do not draw it it it has not draw flag
			if( curr->GetStatus( ) == CBranch::SIMPLIFIED_BUT_NOT_DRAWN)
			{
				// skip on next
				curr = curr->GetChainPointers( )->NextSimplified;
				continue;
			}
				
			// set world matrix
			m_dev->SetTransform( D3DTS_WORLD, &curr->GetCenterMatrix( ) );
			
			// set right texture
			m_dev->SetTexture( 0, m_textures[ level+1][ curr->GetSimplTexIndx( )] );

			if( D3D_OK != m_dev->DrawPrimitive( D3DPT_POINTLIST, 0, 1) )
			{
				MessageBox( 0, "Rendering of PS Index buffer failed", "FAILURE", MB_OK);
				return false;
			}
					
			// skip on next
			curr = curr->GetChainPointers( )->NextSimplified;
		}
	}

	m_dev->SetTexture( 0, NULL );

	this->DisablePointSprites( );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool
CKind::DrawCrown( CNO* tree)
{

	this->EnablePointSprites( 0);

	CBranch *curr;
	curr = tree->GetLevelHeads( 0)->GetChainPointers( )->NextOnLevel;

	// select supportive vertex buffer to draw from
	m_dev->SetFVF( CLEAR_POINT_SPRITES_FVF);
	m_dev->SetStreamSource( 0, m_SuppVertBuf, 0, sizeof( D3DXVECTOR3) );
	
	// set world matrix
	m_dev->SetTransform( D3DTS_WORLD, &curr->GetCenterMatrix( ) );
	
	// set right texture
	m_dev->SetTexture( 0, m_textures[ 0][ curr->GetSimplTexIndx( )] );

	if( D3D_OK != m_dev->DrawPrimitive( D3DPT_POINTLIST, 0, 1) )
	{
		MessageBox( 0, "Rendering of PS Index buffer failed", "FAILURE", MB_OK);
		return false;
	}

	m_dev->SetTexture( 0, NULL );

	this->DisablePointSprites( );

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool
CKind::GenerateTextures( string &path)
{
	char *pom = new char[ path.size( ) + 20];

	DWORD keyColor = 0xffffffff;

	/////////////////////////////////////////
	// create leaves textures
	/////////////////////////////////////////
	for( int i = 0; i < NUMBER_OF_PS_TEXTURES; i++)
	{
		// discard the old one
		if( m_textures[ 3][ i] != NULL)
			m_textures[ 3][ i]->Release( );

		sprintf( pom, "%s%d.png", &path[ 0], i);

		if( FAILED( D3DXCreateTextureFromFileExA( m_dev, pom, D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,keyColor,NULL,NULL,&m_textures[ 3][ i]) ) )
		{
			MessageBox( 0, "Creation of texture failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}			
	}	

	/////////////////////////////////////////
	// other levels textures
	///////////////////////////////////////
	// level 0
	for( int i = 0; i < NUM_OF_SIMPLFD_TEXTRS0; i++)
	{
		// discard the old one
		if( m_textures[ 0][ i] != NULL)
			m_textures[ 0][ i]->Release( );

		sprintf( pom, "%slvl0Tex%d.png", &path[ 0], i);

		if( FAILED( D3DXCreateTextureFromFileExA( m_dev, pom, D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,keyColor,NULL,NULL,&m_textures[ 0][ i]) ) )
		{
			MessageBox( 0, "Creation of texture failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}			
	}

	// level 1
	for( int i = 0; i < NUM_OF_SIMPLFD_TEXTRS1; i++)
	{
		// discard the old one
		if( m_textures[ 1][ i] != NULL)
			m_textures[ 1][ i]->Release( );

		sprintf( pom, "%slvl1Tex%d.png", &path[ 0], i);

		if( FAILED( D3DXCreateTextureFromFileExA( m_dev, pom, D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,keyColor,NULL,NULL,&m_textures[ 1][ i]) ) )
		{
			MessageBox( 0, "Creation of texture failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}			
	}

	// level 2
	for( int i = 0; i < NUM_OF_SIMPLFD_TEXTRS2; i++)
	{
		// discard the old one
		if( m_textures[ 2][ i] != NULL)
			m_textures[ 2][ i]->Release( );

		sprintf( pom, "%slvl2Tex%d.png", &path[ 0], i);

		if( FAILED( D3DXCreateTextureFromFileExA( m_dev, pom, D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,NULL,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,keyColor,NULL,NULL,&m_textures[ 2][ i]) ) )
		{
			MessageBox( 0, "Creation of texture failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}			
	}
	
	// crust texture
	sprintf( pom, "%scrust.png", &path[ 0]);

	if( FAILED( D3DXCreateTextureFromFile( m_dev, pom, &m_crustTexture) ) )
	{
		MessageBox( 0, "Creation of crust texture failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	delete pom;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CKind::EnablePointSprites( int level)
{
	// turn off lighting
	m_dev->SetRenderState( D3DRS_LIGHTING, false);

	m_dev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	//m_dev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	//m_dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	//Set up the render states for using point sprites...
	m_dev->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );    // Turn on point sprites
	m_dev->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );    // Allow sprites to be scaled with distance
	this->SetPSSize( level);
	m_dev->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(1.2f) ); // Float value that specifies the minimum size of point primitives. Point primitives are clamped to this size during rendering. 
	m_dev->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(1.0f) ); // Default 1.0
	m_dev->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.0f) ); // Default 0.0
	m_dev->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.0f) ); // Default 0.0
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CKind::SetPSSize( int level)
{
	m_dev->SetRenderState( D3DRS_POINTSIZE,     FtoDW(m_LSystem->GetPSSizes( )[ level]) );		// Float value that specifies the size to use for point size computation in cases where point size is not specified for each vertex.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
CKind::DisablePointSprites( void)
{
	m_dev->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	m_dev->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

	m_dev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	//m_dev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}