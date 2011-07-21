#ifndef VECTORMATH_HPP_INCLUDED
#define VECTORMATH_HPP_INCLUDED

#include "tools/geometry.hpp"

typedef vector2<double> Vect2;
typedef box<double> Box;

//------------------------------------------------------------------------------------------------
float orthogonalDistance(Vect2 point, Vect2 lineStart, Vect2 lineEnd);
//------------------------------------------------------------------------------------------------
float averageDistance(list<Vect2 > points);
//------------------------------------------------------------------------------------------------
Vect2 averageCenter(list<Vect2 > points);
//------------------------------------------------------------------------------------------------
list<Vect2 > getNearestPoints(list<Vect2 >::iterator center, list<Vect2 > points, float dist);
//------------------------------------------------------------------------------------------------
struct group
{
	Vect2 p;
	int nr; //0 = keine Gruppe

	bool operator<(group& g)
	{
		return this->nr < g.nr;
	}
};
list<group> groupPoints(list<group>::iterator start, list<group> points, float dist);
//------------------------------------------------------------------------------------------------
list<list<Vect2 > > seperateByDistance(list<Vect2 > points, float dist);
//------------------------------------------------------------------------------------------------
list<Vect2 > simplifyLine(list<Vect2 > line, float maxDist);
//------------------------------------------------------------------------------------------------
struct UniqueBox
{
	string id;

	Box shape;
	double scale;

	UniqueBox(string id)			: id(id), shape(Vect2(), Vect2()), scale(1) {};
	UniqueBox(string id, Box size)	: id(id), shape(size), scale(1) {};
};

list<UniqueBox> arrangeInBoxRaster(Vect2 dest, list<UniqueBox> boxes, double border=0);
list<UniqueBox> arrangeInBoxLines (Vect2 dest, list<UniqueBox> boxes, double border=0, double stepsize=1, double* height = NULL, double rowheight=0);
//------------------------------------------------------------------------------------------------
template <typename T>
vector3<T> crossProduct(vector3<T> v1, vector3<T> v2)
{
	vector3<T> result;

	result.x = v1.y * v2.z  -  v1.z * v2.y;
	result.y = v1.z * v2.x  -  v1.x * v2.z;
	result.z = v1.x * v2.y  -  v1.y * v2.x;

	return result;
};
//------------------------------------------------------------------------------
template <typename T>
T dotProduct(vector3<T> v1, vector3<T> v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
};
//------------------------------------------------------------------------------
template <typename T>
vector3<T> rotate(vector3<T> v, vector3<T> axis, double rad)
{
	/// -> http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_given_an_axis_and_an_angle
	axis.getUnitVector();
	double c = cos(rad), s = sin(rad);
	T x=axis.x,y=axis.y,z=axis.z;
	vector3<T> ret;

	ret.x = v.x*(  x*x+(1-x*x)*c  ) + v.y*(  x*y*(1-c)-z*s  ) + v.z*(  x*z*(1-c)+y*s  );
	ret.y = v.x*(  x*y*(1-c)+z*s  ) + v.y*(  y*y+(1-y*y)*c  ) + v.z*(  y*z*(1-c)-x*s  );
	ret.z = v.x*(  x*z*(1-c)-y*s  ) + v.y*(  y*z*(1-c)+x*s  ) + v.z*(  z*z+(1-z*z)*c  );

	return ret;
};
//------------------------------------------------------------------------------

#endif //VECTORMATH_HPP_INCLUDED
