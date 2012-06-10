#include "SpriteLine.hpp"

//------------------------------------------------------------------------------
Vect
SpriteLine::getMinSize()
{
	return this->to;
};
//------------------------------------------------------------------------------
Vect
SpriteLine::getMaxSize()
{
	return this->to;
};
//------------------------------------------------------------------------------
void
SpriteLine::draw(Vect pos)
{
	Vect start	= pos;
	Vect end	= pos+to;

	if(this->to.isNull()) return; /// Start == Ziel

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE);
		glColor4f(col.fr(),col.fg(),col.fb(),col.fa());
		glVertex2f(start.x, start.y);
		glVertex2f(end.x,   end.y);
	glEnd();
	glEnable(GL_TEXTURE_2D);

	/*if(start.y == end.y)
		hlineRGBA(img.getSurface().get(), start.x, end.x, start.y,
					col.r, col.g, col.b, col.a);
	else if(start.x == end.x)
		vlineRGBA(img.getSurface().get(), start.x, start.y, end.y,
					col.r, col.g, col.b, col.a);
	else
	{
		if(this->hasAA())
			aalineRGBA(img.getSurface().get(), start.x, start.y, end.x, end.y,
						col.r, col.g, col.b, col.a);
		else
			lineRGBA(img.getSurface().get(), start.x, start.y, end.x, end.y,
						col.r, col.g, col.b, col.a);
	}*/
};
//------------------------------------------------------------------------------
#include "managers/kernel.hpp"
void
SpriteLine::set(ptree params)
{
	this->to  = Vect(params, "toX", "toY", this->to);
	this->col = params.get("color", this->col.getString());
	setBoolFromPT(params, "AA", this->AA);
};
//------------------------------------------------------------------------------
