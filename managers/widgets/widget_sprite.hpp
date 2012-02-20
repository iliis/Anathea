#ifndef WIDGET_SPRITE_HPP_INCLUDED
#define WIDGET_SPRITE_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/sprite_mgr.hpp"

class WSprite : public Widget
{
	SpritePtr sprite;

	bool handleKeyEvent  (KEY key, bool kstate){return false;};
	bool handlePointEvent(string name, Point oldP, Point newP){return false;};
public:
	WSprite(string name, Kernel* k)
	 : Widget(name, k) {};

	string	getType()			{return "Sprite";}

	void		setSprite(SpritePtr s);
	SpritePtr	getSprite(){return this->sprite;}

	//virtual void update(){if(this->sprite) this->sprite->setSize(this->getSize());};

	void	_draw();

	//virtual void	setAlpha(ColVal a)			{this->Widget::setAlpha(a); sprite->alpha = a;}

protected:
	void	_set(ptree n);
};

#endif // WIDGET_SPRITE_HPP_INCLUDED
