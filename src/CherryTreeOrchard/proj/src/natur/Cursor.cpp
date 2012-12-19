
#include "./Cursor.h"

//#define RATIO 2
//#define INITIAL_RADIUS 1
//#define INITIAL_LENGHT 1

////////////////////////////////////////////////////////////////////
/* Parametrized Constructor */

Cursor::Cursor( float l, float t, float a) :
		m_length( l), m_thickness( t), m_defAngle( a)
{
	m_yaw = 0.0f;
	m_pitch = 0.0f;
	m_roll = 0.0f;

	// load identity matrix at the beginning
	D3DXMatrixIdentity( &m_matrix);

	m_segmentLength = 0;
	m_normalMatrix = m_matrix;

	// begin is in origin at the beginning
	m_segmentBeginPosition = D3DXVECTOR3( 0, 0, 0);
}

////////////////////////////////////////////////////////////////////
// copy ctor

Cursor::Cursor( Cursor* c)
{
	m_matrix = c->m_matrix;

	m_length = c->m_length;
	m_defAngle = c->m_defAngle;
	m_thickness = c->m_thickness;

	m_yaw = c->m_yaw;
	m_pitch = c->m_pitch;
	m_roll = c->m_roll;	

	m_normalMatrix = c->m_normalMatrix;
	m_segmentLength = c->m_segmentLength;
	m_segmentBeginPosition = c->m_segmentBeginPosition;

	lastVertTexCoordZero = c->lastVertTexCoordZero;

	// copy last indeces
	for( int i= 0; i< NUM_OF_VERT_ON; i++)
		m_lastIndeces[ i] = c->m_lastIndeces[ i];
}


////////////////////////////////////////////////////////////////////
/* move the top cursor forward a unit*/

void Cursor::GoForward( void)
{
	//D3DXMATRIX temp;	
	//D3DXMatrixMultiply( &m_matrix, D3DXMatrixTranslation( &temp, 0.0f, m_length, 0.0f), &m_matrix);

	m_segmentLength += m_length;
}

////////////////////////////////////////////////////////////////////
void Cursor::SkipForward( void)
{
	D3DXMATRIX tmp;

	// first move m_matrix with m_segmentLength
	D3DXMatrixMultiply( &m_matrix, D3DXMatrixTranslation( &tmp, 0.0f, m_segmentLength, 0.0f), &m_matrix );

	// then we calculate new segment begin ( new m_segmentBeginPosition is actual end)
	m_segmentBeginPosition = D3DXVECTOR3( 0, 0, 0);
	D3DXVec3TransformCoord( &m_segmentBeginPosition, &m_segmentBeginPosition, &m_matrix );

	// reset m_segmentLength
	m_segmentLength = 0;
}

////////////////////////////////////////////////////////////////////
/* pitches around the X-axis */

void Cursor::pitch( float angle)
{
	//	set m_pitch
	m_pitch += angle;

	D3DXMATRIX temp;	
	D3DXMatrixRotationX( &temp, D3DXToRadian( angle) );
	D3DXMatrixMultiply(&m_matrix, &temp, &m_matrix ); 	
	D3DXMatrixMultiply(&m_normalMatrix, &temp, &m_normalMatrix );
}

////////////////////////////////////////////////////////////////////
/* yaws around the Y-axis */

void Cursor::yaw( float angle)
{
	m_yaw += angle;

	D3DXMATRIX temp;		
	D3DXMatrixRotationY( &temp, D3DXToRadian( angle) );
	D3DXMatrixMultiply( &m_matrix, &temp,  &m_matrix);
	D3DXMatrixMultiply( &m_normalMatrix, &temp,  &m_normalMatrix);
}

////////////////////////////////////////////////////////////////////
/* rolls around the Z-axis */

void Cursor::roll( float angle)
{
	m_roll += angle;

	D3DXMATRIX temp;		
	D3DXMatrixRotationZ( &temp, D3DXToRadian( angle) );
	D3DXMatrixMultiply(&m_matrix, &temp, &m_matrix ); 	
	D3DXMatrixMultiply(&m_normalMatrix, &temp, &m_normalMatrix );
}

////////////////////////////////////////////////

void		Cursor::YawUntilHorizontal( void)
{
	yaw( -m_yaw);
	
	//D3DXMATRIX temp;		
	//D3DXMatrixMultiply( &m_matrix, D3DXMatrixRotationY( &temp, D3DXToRadian( -m_yaw) ),  &m_matrix);
}

////////////////////////////////////////////////
// not implemented

void		Cursor::YawPitchRollRandom( int max)
{
	//int r, rmod;
	//
	//if( max)
	//{
	//	// yaw
	//	r = rand( );
	//	rmod = r % max;
	//	if( r % 2 ) rmod *= -1;
	//	yaw( ( float)rmod);

	//	// pitch
	//	r = rand( );
	//	rmod = r % max;
	//	if( r % 2 ) rmod *= -1;
	//	pitch( ( float)rmod);

	//	// roll
	//	r = rand( );
	//	rmod = r % max;
	//	if( r % 2 ) rmod *= -1;
	//	roll( ( float)rmod);
	//}

}

////////////////////////////////////////////////