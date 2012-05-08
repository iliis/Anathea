#ifndef SIMPLEGRAPH_HPP_INCLUDED
#define SIMPLEGRAPH_HPP_INCLUDED

#include <list>

#include <boost/foreach.hpp>

template<typename T>
class Node
{
	list<Node<T>*> edges;
	T val;
public:
	Node() : val() {};
	Node(T v) : val(v) {};


	inline T get(){return this->val;}
	inline void set(T v){this->val = v;}
	inline list<Node<T>*>& getEdges(){return this->edges;}

	inline T& operator*(){return this->val;}
	bool hasEdgeTo(Node<T>* another)
	{
		if(another == this) return false;

		BOOST_FOREACH(Node<T>* n, edges)
			if(n == another) return true;

		return false;
	};

	void addEdge(Node<T>* n){if(n!=this and !this->hasEdgeTo(n)){this->edges.push_back(n); if(!n->hasEdgeTo(this)){n->addEdge(this);}}}
	void removeEdge(Node<T>* n){this->edges.remove(n);}
};

template <typename T>
class Edge
{
	Node<T> *A, *B;
public:
	Edge(Node<T>* a, Node<T>* b) : A(a), B(b) {};

	inline Node<T>* getA(){return A;}
	inline Node<T>* getB(){return B;}
};

template <typename T>
class SGraph
{
protected:
	list<Node<T>*> nodes;
public:
	virtual ~SGraph(){BOOST_FOREACH(Node<T>* n, this->nodes) delete n;}

	inline void add(Node<T>* n){if(!this->has(n))this->nodes.push_back(n);}
	inline void add(T v){this->add(new Node<T>(v));}

	bool has(Node<T>* node)
	{
		BOOST_FOREACH(Node<T>* n, this->nodes)
			if(n == node) return true;

		return false;
	};

	Node<T>* find(T val)
	{
		BOOST_FOREACH(Node<T>* n, this->nodes)
			if(n->get() == val) return n;

		return NULL;
	}

	inline void remove(Node<T>* node){this->nodes.remove(node);}

	inline list<Node<T>* >&getNodes(){return this->nodes;}

	virtual
	Node<T>* merge(Node<T>* n1, Node<T>* n2)
	{
		Node<T>* result = new Node<T>((n1->get() + n2->get())/2);

		BOOST_FOREACH(Node<T>* edge, n1->getEdges())
			result->addEdge(edge);

		BOOST_FOREACH(Node<T>* edge, n2->getEdges())
			result->addEdge(edge);

		this->add(result);
		return result;
	}

	list<Edge<T> > getEdges()
	{
		list<Edge<T> > edges;
		BOOST_FOREACH(Node<T>* n, this->nodes)
		{
			BOOST_FOREACH(Node<T>* e, n->getEdges())
			{
				bool already_there = false;
				BOOST_FOREACH(Edge<T>& all, edges)
				{
					if((all.getA() == n and all.getB() == e) or (all.getB() == n and all.getA() == e)){already_there = true; break;}
				}

				if(!already_there)
					edges.push_back(Edge<T>(n,e));
			}
		}
		return edges;
	}
};

#endif // SIMPLEGRAPH_HPP_INCLUDED
