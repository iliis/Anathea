#include "tools/mathparser.hpp"

//------------------------------------------------------------------------------------------------
double Log(double x){return log(x);};
double Log10(double x){return log10(x);};

double Sin(double x){return sin(x);};
double Cos(double x){return cos(x);};
double Tan(double x){return tan(x);};
double Arcsin(double x){return asin(x);};
double Arccos(double x){return acos(x);};
double Arctan(double x){return atan(x);};

//already in mathfuncs.hpp
//double toRad(double x){return x/360*PI*2;};
//double toDeg(double x){return x*360/PI/2;};

double Sqrt(double x){return sqrt(x);};

double Add		(double x, double y){return x+y;};
double Subtract	(double x, double y){return x-y;};
double Multiply	(double x, double y){return x*y;};
double Divide	(double x, double y){return x/y;};
double Pow		(double x, double y){return pow(x,y);}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
/*double
ContainerToken::getValue()
{
	double curVal=0; Token* lastT=NULL;

	bool op=false;
	BOOST_FOREACH(Token* t, this->tokens)
	{
		if(t->isOperator() != op)
			throw ERROR("parse", string("Expected a")+(op?"n operator":" value")+"-token instead of a"+(!op?"n operator":" value")+"-token.");

		if(op)
		{
			lastT = t;
		}
		else
		{
			if(lastT != NULL)
			{
				curVal = dynamic_cast<OperatorToken*>(lastT)->calc(curVal, t->getValue());
			}
			else
			{
				curVal = t->getValue();
			}
		}

		op = !op;
	}

	return curVal;
};*/
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void
Parser::changeConstant(string n, double v)
{
	BOOST_FOREACH(Constant& c, this->constants)
	{
		if(c.name == n)
		{
			c.value = v;
			return;
		}
	}
	this->addConstant(n,v);
};
//------------------------------------------------------------------------------------------------
void
Parser::addOperator(char c, int order)
{
	int i=0;
	BOOST_FOREACH(list<Operator>& l, this->operators)
	{
		if(i==order)
		{
			Operator o; o.name=c; o.func=this->getOperatorFunc(c); o.priority=order;
			l.push_back(o);
		};
		++i;
	};
};
//------------------------------------------------------------------------------------------------
Parser::Operator
Parser::getOperator(char c)
{
	BOOST_FOREACH(list<Operator>& l, this->operators)
	{
		BOOST_FOREACH(Operator o, l)
			if(o.name == c) return o;
	};
	return *(*this->operators.begin()).begin();
};
//------------------------------------------------------------------------------------------------
bool
Parser::isAnOperator(char c)
{
	//return c == plus || c == minus || c == multiplication || c == obelus || c == pow;
	BOOST_FOREACH(list<Operator> l, this->operators)
	{
		BOOST_FOREACH(Operator o, l)
		{
			if(o.name == c) return true;
		}
	}
	return false;
};
//------------------------------------------------------------------------------------------------
Token*
Parser::createTreeFromExpression(string expr)
{
	//if string is surrounded by spaces and/or brackets, eliminate them
	expr = trimString(expr);
	while(*expr.begin() == '(' && *(--expr.end()) == ')')
		expr = expr.substr(1, expr.size()-2);

	//get the "main"-operator (eg in "10*2+4" it would be the "+")
	size_t rootOperator = getRootOperator(expr);
	if(rootOperator == string::npos)
		return convertToToken(expr); //the expression is just a number or function

	string  leftExpr = expr.substr(0, rootOperator);
	string rightExpr = expr.substr(rootOperator+1);
	char		  op = *expr.substr(rootOperator, 1).begin();

	//cout << "Expr: ["<< expr <<"] -> left:["<< leftExpr <<"] right:["<< rightExpr <<"]" << endl;

	return new OperatorToken(getOperatorFunc(op), createTreeFromExpression(leftExpr), createTreeFromExpression(rightExpr));
};
//------------------------------------------------------------------------------------------------
double
Parser::parse(string expression)
{
	expression = trimString(expression);

	if(expression == "") return 0;

	//check for correct Brackets:
	if(strCount(expression, '(') != strCount(expression, ')'))
		throw ERROR("parse", string("The number of left and right brackets (\"(\" and \")\") aren't equal in \"")+expression+"\".");
	if(strCount(expression, '[') != strCount(expression, ']'))
		throw ERROR("parse", string("The number of left and right function-brackets (\"[\" and \"]\") aren't equal in \"")+expression+"\".");

	size_t equalPos = expression.find('=');
	string constantName = "Ans";
	if(equalPos != string::npos)
	{
		constantName = getLeftSubstring(expression, "=");
		expression = getRightSubstring(expression, "=");
	}

	expression = this->replaceConstants(expression);

	Token* root = createTreeFromExpression(expression);

	double val = root->getValue();
	delete root;

	this->changeConstant(constantName, val);

	return val;
};
//------------------------------------------------------------------------------------------------
Token*
Parser::convertToToken(string expr)
{
	expr = trimString(expr);

	BOOST_FOREACH(Function f, this->functions)
	{
		size_t posL = expr.find(f.name);
		if(posL != string::npos)
		{
			size_t posR = this->getCorresponingBracket(expr, posL+f.name.size(), '[', ']');
			string substring = expr.substr(posL+f.name.size()+1, posR-(posL+f.name.size()+1));
			/*cout << "expr: ["<<expr<<"]" << endl;
			cout << "function param: " << substring << endl;
			cout << "posL: " << posL << "   posR: " << posR << endl;
			cout << "size: " << f.name.size() << endl;*/
			double fval = this->parse(substring);
			fval = f.func(fval);
			return new ConstantToken(fval);
		};
	}

	return new ConstantToken(atof(expr.c_str()));
};
//------------------------------------------------------------------------------------------------
size_t
Parser::getRootOperator(string expr)
{
	size_t rPos = string::npos;
	int	   rPriority = 99999;

	int i=0; int nrBrackets=0;
	BOOST_FOREACH(char c, expr)
	{
		if(c == '(' || c == '[')
			++nrBrackets;
		else if(c == ')' || c == ']')
			--nrBrackets;
		else if(nrBrackets == 0 && this->isAnOperator(c))
		{
			Operator o = this->getOperator(c);
			if(o.priority < rPriority)
			{
				rPos = i;
				rPriority = o.priority;
			}
		}
		++i;
	}
	return rPos;
};
//------------------------------------------------------------------------------------------------
string
Parser::replaceConstants(string expression)
{
	BOOST_FOREACH(Constant c, this->constants)
	{
		expression = strReplace(expression, c.name, ToString(c.value));
	}
	return expression;
};
//------------------------------------------------------------------------------------------------
bool
Parser::isABracket(char c)
{
	return c == '(' || c == ')' || c == '[' || c == ']';
};
//------------------------------------------------------------------------------------------------
boost::function<double (double x, double y) >
Parser::getOperatorFunc(char o)
{
		 if(o == '+')return &Add;
	else if(o == '-')return &Subtract;
	else if(o == '*')return &Multiply;
	else if(o == '/')return &Divide;
	else if(o == '^')return &Pow;
	else             return &Add;
};
//------------------------------------------------------------------------------------------------
size_t
Parser::getCorresponingBracket(string expression, size_t pos, char bracketLeft, char bracketRight)
{
	int nr=0;
	BOOST_FOREACH(char c, expression.substr(pos))
	{
		if(c == bracketLeft)  ++nr;
		if(c == bracketRight) --nr;
		if(nr == 0)
			return pos;

		++pos;
	};

	return string::npos;
};
//------------------------------------------------------------------------------------------------
