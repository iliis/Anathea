#include "errorclass.hpp"

//------------------------------------------------------------------------------------------------
const string Error::getMessage(const string type, const string text)
{
	if(type == "init")
		return "ERROR while initializing:\n" + text;

	else if(type == "load")
		return "ERROR while loading:\n" + text;

	else if(type == "create")
		return "ERROR while creating:\n" + text;

	else if(type == "draw")
		return "ERROR while drawing:\n" + text;

	else if(type == "opengl")
		return "ERROR: OpenGL encountered a problem:\n" + text;

	else if(type == "parse")
		return "ERROR while parsing:\n" + text;

	else if(type == "notImplemented")
		return "Sorry, but the following isn't yet implemented:\n" + text;

	else if(type == "null" || type == "null pointer")
		return "ERROR: Tried to access a Null-Pointer:\n" + text;

	else if(type == "illegalOperation")
		return "ERROR: Illegal Operation performed:\n" + text;

	else if(type == "notFound")
		return "ERROR: Something couldn't be found:\n" + text;

	else if(type == "fail")
		return "ERROR: Operation failed:\n" + text;

	else
		return "UNSPECIFIC ERROR\n(This usually means, that something went TOTALLY WRONG, it's just something unimportant, or i was just too lazy to write an handle for this type of error.)\n\n"+text;
};
//------------------------------------------------------------------------------------------------
