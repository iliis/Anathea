#ifndef ERRORCLASS_HPP_INCLUDED
#define ERRORCLASS_HPP_INCLUDED

#include <stdexcept>
#include <string.h>

using namespace std;

//------------------------------------------------------------------------------------------------
class Error : public runtime_error
{
	//string type;
	//string text;
public:
	Error(const string type, const string text) : runtime_error(getMessage(type, text)) {};
	const string getMessage(const string type, const string text);
};
//------------------------------------------------------------------------------------------------
/** Für kritische Fehler, die unter keinen Umständen ein weiterlaufen
    des Programmes ermöglichen. Sollte so wenig wie möglich verwendet werden.
    */
class CriticalError : public runtime_error
{
public:
	CriticalError(const string text) : runtime_error(text) {};
};
//------------------------------------------------------------------------------------------------

#endif //ERRORCLASS_HPP_INCLUDED
