#pragma once

#include <map>
#include <vector>
#include <fstream>
#include <stdlib.h>		// rand( )
#include <string>

#define NUM_LVLS 3

// sizes of PS on every level
// level 0
//#define LVL0_PS_SIZE 10.f
//// level 1
//#define LVL1_PS_SIZE 7.5f
//// level 2
//#define LVL2_PS_SIZE 3.0f
//// leaves
//#define LVL3_PS_SIZE 0.4f

using namespace std;

class LSystem {

// public members
public:

	LSystem(); // default constructor
	~LSystem( );
	
	// typedefs
	typedef pair< short, string> varCell;
	typedef vector< varCell> variationType;

	typedef map< char, variationType*> rulesType;

	string getNRewrites( char, char); // returns product of n rewritings of 2nd param

	bool IsItLeftSideOfARule( char);
	bool ContinueGenerating( char);	// returns true if arg is less than NUM_RECURS_LVLS-1 or recLevels[ arg] is nonzero

	// # of non zero levels
	private: int m_activeLevels;
public:	inline int	GetActiveLevels( void)		{	return m_activeLevels;	}

private:	float m_PSSizes[ 4];
public:	inline float *GetPSSizes( void)	{	return m_PSSizes;	}
public:	inline void	SetPSSize( int l, float val)	{	m_PSSizes[ l] = val;	}

	// set
	inline void setRecursionLevel( char val, int indx)	{	m_recursionLevels[ indx] = val; }
	inline void setDefaultAngle( float f)		{	m_defaultAngle = f; }
	inline void setDefaultThick(float i)		{	m_defaultThick = i; }

	// get	
	inline char	getRecursionLevel( int indx)		{ return m_recursionLevels[ indx]; }
	inline float	getDefaultAngle( void)		{ return m_defaultAngle; }
	inline float		getDefaultThick( void)			{ return m_defaultThick; }

	// def Lenght
private:	float m_defLenght;
public:	inline float GetDefaultLenght( void)		{ return m_defLenght; }
public:	inline void SetDefaultLenght( float l)		{ m_defLenght = l; }

	void setAxiom( string &);
	string	&getAxiom( void);

	inline	void		IncreaseRecLevel( int pos)		{	m_recursionLevels[ pos]++;	}
	inline	void		DecreaseRecLevel( int pos)		{	if( m_recursionLevels[ pos] > 1) m_recursionLevels[ pos]--;	}

public:	void RecalculateActiveLvls( void);
public:	int		GetNumOfFInAxiom( void);

	void	ResetRules( void);	

	// i/o operations
	bool Load( char*);		// loads l-sys from file


// private members
private:
	char m_recursionLevels[ NUM_LVLS];		// number of iterations to descend through
	float m_defaultAngle;	
	float m_defaultThick;		// thicknessOfLines

	rulesType	m_rules; // map of replacement rules

	void addRule(char, short, string); //adds a rule to the map	
	
	// used to rewrite a string for one iteration
	string replace(string); 
	
	// utils
	int toInt( string);
	string getLine( ifstream*);	
	bool isItRule( string*, char*, short*, string*);	
	bool LoadRules( ifstream*);

};