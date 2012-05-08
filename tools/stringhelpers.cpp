#include "tools/stringhelpers.hpp"

//------------------------------------------------------------------------------------------------
int toAscii(char const& c)
{
	return int(c);
};
//------------------------------------------------------------------------------------------------
int strCount(string haystack, const char needle)
{
	int nr=0;

	BOOST_FOREACH(char c, haystack)
		if(c == needle)++nr;

	return nr;
};
//------------------------------------------------------------------------------------------------
int strCount(string haystack, string needle)
{
	int nr = 0;
	size_t loc  = haystack.find(needle);
	while(loc != string::npos)
	{
		loc = haystack.find(needle, loc+1);
		nr++;
	}

	return nr;
};
//------------------------------------------------------------------------------------------------
#include <iostream>
using namespace std;

string sillyDigitToString(int foo)
{
	switch(foo)
	{
		case  -1: return  "-1"; break;
		case  -2: return  "-2"; break;
		case  -3: return  "-3"; break;
		case  -4: return  "-4"; break;
		case  -5: return  "-5"; break;
		case  -6: return  "-6"; break;
		case  -7: return  "-7"; break;
		case  -8: return  "-8"; break;
		case  -9: return  "-9"; break;
		case -10: return "-10"; break;
		case -11: return "-11"; break;
		case -12: return "-12"; break;
		case -13: return "-13"; break;
		case -14: return "-14"; break;
		case -15: return "-15"; break;
		case -16: return "-16"; break;
		case -17: return "-17"; break;
		case -18: return "-18"; break;
		case -19: return "-19"; break;
		case -20: return "-20"; break;

		case  0: return  "0"; break;
		case  1: return  "1"; break;
		case  2: return  "2"; break;
		case  3: return  "3"; break;
		case  4: return  "4"; break;
		case  5: return  "5"; break;
		case  6: return  "6"; break;
		case  7: return  "7"; break;
		case  8: return  "8"; break;
		case  9: return  "9"; break;
		case 10: return "10"; break;
		case 11: return "11"; break;
		case 12: return "12"; break;
		case 13: return "13"; break;
		case 14: return "14"; break;
		case 15: return "15"; break;
		case 16: return "16"; break;
		case 17: return "17"; break;
		case 18: return "18"; break;
		case 19: return "19"; break;
		case 20: return "20"; break;
		default: cout << "WARNING: Digit not in Range -20 to 20." << endl; return "0"; break;
	}
};
//------------------------------------------------------------------------------------------------
void printStatusBar(string text, float percent)
{
	for(unsigned int i = 0; i < 13+text.length(); ++i)
	{
		printf("\b");
	}

	printf("%s [", text.c_str());
	for(int i = 0; i < 10; ++i)
	{
		if(i < percent*10)
		{
			printf("#");
		}
		else
		{
			printf(" ");
		}
	}
	printf("]");
}
//------------------------------------------------------------------------------------------------
string
getLeftSubstring(string text, string delimiter)
{
	unsigned int pos = text.find(delimiter);
	if (pos == string::npos)
		return text;

	return text.substr(0, pos);
};

string
getRightSubstring(string text, string delimiter)
{
	unsigned int pos = text.find(delimiter);
	if (pos == string::npos)
		return text;

	return text.substr(pos+1);
};
//------------------------------------------------------------------------------------------------
list<string> splitString(string text, string delimiter)
{
	list<string> results;

	size_t delpos = text.find(delimiter);
	size_t oldpos = -1;
	do
	{
		results.push_back(text.substr(oldpos+1, delpos-oldpos-1));
		oldpos = delpos;
		delpos = text.find(delimiter, delpos+1);
	}
	while(oldpos != string::npos);

	return results;
};
//------------------------------------------------------------------------------
list<string> splitStringWithChars(string text, string delimiters, bool trim)
{
	list<string> results;

	size_t delpos = text.find_first_of(delimiters);
	size_t oldpos = -1;
	do
	{
		string tmp(text.substr(oldpos+1, delpos-oldpos-1));
		if(trim) tmp = trimString(tmp);
		if(!tmp.empty()) results.push_back(tmp);

		oldpos = delpos;
		delpos = text.find_first_of(delimiters, delpos+1);
	}
	while(oldpos != string::npos);

	return results;
};
//------------------------------------------------------------------------------------------------
string fillStringLeft(string text, unsigned int lenght, char fill)
{
    while(text.length() < lenght)
    {
        text = fill+text;
    }
    return text;
};

string fillStringRight(string text, unsigned int lenght, char fill)
{
    while(text.length() < lenght)
    {
        text = text+fill;
    }
    return text;
};
//------------------------------------------------------------------------------------------------
string trimString(string text, string what)
{
	size_t startpos = text.find_first_not_of(what);
	size_t endpos	= text.find_last_not_of (what);

	if(startpos == string::npos || text.empty())
		return "";
	else
		return text.substr(startpos, endpos-startpos+1);
};
//------------------------------------------------------------------------------------------------
string strReplace(string haystack, string needle, string text)
{
	size_t pos = haystack.find(needle);
	while(pos != string::npos)
	{
		haystack.replace(pos, needle.size(), text);
		pos = haystack.find(needle, pos);
	}
	return haystack;
};
//------------------------------------------------------------------------------------------------
list<string> stringToLines(string text)
{
	list<string> lines; ///< Die einzelnen Zeilen des Textes

	size_t i = 0;
	do
	{
		i = text.find_first_of('\n');
		lines.push_back(text.substr(0, i));

		text = text.substr(i+1);
	}
	while( i != string::npos );

	return lines;
}
//------------------------------------------------------------------------------------------------
