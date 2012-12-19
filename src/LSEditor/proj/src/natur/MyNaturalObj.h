#pragma once

#include <string>
#include <stdlib.h>				// for random numbers
#include <d3dx9.h>	// DirectX9

#include <stack>	

#include "./Branch.h"

typedef struct stat_ {
		// SysData
		unsigned int totalSysData;
		unsigned int PSNodesData;
		unsigned int BodyNodesData;		
		
		// buffers on GPU
		unsigned int bodyData;
		unsigned int PSData;
		unsigned int totalGPUData;
		unsigned int numOfPS;
		unsigned int numOfVertices;
	} MyStats;

#define MYSTATS		// enables memory statistics

using namespace std;

class CNO
{

public:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// typedefs 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	typedef		vector< D3DXVECTOR3>	VECTORS;
	typedef		vector< BODY_VERTEX>	BODYVECTORS;
	typedef		vector< unsigned short>	VECTORINDECES;
	typedef		stack< CBranch *> BRANCHSTACK; 

	enum EState {
		IDLE,
		ACTION,
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ctor and destor 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CNO( unsigned int, D3DXVECTOR3 &);
	~CNO( );


	////////////////////////////////////////////////////////////////////
	// containers gathering branches to be drawn
	////////////////////////////////////////////////////////////////////
	
private: CBranch m_LevelHeads[ NUM_LVLS];	// pointer to begins of BranchTree levels
public: inline CBranch* GetLevelHeads( int i)	{ return &m_LevelHeads[ i];	}

	//////////////////////////////
	// WORLD MATRX
	// position and orienation to origin of the world
	//////////////////////////////
private:	D3DXMATRIX	m_worldMatrix;
public:	inline D3DXMATRIX &GetWorldMatrix( void)	{ return m_worldMatrix;	}
public:	inline void	SetWorldMatrix( D3DXMATRIX &m)	{ m_worldMatrix = m;	}

private:	D3DXVECTOR3	m_position;
public:	inline D3DXVECTOR3 &GetPosition( void)	{ return m_position;	}


private:	int	m_lastDistance;
public:	inline int GetLastDistance( void)	{ return m_lastDistance;	}
public:	inline void	SetLastDistance( int m)	{ m_lastDistance = m;	}

private:	float	m_lastDistanceInAnnulus;
public:	inline float GetLastDistanceInAnnulus( void)	{ return m_lastDistanceInAnnulus;	}
public:	inline void	SetLastDistanceInAnnulus( float m)	{ m_lastDistanceInAnnulus = m;	}
		

	////////////////////////////////////////
	// SEED
	////////////////////////////////////////
private:	unsigned int m_seed;
public:	inline unsigned int GetSeed( void)	{ return m_seed;	}
public:	inline void SetSeed( unsigned int s)	{ m_seed = s;	}

	////////////////////////////////////////////////////////////////////
	// resources for storing data for each level of BranchTree
	////////////////////////////////////////////////////////////////////
	// BODY vertex buf for all vertices and normals and tex coords of the body 

private: LPDIRECT3DVERTEXBUFFER9	m_bodyVertBuf;
public: inline	LPDIRECT3DVERTEXBUFFER9			GetBodyVertexBuf( void)	{	return m_bodyVertBuf;	}
public: inline	void		SetBodyVertBuf( LPDIRECT3DVERTEXBUFFER9 val)	{	m_bodyVertBuf = val;	}

	// BODY index buf

private: LPDIRECT3DINDEXBUFFER9 m_indexBuffer;
public: inline	LPDIRECT3DINDEXBUFFER9				GetIndexBuf( void)	{	return m_indexBuffer;	}
public: inline	void		SetIndexBuf( LPDIRECT3DINDEXBUFFER9 val)	{	m_indexBuffer = val;	}

	// PS vertex buffer for all PointSprites

private: LPDIRECT3DVERTEXBUFFER9	m_PSVertBuf;
public: inline	LPDIRECT3DVERTEXBUFFER9			GetPSVertexBuf( void)	{	return m_PSVertBuf;	}	
public: inline	void		SetPSVertBuf( LPDIRECT3DVERTEXBUFFER9 val)	{	m_PSVertBuf = val;	}

