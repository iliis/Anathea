#ifndef MISC_HPP_INCLUDED
#define MISC_HPP_INCLUDED

#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <list>
#include <string>
#include <iostream>

#ifdef USE_TINYXML
#include "tools/ticpp/ticpp.h"
#endif

using namespace std;


#ifdef USE_PROPERTY_TREE
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#endif

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

//------------------------------------------------------------------------------
/// TODO: separate xml and ptree stuff with preprocessor directives
#ifdef USE_TINYXML
#ifdef USE_PROPERTY_TREE
void setBoolFromXML(const ticpp::Element* element, const string attr_name, bool& var);

ptree readXML(const fs::path& p);
void readXML(ticpp::Node* n, ptree& parent);

void setBoolFromPT (const ptree pt,                const string attr_name, bool& var);
//------------------------------------------------------------------------------romPT(const ptree pt,         const string attr_name, bool default_value);
void copyAttributes(ticpp::Element* from, ticpp::Element* to, bool recursive=false); ///< Überschreibt Attribute, wenn schon vorhanden!
void copyAttributes(ptree& from, ptree& to, bool recursive=false); ///< Überschreibt Attribute, wenn schon vorhanden!
//------------------------------------------------------------------------------
inline bool
getBoolFromPT(const ptree pt, const string attr_name, bool default_value)
{
	setBoolFromPT(pt, attr_name, default_value);
	return default_value;
};
//------------------------------------------------------------------------------------------------
void printPTree(ptree& pt, int level=0);
#endif
#endif
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void waitForEnter();
//------------------------------------------------------------------------------------------------
void combine(boost::function<void ()> first, boost::function<void ()> second);
//------------------------------------------------------------------------------------------------
/*class node
{
protected:
	node* parent;
	std::list<node*> childs;
	bool m_isRoot;
public:
	node(node* p) : parent(p), m_isRoot(p==NULL) {};
	node() : parent(NULL), m_isRoot(true) {};
	void setParent(node* p);
	node* getParent();
	std::list<node*> getChilds();
	void addChild(node* n);
	void removeChild(node* n);
	bool isRoot() {return this->m_isRoot;}
};*/
//------------------------------------------------------------------------------------------------
struct voidFunction
{
	boost::function<void ()> func;
	string name;

	voidFunction() : func(NULL), name("NULL") {};
	voidFunction(string n, boost::function<void ()> f):func(f), name(n) {};
};
//------------------------------------------------------------------------------------------------
template <typename T>
bool is_in_intervall(T x, T a, T b, T margin=0)
{
	return x >= min(a,b)-margin and x <= max(a,b)+margin;
}
//------------------------------------------------------------------------------


#endif //MISC_HPP_INCLUDED
