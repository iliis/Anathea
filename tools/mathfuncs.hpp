#ifndef MATHFUNCS_HPP_INCLUDED
#define MATHFUNCS_HPP_INCLUDED

#include <list>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "tools/errorclass.hpp"

#include <boost/foreach.hpp>
//------------------------------------------------------------------------------------------------
//use BOOST_FOREACH instead of these: !!
//eg. BOOST_FOREACH(int i, stdlist_of_ints){do_something(i);}

//#define        foreach(a, b, c) for(list<a>::iterator			b = c. begin(); b != c. end(); ++b)
//#define reverseForeach(a, b, c) for(list<a>::reverse_iterator	b = c.rbegin(); b != c.rend(); ++b)
//------------------------------------------------------------------------------------------------
const float PI=3.141592653589793;
const float EulersNumber=2.71828182845904523536;
//------------------------------------------------------------------------------------------------
inline double toRad(double degree){return degree/180*PI;}
inline double toDeg(double radian){return radian*180/PI;}
//------------------------------------------------------------------------------------------------
template <typename Typ>
Typ maxmin(Typ var, Typ min, Typ max)
{
	if (var < min)
	{
		return min;
	}
	else if(var > max)
	{
		return max;
	}
	return var;
};
//------------------------------------------------------------------------------
template <typename Typ>
Typ mk_angle_in_range(Typ angle)
{
	while(angle > 2*PI) angle -= 2*PI;
	while(angle < 0)    angle += 2*PI;

	return angle;
}
//------------------------------------------------------------------------------------------------
template <typename Typ>
Typ bigger(Typ a, Typ b)
{
	if(a < b){return b;}
	else     {return a;}
}
//------------------------------------------------------------------------------------------------
template <typename Typ>
Typ smaller(Typ a, Typ b)
{
	if(a > b){return b;}
	else     {return a;}
}
//------------------------------------------------------------------------------------------------
template <typename Typ>
Typ sgn(Typ val)
{
	if(val < 0)
		return Typ(-1);
	else if (val > 0)
		return Typ(1);
	else
		return Typ(0);
};
//------------------------------------------------------------------------------------------------
template <typename Typ>
Typ Abs(Typ val)
{
	if(val < 0)
		return -val;
	else
		return val;
}
//------------------------------------------------------------------------------------------------
inline unsigned int nextPowerOfTwo(unsigned int k)
{
        k--;
        for (unsigned int i=1; i<sizeof(unsigned int)*8; i=i*2)
                k = k | k >> i;
        return k+1;
};
//------------------------------------------------------------------------------------------------
inline bool isPowerOfTwo(int k)
{
	return !(k & (k - 1));
};
//------------------------------------------------------------------------------------------------
/*template <typename Typ>
bool abs(Typ val)
{
	return !(val < 0);
};*/
//------------------------------------------------------------------------------------------------
template <typename T>
bool isInList(list<T>& l, T obj)
{
	if(l.empty()){return false;}

	typename list<T>::iterator it;
	for(it = l.begin(); it != l.end(); ++it)
	{
		if(obj == *it)
			return true;
	}
	return false;
};
//------------------------------------------------------------------------------------------------
template <typename Typ>
Typ getRandomElement(list<Typ>& l)
{
	if(l.size() > 0)
	{
		int nr = rand() % l.size();
		typename list<Typ>::iterator it = l.begin();
		for(int i = 0; i < nr; ++i)
		{
			++it;
		}
		return *it;
	}
	else
	{
		throw Error("illegalOperation", "getRandomElement(list<Typ>& l): list is empty");
	}
};
//------------------------------------------------------------------------------------------------
//http://www.java2s.com/Tutorial/Cpp/0040__Data-Types/Printinganunsignedintegerinbits.htm
template <typename Typ>
string getBinary(Typ val)
{
	const int SHIFT = 8 * sizeof( Typ ) - 1;
	const Typ MASK = 1 << SHIFT;

	string ret="";

	for ( int i = 1; i <= SHIFT + 1; i++ )
	{
		ret.append( (val & MASK) ? "1" : "." );
		val <<= 1;

		if ( i % 8 == 0 )
			ret.append(" ");
   }

   return ret;
};
//------------------------------------------------------------------------------------------------
template <typename T>
typename list<T>::iterator getIteratorFromList(list<T> l, unsigned int pos)
{
	if(l.empty()){throw Error("illegalOperation", "getItemFromList(): List is empty!");}
	if(l.size() < pos){throw Error("illegalOperation", "getItemFromList(): Position is not in list!");}

	unsigned int i = 1;

	typename list<T>::iterator it;
	for(it = l.begin(); it != l.end(); ++it)
	{
		if(pos == i)
		{
			return it;
		}
		++i;
	}

	//throw Error("illegal Operation", string("THIS SHOULDN'T HAPPEN... getItemFromList hasn't found Position ")+pos+" in List (with Size "+l.size()+")");
};
//------------------------------------------------------------------------------------------------
unsigned int countOnes(char val);
//------------------------------------------------------------------------------------------------
template <typename T>
static void setNULL(T** array, size_t size)
{
   for (size_t i = 0; i < size; i++)
      array[i] = NULL;
};
//------------------------------------------------------------------------------
template <typename T, size_t size>
static void setNULL(T* (&array)[size])
{
   setNULL(array, size);
};
//------------------------------------------------------------------------------
double linear_interpolate(double val1, double val2, double percent);
//------------------------------------------------------------------------------
double smooth_interpolate(double val1, double val2, double percent);
//------------------------------------------------------------------------------
double interpolate_angle(double a1, double a2, double percent, double(*interpolation)(double, double, double) = &linear_interpolate);
//------------------------------------------------------------------------------


#endif //MATHFUNCS_HPP_INCLUDED