		//////////////////////////////////////////////////////////////////////
		// Arrays that store data for buffers on GPU
		//////////////////////////////////////////////////////////////////////

private:	BODYVECTORS	m_bodyVertices;
public:	BODYVECTORS  &GetBodyVertices( void)	{	return m_bodyVertices;	}

private:	VECTORINDECES	m_bodyIndeces;
public:	VECTORINDECES  &GetBodyIndeces( void)	{	return m_bodyIndeces;	}

private:	VECTORS	m_PS;
public:	VECTORS  &GetPS( void)	{	return m_PS;	}

	// # of active levels. 0 - NUM_LVLS
private: unsigned short	m_curActiveLevels;		
public: unsigned short	GetNumOfActiveLvls( void)	{	return m_curActiveLevels;	}
public: void SetNumOfActiveLvls( unsigned short i)	{	m_curActiveLevels = i;	}
public: void IncNumOfActiveLvls( void)		{	m_curActiveLevels++;	}
public: void DecNumOfActiveLvls( void)		{	m_curActiveLevels--;	}

		private:	unsigned int m_levelBodyDataBeginsAt[ NUM_LVLS];	// where begins PS on i-th level in datacontainer
public:	unsigned int GetLevelBodyDataBeginsAt( int level)	{	return m_levelBodyDataBeginsAt[ level];	}
public:	void	SetLevelBodyDataBeginsAt( unsigned int size, int level)	{	m_levelBodyDataBeginsAt[ level] = size;	}

private:	unsigned int m_levelBodyIndexDataBeginsAt[ NUM_LVLS];	// where begins indeces on i-th level in datacontainer
public:	unsigned int GetLevelBodyIndexDataBeginsAt( int level)	{	return m_levelBodyIndexDataBeginsAt[ level];	}
public:	void	SetLevelBodyIndexDataBeginsAt( unsigned int size, int level)	{	m_levelBodyIndexDataBeginsAt[ level] = size;	}

		// where begins Body vertices on i-th level in datacontainer
private:	unsigned int m_levelPSDataBeginsAt[ NUM_LVLS];
public:	inline unsigned int GetLevelPSDataBeginsAt( int level)	{	return m_levelPSDataBeginsAt[ level];	}
public:	inline void	SetLevelPSDataBeginsAt( unsigned int size, int level)	{	m_levelPSDataBeginsAt[ level] = size;	}

private:	CNO::BRANCHSTACK m_simplBrStack;
public:	inline CNO::BRANCHSTACK &GetCurEndSiplifiedStack( void)	{	return m_simplBrStack;	}

//private:	TCammandStack m_commandStack;
//public:	inline CNO::TCammandStack &GetCommandStack( void)	{	return m_commandStack;	}

private:	CNO::EState m_currState;
public:	inline CNO::EState GetCurrState( void)	{	return m_currState;	}
public:	inline void SetCurrState( CNO::EState s)	{	m_currState = s;	}

		// indicate how many LoD oprations will be performed in one tick
private:	char	m_LoDSteps[ NUM_LVLS];
public:	inline char *GetLoDSteps( void)	{ return m_LoDSteps;	}
public:	inline void	SetLoDSteps( char c, int level)	{ m_LoDSteps[ level] = c;	}

		// distances of centers of branches from its bases
private:	float	m_centerDistances[ NUM_LVLS];
public:	inline float *GetCenterDistances( void)	{ return m_centerDistances;	}
public:	inline void	SetCenterDistances( float c, int level)	{ m_centerDistances[ level] = c;	}

	///////////////////////////////////////////////////
	// member functions	
	///////////////////////////////////////////////////

	bool	Draw( LPDIRECT3DDEVICE9, bool);		//	 draws the body
	void	DisposeLevel( int);		// disposes buffers of given level
	void	DeleteLevel( int);

	void	Simplify( void);
	void	Enhance( void);

	void	ForceSimplify( void);
	void	ForceEnhance( void);

	MyStats stats;

#ifdef MYSTATS
	void ResetStats( void)	{	ZeroMemory( &stats, sizeof( stats) ); stats.totalSysData = sizeof( this);	}
#endif
	

	// utils
	
};