#include "widget_button.hpp"

//------------------------------------------------------------------------------
WButton::WButton(string name, Kernel* k)
	 : Widget(name,k),
	   label(k->guiMgr->createWidget<WText>(name+"_label")),
	   bg_normal(k->graphicsMgr->getFallbackImage()),
	   bg_hover (k->graphicsMgr->getFallbackImage()),
	   bg_active(k->graphicsMgr->getFallbackImage()),
	   align(CENTER), state(NORMAL)
{
	this->slots.add("clicked");
	label->cast<WText>()->setText(name);

	k->addToDo(boost::bind(&WButton::setLabel, this, label));

	this->setSize(bg_normal.getSize().cast<Vect::T>());
};
//------------------------------------------------------------------------------
void
WButton::_set(ptree n)
{

};
//------------------------------------------------------------------------------
void
WButton::setLabel(WidgetPtr l)
{
	this->label->removeParent();
	this->label = l;
	this->addChild(label);

	/// center label
	label->setRelativeTo(this->align, true, MIDDLE, true, shared_from_this());
};
//------------------------------------------------------------------------------
void
WButton::setText(string text)
{
	boost::shared_ptr<WText> lbl = boost::dynamic_pointer_cast<WText>(this->label);

	if(!lbl) this->setLabel(kernel->guiMgr->createWidget<WText>(this->name.get()+"_label"));

	label->cast<WText>()->setText(text);
	label->cast<WText>()->setAlign(this->align);
};
//------------------------------------------------------------------------------
void
WButton::setAlign(Align a)
{
	this->align = a;

	boost::shared_ptr<WText> lbl = boost::dynamic_pointer_cast<WText>(this->label);
	if(lbl)
		lbl->setAlign(a);

	label->setRelativeTo(this->align, true, MIDDLE, true, shared_from_this());
};
//------------------------------------------------------------------------------
void
WButton::setTripleBG(Image const& i, Orientation orient)
{
	Image bn = i, bh = i, ba = i;
	if(orient == VERTICAL)
	{
		bn.setUV(Box(0,0    ,1,1/3.0), true);
		bh.setUV(Box(0,1/3.0,1,1/3.0), true);
		ba.setUV(Box(0,2/3.0,1,1/3.0), true);
	}
	else
	{
		bn.setUV(Box(0    ,0,1/3.0,1), true);
		bh.setUV(Box(1/3.0,0,1/3.0,1), true);
		ba.setUV(Box(2/3.0,0,1/3.0,1), true);
	}
	this->setBGnormal(bn);
	this->setBGhover (bh);
	this->setBGactive(ba);
};
//------------------------------------------------------------------------------
void
WButton::_draw()
{
	Image img = this->bg_normal;

	if(state == HOVER)
		img = this->bg_hover;
	else if(state == ACTIVE)
		img = this->bg_active;

	img.color.a = this->alpha;

	if(img.valid())
		img.draw(this->getBB());
	else
		throw Error("draw", "WButton::_draw(): Background image not valid.");
};
//------------------------------------------------------------------------------
bool
WButton::handlePointMovEvent(PointKey oldP, PointKey newP)
{
	if(newP.point.collide(this->getBB()) and newP.visible)
	{
		if(this->state == NORMAL)
			this->state = HOVER;

		/// others may want to know about this too ;)
		/// TODO: PointMovEvent should maybe be un-catchable
		//return true;
	}
	else if(oldP.point.collide(this->getBB()))
		this->state = NORMAL;
		/// no return true here, as we didn't handle the new point

	return false;
};
//------------------------------------------------------------------------------
bool
WButton::handlePointHitEvent(PointKey pk)
{
	if(pk.state and pk.visible and this->getBB().collide(pk.point.pos))
	{
		this->state = ACTIVE;
		return true;
	}
	else if(!pk.state and this->getBB().collide(pk.point.pos) and this->state == ACTIVE)
	{
		this->slots.call("clicked");
		this->state = HOVER;

		return true;
	}
	else
		return false;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
