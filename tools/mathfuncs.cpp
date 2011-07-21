#include "mathfuncs.hpp"

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
unsigned int countOnes(char val)
{
	const int SHIFT = 7;
	const char MASK = 1 << SHIFT;

	unsigned int ret = 0;

	for ( int i = 1; i <= SHIFT + 1; i++ )
	{
		if(val & MASK){++ret;}
		val <<= 1;
	}

	return ret;
};
//------------------------------------------------------------------------------------------------
double
linear_interpolate(double val1, double val2, double percent)
{
	return val1*(1-percent) + val2*percent;
};
//------------------------------------------------------------------------------------------------
double
smooth_interpolate(double val1, double val2, double percent)
{
	/// y1 + (y2-y1) * (1-cos( x/delta * PI ))/2
	return val1 + (val2-val1) * (1-cos(percent*PI))/2;
}
//------------------------------------------------------------------------------
double
interpolate_angle(double a1, double a2, double percent, double(*interpolation)(double, double, double))
{
	double a;

	if(Abs(a2-a1) <= PI)
		a = (*interpolation)(a1,a2,percent);
	else
	{
		if(a1<a2)
			a = (*interpolation)(a1+2*PI,a2,percent);
		else
			a = (*interpolation)(a1,a2+2*PI,percent);
	}

	if(a<0)
		return a+2*PI;
	else if(a>2*PI)
		return a-2*PI;
	else
		return a;
};
//------------------------------------------------------------------------------------------------
