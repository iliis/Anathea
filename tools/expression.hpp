#ifndef ATTRIBUTE_HPP_INCLUDED
#define ATTRIBUTE_HPP_INCLUDED

#include <algorithm>
#include <string>

#include <boost/signals.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "tools/errorclass.hpp"


#ifdef USE_PROPERTY_TREE
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#endif

/*
 * Expression<T>:
 *   A general way of expressing dependent variables (similiar to constraints
 *   in CAD software but without clever 'dependency resolution'). A simple
 *   example:
 * 
 *   Expression<int> a, b, c;
 * 
 *   c = a.ref() + b.ref(); // create expression (
 *   a = 10;
 *   b =  5;  // c is now 15
 * 
 *   c = a.ref() + b.ref() - 4; // c is now 11
 *   b = 4; // c is now 10
 *   c = 1; // this destroys the expression created above
 *   a = 100; // c is still 1
 * 
 *   The implementation is feed-forward based, eg. when @a changes a foreach
 *   travels each dependency of @a (only @c here) and calls their 'get()'
 *   methods which in turn call their 'predecessors' get().
 * 
 *   c = a.ref() + b.ref();
 *   a = 10;
 * 
 *   call stack:
 *   
 *   a.set(10)
 *    a.on_change()
 *     c.update()
 *      c.get()
 *       c.val = a.get() + b.get() // may be quite recursive ;)
 *      c.on_update() // does nothing (no Expression depends on c)
 * 
 * ExpressionRef<T>:
 *   Actually stores the calculations done when Expression.get() is called.
 * 
 */ 


#define CHECK_FOR_CYCLES /// still possible! (a -> b -> a)

template <typename T>
class Expression;

template <typename T>
typename Expression<T>::ExpressionRefPtr CONST(T val);


template <typename Typ>
class Expression
{

public:
///   DEPENDENCIES
///----------------------------------------------------------------------------
	class ExpressionRef
	{
	public:
		virtual ~ExpressionRef()
		{

		};

		virtual ExpressionRef* copy() const = 0;


		virtual void add_link(Expression* ptr) = 0;
		virtual void remove_link(Expression* ptr) = 0;

		virtual Typ get() = 0;
		virtual bool check_for_cycle(Expression* a){return false;}
	};

	/// so we can use overloaded operators :)
	class ExpressionRefPtr
	{
	public:
		ExpressionRef* ptr;
	public:

		ExpressionRefPtr(ExpressionRef* p)             : ptr(p)       {assert(ptr);}
		ExpressionRefPtr(const ExpressionRefPtr& erp) : ptr(erp.ptr) {assert(erp.ptr);}

		ExpressionRefPtr& operator=(const ExpressionRefPtr& rhs){this->ptr = rhs.ptr; return *this;}

		inline
		operator ExpressionRef* (){return ptr;}
	};

	class ExprDirectRef : public ExpressionRef
	{
		Expression<Typ>& attr;
	public:
		ExprDirectRef(Expression<Typ>& a) : attr(a) {};

		virtual ExpressionRef* copy() const {return new ExprDirectRef(attr);};

		Typ get(){return attr.get();};
		virtual bool check_for_cycle(Expression* a){return this->attr.check_for_cycle(a);}

		void add_link(Expression* ptr){attr.add_link(ptr);};
		void remove_link(Expression* ptr){attr.remove_link(ptr);};
	};

	class ExprConstRef : public ExpressionRef
	{
		Typ val;
	public:
		ExprConstRef(const Typ& v) : val(v) {};

		ExpressionRef* copy() const {return new ExprConstRef(this->val);}

		Typ get(){return val;}
		bool check_for_cycle(Expression* a){return false;}

		void add_link(Expression* ptr){};
		void remove_link(Expression* ptr){};
	};

	class ExprSingleRef : public ExpressionRef
	{
	protected:
		ExpressionRef *ref;
	public:
		ExprSingleRef(ExpressionRef* A) : ref(A) {assert(A);};
		~ExprSingleRef(){delete ref;}

		virtual ExpressionRef* copy() const = 0;
		virtual bool check_for_cycle(Expression* A){return ref->check_for_cycle(A);}

		void add_link(Expression* ptr){ref->add_link(ptr);};
		void remove_link(Expression* ptr){ref->remove_link(ptr);};
	};

	class ExprTwinRef : public ExpressionRef
	{
	protected:
		ExpressionRef *a, *b;
	public:
		ExprTwinRef(ExpressionRef* A, ExpressionRef* B) : a(A), b(B) {assert(a); assert(b);};
		~ExprTwinRef(){delete a; delete b;}

