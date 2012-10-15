#include "widget_textinput.hpp"

//------------------------------------------------------------------------------
void
WTextInput::_draw()
{
	Image img = this->border_normal;

	if(this->hasFocus())
		img = this->border_focused;

	img.color.a = this->alpha;

	if(img.valid())
		img.draw(this->getBB());
	else
		throw ERROR("draw", "WButton::_draw(): Background image not valid.");

	WText::_draw();

	if(this->hasFocus())
	{
		VectInt size = this->calculateTextSize(this->text.substr(0, this->cursor));
		Vect pos = this->getAbsPos();
		drawLine(pos+Vect(padding+size.x, padding), pos+Vect(padding+size.x,padding+size.y));
	}
};
//------------------------------------------------------------------------------
bool
WTextInput::handleKeyEvent(KEY key, bool state)
{
	if(!hasFocus() || !state) return false;

	if(this->cursor > this->text.size()) this->cursor = this->text.size();

	if(key == KEY_BACKSPACE && this->cursor > 0)
	{
		this->text.erase(this->cursor-1, 1);
		this->cursor--;
	}
	else if(key == KEY_DELETE && this->cursor < this->text.size())
	{
		this->text.erase(this->cursor, 1);
	}
	else if(key == KEY_LEFT && this->cursor > 0)
		this->cursor--;
	else if(key == KEY_RIGHT && this->cursor < this->text.size())
		this->cursor++;
	else if(key == KEY_END)
		this->cursor = this->text.size();
	else if(key == KEY_HOME)
		this->cursor = 0;
	else if(isReadable(key))
	{
		string t = keyToString(key);

		if(kernel->inputMgr->isShiftOrCapsPressed())
			boost::to_upper(t);

		this->text.insert(this->cursor, t);
		this->cursor += t.size();
	}

	this->render();

	return true;
};
//------------------------------------------------------------------------------
void
WTextInput::_set(ptree n)
{
	WText::_set(n);

	if(n.get_child_optional("border_normal"))
		this->setBorderNormal (kernel->graphicsMgr->loadImage(n.get_child("border_normal")));

	if(n.get_child_optional("border_focused"))
		this->setBorderFocused(kernel->graphicsMgr->loadImage(n.get_child("border_focused")));
};
//------------------------------------------------------------------------------
bool
WTextInput::handlePointHitEvent(PointKey pk)
{
	if(pk.point.collide(this->getBB()) && pk.state)
	{
		this->focus();

		// try to approximate the point in the text where the user clicked
		// this is very crude (linear, only one line)
		FNumber p = (pk.point.pos.x - padding - this->abs_x) / (this->width - 2*padding);
		if(p>1) p = 1;
		if(p<0) p = 0;
		this->cursor = this->text.size()*p;

		return true;
	}
	else
		return false;
};
//------------------------------------------------------------------------------
