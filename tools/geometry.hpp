#ifndef GEOMETRY_HPP_INCLUDED
#define GEOMETRY_HPP_INCLUDED

#include <string>
#include <iostream>
#include <math.h>

#include "tools/errorclass.hpp"
#include "tools/mathfuncs.hpp"
#include "tools/stringhelpers.hpp"

#ifdef USE_TINYXML
#include "tools/ticpp/ticpp.h"
#endif

#ifdef USE_PROPERTY_TREE
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#endif

inline bool valid_dim2(const int& x){return x==1 or x==2;}
inline bool valid_dim3(const int& x){return x==1 or x==2 or x==3;}

template <typename T>
inline T get_dim2(int dim, T x, T y) {return dim==1?x:y;}

template <typename T>
inline T get_dim3(int dim, T x, T y, T z){return dim==1?x:(dim==2?y:z);}

inline string get_dim2XY(int dim){return get_dim2(dim,"x","y");}

using namespace std;

//------------------------------------------------------------------------------------------------
template <typename Typ>
class vector2
{
public:
    Typ x, y;
    typedef Typ T;

    vector2() : x(0), y(0) {};
    vector2(Typ xx, Typ yy) : x(xx), y(yy) {};
#ifdef USE_TINYXML
    vector2(ticpp::Element* element, string X="x", string Y="y", vector2<Typ> def = vector2<Typ>()) : x(0),y(0)// x(getAttribute<Typ>(element, X, def.x)), y(getAttribute<Typ>(element, Y, def.y))
    {
    	if(element != NULL)
    	{
    		element->GetAttributeOrDefault(X, &this->x, def.x);
    		element->GetAttributeOrDefault(Y, &this->y, def.y);
			//if(element->Attribute(X) != NULL){element->QueryValueAttribute<Typ>(X, &this->x);}
			//if(element->Attribute(Y) != NULL){element->QueryValueAttribute<Typ>(Y, &this->y);}
    	}
    };
#endif

#ifdef USE_PROPERTY_TREE
	vector2(ptree pt, string X="x", string Y="y", vector2<Typ> def = vector2<Typ>())
	{
		this->x = pt.get<Typ>(X, def.x);
		this->y = pt.get<Typ>(Y, def.y);
	};
#endif

	vector2(string v) : x(0),y(0)
	{
		string::size_type
		c1 = v.find_first_of(',');

		this->x = atoi(v.substr(0,c1).c_str());
		this->y = atoi(v.substr(c1+1).c_str());
	}

	inline vector2<Typ> Xonly() const {return vector2<Typ>(this->x,0);}
	inline vector2<Typ> Yonly() const {return vector2<Typ>(0,this->y);}
	inline vector2<Typ> abs()   const {return vector2<Typ>(Abs(this->x), Abs(this->y));}

