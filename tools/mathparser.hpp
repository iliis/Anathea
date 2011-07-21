#ifndef SMALL_MATHPARSER_HPP_INCLUDED
#define SMALL_MATHPARSER_HPP_INCLUDED

#include <cmath>
#include <list>
#include <boost/function.hpp>
//#include <boost/bind.hpp>
#include "tools/stringhelpers.hpp"

using namespace std;


double Log(double x);
double Log10(double x);

double Sin(double x);
double Cos(double x);
double Tan(double x);
double Arcsin(double x);
double Arccos(double x);
double Arctan(double x);

double toRad(double x);
double toDeg(double x);

double Sqrt(double x);

double Add		(double x, double y);
double Subtract	(double x, double y);
double Multiply	(double x, double y);
double Divide	(double x, double y);
double Pow		(double x, double y);


class Token
{
public:
	virtual double getValue()=0;
	virtual bool isOperator()=0;
};

class OperatorToken : public Token
{
	boost::function<double (double x, double y) > func;
	Token *left, *right;
public:
	OperatorToken(boost::function<double (double x, double y) > f, Token* l, Token* r) : func(f), left(l), right(r) {};
	~OperatorToken(){delete left; delete right;}
	double getValue(){return func(left->getValue(), right->getValue());}
	bool isOperator(){return true;}
	double calc(double left, double right){return func(left, right);}
};

class ConstantToken : public Token
{
	double val;
public:
	ConstantToken(double v) : val(v) {};
	double getValue(){return val;}
	bool isOperator(){return false;}
};

/*class ContainerToken : public Token
{
	list<Token*> tokens;
public:
	~ContainerToken(){BOOST_FOREACH(Token* t, this->tokens){delete t;}};
	double getValue();
	bool isOperator(){return false;}
	void addToken(Token* t){this->tokens.push_back(t);};
};*/


class Parser
{
public:
	struct Constant
	{
		string name;
		double value;
	};

	struct Function
	{
		string name;
		boost::function<double (double x) > func;
	};

	struct Operator
	{
		char name; int priority;
		boost::function<double (double x, double y)> func;
	};

	list<list<Operator> > operators;
	list<Constant> constants;
	list<Function> functions;
	//const char plus, minus, multiplication, obelus, pow;
	//const char leftBracket, rightBracket, leftFuncBracket, rightFuncBracket;

	inline void addConstant(string n, double v){Constant c; c.name = n; c.value = v; this->constants.push_back(c);}
		   void changeConstant(string n, double v);
	inline void addFunction(string n, boost::function<double (double x) > fun){Function f; f.name = n; f.func = fun; this->functions.push_back(f);}
		   void addOperator(char c, int order);
	Operator	getOperator(char c);

	//Parser(char plus='+', char minus='-', char multiplication='*', char obelus='/', char pow='^', char leftBracket='(', char rightBracket=')', char leftFuncBracket='[', char rightFuncBracket=']')
	// : plus(plus), minus(minus), multiplication(multiplication), obelus(obelus), pow(pow), leftBracket(leftBracket), rightBracket(rightBracket), leftFuncBracket(leftFuncBracket), rightFuncBracket(rightFuncBracket)
	Parser()
	{
		//standard math functions:
		this->addFunction("Ln",  Log);
		this->addFunction("Log", Log10);
		this->addFunction("Sin", Sin);
		this->addFunction("Cos", Cos);
		this->addFunction("Tan", Tan);
		this->addFunction("Arcsin", Arcsin);
		this->addFunction("Arccos", Arccos);
		this->addFunction("Arctan", Arctan);
		this->addFunction("Deg", toDeg);
		this->addFunction("Rad", toRad);
		this->addFunction("Sqrt", Sqrt);
		//this->addFunction("", &);

		this->addConstant("_PI", PI);
		this->addConstant("_E",  EulersNumber);
		this->addConstant("_THEANSWER", 42);
		//this->addConstant("", );

		list<Operator> emptyList;
		this->operators.push_back(emptyList);
		this->operators.push_back(emptyList);
		this->operators.push_back(emptyList);
		this->operators.push_back(emptyList);
		this->operators.push_back(emptyList);

		//simple Operators
		this->addOperator('+', 0);
		this->addOperator('-', 0);
		this->addOperator('*', 1);
		this->addOperator('/', 1);
		this->addOperator('^', 2);
	};

	double parse(string expression);
	string replaceConstants(string expression);

	bool isABracket(char c);
	bool isAnOperator(char c);
	size_t getCorresponingBracket(string expression, size_t pos, char bracketLeft='(', char bracketRight=')');

	boost::function<double (double x, double y) > getOperatorFunc(char o);
	Token* convertToToken(string expr);
	Token* createTreeFromExpression(string expr);

	inline list<Function> getFunctions(){return this->functions;}
				string    getFunctionNames(){string ret=""; BOOST_FOREACH(Function f, this->functions){ret = ret+f.name+"\n";} return ret;}

	size_t getRootOperator(string expr);
};

#endif // SMALL_MATHPARSER_HPP_INCLUDED
