#ifndef WIDGET_BUTTON_HPP_INCLUDED
#define WIDGET_BUTTON_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

#include "managers/widgets/widget_text.hpp"

class WButton : public Widget
{
	WidgetPtr label;
	Image bg_normal, bg_hover, bg_active;
public:
	WButton(string name, Kernel* k);

	enum ButtonState
	{
		NORMAL, HOVER, ACTIVE
	};

	// GET

	// SET
	void _set(ptree n);

	void setLabel(WidgetPtr l);
	void setText(string text);
	void setBGnormal(Image const& i){this->bg_normal = i; this->setSize(i.getSize());}
	void setBGhover (Image const& i){this->bg_hover  = i;}
	void setBGactive(Image const& i){this->bg_active = i;}
	void setTripleBG(Image const& i);
	void setTripleBG(string const& path){setTripleBG(kernel->graphicsMgr->loadImage(path));}


protected:
	ButtonState state;

	virtual bool handlePointMovEvent(PointKey oldP, PointKey newP);
	virtual bool handlePointHitEvent(PointKey pk);

	virtual void _draw();
};

#endif // WIDGET_BUTTON_HPP_INCLUDED
