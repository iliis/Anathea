#include "managers/widget.hpp"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Widget::Widget(string Name, Kernel* k)
 :  name(Name), visible(true), draw_bounding_box(false), alpha(A_OPAQUE),
	kernel(k)
{
	assert(k);

	slots.add("parent");
	slots.add("childAdded");
	slots.add("childRemoved");

	slots.add("deleted");

	this->abs_x = this->rel_x;
	this->abs_y = this->rel_y;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool
Widget::hasChild(WidgetPtr child)
{
	return find(this->childs.begin(), this->childs.end(), child)  !=  childs.end();
};
//------------------------------------------------------------------------------
WidgetPtr
Widget::getChild(string path, bool throwIfNotFound)
{
	size_t delimiter = path.find_first_of('/');
	if(delimiter == string::npos)
	{
	/// Pfad bezeichnet direkt Objekt
		BOOST_FOREACH(WidgetPtr c, this->childs)
			if(c->name.get() == path || c->getType() == path)
				return c;
	}
	else
	{
		string root = path.substr(0, delimiter);
		path        = path.substr(delimiter+1);

		BOOST_FOREACH(WidgetPtr c, this->childs)
		{
			if(c->name.get() == root || c->getType() == root)
			{
				WidgetPtr ptr = c->getChild(path);
					if(ptr != WidgetPtr())
						return ptr;
			}
		}
	}

	if(throwIfNotFound)
		throw ERROR("notFound", "Widget '"+this->name.get()+"' ("+this->getType()+") has no child named '"+path+"'.");
	else
		return WidgetPtr();
};
//------------------------------------------------------------------------------
//------------- SET ------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void
Widget::setRelativeTo(Align horiz, bool h_inside, Vlign vert, bool v_inside, WidgetPtr relativeTo)
{
	assert(relativeTo);

	/// horizontal
	switch(horiz)
	{
		case LEFT:
			if(h_inside)
				rel_x = FNumber(0);
			else
				rel_x = this->width.ref()*(-1);
			break;

		case CENTER:
			rel_x = relativeTo->width.ref()/2-this->width.ref()/2; break;

		case RIGHT:
			if(h_inside)
				rel_x = relativeTo->width.ref()-this->width.ref();
			else
				rel_x = relativeTo->width;
			break;
	}

	/// vertical
	switch(vert)
	{
		case TOP:
			if(v_inside)
				rel_y = FNumber(0);
			else
				rel_y = this->height.ref()*(-1);
			break;

		case MIDDLE:
			rel_y = relativeTo->height.ref()/2-this->height.ref()/2; break;

		case BOTTOM:
			if(v_inside)
				rel_y = relativeTo->height.ref()-this->height.ref();
			else
				rel_y = relativeTo->height;
			break;
	}

	abs_x = relativeTo->abs_x.ref() + this->rel_x.ref();
	abs_y = relativeTo->abs_y.ref() + this->rel_y.ref();
};
//------------------------------------------------------------------------------
#include "managers/kernel.hpp"
void
Widget::set(ptree n)
{
	/// set only if attribute exists (otherwise, Expressions may get unintentionally unlinked.)

	/// Standard-Attribute setzten
	this->rel_x  = n.get_optional<Vect::T>("geometry.x");
	this->rel_y  = n.get_optional<Vect::T>("geometry.y");
	this->width  = n.get_optional<Vect::T>("geometry.w");
	this->height = n.get_optional<Vect::T>("geometry.h");


	if(n.get_child_optional("flags"))
	{
		// this->visible is an expression<bool>, so no ref possible
		bool v = this->visible;
		setBoolFromPT(n, "flags.visible", v);      if(this->visible != v)           this->visible = v;

		bool b = this->draw_bounding_box;
		setBoolFromPT(n, "flags.bounding_box", b); if(this->draw_bounding_box != b) this->draw_bounding_box = b;
	}


	BOOST_FOREACH(ptree::value_type& child, n)
	{
		/// define slots  (for widget-specifig events like "button clicked" or "window closed")
		if(child.first == "slot")
			this->slots.add(child.second.get<string>("name")); ///< will not create duplicates


		/// define events (global slots, if the corresponding slot of this widget gets called, a global
		///                event gets triggered trough the guiManager)
		if(child.first == "event")
		{
			string event_name = child.second.get<string>("name");
			string event_slot = child.second.get("slot", "clicked"); /// default widget slot is "clicked" (every button has one)

			/// add Slot if this widget doesn't have it already
			if(!this->hasSlot(event_slot)) this->slots.add(event_slot);

			this->kernel->guiMgr->connectEvents(shared_from_this(), event_name, event_slot);
		}

		/// we can add new widgets by adding them in a sub-tag <children>
		/// WARNING: make sure, this isn't global or of the same type as this widget, as this WILL cause an infinite loop!
		if(child.first == "children")
		{
			BOOST_FOREACH(ptree::value_type& c, child.second)
				WidgetPtr child_widget = kernel->guiMgr->createWidgetFromPT(c.second, shared_from_this());
		}
	}

	this->_set(n);
};
//------------------------------------------------------------------------------
//------------- INPUT ----------------------------------------------------------
//------------------------------------------------------------------------------
bool
Widget::keyListener(KEY name, bool state)
{
	if(!this->isVisible()) return false;

	BOOST_FOREACH(WidgetPtr c, this->childs)
		if (c->keyListener(name, state))
			return true; ///< Ein Kind hat Input vearbeitet

	if(this->handleKeyEvent(name, state))
		return true; ///< Widget hat Input selber verarbeitet

	return false; ///< Input ist noch unverarbeitet
};
//------------------------------------------------------------------------------
bool
Widget::pointMovListener(PointKey oldPoint, PointKey newPoint)
{
	if(!this->isVisible()) return false;

	/// Mauseingaben sind ABSOLUT zum Widget selber
	//oldPoint.point.pos = oldPoint.point.pos - this->getRelPos();
	//newPoint.point.pos = newPoint.point.pos - this->getRelPos();

	/// genau das selbe Schema wie beim keyListener(...)
	BOOST_FOREACH(WidgetPtr c, this->childs)
		if (c->pointMovListener(oldPoint, newPoint))
			return true; ///< Ein Kind hat Input vearbeitet

	if(this->handlePointMovEvent(oldPoint, newPoint))
		return true; ///< Widget hat Input selber verarbeitet

	return false; ///< Input ist noch unverarbeitet
};
//------------------------------------------------------------------------------
bool
Widget::pointHitListener(PointKey pk)
{
	if(!this->isVisible()) return false;

	/// Mauseingaben sind ABSOLUT zum Widget selber
	//pk.point.pos = pk.point.pos - this->getRelPos();

	/// genau das selbe Schema wie beim keyListener(...)
	BOOST_FOREACH(WidgetPtr c, this->childs)
		if (c->pointHitListener(pk))
			return true; ///< Ein Kind hat Input vearbeitet

	if(this->handlePointHitEvent(pk))
		return true; ///< Widget hat Input selber verarbeitet

	return false; ///< Input ist noch unverarbeitet
};
//------------------------------------------------------------------------------
//------------- DIV ------------------------------------------------------------
//------------------------------------------------------------------------------
void
Widget::draw()
{
	if(this->isVisible())
	{
		this->_draw();

		if(this->draw_bounding_box)
			this->kernel->graphicsMgr->drawBoxToScreen(this->getBB());

		this->drawChilds();
	}
};
//------------------------------------------------------------------------------
void
Widget::drawChilds()
{
	BOOST_REVERSE_FOREACH(WidgetPtr w, this->childs)
	{
		w->draw();
	}
};
//------------------------------------------------------------------------------
void
Widget::addChild(WidgetPtr child)
{
	child->setParent(shared_from_this());
};
//------------------------------------------------------------------------------
/*
void
Widget::addChildDuringConstructor(WidgetPtr child)
{
	/// This is somewhat of a hack.
	/// It would be better to handle this in guiMgr::createWidget() somehow...

	this->kernel->addToDo(boost::bind(&Widget::addChild, this, child));
};*/
//------------------------------------------------------------------------------
void
Widget::setParent(WidgetPtr new_parent)
{
	/// falls kein parent, oder zumindest ein anderen
	if(this->parent.expired() || WidgetPtr(this->parent) != new_parent)
	{
		if(this->hasParent())
			this->removeParent();

		this->parent = new_parent;
		new_parent->childs.push_back(shared_from_this());

		//this->setRelativeToParent(LEFT,true,TOP,true);
		this->alpha = new_parent->alpha.ref();

		this->slots.call("parent");
		new_parent->slots.call("childAdded");
	}
};
//------------------------------------------------------------------------------
void
Widget::removeChildren()
{
	BOOST_FOREACH(WidgetPtr c, this->childs)
		this->removeChild(c);
};
//------------------------------------------------------------------------------
void
Widget::removeChild(WidgetPtr child)
{
	if(child->parent.lock() == shared_from_this()) child->removeParent();
};
//------------------------------------------------------------------------------
void
Widget::removeChild(string child_name)
{
	BOOST_FOREACH(WidgetPtr c, this->childs)
	{
		if(c->name.get() == child_name)
			{this->removeChild(c); return;}
	}
};
//------------------------------------------------------------------------------
void
Widget::removeParent()
{
	if(shared_ptr<Widget> tmp_parent = this->parent.lock())
	{
		tmp_parent->childs.remove(shared_from_this());
		this->parent.reset();

		this->alpha.unlink();

		this->slots.call("parent");
		tmp_parent->slots.call("childRemoved");
	}
};
//------------------------------------------------------------------------------
void
Widget::moveChildToFront(WidgetPtr obj)
{
	this->childs.splice(this->childs.begin(),
						 this->childs,
						 find(this->childs.begin(),
							  this->childs.end(),
							  obj));

	//if(this->hasParent())
		//this->getParent()->moveChildToFront(shared_from_this());
};
//------------------------------------------------------------------------------
void
Widget::moveChildToBG(WidgetPtr obj)
{
	this->childs.splice(this->childs.end(),
						 this->childs,
						 find(this->childs.begin(),
							  this->childs.end(),
							  obj));
	//if(this->hasParent())
		//this->getParent()->moveChildToBG(shared_from_this());
};
//------------------------------------------------------------------------------
void
Widget::moveToFront()
{
	if(this->hasParent())
		this->getParent()->moveChildToFront(shared_from_this());
	else
		this->kernel->guiMgr->moveRootWidgetToFront(shared_from_this());
};
//------------------------------------------------------------------------------
void
Widget::moveToBG()
{
	if(this->hasParent())
		this->getParent()->moveChildToBG(shared_from_this());
	else
		this->kernel->guiMgr->moveRootWidgetToBack(shared_from_this());
}
//------------------------------------------------------------------------------
void
Widget::deleteMe(bool deleteFromMgr)
{
	this->deactivateSlots();

	if(this->hasParent())
		this->getParent()->removeChild(this->name);
	else if(deleteFromMgr)
		this->kernel->guiMgr->deleteWidget(this->name);

	this->visible = false;

	this->childs.clear();
};
//------------------------------------------------------------------------------
void
Widget::clearSlots()
{
	Slots::SignalPtr del_slot = this->slots.get("deleted");
	this->slots.clear();
	this->slots.add("deleted", del_slot);

	BOOST_FOREACH(WidgetPtr c, this->childs)
		c->clearSlots();
}
//------------------------------------------------------------------------------
//---------- ANIMATIONS --------------------------------------------------------
//------------------------------------------------------------------------------
TimelinePtr
Widget::fadeOut(FNumber sec)
{
	TimelinePtr tl = this->kernel->animMgr->from_to(this->alpha, A_TRANSPARENT, fromSeconds(sec), true);
	this->alpha = tl->value.ref();

	return tl;
};
//------------------------------------------------------------------------------
TimelinePtr
Widget::fadeIn(FNumber sec)
{
	TimelinePtr tl(new Timeline());

	tl->addKeyframe(Keyframe(0, this->alpha));
	tl->addKeyframe(Keyframe(fromSeconds(sec), A_OPAQUE));

	this->kernel->animMgr->addAnim(tl, true);

	return tl;
};
//------------------------------------------------------------------------------
TimelinePtr
Widget::moveAnim(Vect dest, FNumber sec)
{
	/// TODO: implement relative and absolute version of this?

	TimelinePtr tlX = this->kernel->animMgr->from_to(this->abs_x, dest.x, fromSeconds(sec), true);
	TimelinePtr tlY = this->kernel->animMgr->from_to(this->abs_y, dest.y, fromSeconds(sec), true);

	this->abs_x = tlX->value.ref();
	this->abs_y = tlY->value.ref();

	return tlX;
};
//------------------------------------------------------------------------------
TimelinePtr
Widget::fadeOutAndDelete(FNumber sec, bool deleteFromMgr)
{
    TimelinePtr tl = this->fadeOut(sec);

    tl->setEndEvent(boost::bind(&Widget::deleteMe,this,deleteFromMgr));

    return tl;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include <boost/algorithm/string/predicate.hpp>
bool
order_by_name(WidgetPtr& A, WidgetPtr& B)
{
	return boost::algorithm::ilexicographical_compare(A->name.get(), B->name.get());
};
//------------------------------------------------------------------------------
