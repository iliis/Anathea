#include "widget_image.hpp"

//------------------------------------------------------------------------------
void
WImage::setImage(string path)
{
	if(!path.empty())
		this->setImage(this->kernel->graphicsMgr->loadImage(path));
};
//------------------------------------------------------------------------------
void
WImage::setImage(Image i)
{
	this->img = i;
	this->fitToImage();
};
//------------------------------------------------------------------------------
void
WImage::fitToImage()
{
	this->width  = img.getSize().x;
	this->height = img.getSize().y;
};
//------------------------------------------------------------------------------
void
WImage::_set(ptree n)
{
	this->setImage(n.get<string>("image", ""));
};
//------------------------------------------------------------------------------
void
WImage::_draw()
{
	this->img.color.a = this->alpha;

	if(this->img.valid())
		this->img.draw(this->getBB());
	else
		throw Error("draw","WImage::draw(): Image is not valid!");
};
//------------------------------------------------------------------------------
