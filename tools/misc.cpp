#include "misc.hpp"


#ifdef USE_TINYXML
#ifdef USE_PROPERTY_TREE
//------------------------------------------------------------------------------
void
printPTree(ptree& pt, int level)
{
	if(level==0)
		cout << "'";

	cout << pt.data() << "'" << endl;

	if(!pt.empty())
	{
		BOOST_FOREACH(ptree::value_type &v, pt)
		{
			for(int i=1;i<level;++i) cout << " . ";
			if (level>0) cout << " |-";

			cout << " " << v.first << " = '";
			printPTree(v.second, level+1);
		}
	}
}
//------------------------------------------------------------------------------
#include "tools/stringhelpers.hpp"
void
setBoolFromXML(const ticpp::Element* element, const string attr_name, bool& var)
{
	string tmp="";
	element->GetAttributeOrDefault(attr_name, &tmp, var);
	boost::trim(tmp);
	boost::to_lower(tmp);

	if(tmp == "false" || tmp == "" || tmp == "0")
		var = false;
	else
		var = true;
};
//------------------------------------------------------------------------------
void
setBoolFromPT(const ptree pt, const string attr_name, bool& var)
{
	string tmp="";
	tmp = pt.get<string>(attr_name, ToString(var));
	boost::trim(tmp);
	boost::to_lower(tmp);

	if(tmp == "false" || tmp == "" || tmp == "0")
		var = false;
	else
		var = true;
};
//------------------------------------------------------------------------------
void
copyAttributes(ticpp::Element* from, ticpp::Element* to, bool recursive)
{
	ticpp::Iterator<ticpp::Attribute> attr;
	for(attr = attr.begin(from); attr != attr.end(); attr++)
	{
		to->SetAttribute((*attr).Name(), (*attr).Value());
	}

	if(recursive && !to->NoChildren())
	{
		ticpp::Iterator< ticpp::Node > child;
		for(child = child.begin(to); child != child.end(); child++)
			copyAttributes(from, child->ToElement(), true);
	}
};
//------------------------------------------------------------------------------
void
copyAttributes(ptree& from, ptree& to, bool recursive)
{
	BOOST_FOREACH(ptree::value_type& v, from)
	{
		to.put(v.first, v.second.get_value<string>());

		if(recursive)
			copyAttributes(v.second,to.get_child(v.first),true);
	}
};
//------------------------------------------------------------------------------
ptree
readXML(const fs::path& p)
{
	if(fs::exists(p)
	&& fs::is_regular_file(p))
	{
		ticpp::Document doc(p.string());
		doc.LoadFile();

		/// 1. child: <?xml...?>, 2. child: <root>
		ticpp::Node* n = doc.FirstChild()->NextSibling()->FirstChild();
		ptree pt;

		readXML(n, pt);

		return pt;
	}
	else
	{
		/// TODO: create default stylesheet for every widget, then enable this warning again
		//cout << "WARNING: XML-File '" << p.string() << "' doesn't exist" << endl;
		return ptree(); /// gib einfach leere Daten falls Datei nicht gefunden
	}
};
//------------------------------------------------------------------------------
void
readXML(ticpp::Node* n, ptree& parent)
{
	assert(n);

	/// alle Elemente durchgehen
	do
	{
		/// Element selber erzeugen
		string val = n->Value();
		ptree& cur_element = parent.add(val, val);

		if(n->Type() == TiXmlNode::ELEMENT)
		{
			/// Attribute einlesen
			ticpp::Iterator<ticpp::Attribute> attr;
			for(attr = attr.begin(n->ToElement()); attr != attr.end(); attr++)
			{
				string name, value;
				attr.Get()->GetName(&name); attr.Get()->GetValue(&value);

				cur_element.put(name, value); ///< put überschreibt!
			}
		}

		/// Child-Nodes einlesen
		if(!n->NoChildren())
		{
			//parent.add(val+".<children>", "");
			readXML(n->FirstChild(), parent.get_child(val)); //+".<children>"));
		}
	}
	while( (n = n->NextSibling(false)) != NULL );





	/*ticpp::Iterator< ticpp::Node > child;
	for(child = child.begin(n); child != child.end(); child++)
	{
		pt.put("<children>."+child.Get()->Value(), readXML(child.Get()));
	}*/
};
#endif
#endif
//------------------------------------------------------------------------------------------------
/*node* node::getParent()
{
	return this->parent;
};
//------------------------------------------------------------------------------------------------
std::list<node*> node::getChilds()
{
	return this->childs;
};
//------------------------------------------------------------------------------------------------
void node::addChild(node* n)
{
	this->childs.push_back(n);
	n->setParent(this);
};
//------------------------------------------------------------------------------------------------
void node::removeChild(node* n)
{
	this->childs.remove(n);
};
//------------------------------------------------------------------------------------------------
void node::setParent(node* p)
{
	this->parent = p;
};*/
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void combine(boost::function<void ()> first, boost::function<void ()> second)
{
	first();
	second();
};
//------------------------------------------------------------------------------------------------
//http://www.c-plusplus.de/forum/viewtopic-var-t-is-111042.html
void waitForEnter()
{
    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get();
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
