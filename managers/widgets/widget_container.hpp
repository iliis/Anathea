#ifndef WIDGET_CONTAINER_HPP_INCLUDED
#define WIDGET_CONTAINER_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

class WContainer : public Widget
{
	bool hide_overflow;
public:
	WContainer(string name, Kernel* k)
	 : Widget(name, k), hide_overflow(false) {};

	// GET
	//-------------------------------------------------------------------------
	inline bool isOverflowHidden(){return this->hide_overflow;}
	virtual const char* getType()        const {return "Container";}

	// SET
	//-------------------------------------------------------------------------
	void _set(ptree n){};

	void hideOverflow(bool hide=true){this->hide_overflow = hide;}



	// MISC
	//-------------------------------------------------------------------------
	virtual void draw();
	virtual void insert(WidgetPtr w);

	bool pointMovListener(PointKey oldPoint, PointKey newPoint);
	bool pointHitListener(PointKey pk);

protected:

};

#endif // WIDGET_CONTAINER_HPP_INCLUDED
