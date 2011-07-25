#include "widget_container.hpp"

//------------------------------------------------------------------------------
void
WContainer::draw()
{
	if(this->isVisible())
	{
		if(this->draw_bounding_box)
			this->kernel->graphicsMgr->drawBoxToScreen(this->getBB());

		if(this->hide_overflow)
		{
			bool previously_cropped = kernel->graphicsMgr->scissorsEnabled();
			Box  prev = kernel->graphicsMgr->getScissors();

			kernel->graphicsMgr->setScissors(this->getBB(), true);

			this->drawChilds();

			if(previously_cropped)
				kernel->graphicsMgr->setScissors(prev, true);
			else
				kernel->graphicsMgr->disableScissors();
		}
		else
			this->drawChilds();
	}
};
//------------------------------------------------------------------------------
void
WContainer::insert(WidgetPtr w)
{
	this->addChild(w);
	w->setRelativeTo(LEFT,true,TOP,true,shared_from_this());
};
//------------------------------------------------------------------------------
//------------- INPUT ----------------------------------------------------------
//------------------------------------------------------------------------------
bool
WContainer::pointMovListener(PointKey oldPoint, PointKey newPoint)
{
	if(!this->isVisible()) return false;

	/// Mauseingaben sind ABSOLUT zum Widget selber
	//oldPoint.point.pos = oldPoint.point.pos - this->getRelPos();
	//newPoint.point.pos = newPoint.point.pos - this->getRelPos();

	/// nur weitergeben, falls Event auch Container selber betrifft
	if(this->hide_overflow)
	{
		if(!this->getBB().collide(oldPoint.point.pos))
			oldPoint.visible = false;
		if(!this->getBB().collide(newPoint.point.pos))
			newPoint.visible = false;
	}

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
WContainer::pointHitListener(PointKey pk)
{
	if(!this->isVisible()) return false;

	/// Mauseingaben sind ABSOLUT zum Widget selber
	//pk.point.pos = pk.point.pos - this->getRelPos();

	/// nur weitergeben, falls Event auch Container selber betrifft
	if(this->hide_overflow and !this->getBB().collide(pk.point.pos))
		pk.visible = false;

	/// genau das selbe Schema wie beim keyListener(...)
	BOOST_FOREACH(WidgetPtr c, this->childs)
		if (c->pointHitListener(pk))
			return true; ///< Ein Kind hat Input vearbeitet

	if(this->handlePointHitEvent(pk))
		return true; ///< Widget hat Input selber verarbeitet

	return false; ///< Input ist noch unverarbeitet
};
//------------------------------------------------------------------------------
