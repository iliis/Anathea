#include "widget_sprite.hpp"
#include "managers/kernel.hpp"

//------------------------------------------------------------------------------
void
WSprite::setSprite(SpritePtr s)
{
	this->sprite = s;

	if(!s)
		this->setSize(Vect(0,0));
	else
	{
		this->setSize(s->getSize());
		s->alpha  = this->alpha; // TODO: alpha is sometimes implemented as float and sometimes als ColVal (int)
		s->width  = this->width.ref();
		s->height = this->height.ref();
	}
};
//------------------------------------------------------------------------------
void
WSprite::_draw()
{
	if(this->sprite != SpritePtr())
		this->sprite->draw(this->getAbsPos());
};
//------------------------------------------------------------------------------
void
WSprite::_set(ptree n)
{
	string type = n.get("type", "");

	if(type == "")
		throw Error("notFound", "Can't create WidgetSprite: Attribute 'type' is missing.");

	// TODO
	//throw Error("notImplemented", "WSprite::_set(ptree n)");
	//this->setSprite(this->kernel->guiMgr->createSpriteFromPT(type, n));
};
//------------------------------------------------------------------------------
