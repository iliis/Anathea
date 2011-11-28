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
	Align align;
public:
	WButton(string name, Kernel* k);

	enum ButtonState
	{
		NORMAL, HOVER, ACTIVE
	};

	// GET
	//-------------------------------------------------------------------------
	inline Align        getAlign() const {return this->align;}
	virtual const char* getType()        const {return "Button";}
	inline WidgetPtr    getLabel() const {return this->label;}

	// SET
	//-------------------------------------------------------------------------
	void _set(ptree n);

	void setLabel(WidgetPtr l);
	void setText(string text);
	void setBGnormal(Image const& i){this->bg_normal = i; this->setSize(i.getSize());}
	void setBGhover (Image const& i){this->bg_hover  = i;}
	void setBGactive(Image const& i){this->bg_active = i;}
	void setTripleBG(Image const& i, Orientation orient = VERTICAL);
	void setTripleBG(string const& path, Orientation orient = VERTICAL)
		{setTripleBG(kernel->graphicsMgr->loadImage(path), orient);}
	void hideLabel(bool hide=true){this->label->visible = !hide;}
	void setAlign(Align a);


protected:
	ButtonState state;

	virtual bool handlePointMovEvent(PointKey oldP, PointKey newP);
	virtual bool handlePointHitEvent(PointKey pk);

	virtual void _draw();
};

#endif // WIDGET_BUTTON_HPP_INCLUDED
