#include "misc.hpp"

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
