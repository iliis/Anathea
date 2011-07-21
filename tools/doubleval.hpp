#ifndef DOUBLEVAL_HPP_INCLUDED
#define DOUBLEVAL_HPP_INCLUDED

#include "tools/stringhelpers.hpp"
#include "tools/mathfuncs.hpp"

#include <iostream>

//------------------------------------------------------------------------------------------------
template <typename Typ, Typ maxval>
class doubleval
{
	Typ b, l;
public:
	doubleval() : b(0), l(0) {};
	doubleval(int bb, int ll) : b(bb), l(ll)
	{
		//while(this->l < 0)		{this->l += maxval; this->b-=sgn(this->b);}
		//while(this->l > maxval)	{this->l -= maxval; this->b+=sgn(this->b);}
		this->set(bb, ll);
	};
	doubleval(double val)
	{
		this->set(val);
	};
	doubleval(string val, string delimiter=".")
	{
		val = trimString(val);

		int bb = atoi(getLeftSubstring(val, delimiter).c_str());

		int		numberOfDigits	= floor(log10(maxval));
		string	ll_string		= getRightSubstring(val, delimiter);
		int		ll				= atoi(fillStringRight(ll_string, numberOfDigits, '0').c_str());

		this->set(bb, ll);
	};

	bool operator==(doubleval dval)
	{return this->b == dval.b && this->l == dval.l;}

	bool operator!=(doubleval dval)
	{return !(*this == dval);}

	bool operator>(doubleval dval)
	{
		if(this->b == dval.b)
		{return this->l > dval.l;}
		else
		{return this->b > dval.b;}
	};

	bool operator<(doubleval dval)
	{
		if(this->b == dval.b)
		{return this->l < dval.l;}
		else
		{return this->b < dval.b;}
	};

	bool operator>=(doubleval dval)
	{return !((*this) < dval);}

	bool operator<=(doubleval dval)
	{return !((*this) > dval);}

	doubleval operator+(doubleval dval)
	{
		return doubleval(this->b + dval.b, this->l + dval.l);
	};

	doubleval operator-(doubleval dval)
	{
		return doubleval(this->b - dval.b, this->l - dval.l);
	};

	doubleval operator+(double val)
	{return (*this) + doubleval(val);};

	doubleval operator-(double val)
	{return (*this) - doubleval(val);};
	Typ getB(){return b;}
	Typ getL(){return l;}
	void setB(Typ bb)
	{this->b = bb;}
	void setL(Typ ll)
	{
		this->l = ll;
		while(this->l < 0)		{this->l += maxval; this->b-=sgn(this->b);}
		while(this->l > maxval)	{this->l -= maxval; this->b+=sgn(this->b);}
	}
	float getFloat()
	{return ( float(this->b)* float(maxval)+ float(this->l))/ float(maxval);}
	double getDouble()
	{return (double(this->b)*double(maxval)+double(this->l))/double(maxval);}
	void set(double val)
	{
		this->setB((Typ)floor(val));
		this->setL((Typ)floor(((val-floor((double)val))*maxval)+0.5)); //Nachkommastellen extrahieren
	}
	void set(Typ bb, Typ ll)
	{this->setB(bb); this->setL(ll);}

	doubleval getAbsolute()
	{
		if(this->b >= 0)
			return *this;
		else
			return doubleval(-this->b-1, maxval - this->l);
	}
};
//------------------------------------------------------------------------------------------------
template <typename Typ, Typ maxval>
ostream& operator<<(ostream& out, doubleval<Typ, maxval> val)
{
	out << "{" << val.getB() << "." << fillStringLeft(ToString(val.getL()), Log10(maxval), '0') << "}";
	return out;
}

#endif //DOUBLEVAL_HPP_INCLUDED