		virtual ExpressionRef* copy() const  = 0;
		virtual bool check_for_cycle(Expression* A){return a->check_for_cycle(A) or b->check_for_cycle(A);}

		void add_link(Expression* ptr){a->add_link(ptr); b->add_link(ptr);};
		void remove_link(Expression* ptr){a->remove_link(ptr); b->remove_link(ptr);};
	};

	/// + ADDITION
	class ExprSumRef : public ExprTwinRef
	{
	public:
		ExprSumRef(ExpressionRef* A, ExpressionRef* B) : ExprTwinRef(A,B) {};
		Typ get(){return this->a->get() + this->b->get();};

		virtual ExpressionRef* copy() const {return new ExprSumRef(this->a->copy(),this->b->copy());};
	};

	/// - SUBTRACTION
	class ExprSubtrRef : public ExprTwinRef
	{
	public:
		ExprSubtrRef(ExpressionRef* A, ExpressionRef* B) : ExprTwinRef(A,B) {};
		Typ get(){return this->a->get() - this->b->get();};

		virtual ExpressionRef* copy() const {return new ExprSubtrRef(this->a->copy(),this->b->copy());};
	};

	/// - NEGATION
	class ExprNegRef : public ExprSingleRef
	{
	public:
		ExprNegRef(ExpressionRef* A) : ExprSingleRef(A) {};
		Typ get(){return -this->ref->get();};

		virtual ExpressionRef* copy() const {return new ExprNegRef(this->ref->copy());};
	};

	/// * MULTIPLICATION
	class ExprMultRef : public ExprTwinRef
	{
	public:
		ExprMultRef(ExpressionRef* A, ExpressionRef* B) : ExprTwinRef(A,B) {};
		Typ get(){return this->a->get() * this->b->get();};

		virtual ExpressionRef* copy() const {return new ExprMultRef(this->a->copy(),this->b->copy());};
	};

	/// / DIVISION
	class ExprDivRef : public ExprTwinRef
	{
	public:
		ExprDivRef(ExpressionRef* A, ExpressionRef* B) : ExprTwinRef(A,B) {};
		Typ get(){return this->a->get() / this->b->get();};

		virtual ExpressionRef* copy() const {return new ExprDivRef(this->a->copy(),this->b->copy());};
	};

	/// If/else
	class ExprIfElseRef : public ExprTwinRef
	{
		boost::function<bool(Typ,Typ)> statement;
	public:
		ExprIfElseRef(ExpressionRef* A, ExpressionRef* B, boost::function<bool(Typ,Typ)> statement) : ExprTwinRef(A,B), statement(statement) {};

		Typ get()
		{
			Typ av = this->a->get();
			Typ bv = this->b->get();

			return (statement(av,bv) ? av : bv );
		};

		virtual ExpressionRef* copy() const {return new ExprIfElseRef(this->a->copy(),this->b->copy(), statement);};
	};

	/// Max
	class ExprMaxRef : public ExprTwinRef
	{
	public:
		ExprMaxRef(ExpressionRef* A, ExpressionRef* B) : ExprTwinRef(A,B) {};
		Typ get(){return max(this->a->get(), this->b->get());};

		virtual ExpressionRef* copy() const {return new ExprMaxRef(this->a->copy(),this->b->copy());};
	};

	/// Min
	class ExprMinRef : public ExprTwinRef
	{
	public:
		ExprMinRef(ExpressionRef* A, ExpressionRef* B) : ExprTwinRef(A,B) {};
		Typ get(){return min(this->a->get(), this->b->get());};

		virtual ExpressionRef* copy() const {return new ExprMinRef(this->a->copy(),this->b->copy());};
	};

	/// f(x) FUNCTION
	class ExprFuncRef : public ExprSingleRef
	{
		boost::function<Typ(Typ)> func;
	public:
		ExprFuncRef(boost::function<Typ(Typ)> f, ExpressionRef* A) : ExprSingleRef(A), func(f) {};
		Typ get(){return func(this->ref->get());};

		virtual ExpressionRef* copy() const {return new ExprFuncRef(this->func, this->ref->copy());};
	};

private:
///   PROPERTIES
///----------------------------------------------------------------------------

	Typ val; // the actual value of this Expression, may be the result of a complex calculation stored in @expr
	ExpressionRef* expr; // how to calculate the value of this instance based on other Expressions (NOT the children, this is 'backwards')
	list<Expression*> children; // Expressions which depend on this value -> forward (on_change traverses this)

	typedef Typ T;


public:

///   CONSTRUCTORS
///----------------------------------------------------------------------------

