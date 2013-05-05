#ifndef WIDGET_IMAGE_HPP_INCLUDED
#define WIDGET_IMAGE_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

class WImage : public Widget
{
	Image img;
public:
	WImage(string Name, Kernel* k)
	 : Widget(Name,k), img(k->graphicsMgr->getFallbackImage())
	 {
		fitToImage();
	 };

	void setImage(Image i);
	void setImage(string path);
	void fitToImage();
	void fitToParent();

	virtual void   _set(ptree n);

	Image& getImage(){return this->img;}
	virtual const char* getType()        const {return "Image";}

protected:
	virtual void _draw();
};

#endif // WIDGET_IMAGE_HPP_INCLUDED
