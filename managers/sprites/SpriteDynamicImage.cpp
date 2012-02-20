#include "managers/sprites/SpriteDynamicImage.hpp"

//------------------------------------------------------------------------------
void
SpriteDynamicImage::set(ptree params)
{
	this->SpriteImage::set(params);
	this->setSize(Vect(params, "w", "h", this->getSize()));
	float all = params.get("all",0.0f);
	if(all >= 0)
		{ p1 = Vect(all,all); p2 = Vect(all,all);}

	this->p1 = Vect(params, "left", "top", this->p1);
	this->p2 = Vect(params, "right", "bottom", this->p2);
};
//------------------------------------------------------------------------------

/*   .----- p (=pos)
 *  V  left              right
 *  0 ------|----------|--------|
 *  |       |          |        | top
 *  |------ p1---------|--------|
 *  |       |          |        |
 *  |-------|--------- p2 ------|
 *  |       |          |        | bottom
 *  |-------|----------|-------size
 */

void
SpriteDynamicImage::draw(Vect pos)
{
	if(this->isVisible())
	{
		Vect imgsize = this->img.getSize().cast<Vect::T>();
		Vect p = pos;
		Vect::T left = p1.x, top = p1.y, right = p2.x, bottom = p2.y;
		typedef Box B; typedef Vect V;

		/// Ecken zeichnen:
		if(top>0 and left>0)		img.draw(p,						B(0,0, left,top), false); ///< oben links
		if(top>0 and right>0)		img.draw(p+V(width-right,  0),	B(imgsize.x-right,  0,	right,top), false); ///< oben rechts
		if(bottom>0 and left>0)	img.draw(p+V(0, height-bottom),	B(0, imgsize.y-bottom,	left,bottom), false); ///< unten links
		if(bottom>0 and right>0)	img.draw(p+getSize()-p2,B(imgsize-p2,p2), false); ///< unten rechts

		/// Kanten zeichnen:

		/// oben
		if(top > 0)
		img.draw(B(p.x+left,p.y,     width-left-right,top), // position
				 B(    left,  0, imgsize.x-left-right,top), false); // uv-data

		/// links
		if(left > 0)
		img.draw(B(p.x, p.y+top,   left,    height-top-bottom), // position
				 B(  0,     top,   left, imgsize.y-top-bottom), false); // uv-data

		/// unten
		if(bottom > 0)
		img.draw(B(p.x+left,p.y+height-bottom,       width-left-right,bottom), // position
				 B(    left, imgsize.y-bottom,   imgsize.x-left-right,bottom), false); // uv-data

		/// rechts
		if(right > 0)
		img.draw(B(p.x+width -right, p.y+top,   right,    height-top-bottom), // position
				 B( imgsize.x-right,     top,   right, imgsize.y-top-bottom), false); // uv-data

		/// Mitte zeichnen:
		if(left+right < min(imgsize.x,width.get()) and top+bottom < min(imgsize.y,height.get()))
		img.draw(B(p.x+left, p.y+top,     width-left-right,    height-top-bottom),
				 B(    left,     top, imgsize.x-left-right, imgsize.y-top-bottom), false);
	}
};
//------------------------------------------------------------------------------
