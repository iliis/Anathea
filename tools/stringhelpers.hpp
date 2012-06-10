#ifndef STRINGHELPERS_HPP_INCLUDED
#define STRINGHELPERS_HPP_INCLUDED

#include <stdio.h>
#include <string>
#include <sstream>
#include <list>

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

using namespace std;

//------------------------------------------------------------------------------------------------
const unsigned char AE = static_cast<unsigned char>(196);
const unsigned char ae = static_cast<unsigned char>(228);
const unsigned char OE = static_cast<unsigned char>(214);
const unsigned char oe = static_cast<unsigned char>(246);
const unsigned char UE = static_cast<unsigned char>(220);
const unsigned char ue = static_cast<unsigned char>(252);
//------------------------------------------------------------------------------------------------

/*
template <typename Typ>
string const operator+(const char* const& a, Typ const& b)
{
	return string(a) + b;
}

template <typename Typ>
string const operator+(char* const& a, Typ const& b)
{
	return string(a) + b;
}*/
//------------------------------------------------------------------------------------------------
template <typename Typ>
string ToString(Typ bla)
{
	ostringstream tmp;
	tmp << bla;
	return tmp.str();
}
//------------------------------------------------------------------------------
string inline const operator+(string const& a, double b) {return a+ToString(b);};
string inline const operator+(string const& a, float  b) {return a+ToString(b);};
string inline const operator+(string const& a, int    b) {return a+ToString(b);};
string inline const operator+(string const& a, unsigned int         b) {return a+ToString(b);};
string inline const operator+(string const& a, long int             b) {return a+ToString(b);};
string inline const operator+(string const& a, unsigned long int    b) {return a+ToString(b);};
//------------------------------------------------------------------------------------------------
bool isQuote(const char& c);
//------------------------------------------------------------------------------------------------
int toAscii(char const& c);
//------------------------------------------------------------------------------------------------
int strCount(string haystack, string needle);
//------------------------------------------------------------------------------------------------
int strCount(string haystack, const char needle);
//------------------------------------------------------------------------------------------------
void printStatusBar(string text, float percent);
//------------------------------------------------------------------------------------------------
string		getLeftSubstring(string text, string delimiter);
string		getRightSubstring(string text, string delimiter);
//------------------------------------------------------------------------------------------------
list<string> splitString(string text, string delimiter);
list<string> splitStringWithChars(string text, string delimiters = " ,;:|", bool trim=false);
// splits a string and correctly interprets quotes
// eg. [a,a,"a,a",a]        -> [a], [a], ["a,a"], [a]
//     [a,aa'aa,",a,",a',a] -> [a], [aa'aa,",a,",a'], [a]
list<string> splitStringWithCharsAndQuotes(string text, string delimiters = " ,;:|", bool trim = false);
//------------------------------------------------------------------------------------------------
string fillStringLeft (string text, unsigned int lenght, char fill);
string fillStringRight(string text, unsigned int lenght, char fill);
//------------------------------------------------------------------------------------------------
string trimString(string text, string what = " \t\n\r");
//------------------------------------------------------------------------------------------------
string strReplace(string haystack, string needle, string text);
//------------------------------------------------------------------------------------------------
string sillyDigitToString(int foo);
//------------------------------------------------------------------------------------------------
/// zerlegt \a text in einzelne Linien (von "\n" getrennt)
list<string> stringToLines(string text);
//------------------------------------------------------------------------------------------------

#endif //STRINGHELPERS_HPP_INCLUDED
