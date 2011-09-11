#ifndef WIDGET_TEXT_HPP_INCLUDED
#define WIDGET_TEXT_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

class WText : public Widget
{
	string text;
	Font   font;
	Image  buffer;
	Color  color, background;
	bool transparent;
	Align align;
public:
	WText(string name, Kernel* k)
	 : Widget(name,k), text(""), font(k->graphicsMgr->getFallbackFont()), buffer(k->graphicsMgr->getFallbackImage()),
	   color(A_OPAQUE,A_OPAQUE,A_OPAQUE,A_OPAQUE),background(A_TRANSPARENT,A_TRANSPARENT,A_TRANSPARENT,A_TRANSPARENT),
	   transparent(true), align(LEFT) {};


	inline bool isEmpty(){return text.empty();}

	void render();

	// GET
	inline string getText() const {return this->text;}
	inline Font   getFont() const {return this->font;}
	inline Color  getColor() const {return this->color;}
	inline Color  getBG()   const {return this->background;}
	inline bool   isTransparent() const {return this->transparent;}
	inline Align  getAlign() const {return this->align;}

	virtual const char* getType()        const {return "Text";}

	//inline Image getBuffer() {return this->buffer;}

	// SET
	void _set(ptree n);
	void setFont(Font f);
	void setFontSize(int s);
	void setText(string t);
	void setColor(Color c) {this->color = c;      this->render();}
	void setBG(Color c)    {this->background = c; this->render();}
	void setTransparent(bool t){this->transparent = t; this->render();}
	void setAlign(Align a) {this->align = a;      this->render();}


protected:
	virtual void _draw();
};


#endif // WIDGET_TEXT_HPP_INCLUDED
