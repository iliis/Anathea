#ifndef WIDGET_LIST_HPP_INCLUDED
#define WIDGET_LIST_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

#include "managers/widgets/widget_container.hpp"

class WList : public WContainer
{
protected:
	Orientation orientation;
	bool wrap_horiz, wrap_vert; ///< fit list size to contain all items (otherwise, resize items to fit in list)

	void updateSize();
	void append(WidgetPtr w, list<WidgetPtr>::iterator to);

public:
	WList(string name, Kernel* k);

	Expression<FNumber> padding, padding_left, padding_right, padding_top, padding_bottom, item_spacing;
	Expression<FNumber> content_width, content_height;

	virtual void insert(WidgetPtr w);
	void clear();
};

#endif // WIDGET_LIST_HPP_INCLUDED
