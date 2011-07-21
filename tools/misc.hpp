#ifndef MISC_HPP_INCLUDED
#define MISC_HPP_INCLUDED

#include <boost/function.hpp>
#include <list>
#include <string>
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------------------------
void waitForEnter();
//------------------------------------------------------------------------------------------------
void combine(boost::function<void ()> first, boost::function<void ()> second);
//------------------------------------------------------------------------------------------------
class node
{
protected:
	node* parent;
	std::list<node*> childs;
	bool m_isRoot;
public:
	node(node* p) : parent(p) {if(p==NULL){this->m_isRoot=true;}else{this->m_isRoot=false;}};
	node() : parent(NULL), m_isRoot(true) {};
	void setParent(node* p);
	node* getParent();
	std::list<node*> getChilds();
	void addChild(node* n);
	void removeChild(node* n);
	bool isRoot() {return this->m_isRoot;}
};
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