	template <typename T>
	vector2<Typ>& operator=(vector2<T>& other)
	{
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

    bool operator==(vector2<Typ> vgl) const
    {return this->x == vgl.x && this->y == vgl.y;}
    bool operator!=(const vector2<Typ> &vgl) const
    {return !(*this == vgl);}
    vector2<Typ> operator+(const vector2<Typ> &add) const
    {
    	vector2<Typ> p;
		p.x = x + add.x;
		p.y = y + add.y;
		return p;
	};
	vector2<Typ> operator-(const vector2<Typ> &sub) const
    {
    	vector2<Typ> p;
		p.x = x - sub.x;
		p.y = y - sub.y;
		return p;
	};
	vector2<Typ> operator*(const Typ &scalar) const
	{
		vector2<Typ> p;
		p.x = x*scalar;
		p.y = y*scalar;
		return p;
	};
	vector2<Typ> operator*(const vector2<Typ> &vect) const
	{
		vector2<Typ> p;
		p.x = x*vect.x;
		p.y = y*vect.y;
		return p;
	};
	vector2<Typ> operator/(const Typ &scalar) const
	{
		vector2<Typ> p;
		p.x = x/scalar;
		p.y = y/scalar;
		return p;
	};
	vector2<Typ> operator/(const vector2<Typ> &vect) const
	{
		vector2<Typ> p;
		p.x = x/vect.x;
		p.y = y/vect.y;
		return p;
	};

	Typ& operator[](const int dim)
	{
		assert(valid_dim2(dim));

		return (dim==1?this->x:this->y);
	}

	vector2<Typ> rotateCCW()
	{
		this->y = -this->y;
		return *this;
	};

	vector2<Typ> rotate(double angle)
	{
		Typ X = this->x; double S = sin(angle);
		Typ Y = this->y; double C = cos(angle);

		this->x = C*X - S*Y;
		this->y = S*X + C*Y;

		return *this;
	}

	vector2<Typ> scaleXto(double newX)
	{
		this->y = newX / this->x * this->y;
		this->x = newX;
		return *this;
	};

	vector2<Typ> scaleYto(double newY)
	{
		this->x = newY / this->y * this->x;
		this->y = newY;
		return *this;
	};

	vector2<Typ> scaleTo(double new_lenght)
	{
		Typ l = this->length;
		this->x = this->x / l * new_lenght;
		this->y = this->y / l * new_lenght;
		return *this;
	}

	vector2<Typ> Floor() ///< grossgeschrieben weils sonst mit "normalem" floor(x) durcheinandergebracht wird
	{
		this->x = floor(this->x);
		this->y = floor(this->y);
		return *this;
	}

	Typ length() const
	{
		return sqrt(this->x*this->x + this->y*this->y);
	};

	bool isZero() const
	{
		return this->x == 0 && this->y == 0;
	};

	vector2<Typ>& makeUnitVector()
	{
		Typ l = this->length();
		this->x = this->x / l;
		this->y = this->y / l;
		return *this;
	};

	vector2<Typ> getUnitVector()
	{
		Typ l = this->length();
		return vector2<Typ>(this->x / l, this->y / l);
	}

	template <typename Dest>
	vector2<Dest> cast() const
	{
		vector2<Dest> tmp(static_cast<Dest>(this->x), static_cast<Dest>(this->y));
		return tmp;
	};

	bool isNull() {return this->x == 0 && this->y == 0;}
	string const print() const
	{
		return std::string("[ ") + this->x + " | " + this->y + " ]";
	};
	string printCSV()
		{return std::string() + this->x + "," + this->y;}

	template <typename T>
	friend ostream &operator<<(ostream& stream, vector2<T>& obj);

	template <typename T>
	friend istream &operator>>(istream& stream, vector2<T>& obj);
};

template <typename Typ>
ostream &operator<<(ostream &stream, vector2<Typ>& obj)
{
	stream << obj.print();
	return stream;
};

template <typename Typ>
istream &operator>>(istream &stream, vector2<Typ>& obj)
{
	stream >> obj.x;
	stream >> obj.y;
	return stream;
};

#define vect2for(ITER, LIMIT) for((ITER).y = 0; (ITER).y < (LIMIT).y; ++(ITER).y) for((ITER).x = 0; (ITER).x < (LIMIT).x; ++(ITER).x)

/// operator<< has to be in namespace boost for boost::variant
namespace boost
{
	template <typename T>
	std::ostream& operator<<(std::ostream& os, const vector2<T>& v)
	{
		os << "[ " << v.x << " | " << v.y << " ]";
		return os;
	}
}
//------------------------------------------------------------------------------------------------
template <typename Typ>
class vector3
{
public:
    Typ x, y, z;

    typedef Typ T;

    vector3() : x(0), y(0), z(0) {};
    vector3(Typ xx, Typ yy, Typ zz) : x(xx), y(yy), z(zz) {};
#ifdef USE_TINYXML
    /*vector3(TiXmlElement* element, string X, string Y, string Z) : x(0), y(0), z(0)
    {
    	if(element->Attribute(X) != NULL){element->QueryValueAttribute<Typ>(X, &this->x);}
    	if(element->Attribute(Y) != NULL){element->QueryValueAttribute<Typ>(Y, &this->y);}
    	if(element->Attribute(Z) != NULL){element->QueryValueAttribute<Typ>(Z, &this->z);}
    };*/
    vector3(TiXmlElement* element, string X, string Y, string Z, vector3<Typ> def = vector3<Typ>()) : x(def.x), y(def.y), z(def.z)
    {
    	if(element != NULL)
    	{
			if(element->Attribute(X) != NULL){element->QueryValueAttribute<Typ>(X, &this->x);}
			if(element->Attribute(Y) != NULL){element->QueryValueAttribute<Typ>(Y, &this->y);}
			if(element->Attribute(Z) != NULL){element->QueryValueAttribute<Typ>(Z, &this->z);}
    	}
    };
#endif

#ifdef USE_PROPERTY_TREE
	vector3(ptree pt, string X="x", string Y="y", string Z="z", vector3<Typ> def = vector3<Typ>())
	{
		this->x = pt.get<Typ>(X, def.x);
		this->y = pt.get<Typ>(Y, def.y);
		this->z = pt.get<Typ>(Z, def.z);
	};
#endif

