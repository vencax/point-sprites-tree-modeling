
#include "./LSystem.h"

#include <ctype.h>

#define isDig(x) ( ((x) >= '0') && ((x) <= '9') ) ? true : false
#define isAlph(x) ( ( ((x) >= 'a') && ((x) <= 'z') ) || ( ((x) >= 'A') && ((x) <= 'Z') ) ) ? true : false

#define LINE_LENGTH 200


/*Default Constructor*/
LSystem::LSystem()
{
	// setup initials
	setDefaultAngle( 0);	
	setDefaultThick( 0);

	m_activeLevels = 0;
}

// desttor
LSystem::~LSystem( )
{
	ResetRules( );
}

void
LSystem::ResetRules( void)
{
	variationType* variations;
	string *right;

	// go through all rules
	for( rulesType::iterator itr= m_rules.begin( ); itr != m_rules.end( ); itr++)
	{
		// for each rule go through all variations
		variations = itr->second;
		for( variationType::iterator it= variations->begin( ); it != variations->end( ); it++)
		{
			// in each variation clear right side string
			right = &( it->second);
			right->clear( );
		}
		// clear variations vector
		variations->clear( );
		// end delete it
		delete variations;
	}
	// clear rules map
	m_rules.clear( );
}

void
	LSystem::setAxiom( string &s)
{
	addRule( 'X', 100, s);
}

