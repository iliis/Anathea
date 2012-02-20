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
		//this->setAlpha(this->getAlpha());
		this->setSize(s->getSize());
		s->alpha = this->alpha.ref();
		s->size  = this->size.ref();
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

	this->setSprite(this->kernel->guiMgr->createSpriteFromPT(type, n));
};
//------------------------------------------------------------------------------