	vector3(string v) : x(0),y(0),z(0)
	{
		string::size_type
		c1 = v.find_first_of(','),
		c2 = v.find_first_of(',', c1+1);

		this->x = atoi(v.substr(0,c1).c_str());
		this->y = atoi(v.substr(c1+1,c2-c1).c_str());
		this->z = atoi(v.substr(c2+1).c_str());
	}

	inline vector3<Typ> Xonly(){return vector3<Typ>(this->x,0,0);}
	inline vector3<Typ> Yonly(){return vector3<Typ>(0,this->y,0);}
	inline vector3<Typ> Zonly(){return vector3<Typ>(0,0,this->z);}

	inline vector3<Typ> abs()  {return vector3<Typ>(abs(this->x), abs(this->y), abs(this->z));}

    bool operator==(vector3<Typ> vgl)
    {return this->x == vgl.x && this->y == vgl.y && this->z == vgl.z;}
    bool operator!=(const vector3<Typ> &vgl)
    {return !(*this == vgl);}

    vector3<Typ> operator+(const vector3<Typ> &add)
    {
    	vector3<Typ> p;
		p.x = this->x + add.x;
		p.y = this->y + add.y;
		p.z = this->z + add.z;
		return p;
	};
	vector3<Typ> operator-(const vector3<Typ> &sub)
    {
    	vector3<Typ> p;
		p.x = this->x - sub.x;
		p.y = this->y - sub.y;
		p.z = this->z - sub.z;
		return p;
	};
	vector3<Typ> operator*(const Typ &scalar)
	{
		vector3<Typ> p;
		p.x = this->x*scalar;
		p.y = this->y*scalar;
		p.z = this->z*scalar;
		return p;
	};
	vector3<Typ> operator*(const vector3<Typ> &vect)
	{
		vector2<Typ> p;
		p.x = this->x*vect.x;
		p.y = this->y*vect.y;
		p.z = this->z*vect.z;
		return p;
	};
	vector3<Typ> operator/(const Typ &scalar)
	{
		vector3<Typ> p;
		p.x = this->x/scalar;
		p.y = this->y/scalar;
		p.z = this->z/scalar;
		return p;
	};

	Typ& operator[](const int dim)
	{
		assert(valid_dim3(dim));

		return (dim==1?this->x:(dim==2?this->y:this->z));
	}

	Typ length()
	{
		return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
	};

	bool isZero()
	{
		return this->x == 0 && this->y == 0 && this->z == 0;
	}

	vector3<Typ> getUnitVector()
	{
		Typ l = this->length();
		this->x = this->x / l;
		this->y = this->y / l;
		this->z = this->z / l;
		return *this;
	};

	vector3<Typ> rotate90X()
	{
		/// z -> -y
		/// y ->  z

		Typ tmp_y = this->y;
		this->y  = this->z;
		this->z  = -tmp_y;
		return *this;
	}

	vector3<Typ> rotate90Y()
	{
		/// x -> -z
		/// z ->  x

		Typ tmp_x = this->x;
		this->x  = -this->z;
		this->z  = tmp_x;
		return *this;
	}

	vector3<Typ> rotate90Z()
	{
		/// x ->  y
		/// y -> -x

		Typ tmp_x = this->x;
		this->x  = this->y;
		this->y  = -tmp_x;
		return *this;
	}

	vector3<Typ> Floor()
	{
		this->x = floor(this->x);
		this->y = floor(this->y);
		this->z = floor(this->z);
		return *this;
	}

