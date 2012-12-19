#pragma once

#include <d3dx9.h>	// DirectX9

#define NUM_OF_VERT_ON 4  // 3->trangle
#define MIN_RADIUS 0.008f

class Cursor {

	
public:
	
	Cursor( float, float, float);	//  Parameterized contructor
	Cursor( Cursor*);		// copy ctor
	
	// skelet
	D3DXMATRIX	m_matrix;		// matrix that holds the position and orientation of cursor			

	float						m_length,
								m_thickness,
								m_defAngle,

								m_yaw,
								m_pitch,
								m_roll;

	// plast
	D3DXMATRIX	m_normalMatrix;		// matrix that holds only orientation of cursor for normals
								
	float  m_segmentLength;		// lenght of actual segment

	// initial values to draw segment after we know how long it is
	D3DXVECTOR3 m_segmentBeginPosition;	// initial position

	unsigned int m_lastIndeces[ NUM_OF_VERT_ON];
	bool lastVertTexCoordZero;		// flag if there was tv coord zero

	//	functions
	void		GoForward( void);		/* move the top cursor forward a actual lenght unit*/
	void		SkipForward( void);		/* move the top cursor forward a actual lenght unit without drawing*/
	void		pitch( float);		/* pitches around the X-axis */
	void		yaw( float);		/* yaws around the Y-axis */
	void		roll( float);			/* rolls around the Z-axis */
	// special commands
	void		YawUntilHorizontal( void);
	inline		void		ChangeLength( float m)		{		m_length *= m;		}	
	inline		void		ChangeThickness( float m)	{		m_thickness *= m;	}
	inline void	ChangeDefaultAngle( float m)		{	m_defAngle = ( m_defAngle * m);	}

	void		YawPitchRollRandom( int);	// not implemented

};
