
#include <string>

using namespace std;

typedef enum {
	WRONG_INPUT,
	WRONG_PARAMETR,
	END_FORGOTTEN
}	ERRTYPE;	

class CMyExcept
{
private:
	ERRTYPE	m_type;
	string		m_reason;

public:
	CMyExcept( ERRTYPE);
	~CMyExcept(void);

	const char* GetReason( void);
};