	Expression()                     : val(),  expr(NULL) {};
	Expression(Typ v)                : val(v), expr(NULL) {};
	Expression(ExpressionRef* ref)   : val(),  expr(NULL) {this->link(ref);};
	Expression(ExpressionRefPtr ref) : val(),  expr(NULL) {this->link(ref);}

	Expression(const Expression& attr)
		: val(attr.val), expr(NULL)
	{
		if(attr.isLinked())
			this->expr = attr.expr->copy();
	}



	~Expression(){on_deletion(); this->unlink();}

///----------------------------------------------------------------------------

	void on_change()
	{
		BOOST_FOREACH(Expression* c, this->children)
			c->update();
	}

	void on_deletion()
	{
		list<Expression*> tmp_children;
		tmp_children.swap(this->children);

		BOOST_FOREACH(Expression* c, tmp_children)
			c->unlink();
	}

	inline
	void add_link(Expression* ptr)
	{
		assert(ptr);
		this->children.push_back(ptr);
	}

	inline
	void remove_link(Expression* ptr)
	{
		this->children.remove(ptr);
	}

///----------------------------------------------------------------------------

	inline bool isLinked() const {return this->expr!=NULL;}

	inline   Typ get() const  {return val;}
	inline
	operator Typ    () const  {return this->get();}

	void set(const Typ& v)
	{
		if(this->isLinked())
			this->unlink(); ///< create a new attribute

		if(val!=v)
		{
			this->val = v;
			on_change();
		}
	}

	void update()
	{
		if(this->isLinked())
		{
			Typ v = this->expr->get();
			if(this->val != v) {this->val = v; on_change();}
		}
	};

	void unlink()
	{
		if(this->isLinked())
		{
			this->update();
			this->expr->remove_link(this);

			delete this->expr;
			this->expr = NULL;
		}
	}

	inline
	void link(Expression<Typ>& a){this->link(new ExprDirectRef(a));};

	void link(ExpressionRef* r)
	{
		assert(r);
		if(this->expr != r)
		{

#ifdef CHECK_FOR_CYCLES
			if(r->check_for_cycle(this))
				throw ERROR("illegalOperation", "Can't create a reference as this would create a cycle.");
#endif

			this->unlink();
			this->expr = r;
			this->expr->add_link(this);
			this->update();
		}
	};

///----------------------------------------------------------------------------

	bool check_for_cycle(Expression* ptr)
	{
		assert(ptr);

		if(ptr == this)
			return true;
		else if(!this->isLinked())
			return false;
		else
			return this->expr->check_for_cycle(ptr);
	}


/// GET A POINTER
///----------------------------------------------------------------------------

	inline ExpressionRefPtr ref() {return ExpressionRefPtr(new ExprDirectRef(*this));}


/// ASSIGNMENT
///----------------------------------------------------------------------------

	inline Expression<Typ>& operator=(const Typ& v)     {this->set(v);  return *this;}
	inline Expression<Typ>& operator=(Expression<Typ>& a){this->link(a); return *this;}
	//inline void operator=(ExpressionRef*   r){this->link(r);}
	inline Expression<Typ>& operator=(ExpressionRefPtr r){this->link(r); return *this;}


#ifdef USE_PROPERTY_TREE
	/// TODO: test this
	Expression<Typ>& operator=(const boost::optional<Typ>& val) {if(val) this->set(val.get()); return *this;}
#endif


///----------------------------------------------------------------------------


	ExpressionRef* plus (ExpressionRef* r)         {assert(r); return new ExprSumRef  (this->ref(), r);};
	ExpressionRef* minus(ExpressionRef* r)         {assert(r); return new ExprSubtrRef(this->ref(), r);};
	ExpressionRef* neg  ()                         {           return new ExprNegRef  (this->ref());};
	ExpressionRef* times(ExpressionRef* r)         {assert(r); return new ExprMultRef (this->ref(), r);};
	ExpressionRef* divided_by(ExpressionRef* r)    {assert(r); return new ExprDivRef  (this->ref(), r);};
	ExpressionRef* func (boost::function<Typ(Typ)> f){         return new ExprFuncRef (f, this->ref());};


	Expression<Typ>& operator+=(ExpressionRefPtr r)
	{
		ExpressionRef* orig = this->expr;
		this->expr = NULL;

		this->link(new ExprSumRef(orig, r));

		return *this;
	}


	// Sorry about those "friends" and silly pointer-wrappers (ExpressionRefPtr), but C++ has some quirks.
	// If you know a better way to achieve the following, please send me a message!
	friend ExpressionRefPtr operator+(ExpressionRefPtr l, ExpressionRefPtr r){return new ExprSumRef(l.ptr, r.ptr);};
	friend ExpressionRefPtr operator+(ExpressionRefPtr l, Typ r)             {return new ExprSumRef(l.ptr, CONST<Typ>(r));};
	friend ExpressionRefPtr operator+(Typ l, ExpressionRefPtr r)             {return new ExprSumRef(CONST<Typ>(l), r.ptr);};

