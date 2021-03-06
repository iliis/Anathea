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
};
//------------------------------------------------------------------------------
void
WButton::init()
{
	this->setLabel(this->label);
};
//------------------------------------------------------------------------------
#include "managers/widgets/widget_image.hpp"
void
WButton::_set(ptree n)
{
	/// TODO: implement a text&image-label?


	if(n.get_child_optional("label.text"))
	{
		boost::shared_ptr<WText> lbl = boost::dynamic_pointer_cast<WText>(this->label);
		if(!lbl) this->setLabel(kernel->guiMgr->createWidget<WText>(this->name.get()+"_label"));

		this->label->set(n.get_child("label"));
	}

	if(n.get_child_optional("label.image"))
	{
		/// TODO: use PTree-API here as well (like label.text above)

		string path = n.get<string>("label.image");
		this->setLabel(kernel->guiMgr->createWidget<WImage>(this->name.get() + "_label_image",
															mkPtree("image.path", path.c_str())));
	}

	if(n.get_child_optional("label.padding"))
	{
		/// TODO: use text.padding instead of baking own thing (or not, as button may have a generic widget as label)

		FNumber p = n.get<FNumber>("label.padding", 0);
		this->setAutoHeight(p);
		this->setAutoWidth (p);
	}

	if(n.get_child_optional("triple_background"))
	{
		Orientation orient = VERTICAL;
		string tmp = n.get("triple_background.orientation", "vertical");
		boost::trim(tmp); boost::to_lower(tmp);

		if(tmp == "horiz" || tmp == "horizontal" || tmp == "h" || tmp == "-" || tmp == "--") orient = HORIZONTAL;

		this->setTripleBG(this->kernel->graphicsMgr->loadImage(n.get_child("triple_background")), orient);
	}

	if(n.get_child_optional("background_normal"))
		this->setBGnormal(kernel->graphicsMgr->loadImage(n.get_child("background_normal")));

	if(n.get_child_optional("background_hover"))
		this->setBGhover (kernel->graphicsMgr->loadImage(n.get_child("background_hover")));

	if(n.get_child_optional("background_active"))
		this->setBGactive(kernel->graphicsMgr->loadImage(n.get_child("background_active")));

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

	/// set button size to label
	/// this tries to guess padding by looking at the background's NinePatch-Data
	/// (however, this doesn't change, when updating the background...)
	this->setAutoHeight();
	this->setAutoWidth();

	/// link important attributes
	this->label->alpha   = this->alpha.ref();
	this->label->visible = this->visible.ref();
};
//------------------------------------------------------------------------------
void
WButton::setText(string text)
{
	boost::shared_ptr<WText> lbl = boost::dynamic_pointer_cast<WText>(this->label);

	if(!lbl) this->setLabel(kernel->guiMgr->createWidget<WText>(this->name.get()+"_label"));

	label->cast<WText>()->setText(text, this->align);
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
		throw ERROR("draw", "WButton::_draw(): Background image not valid.");
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
void
WButton::setAutoWidth()
{
	if(this->bg_normal.NinePatch().enabled)
		this->setAutoWidth(this->bg_normal.NinePatch().left + this->bg_normal.NinePatch().right);
	else
		this->setAutoWidth(0);
};
//------------------------------------------------------------------------------
void
WButton::setAutoHeight()
{
	if(this->bg_normal.NinePatch().enabled)
		this->setAutoHeight(this->bg_normal.NinePatch().top + this->bg_normal.NinePatch().bottom);
	else
		this->setAutoHeight(0);
};
//------------------------------------------------------------------------------
