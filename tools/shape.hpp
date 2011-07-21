#ifndef SHAPE_HPP_INCLUDED
#define SHAPE_HPP_INCLUDED

#include "tools/geometry.hpp"
#include "managers/definitions.hpp"

//------------------------------------------------------------------------------------------------

using namespace std;

//typedef box<double> Box;
//typedef vector2<double> Vect;

//------------------------------------------------------------------------------------------------

struct Point;
struct Rect;

//------------------------------------------------------------------------------------------------
struct Shape
{
	Vect pos;

	Shape() : pos(Vect(0,0)) {};
	Shape(Vect position) : pos(position) {};

	virtual bool collide(Point const& p) const = 0;
	virtual bool collide(Rect  const& r) const = 0;
	virtual Box getBoundingBox() const = 0;
};
//------------------------------------------------------------------------------------------------
struct Point : public Shape
{
	Point() : Shape() {};
	Point(Vect position) : Shape(position) {};

	virtual bool collide(Point const& p) const {return p.pos == this->pos;};
	virtual bool collide(Rect  const& r) const;
	         bool collide(Box   const& b) const;

	Point& operator=(const Vect vect){this->pos = vect; return *this;}

	virtual Box getBoundingBox() const {return Box(pos.x, pos.y, 0, 0);}
};
//------------------------------------------------------------------------------------------------
struct Rect : public Shape
{
	Vect size;

	Rect() : Shape(), size(Vect(0,0)) {};
	Rect(Vect size) : Shape(), size(size) {};
	Rect(Vect position, Vect size) : Shape(position), size(size) {};
	Rect(Box b) : Shape(b.pos), size(b.size) {};

	virtual bool collide(Point const& p) const {return this->getBoundingBox().collide(p.pos);};
	virtual bool collide(Rect  const& r) const;

	Rect& operator=(const Box b) {this->pos = b.pos; this->size = b.size; return *this;}

	virtual Box getBoundingBox() const {return Box(pos, size);}
};
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

#endif // SHAPE_HPP_INCLUDED
