#ifndef TINYXML_HPP_INCLUDED
#define TINYXML_HPP_INCLUDED

#include <string>
#include "tools/ticpp/ticpp.h"
#include "tools/errorclass.hpp"

#ifndef USE_TINYXML
#define USE_TINYXML
#endif


using namespace std;

//------------------------------------------------------------------------------------------------
template <typename Typ>
Typ getAttribute(ticpp::Element* element, string name, Typ def)
{
	if(element != NULL && element->HasAttribute(name))
	{
		Typ tmp = def;
		element->GetAttribute(name, &tmp);
		return tmp;
	}
	else
	{
		return def;
	}
};
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------

#endif //TINYXML_HPP_INCLUDED
