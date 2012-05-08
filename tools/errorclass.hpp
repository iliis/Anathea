#ifndef ERRORCLASS_HPP_INCLUDED
#define ERRORCLASS_HPP_INCLUDED

#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "definitions.hpp"

using namespace std;

//------------------------------------------------------------------------------
// Makro für detailiertere Fehlermeldungen
#define ERROR(type, msg) Error((type), (msg), __FILE__, __LINE__)

//------------------------------------------------------------------------------
class Error : public runtime_error
{
	//string type;
	//string text;
	int err_no;
public:
	                  Error(const string type, const string message, const string file = "", const int line = 0)
	                   : runtime_error(getMessage(type, message, file, line)), err_no(errno) {};
	                   
	const string getMessage(const string type, const string message, const string file = "", const int line = 0);
};
//------------------------------------------------------------------------------
/** Für kritische Fehler, die unter keinen Umständen ein weiterlaufen
    des Programmes ermöglichen. Sollte so wenig wie möglich verwendet werden.
    */
class CriticalError : public runtime_error
{
public:
	CriticalError(const string text) : runtime_error(text) {};
};
//------------------------------------------------------------------------------

#endif //ERRORCLASS_HPP_INCLUDED
