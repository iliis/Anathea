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
WImage::fitToParent()
{
	if(this->hasParent())
	{
		this->width  = this->getParent()->width.ref();
		this->height = this->getParent()->height.ref();
		this->abs_x  = this->getParent()->abs_x.ref();
		this->abs_y  = this->getParent()->abs_y.ref();
	}
};
//------------------------------------------------------------------------------
void
WImage::_set(ptree n)
{
	if(n.get_child_optional("image"))
		this->setImage(kernel->graphicsMgr->loadImage(n.get_child("image")));
};
//------------------------------------------------------------------------------
void
WImage::_draw()
{
	this->img.color.a = this->alpha;

	if(this->img.valid())
		this->img.draw(this->getBB());
	else
		throw ERROR("draw","WImage::draw(): Image is not valid!");
};
//------------------------------------------------------------------------------
