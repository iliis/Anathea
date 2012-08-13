#include "tools/color.hpp"

//------------------------------------------------------------------------------
void
Color::set(ColVal R, ColVal G, ColVal B, ColVal A, ColVal A2)
{
	this->r = R;
	this->g = G;
	this->b = B;
	this->a = A;
	this->a2 = A2;
};
//------------------------------------------------------------------------------
void
Color::set(string col)
{
	boost::trim(col);

	Color retval(BLACK);

	/// erstes zeichen == # ? -> HEX-Farbe
	if(col[0] == '#' || (col[0] == '0' && col[1] == 'x'))
	{
		if(col[0] == '#') col = col.substr(1);
		else col = col.substr(2);

		string r="", g="", b="", a="FF";

		if(col.length() == 6 or col.length() == 8)
		{
			r = col.substr(0, 2);
			g = col.substr(2, 2);
			b = col.substr(4, 2);

			if(col.length() == 8)
				a = col.substr(6, 2);
		}
		else if(col.length() == 3 or col.length() == 4)
		{
			r = col.substr(0, 1); r=r+r;
			g = col.substr(1, 1); g=g+g;
			b = col.substr(2, 1); b=b+b;

			if(col.length() == 4)
				{a = col.substr(3, 1); a=a+a;}
		}
		else
			throw ERROR("parse", "Color ("+col+") is not in a legal HEX-format (#RRGGBB[AA] or 0xRRGGBB[AA] or 0x/#RGB[A]): too long/short.");

		try
		{
			retval.r = boost::lexical_cast<int_from_hex>(r);
			retval.g = boost::lexical_cast<int_from_hex>(g);
			retval.b = boost::lexical_cast<int_from_hex>(b);
			retval.a = boost::lexical_cast<int_from_hex>(a);
		}
		catch(boost::bad_lexical_cast &)
		{
			throw ERROR("parse", "Color ("+col+") is not in a legal HEX-format (#RRGGBB[AA] or 0xRRGGBB[AA] or 0x/#RGB[A]).");
		}
	}
	else
	{
		size_t p1 = col.find_first_of(", |/-");
		size_t p2 = col.find_first_of(", |/-", p1+1);
		size_t p3 = col.find_first_of(", |/-", p2+1);
		/// enthälts Kommata oder anderes Trennzeichen?
		if(p1 != string::npos && p2 != string::npos)
		{
			string r="", g="", b="", a="255";

			r = col.substr(0, p1);
			g = col.substr(p1+1, p2-p1-1);
			b = col.substr(p2+1, p3-p2-1);

			if(p3 != string::npos) /// es hat vier Werte -> Rest ist Alpha
				a = col.substr(p3+1);

			try
			{
				retval.r = boost::lexical_cast<int>(r);
				retval.g = boost::lexical_cast<int>(g);
				retval.b = boost::lexical_cast<int>(b);
				retval.a = boost::lexical_cast<int>(a);
			}
			catch(boost::bad_lexical_cast &)
			{
				throw ERROR("parse", "Color ("+col+") is not in a legal Format (r,g,b[,a]).\nr='"+r+"', g='"+g+"', b='"+b+"', a='"+a+"'");
			}
		}
		/// versuches mit Namen
		else
		{
			boost::to_lower(col);

					if(col == "black")	retval = BLACK;
			else	if(col == "white")	retval = WHITE;
			else	if(col == "red")	retval = RED;
			else	if(col == "green")	retval = GREEN;
			else	if(col == "blue")	retval = BLUE;
			else	if(col == "yellow")	retval = YELLOW;
			else	if(col == "orange")	retval = ORANGE;
			else	if(col == "pink")	retval = PINK;
			else	if(col == "lightblue")	retval = LIGHTBLUE;
			else	if(col == "brown")	retval = BROWN;
			else	if(col == "grey")	retval = GREY;
			else
				throw ERROR("parse", "Color ("+col+") couldn't be parsed.");
		}
	}

	this->r = retval.r;
	this->g = retval.g;
	this->b = retval.b;
	this->a = retval.a;
};
//------------------------------------------------------------------------------
bool
Color::operator==(Color const& col)
{
	return this->r == col.r &&
	        this->g == col.g &&
	        this->b == col.b &&
	        this->a == col.a;
};
//------------------------------------------------------------------------------
SDL_Color
Color::getSDL_Color()
{
	SDL_Color c = {this->r, this->g, this->b, this->a*this->a2/A_OPAQUE};
	return c;
};
//------------------------------------------------------------------------------