string &
	LSystem::getAxiom( void)
{
	rulesType::iterator it = m_rules.find( 'X');

	return (it->second)->begin( )->second;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* adds a rule to the L-System*/
void
LSystem::addRule(char c, short probab, string s)
{	
	// try to finf if rule c is already in map
	rulesType::iterator it = m_rules.find( c);

	if( it == m_rules.end( ) )		// there is not, so add
	{
		// create vector fo rule variations
		variationType* variat = new variationType;	

		// insert first new value
		variat->push_back( make_pair( probab, s) );

		// insert new rule into map
		m_rules.insert( rulesType::value_type( c, variat) );

	} else // there already is, so try to add into array
	{
		variationType* variat = it->second;

		// add new variation
		variat->push_back( make_pair( probab, s) );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
/* returns product of n rewritings */
string 
LSystem::getNRewrites( char n, char what)
{
	string out;

	out += what;
	for (int i= 0; i< n;i++)
	{
		out = replace( out);
	}
	
	return out;
}

/*used to rewrite a string for one iteration*/
string
	LSystem::replace(string s)
{
	char c;
	string out = "";
	short randNum;
	
	string temp = "";
	for( unsigned i = 0 ; i<s.length() ; i++)
	{		
		// get rewrited char
		c = s[i];

		// find rule for c
		rulesType::iterator it= m_rules.find( c);
		if( it == m_rules.end( ) )		// there is no rule with c on left side
		{
			// out only c,  don't rewrite this char cos it's TERMINAL
			out += c;		
		} else 
		{
			// otherwise prepare rundom number			
			randNum = short( rand ( ) % 100);
			
			// find nearest low border in probability interval
			variationType* var = it->second;
			variationType::iterator i= var->begin( );
			while(  ( i != var->end( ))  && ( i->first < randNum) )
				i++;
			
			// if we reach end of vector error occured
			if( i == var->end( ))
				return NULL;
			else
				out += i->second;
		}	
	}
	
	return out;
}

void
LSystem::RecalculateActiveLvls( void)
{
	int i = 0;
	m_activeLevels = 0;

	if( m_recursionLevels[ i])
		m_activeLevels++;
	
	i++;

	while( i < NUM_LVLS)
	{
		if( m_recursionLevels[ i] && m_recursionLevels[ i-1])	
			m_activeLevels++;
		i++;
	}
		
}

string 
	LSystem::getLine( ifstream *f)
// reads one single line from char field and return it
/// reads only the segment before '#'. Rest of line is omited. 
// Return first valid line( first no-comment line)
// Line does not contain comments introduced by '#'
{
	string s;
	s.clear( );
	
	char buf[ LINE_LENGTH+1];
	int pos= 0;

	// try to get line from file
	while( ! f->fail() && ( s.size( ) == 0) )
	{
		f->getline( buf, LINE_LENGTH);
		while( ( pos < LINE_LENGTH) && ( buf[ pos]) && ( buf[ pos] != '#') )
		{
			s += buf[ pos];
			pos++;
		}
	}
	
	return s;
}

/////////////////////////////////////////////////////////////////////////////////////////
///  checks if given char is a rule( in rule container)

bool LSystem::IsItLeftSideOfARule( char c)
{
	rulesType::iterator it = m_rules.find( c);

	if( it == m_rules.end( ) )	
		return false;	// there is not
	else
		return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool LSystem::ContinueGenerating( char level)
{
	if( ( level < (NUM_LVLS-1)) && ( m_recursionLevels[ level+1]) )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////////////////////

int
LSystem::GetNumOfFInAxiom( void)
{
	int tmp= 0;

	for( int i= 0; i < ( int)getAxiom( ).size( ); i++)
		if( getAxiom( )[ i] == 'F')
			tmp++;

	return tmp;
}

//////////////////////////////////////////////////////////////////////////////////////////
int LSystem::toInt( string s)
{
	int value= 0;
	int pos= 0;
	char c= s[ pos];

	while( isDig( c))
	{
		value = value * 10 + ( c - '0');
		pos++;
		c= s[ pos];
	}
	return value;
}

// I/O
bool
	LSystem::Load( char* file)
{
	// STRUCTURE OF LSYS FILE:
	// =================
	// recursion levels separated by '-'
	// basic angle
	// thickness
	// lenght
	// PointSprites sizes separated by '-'
	// axiom
	// all other lines are rulez until "@" line
	unsigned long position = 0;
	string line;
	int val;
	float fval;

	ifstream f;

	// try to open the file
	f.open( file);
	if( ! f.good() )
		return false;

	////////////////////////////////
	// first we try to load recurse level	s
	////////////////////////////////
	line.clear( );
	line = getLine( &f);
	// check if there is end of file
	if( line.size( ) == 0)
		return false;

	unsigned int j= 0;
	for( int i= 0; i< NUM_LVLS; i++)
	{
		val = 0;

		// check if there is what to load
		if( j > line.size( ) )
			return false;

		while( ( j < 8) && isdigit( line[ j]) )
		{
			val = ( val * 10) + line[ j] - '0';
			j++;
		}

		// recurson level
		m_recursionLevels[ i] = val;

		// if non zero increase active levels
		if( val)
			m_activeLevels++;

		// skip separator
		j++;
	}


	// check if there is at least nonzero in the first recLevel
	if( !m_recursionLevels[ 0])
		return false;

	////////////////////////////////
	// then m_defaultAngle
	////////////////////////////////
	line.clear( );
	line = getLine( &f);
	// check if there is end of file
	if( line.size( ) == 0)
		return false;

	fval = ( float)atof( &line[ 0]);
	if( fval > 0)
		m_defaultAngle = fval;
	else
		return false;

	////////////////////////////////
	// then m_defaultThickness
	////////////////////////////////
	line.clear( );
	line = getLine( &f);
	// check if there is end of file
	if( line.size( ) == 0)
		return false;

	fval = ( float)atof( &line[ 0]);
	if( fval > 0)
		m_defaultThick = fval;
	else
		return false;

	////////////////////////////////
	// default lenght
	////////////////////////////////
	line.clear( );
	line = getLine( &f);
	// check if there is end of file
	if( line.size( ) == 0)
		return false;

	fval = ( float)atof( &line[ 0]);
	if( fval > 0)
		m_defLenght = fval;
	else
		return false;

	////////////////////////////////
	// PS Sizes
	////////////////////////////////
	line.clear( );
	line = getLine( &f);
	// check if there is end of file
	if( line.size( ) == 0)
		return false;

	char tmp[ 32];
	j = 0;
	int k = 0;

	for( int i= 0; i< 4; i++)
	{
		while( ( j< line.size( )) && line[ j] != '-')
		{
			tmp[ k] = line[ j];
			k++; j++;
		}
		tmp[ k] = 0;

		// PS Size
		m_PSSizes[ i] = ( float)atof( tmp);

		// skip separator
		j++;

		k = 0;		
	}

	////////////////////////////////
	// AXIOM
	////////////////////////////////
	line.clear( );
	line = getLine( &f);
	// check if there is end of file
	if( line.size( ) == 0)
		return false;

	// are there any space? If yes then error
	for( unsigned int j= 0; j< line.size( ); j++)
		if( ( line[ j] == ' ') && ( line[ j] == '\n') && ( line[ j] == '\t') )
			return false;

	setAxiom( line);

	if( ! LoadRules( &f))
		return false;

	// close the file
	f.close( );
	
	return true;
}



bool
	LSystem::LoadRules( ifstream *f)
{
	char left;
	short prob;
	string right;

	string line, rule;

	line.clear( );
	line = getLine( f);
	// check if there is end of file
	if( line.size( ) == 0)
		return false;

	while( line[ 0] != '@')					// while not sign for end of lsystem 
	{
		// is line a rule in right format?
		if( isItRule( &line, &left, &prob, &right) )
			addRule( left, prob, right);
		else
			return false;

		// load new line
		line.clear( );
		line = getLine( f);
		// check if there is end of file
		if( line.size( ) == 0)
			return false;
	}

	return true;
}

// check if line is rule in right shape...
bool
LSystem::isItRule( string *line, char *c, short *prob, string *right)
{
	// rule has to look like:
	// <left side>(<probability(int from 1 to 100)>)=<rightside>
	unsigned short pos= 0;
	int val = 0;
	char ch;

	// check left side
	ch= ( *line)[ pos++];
	if( ! isAlph( ch))		
		return false;
	else
		*c = ch;

	// check probability
	if( ( *line)[ pos++] != '(')
		return false;

	ch= ( *line)[ pos];
	while( isDig( ch))
	{
		val = val * 10 + ( ch - '0');
		pos++;
		ch= ( *line)[ pos];
	}
	// check if next char is ')'
	if( ch != ')')
		return false;

	// assign loaded value to prob
	*prob = val;

	// skip from '(' to '='
	pos++;
	if( ( *line)[ pos++] != '=')
		return false;

	// the rest of line is right side of rule
	// i don't check any error in it cos i don't know
	// what is an error. So copy it to right
	right->clear( );
	while( pos < line->size( ))
	{
		*right += ( *line)[ pos];
		pos++;
	}

	return true;
}