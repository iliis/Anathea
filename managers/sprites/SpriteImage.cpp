#include "SpriteImage.hpp"

//------------------------------------------------------------------------------
void
SpriteImage::draw(Vect pos)
{
	if(this->isVisible())
	{
		this->img.color.setA(this->alpha);
		this->img.draw(Box(pos,this->getSize()));
	}
};
//------------------------------------------------------------------------------
#include "managers/kernel.hpp"
void
SpriteImage::set(ptree params)
{
	boost::optional<string> path = params.get_optional<string>("path");
	if(path and *path != "")
		this->setImage(this->graphicsMgr->loadImage(*path));
};
//------------------------------------------------------------------------------
