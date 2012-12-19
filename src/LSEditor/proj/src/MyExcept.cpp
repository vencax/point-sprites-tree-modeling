
#include "./MyExcept.h"

// ----------------------------------

CMyExcept::CMyExcept( ERRTYPE type)
{
	m_type = type;
	switch( m_type)
	{
	case WRONG_INPUT:	m_reason = "Wrong Input, please check it"; break;
	case WRONG_PARAMETR:	m_reason = "Wrong parametr of lsys"; break;
	case	END_FORGOTTEN:	m_reason = "Check the terminating '@' character!"; break;
	}
}

// ----------------------------------

CMyExcept::~CMyExcept(void)
{
	m_reason.clear( );
}

// ----------------------------------

const char* CMyExcept::GetReason( void)
{
	return m_reason.c_str( );
}

// ----------------------------------
