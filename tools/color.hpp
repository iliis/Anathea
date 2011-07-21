#ifndef COLOR_HPP_INCLUDED
#define COLOR_HPP_INCLUDED

#include <iostream>
#include <string.h>

#include <SDL/SDL.h>

#include "boost/lexical_cast.hpp"

#include "tools/stringhelpers.hpp"
#include "tools/errorclass.hpp"

using namespace std;

//------------------------------------------------------------------------------

/// von http://stackoverflow.com/questions/1070497/c-convert-hex-string-to-signed-integer (25.12.09)
class int_from_hex   // For use with boost::lexical_cast
{
    int value;
public:
    operator int() const { return value; }
    friend std::istream& operator>>( std::istream& in, int_from_hex& outValue )
    {
        return in >> std::hex >> outValue.value;
    }
};

//------------------------------------------------------------------------------

typedef int ColVal;

const ColVal A_OPAQUE           = 255;
const ColVal A_HALF_TRANSPARENT = 127;
const ColVal A_TRANSPARENT      =   0;


//------------------------------------------------------------------------------
struct Color
{
//---------- ATTRIBUTES --------------------------------------------------------
	ColVal r,g,b,a,a2;
//---------- CONSTRUCTOR -------------------------------------------------------
	Color(ColVal R, ColVal G, ColVal B, ColVal A = A_OPAQUE, ColVal A2 = A_OPAQUE)
	 : r(R), g(G), b(B), a(A), a2(A2) {};

	Color(string col)
	 : r(0), g(0), b(0), a(0), a2(A_OPAQUE)
		{this->set(col);};

	Color()
	 : r(0), g(0), b(0), a(0), a2(A_OPAQUE)
		{};

//---------- SET ---------------------------------------------------------------
	void set(string col);
	void set(ColVal R, ColVal G, ColVal B, ColVal A = A_OPAQUE, ColVal A2 = A_OPAQUE);
	void setR(ColVal R){this->r = R;}
	void setG(ColVal G){this->g = G;}
	void setB(ColVal B){this->b = B;}
	void setA(ColVal A){this->a = A;}
	void setSecondaryAlpha(ColVal A2){this->a2 = A2;}

//---------- GET ---------------------------------------------------------------
	/// for opengl:
	inline float fr(){return float(r)/A_OPAQUE;}
	inline float fg(){return float(g)/A_OPAQUE;}
	inline float fb(){return float(b)/A_OPAQUE;}
	inline float fa(){return float(a*a2)/float(A_OPAQUE*A_OPAQUE);}

//---------- MISC --------------------------------------------------------------
	bool operator==(Color const& col);
	bool operator!=(Color const& col){return ! (*this == col);}

	SDL_Color getSDL_Color();
	string getString(){return ToString(r)+","+ToString(g)+","+ToString(b)+","+ToString(a);}
};
//------------------------------------------------------------------------------



/// operator<< has to be in namespace boost for boost::variant
namespace boost
{
	inline std::ostream& operator<<(std::ostream& os, const Color& c)
	{
		os << "{" << c.r << ", " << c.g << ", " << c.b << ", " << c.a << "}";
		return os;
	}
};




const Color BLACK	= Color(0,0,0);
const Color WHITE	= Color(255,255,255);
const Color RED	= Color(255,0,0);
const Color GREEN	= Color(0,255,0);
const Color BLUE	= Color(0,0,255);
const Color YELLOW	= Color(255,255,0);
const Color ORANGE	= Color(255,128,0);
const Color PINK	= Color(255,0,255);
const Color LIGHTBLUE	= Color(0,128,255);
const Color BROWN	= Color(156,128,100);
const Color GREY	= Color(128,128,128);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const int32 RMASK = 0xff000000
	const int32 GMASK = 0x00ff0000
	const int32 BMASK = 0x0000ff00
	const int32 AMASK = 0x000000ff
#else
	const Uint32 RMASK = 0x000000ff;
	const Uint32 GMASK = 0x0000ff00;
	const Uint32 BMASK = 0x00ff0000;
	const Uint32 AMASK = 0xff000000;
#endif

//------------------------------------------------------------------------------

#endif // COLOR_HPP_INCLUDED
