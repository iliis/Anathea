#ifndef WIDGET_TEXTINPUT_HPP_INCLUDED
#define WIDGET_TEXTINPUT_HPP_INCLUDED

#include "widget_text.hpp"
#include <boost/algorithm/string.hpp>


class WTextInput : public WText
{
	Image border_normal, border_focused;
	size_t cursor;

public:

	WTextInput(string name, Kernel* k)
	 : WText(name, k),
	   border_normal( k->graphicsMgr->getFallbackImage()),
	   border_focused(k->graphicsMgr->getFallbackImage()),
	   cursor(0)
	{}

	inline void focus()  {this->kernel->guiMgr->setFocusToWidget(shared_from_this());}
	inline void unfocus(){this->kernel->guiMgr->removeFocus();}

	// GET
	//-------------------------------------------------------------------------
	virtual const char* getType() const {return "TextInput";}
	inline bool hasFocus(){return this->kernel->guiMgr->hasFocus(shared_from_this());}

	// SET
	//-------------------------------------------------------------------------
	void _set(ptree n);

	inline void setBorderNormal (Image const& b) {this->border_normal  = b;}
	inline void setBorderFocused(Image const& b) {this->border_focused = b;}

protected:
	virtual void _draw();
	virtual bool handleKeyEvent(KEY key, bool state);
	virtual bool handlePointHitEvent(PointKey pk);
};

#endif // WIDGET_TEXTINPUT_HPP_INCLUDED