	vector2<Typ> getVect2()
	{
		return vector2<Typ>(this->x, this->y);
	};

	template <typename Dest>
	vector3<Dest> get()
	{
		vector3<Dest> tmp(static_cast<Dest>(this->x), static_cast<Dest>(this->y), static_cast<Dest>(this->z));
		return tmp;
	};

	bool isNull() {return this->x == 0 && this->y == 0 && this->z == 0;}
	string print()
	{
		return std::string("[ ") + this->x + " | " + this->y + " | " + this->z + " ]";
	};
	string printCSV()
		{return std::string() + this->x + "," + this->y + "," + this->z;}

	template <typename T>
	friend ostream &operator<<(ostream& stream, vector3<T>& obj);

	template <typename T>
	friend istream &operator>>(istream& stream, vector3<T>& obj);
};

template <typename Typ>
ostream &operator<<(ostream &stream, vector3<Typ>& obj)
{
	stream << obj.print();
	return stream;
};

template <typename Typ>
istream &operator>>(istream &stream, vector3<Typ>& obj)
{
	stream >> obj.x;
	stream >> obj.y;
	stream >> obj.z;
	return stream;
};

//------------------------------------------------------------------------------

class LINES_PARALLEL : public Error
{
	//string type;
	//string text;
public:
	LINES_PARALLEL(string lines_data = "")
	: ERROR("fail", "Cannot calc intersection of lines because they are parallel."+lines_data) {};
};

//------------------------------------------------------------------------------
// calculates the intersection-point of two parametric vector lines
template <typename Typ>
vector2<Typ> lineIntersection(vector2<Typ> l1_start, vector2<Typ> l1_direction,
							  vector2<Typ> l2_start, vector2<Typ> l2_direction)
{
	Typ det = l1_direction.y*l2_direction.x - l1_direction.x*l2_direction.y;
	if(det == 0) // Determinante null => keine Lösung => l1_dir = l2_dir
		throw LINES_PARALLEL("\nL1: start: "+l1_start.print()+" direction: "+l1_direction.print()
							+"\nL2: start: "+l2_start.print()+" direction: "+l2_direction.print());
	else
	{
		return l1_start + l1_direction * (l2_direction.y*(l1_start.x-l2_start.x) - l2_direction.x*(l1_start.y-l2_start.y))/det;
	}
}
//------------------------------------------------------------------------------
// only checks if the intersection point of the two lines actually lies on both lines
template <typename Typ>
bool actuallyIntersectsLine(vector2<Typ> l1_start, vector2<Typ> l1_direction,
							vector2<Typ> l2_start, vector2<Typ> l2_direction)
{
	Typ det = l1_direction.y*l2_direction.x - l1_direction.x*l2_direction.y;
	if(det == 0) // Determinante null => keine Lösung => l1_dir = l2_dir
		return (l1_start == l2_start);
		//throw LINES_PARALLEL("\nL1: start: "+l1_start.print()+" direction: "+l1_direction.print()
		//					+"\nL2: start: "+l2_start.print()+" direction: "+l2_direction.print());
	else
	{
		Typ factor1 = (l2_direction.y*(l1_start.x-l2_start.x) - l2_direction.x*(l1_start.y-l2_start.y))/det;
		Typ factor2 = (l1_direction.y*(l1_start.x-l2_start.x) - l1_direction.x*(l1_start.y-l2_start.y))/det;

		return factor1 >= 0 && factor1 <=1
			&& factor2 >= 0 && factor2 <=1;
	}
}
//------------------------------------------------------------------------------
/// Eigentlich ne QR-Zerlegung ;)

template <typename Typ>
vector2<Typ> orthproj_to_line(vector2<Typ> line_start, vector2<Typ> line_direction, vector2<Typ> point)
{
	line_direction = line_direction.getUnitVector();
	point          = point - line_start;
	Typ factor     = point.x*line_direction.x + point.y*line_direction.y;

	return line_direction*factor + line_start;
}

//------------------------------------------------------------------------------

#define vect3for(ITER, LIMIT) for((ITER).z = 0; (ITER).z < (LIMIT).z; ++(ITER).z) for((ITER).y = 0; (ITER).y < (LIMIT).y; ++(ITER).y) for((ITER).x = 0; (ITER).x < (LIMIT).x; ++(ITER).x)

/// operator<< has to be in namespace boost for boost::variant
namespace boost
{
	template <typename T>
	std::ostream& operator<<(std::ostream& os, const vector3<T>& v)
	{
		os << "[ " << v.x << " | " << v.y << " | " << v.z << " ]";
		return os;
	}
}
//------------------------------------------------------------------------------------------------
template <typename Typ>
vector2<Typ> toVector2(Typ x, Typ y)
{
	return vector2<Typ>(x, y);
};
//------------------------------------------------------------------------------------------------
template <typename Typ>
vector3<Typ> toVector3(Typ x, Typ y, Typ z)
{
	return vector3<Typ>(x, y, z);
};
//------------------------------------------------------------------------------------------------
template<typename Typ>
vector2<Typ> fitWhileRetainRatio(vector2<Typ> source, vector2<Typ> dest)
{
	vector2<Typ> xyRatio = dest / source;
	Typ ratio  = smaller<Typ>(xyRatio.x, xyRatio.y);
	return source*ratio;
};
//------------------------------------------------------------------------------
/// gibt den grösseren der beiden Vektoren zurück (x und y unabhängig)
template <typename T>
vector2<T> max(const vector2<T> v1, const vector2<T> v2)
{
	vector2<T> result;
	if(v1.x > v2.x) result.x = v1.x; else result.x = v2.x;
	if(v1.y > v2.y) result.y = v1.y; else result.y = v2.y;
	return result;
};
//------------------------------------------------------------------------------
/// gibt den kleineren der beiden Vektoren zurück (x und y unabhängig)
template <typename T>
vector2<T> min(const vector2<T> v1, const vector2<T> v2)
{
	vector2<T> result;
	if(v1.x < v2.x) result.x = v1.x; else result.x = v2.x;
	if(v1.y < v2.y) result.y = v1.y; else result.y = v2.y;
	return result;
};
//------------------------------------------------------------------------------
/// setzt einen Vektor, so dass er nicht grösser/kleiner als max/min ist
/// (x und y unabhängig, retval==true => hat gepasst)
/// also min < fit < max
/// und und Vect(0,0) = beliebig
template <typename T>
bool minmaxVect(const vector2<T> min, const vector2<T> max, vector2<T>& fit)
{
	bool ret=true; ///< fit?

	if(fit.x > max.x and max.x != 0)
	{
		fit.x = max.x;
		ret = false;
	}
	else if(fit.x < min.x and min.x != 0)
	{
		fit.x = min.x;
		ret = false;
	}
	/// else -> fit.x fits :)


	if(fit.y > max.y and max.y != 0)
	{
		fit.y = max.y;
		ret = false;
	}
	else if(fit.y < min.y and min.y != 0)
	{
		fit.y = min.y;
		ret = false;
	}
	/// else -> fit.y fits :)

	return ret;
};
//------------------------------------------------------------------------------------------------
template <typename Typ>
class box
{
public:
    vector2<Typ> pos;
    vector2<Typ> size;

