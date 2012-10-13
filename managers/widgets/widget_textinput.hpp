#ifndef WIDGET_TEXTINPUT_HPP_INCLUDED
#define WIDGET_TEXTINPUT_HPP_INCLUDED

#include "widget_text.hpp"

class WTextInput : public WText
{
	Image border_normal, border_focused;
	string::iterator cursor;


public:
	WTextInput(string name, Kernel* k)
	 : WidgetText(name, k), cursor(this->text.begin())
	{

	}

	virtual const char* getType() const {return "TextInput";}

	void focus();
	void unfocus();

protected:
	virtual void _draw();
	virtual bool handleKeyEvent(KEY key, bool state);
};

#endif // WIDGET_TEXTINPUT_HPP_INCLUDED
