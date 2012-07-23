#ifndef ERRORCLASS_HPP_INCLUDED
#define ERRORCLASS_HPP_INCLUDED

#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "stringhelpers.hpp"


//------------------------------------------------------------------------------
// Makro für detailiertere Fehlermeldungen
#define ERROR(type, msg) Error((type), (msg), __FILE__, __LINE__)
#define ERROR_ERRNO(msg) Error("errno",(msg), __FILE__, __LINE__, errno)

#define WARNING(msg) cout << "WARNING: " << (msg) << endl;

//------------------------------------------------------------------------------
class Error : public runtime_error
{
public:
	                       Error(const std::string type, const std::string message, const std::string file = "", const int line = 0, const int custom_errno = 0)
	                   : runtime_error(getMessage(type, message, file, line, custom_errno == 0?errno:custom_errno)) {};

	const std::string getMessage(const std::string type, const std::string message, const std::string file = "", const int line = 0, const int err = 0);
};
//------------------------------------------------------------------------------
/** Für kritische Fehler, die unter keinen Umständen ein weiterlaufen
    des Programmes ermöglichen. Sollte so wenig wie möglich verwendet werden.
    */
class CriticalError : public runtime_error
{
public:
	CriticalError(const std::string text) : runtime_error(text) {};
};
//------------------------------------------------------------------------------

#endif //ERRORCLASS_HPP_INCLUDED