	friend ExpressionRefPtr operator-(ExpressionRefPtr l, ExpressionRefPtr r){return new ExprSubtrRef(l.ptr, r.ptr);};
	friend ExpressionRefPtr operator-(ExpressionRefPtr l, Typ r)             {return new ExprSubtrRef(l.ptr, CONST<Typ>(r));};
	friend ExpressionRefPtr operator-(Typ l, ExpressionRefPtr r)             {return new ExprSubtrRef(CONST<Typ>(l), r.ptr);};

	friend ExpressionRefPtr operator*(ExpressionRefPtr l, ExpressionRefPtr r){return new ExprMultRef(l.ptr, r.ptr);};
	friend ExpressionRefPtr operator*(ExpressionRefPtr l, Typ r)             {return new ExprMultRef(l.ptr, CONST<Typ>(r));};
	friend ExpressionRefPtr operator*(Typ l, ExpressionRefPtr r)             {return new ExprMultRef(CONST<Typ>(l), r.ptr);};

	friend ExpressionRefPtr operator/(ExpressionRefPtr l, ExpressionRefPtr r){return new ExprDivRef(l.ptr, r.ptr);};
	friend ExpressionRefPtr operator/(ExpressionRefPtr l, Typ r)             {return new ExprDivRef(l.ptr, CONST<Typ>(r));};
	friend ExpressionRefPtr operator/(Typ l, ExpressionRefPtr r)             {return new ExprDivRef(CONST<Typ>(l), r.ptr);};
};

template <typename T>
typename Expression<T>::ExpressionRefPtr CONST(T val)
	{return new typename Expression<T>::ExprConstRef(val);};

template <typename T>
typename Expression<T>::ExpressionRefPtr CONST(Expression<T> val)
	{return new typename Expression<T>::ExprConstRef(val.get());};

template <typename T>
typename Expression<T>::ExpressionRefPtr EXPR(string name,
													typename Expression<T>::ExpressionRef* a,
													typename Expression<T>::ExpressionRef* b = NULL)
{
	typedef Expression<T> E;

	assert(a);
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);

	if     (name == "+" || name == "plus"  || name == "add")
		return new typename E::ExprSumRef(a,b);
	else if(name == "-" || name == "minus" || name == "subtract")
		return new typename E::ExprSubtrRef(a,b);
	else if(name == "*" || name == "times" || name == "multiply")
		return new typename E::ExprMultRef(a,b);
	else if(name == "/" || name == "divided_by" || name == "divide")
		return new typename E::ExprDivRef(a,b);
	else if(name == "min")
		return new typename E::ExprMinRef(a,b);
	else if(name == "max")
		return new typename E::ExprMaxRef(a,b);
	else if(name == "neg" || name == "negate")
		return new typename E::ExprNegRef(a);
	else
		throw ERROR("parse", "Unknown expression token '"+name+"'.");
};


/// usage eg.:
/// foo = MAX<float>(a.ref(), b.ref()) + 100;
template <typename T>
typename Expression<T>::ExpressionRefPtr MAX(typename Expression<T>::ExpressionRefPtr a,
											 typename Expression<T>::ExpressionRefPtr b)
{
	return new typename Expression<T>::ExprMaxRef(a,b);
}

template <typename T>
typename Expression<T>::ExpressionRefPtr EXPR(string name,
													typename Expression<T>::ExpressionRef* a,
													T b)
{return EXPR<T>(name,a,CONST<T>(b));}

template <typename T>
typename Expression<T>::ExpressionRefPtr EXPR(string name,
													T a,
													typename Expression<T>::ExpressionRef* b)
{return EXPR<T>(name,CONST<T>(a),b);}

template <typename T>
typename Expression<T>::ExpressionRefPtr FUNC(boost::function<T(T)> func,
											  typename Expression<T>::ExpressionRef* a)
{
	assert(a);
	return new typename Expression<T>::ExprFuncRef(func, a);
};

template <typename T>
typename Expression<T>::ExpressionRefPtr IF_ELSE(	typename Expression<T>::ExpressionRef* a,
													typename Expression<T>::ExpressionRef* b,
													boost::function<bool(T,T)> func)
{
	assert(a); assert(b);
	return new typename Expression<T>::ExprIfElseRef(a,b, func);
};

#endif // ATTRIBUTE_HPP_INCLUDED
