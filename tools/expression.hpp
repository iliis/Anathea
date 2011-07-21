#ifndef ATTRIBUTE_HPP_INCLUDED
#define ATTRIBUTE_HPP_INCLUDED

#include <algorithm>
#include <string>

#include <boost/signals.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include "tools/errorclass.hpp"



#define CHECK_FOR_CYCLES

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
		//list<boost::signals::connection> connections;


	public:
		list<boost::signal<void()>*>    on_change, on_deletion;
		list<boost::signal<void(Typ)>*> on_change_val;

		virtual ~ExpressionRef()
		{
			/*BOOST_FOREACH(boost::signals::connection& c, this->connections)
				c.disconnect();

			this->connections.clear();*/
		};

		virtual ExpressionRef* copy() const = 0;


		void link_signals(Expression& attr)
		{
			//this->connections.push_back(attr.on_deletion  .connect(on_deletion));
			//this->connections.push_back(attr.on_change    .connect(on_change));
			//this->connections.push_back(attr.on_change_val.connect(on_change_val));
			this->on_change    .push_back(&attr.on_change);
			this->on_deletion  .push_back(&attr.on_deletion);
			this->on_change_val.push_back(&attr.on_change_val);
		};

		void link_signals(ExpressionRef& attr)
		{
			/*this->connections.push_back(attr.on_deletion  .connect(on_deletion));
			this->connections.push_back(attr.on_change    .connect(on_change));
			this->connections.push_back(attr.on_change_val.connect(on_change_val));*/

			this->on_change    .insert(this->on_change.end(),     attr.on_change.begin(),     attr.on_change.end());
			this->on_deletion  .insert(this->on_deletion.end(),   attr.on_deletion.begin(),   attr.on_deletion.end());
			this->on_change_val.insert(this->on_change_val.end(), attr.on_change_val.begin(), attr.on_change_val.end());
		};

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
		ExprDirectRef(Expression<Typ>& a) : attr(a) {link_signals(a);};

		virtual ExpressionRef* copy() const {return new ExprDirectRef(attr);};

		Typ get(){return attr.get();};
		virtual bool check_for_cycle(Expression* a){return &this->attr == a;}
	};

	class ExprConstRef : public ExpressionRef
	{
		Typ val;
	public:
		ExprConstRef(const Typ& v) : val(v) {};

		ExpressionRef* copy() const {return new ExprConstRef(this->val);}

		Typ get(){return val;}
		bool check_for_cycle(Expression* a){return false;}
	};

	class ExprSingleRef : public ExpressionRef
	{
	protected:
		ExpressionRef *ref;
	public:
		ExprSingleRef(ExpressionRef* A) : ref(A) {assert(A); link_signals(*ref);};
		~ExprSingleRef(){delete ref;}

		virtual ExpressionRef* copy() const = 0;
		virtual bool check_for_cycle(Expression* A){return ref->check_for_cycle(A);}
	};

	class ExprTwinRef : public ExpressionRef
	{
	protected:
		ExpressionRef *a, *b;
	public:
		ExprTwinRef(ExpressionRef* A, ExpressionRef* B) : a(A), b(B) {assert(a); assert(b); link_signals(*a); link_signals(*b);};
		~ExprTwinRef(){delete a; delete b;}

		virtual ExpressionRef* copy() const  = 0;
		virtual bool check_for_cycle(Expression* A){return a->check_for_cycle(A) or b->check_for_cycle(A);}
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

	Typ val;
	ExpressionRef* expr;
	list<boost::signals::connection> connections;


public:
	boost::signal<void()>    on_change, on_deletion;
	boost::signal<void(Typ)> on_change_val;

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
			on_change_val(v);
		}
	}

	void update()
	{
		if(this->isLinked())
		{
			Typ v = this->expr->get();
			if(this->val != v) { this->val = v; on_change(); on_change_val(this->val);}
		}
	};

	void unlink()
	{
		if(this->isLinked())
		{
			this->update();
			delete this->expr;
			this->expr = NULL;

			BOOST_FOREACH(boost::signals::connection& c, this->connections)
				c.disconnect();

			this->connections.clear();
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
				throw Error("illegalOperation", "Can't create a reference as this would create a cycle.");
#endif

			this->unlink();
			this->expr = r;

			/*this->connections.push_back(this->expr->on_deletion  .connect(boost::bind(&Expression<Typ>::unlink, this)));
			this->connections.push_back(this->expr->on_change    .connect(boost::bind(&Expression<Typ>::update, this)));
			this->connections.push_back(this->expr->on_change_val.connect(on_change_val));*/

			BOOST_FOREACH(boost::signal<void()>* sig_ptr, this->expr->on_deletion)
				this->connections.push_back(sig_ptr->connect(boost::bind(&Expression<Typ>::unlink, this)));

			BOOST_FOREACH(boost::signal<void()>* sig_ptr, this->expr->on_change)
				this->connections.push_back(sig_ptr->connect(boost::bind(&Expression<Typ>::update, this)));

			BOOST_FOREACH(boost::signal<void(Typ)>* sig_ptr, this->expr->on_change_val)
				this->connections.push_back(sig_ptr->connect(this->on_change_val));

			this->update();
		}
	};


	inline ExpressionRefPtr ref() {return ExpressionRefPtr(new ExprDirectRef(*this));}

	inline void operator=(const Typ& v)     {this->set(v);}
	inline void operator=(Expression<Typ>& a){this->link(a);}
	//inline void operator=(ExpressionRef*   r){this->link(r);}
	inline void operator=(ExpressionRefPtr r){this->link(r);}



	ExpressionRef* plus (ExpressionRef* r)         {assert(r); return new ExprSumRef  (this->ref(), r);};
	ExpressionRef* minus(ExpressionRef* r)         {assert(r); return new ExprSubtrRef(this->ref(), r);};
	ExpressionRef* neg  ()                         {           return new ExprNegRef  (this->ref());};
	ExpressionRef* times(ExpressionRef* r)         {assert(r); return new ExprMultRef (this->ref(), r);};
	ExpressionRef* divided_by(ExpressionRef* r)    {assert(r); return new ExprDivRef  (this->ref(), r);};
	ExpressionRef* func (boost::function<Typ(Typ)> f){         return new ExprFuncRef (f, this->ref());};


	void operator+=(ExpressionRefPtr r)
	{
		ExpressionRef* orig = this->expr;
		this->expr = NULL;

		this->link(new ExprSumRef(orig, r));
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
		throw Error("parse", "Unknown expression token '"+name+"'.");
};

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