    box() : pos(), size() {};
    box(vector2<Typ> p, vector2<Typ> s) : pos(p), size(s) {};
    box(Typ x, Typ y, Typ w, Typ h) : pos(x, y), size(w, h) {};

#ifdef USE_PROPERTY_TREE
	box(ptree pt, string X="x", string Y="y", string W="w", string H="h", box<Typ> def = box<Typ>())
	 : pos(pt, X, Y, def.pos),
	   size(pt, W, H, def.size)
	{};
#endif

	template <typename Dest>
	box<Dest> get()
	{
		box<Dest> tmp(this->pos.get<Dest>(), this->size.get<Dest>());
		return tmp;
	};

    template <typename Q>
    bool collide(const Q &x, const Q &y)
    {
        return this->collide(vector2<Q>(x,y));
    };

	// assumes size is not negative!
    template <typename Q>
    bool collide(const vector2<Q> &p)
    {
        return  p.x >= this->pos.x &&
                p.y >= this->pos.y &&
                p.x <= this->pos.x+this->size.x &&
                p.y <= this->pos.y+this->size.y;
    };

	// assumes size is not negative!
    template <typename Q>
    bool collide(const box<Q> &b)
    {
        return  b.pos.x <= this->pos.x+this->size.x &&
                b.pos.y <= this->pos.y+this->size.y &&
                b.pos.x+b.size.x >= this->pos.x &&
                b.pos.y+b.size.y >= this->pos.y;
    };

