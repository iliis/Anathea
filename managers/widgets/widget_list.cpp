#include "widget_list.hpp"

//------------------------------------------------------------------------------
WList::WList(string name, Kernel* k)
 : WContainer(name,k), orientation(VERTICAL),
   wrap_horiz(true), wrap_vert(true)
{
	this->slots.get("childRemoved")->connect(boost::bind(&WList::updateSize, this));
	//this->hideOverflow(false);
};
//------------------------------------------------------------------------------
void
WList::append(WidgetPtr w, list<WidgetPtr>::iterator to)
{
	/// TODO: implement proper insertion not only at end

	if(to == this->childs.end())
	{
		w->abs_x = this->abs_x.ref();
		w->abs_y = this->abs_y.ref();
	}
	else
	{
		WidgetPtr last = *to;

		if(this->orientation == HORIZONTAL)
		{
			w->abs_x = last->abs_x.ref() + last->width.ref() + item_spacing.ref();
			w->abs_y = this->abs_y.ref();
		}
		else
		{
			w->abs_x = this->abs_x.ref();
			w->abs_y = last->abs_y.ref() + last->height.ref() + item_spacing.ref();
		}
	}
};
//------------------------------------------------------------------------------
void
WList::insert(WidgetPtr w)
{
	if(!w->hasParent() or w->getParent().get() != this)
	{
		this->append(w, --this->childs.end());

		WContainer::addChild(w);

		this->updateSize();
	}
};
//------------------------------------------------------------------------------
void
WList::insert(list<WidgetPtr> ws)
{
	BOOST_FOREACH(WidgetPtr w, ws)
	{
		if(!w->hasParent() or w->getParent().get() != this)
		{
			this->append(w, --this->childs.end());
			WContainer::addChild(w);
		}
	}

	/// we only need to update once, so this saves quite a bit of overhead
	this->updateSize();
};
//------------------------------------------------------------------------------
void
WList::updateSize()
{
	/// this function creates quite a bit of overhead and is not very optimized...
	/// main point: it creates a linear list of dependency of addition when a tree or something would be faster

	if(this->childs.empty())
	{
		this->width  = 0;
		this->height = 0;
	}
	else
	{
		if(this->orientation == HORIZONTAL)
		{
			content_width  = this->childs.front()->width.ref();

			Expression<FNumber>::ExpressionRefPtr max_size = this->childs.front()->height.ref();

			list<WidgetPtr>::iterator it = this->childs.begin(); ++it;
			for(;it != this->childs.end(); ++it)
			{
				content_width  += (*it)->width.ref();
				max_size = Expression<FNumber>::ExpressionRefPtr(new Expression<FNumber>::ExprMaxRef(max_size.ptr, (*it)->height.ref()));
			}
			content_width += this->item_spacing.ref() * this->childs.size();

			content_height.link(max_size.ptr);

			if(!this->wrap_vert)
			{
				BOOST_FOREACH(WidgetPtr c, this->childs)
					c->height = this->height.ref();
			}
		}
		else
		{
			content_height  = this->childs.front()->height.ref();

			Expression<FNumber>::ExpressionRefPtr max_size = this->childs.front()->width.ref();

			list<WidgetPtr>::iterator it = this->childs.begin(); ++it;
			for(;it != this->childs.end(); ++it)
			{
				content_height  += (*it)->height.ref();
				max_size = Expression<FNumber>::ExpressionRefPtr(new Expression<FNumber>::ExprMaxRef(max_size.ptr, (*it)->width.ref()));
			}
			content_height += this->item_spacing.ref() * this->childs.size();

			content_width.link(max_size.ptr);

			if(!this->wrap_horiz)
			{
				BOOST_FOREACH(WidgetPtr c, this->childs)
					c->width = this->width.ref();
			}
		}

		if(this->wrap_horiz) this->width  = content_width;
		if(this->wrap_vert)  this->height = content_height;

		/// is this really necessary?
		for(list<WidgetPtr>::iterator it = this->childs.begin(); it!= this->childs.end(); ++it)
		{
			list<WidgetPtr>::iterator it_before = it; --it_before;
			this->append(*it, it_before);
		}
	}
};
//------------------------------------------------------------------------------
void
WList::clear()
{
	this->childs.clear();
	this->updateSize();
}
//------------------------------------------------------------------------------
