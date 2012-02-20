#include "managers/sprites/SpriteMultiImage.hpp"

//------------------------------------------------------------------------------
void
SpriteMultiImage::set(ptree params)
{
	this->SpriteImage::set(params);
	this->frames.clear();

	this->default_frame = params.get("framenr",this->default_frame);

	vector2<unsigned int> rowcols(params,"cols","rows",vector2<unsigned int>(1,1));
	Vect framesize(img.getSize().cast<Vect::T>() / rowcols.cast<Vect::T>());

	vector2<unsigned int> pos(0,0);
	vect2for(pos, rowcols)
	{
		this->addFrame(Box(pos.cast<Vect::T>()*framesize,framesize));
	}

	if(params.get_child_optional("<children>.frame"))
	{
		BOOST_FOREACH(ptree::value_type &v, params.get_child("<children>"))
		{
			if(v.first == "frame")
			{
				Box b;
				b.pos  = Vect(v.second, "x", "h", Vect(0,0));
				b.size = Vect(v.second, "w", "h", Vect(0,0));
				this->addFrame(b);
			}
		}
	}
};
//------------------------------------------------------------------------------
Box
SpriteMultiImage::getFrame(unsigned int nr)
{
	if(nr >= this->frames.size())
		throw Error("illegalOperation","MultiImage '"+this->img.getName()+"' has no frame No."+nr+"!");
	else
		return this->frames[nr];
}
//------------------------------------------------------------------------------
void
SpriteMultiImage::draw(Vect pos)
{
	this->draw(pos,this->getFrame(this->default_frame));
};
//------------------------------------------------------------------------------
void
SpriteMultiImage::draw(Vect pos, unsigned int framenr)
{
	this->draw(pos,this->getFrame(framenr));
};
//------------------------------------------------------------------------------
void
SpriteMultiImage::draw(Vect pos, Box frame)
{
	this->img.draw(Box(pos,this->getSize()), frame, false);
};
//------------------------------------------------------------------------------