    // assumes size is not negative!
    bool collide_line(vector2<Typ> &a, vector2<Typ> &b)
    {
    	// "optimization": kills all certain negatives
    	if( (a.x < this->pos.x and b.x < this->pos.x) or
			(a.y < this->pos.y and b.y < this->pos.y) or
			(a.x > this->pos.x+this->size.x and b.x > this->pos.x+this->size.x) or
			(a.y > this->pos.y+this->size.y and b.y > this->pos.y+this->size.y))
				return false; // obiously no collision possible :P
		else
		// if one of the endpoints of the line is IN the box then we do have a positive collision!
		if( this->collide(a) || this->collide(b) )
			return true;
		else
		if( actuallyIntersectsLine<Typ>(this->pos, this->size.Xonly(), a, b-a) || // intersection with top line?
			actuallyIntersectsLine<Typ>(pos, size.Yonly(), a, b-a) || // left
			actuallyIntersectsLine<Typ>(pos+size.Yonly(), size.Xonly(), a, b-a) || // bottom
			actuallyIntersectsLine<Typ>(pos+size.Xonly(), size.Yonly(), a, b-a) )// right
				return true;
		else
			return false; // definitely no collision
    };

    bool operator==(const box<Typ> &vgl)
        {return this->pos == vgl.pos && this->size == vgl.size;}

    bool operator!=(const box<Typ> &vgl)
        {return !(*this == vgl);}

	box<Typ> operator+(const box<Typ> &add)
	{
		box<Typ> s;
		s.pos = this->pos + add.pos;
		s.size = this->size + add.size;
		return s;
	};

	box<Typ> operator-(const box<Typ> &sub)
	{
		box<Typ> d;
		d.pos = this->pos - sub.pos;
		d.size = this->size - sub.size;
		return d;
	};

	string print()
	{
		return this->pos.print() + "[ " + this->size.x + " x " + this->size.y + " ]";
	};

	void center(box<Typ> frame)
	{
		this->pos = frame.pos + frame.size/2 - this->size/2;
	};
};
//------------------------------------------------------------------------------------------------
template <typename Typ>
struct CBoxEdge
{
	vector2<Typ> p1,p2;

	void assertValid()
	{
		/// ordered and flat?
		assert((this->p1.y <= this->p2.y and this->p1.x == this->p2.x)
			or (this->p1.x <= this->p2.x and this->p1.y == this->p2.y));
	}

	CBoxEdge() : p1(0,0), p2(0,0){};
	CBoxEdge(Typ p1_x, Typ p1_y, Typ p2_x, Typ p2_y)
	 : p1(p1_x, p1_y), p2(p2_x, p2_y)
	{
		if(p1.x > p2.x) swap(p1.x, p2.x);
		if(p1.y > p2.y) swap(p1.y, p2.y);

		this->assertValid();
	};
	CBoxEdge(vector2<Typ> P1, vector2<Typ> P2) : CBoxEdge(P1.x, P1.y, P2.x, P2.y) {};

	inline vector2<Typ> getP1(){return p1;}
	inline vector2<Typ> getP2(){return p2;}
	inline void setP1(vector2<Typ> v){this->p1 = v; this->assertValid(); }
	inline void setP2(vector2<Typ> v){this->p2 = v; this->assertValid(); }

	inline CBoxEdge<Typ> operator+(vector2<Typ> v)
		{return CBoxEdge(p1+v, p2+v);}

	inline CBoxEdge<Typ> operator-(vector2<Typ> v)
		{return CBoxEdge(p1-v, p2-v);}

	inline CBoxEdge<Typ> operator*(Typ s)
		{return CBoxEdge(p1*s, p2*s);}

