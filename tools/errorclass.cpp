#include "errorclass.hpp"



//------------------------------------------------------------------------------------------------
const std::string Error::getMessage(const std::string type, const std::string message, const std::string file, const int line, const int err)
{
	const std::string text = message + "\n  (in file '" + file + "' at line " + ToString(line)+")";

	if(type == "init")
	{
		return "ERROR while initializing:\n" + text;
	}
	else if(type == "load")
	{
		return "ERROR while loading:\n" + text;
	}
	else if(type == "create")
	{
		return "ERROR while creating:\n" + text;
	}
	else if(type == "draw")
	{
		return "ERROR while drawing:\n" + text;
	}
	else if(type == "parse")
	{
		return "ERROR while parsing:\n" + text;
	}
	else if(type == "notImplemented")
	{
		return "Sorry, but the following isn't yet implemented:\n" + text;
	}
	else if(type == "null" || type == "null pointer")
	{
		return "ERROR: Tried to access a Null-Pointer:\n" + text;
	}
	else if(type == "illegalOperation")
	{
		return "ERROR: Illegal Operation performed:\n" + text;
	}
	else if(type == "notFound")
	{
		return "ERROR: Something couldn't be found:\n" + text;
	}
	else if(type == "fail")
	{
		return "ERROR: Operation failed:\n" + text;
	}
	else if(type == "errno")
	{
		return "ERROR: " + text + "\n error #" + ToString(err) + ": " + strerror(err) + "\n";
	}
	else
	{
		return "UNSPECIFIC ERROR\n(This usually means, that something went TOTALLY WRONG, it's just something unimportant, or i was just too lazy to write an handle for this type of error.)\n\n"+text;
	}
}
//------------------------------------------------------------------------------------------------