	inline CBoxEdge<Typ> operator/(Typ s)
		{return CBoxEdge(p1/s, p2/s);}
};
//------------------------------------------------------------------------------------------------
/// the same as \a Box, but with zero at center instead of topleft
/// use CBox instead, as it is a bit more elegant ;)
template <typename Typ>
struct CBox
{
	CBox() : center(), extend() {};
    CBox(vector2<Typ> c, vector2<Typ> e) : center(c), extend(e) {};
    CBox(Typ x, Typ y, Typ w, Typ h) : center(x, y), extend(w, h) {};
    CBox(CBoxEdge<Typ> e) : center((e.p1+e.p2)/2), extend(e.p2-center) {};

	vector2<Typ> center, extend;

	Typ getWidth() {return extend.x*2;}
	Typ getHeight(){return extend.y*2;}
	vector2<Typ> getSize(){return extend*2;}

	inline box<Typ> getNBox(){return box<Typ>(center-extend, extend*2);}

	inline Typ min(int dim) {return center[dim] - extend[dim];} ///< left / top side
	inline Typ max(int dim) {return center[dim] + extend[dim];} ///< right / bottom side

	template <typename Dest>
	CBox<Dest> get()
	{
		CBox<Dest> tmp(this->center.get<Dest>(), this->extend.get<Dest>());
		return tmp;
	};

	/// edges
	inline CBoxEdge<Typ> left_edge()
		{return CBoxEdge<Typ>(center.x-extend.x, center.y-extend.y, center.x-extend.x, center.y+extend.y);}
	inline CBoxEdge<Typ> right_edge()
		{return CBoxEdge<Typ>(center.x+extend.x, center.y-extend.y, center.x+extend.x, center.y+extend.y);}
	inline CBoxEdge<Typ> top_edge()
		{return CBoxEdge<Typ>(center.x-extend.x, center.y-extend.y, center.x+extend.x, center.y-extend.y);}
	inline CBoxEdge<Typ> bottom_edge()
		{return CBoxEdge<Typ>(center.x-extend.x, center.y+extend.y, center.x+extend.x, center.y+extend.y);}

	inline
	CBoxEdge<Typ> edgeX(Typ dir_x)
	{
		if(dir_x < 0)
			return left_edge();
		else
			return right_edge();
	}

	inline
	CBoxEdge<Typ> edgeY(Typ dir_y)
	{
		if(dir_y < 0)
			return top_edge();
		else
			return bottom_edge();
	}

	inline
	CBoxEdge<Typ> edge(int dim, Typ dir)
	{
		assert(valid_dim2(dim));

		return (dim == 1? edgeX(dir) : edgeY(dir) );
	}



    bool operator==(const CBox<Typ> &vgl)
        {return this->center == vgl.center && this->extend == vgl.extend;}

    bool operator!=(const CBox<Typ> &vgl)
        {return !(*this == vgl);}

	CBox<Typ> operator+(const CBox<Typ> &add)
	{
		CBox<Typ> s;
		s.center = this->center + add.center;
		s.extend = this->extend + add.extend;
		return s;
	};

	CBox<Typ> operator-(const CBox<Typ> &add)
	{
		CBox<Typ> s;
		s.center = this->center - add.center;
		s.extend = this->extend - add.extend;
		return s;
	};

	string print()
	{
		return "Center: "+this->center.print() + "[ " + this->extend.x*2 + " x " + this->extend.y*2 + " ]";
	};


	bool collides(const CBox<Typ>& other)
	{
		vector2<Typ> delta((this->center - other.center).abs);
		return delta.x <= this->extend.x+other.extend.x and delta.y <= this->extend.y+other.extend.y;
	}

	bool collides(const vector2<Typ>& point)
	{
		return     point.x >= this->center.x - this->extend.x
		       and point.x <= this->center.x + this->extend.x

		       and point.y >= this->center.y - this->extend.y
		       and point.y <= this->center.y + this->extend.y;
	}

	/*bool collides(const vector2<Typ> &p1, const vector2<Typ> &p2)
	{
		vector2<Typ> delta, relpos;
		// NOT YET IMPLEMENTED
	}*/

	vector2<Typ> manhattan_dist_between(CBox<Typ>& other)
	{
		Typ d = (other.center - this->center).abs - this->extend - other.extend;
		return (d > 0?d:0);}
};
//------------------------------------------------------------------------------------------------

#endif //GEOMETRY_HPP_INCLUDED
