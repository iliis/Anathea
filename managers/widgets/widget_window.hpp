#ifndef WIDGET_WINDOW_HPP_INCLUDED
#define WIDGET_WINDOW_HPP_INCLUDED

#include "managers/widget.hpp"
#include "managers/image_mgr.hpp"
#include "managers/kernel.hpp"

#include "managers/widgets/widget_text.hpp"
#include "managers/widgets/widget_button.hpp"
#include "managers/widgets/widget_container.hpp"

class WWindow : public Widget
{
	Image title_bar, content_bg;
	WidgetPtr title;
	shared_ptr<WContainer> content;
	shared_ptr<WButton>    close_button; Slots::Connection close_connection;
public:
	WWindow(string name, Kernel* k);

	Expression<FNumber> border, title_bar_height, padding, padding_top, padding_bottom, padding_left, padding_right;

	// SET
	//-------------------------------------------------------------------------
	inline shared_ptr<WContainer> getContainer()   {return this->content;}
	inline shared_ptr<WButton>    getCloseButton() {return this->close_button;}
	inline WidgetPtr              getTitleWidget() {return this->title;}
	inline Image&                 getTitleBar()    {return this->title_bar;}
	inline Image&                 getBG()          {return this->content_bg;}

	// SET
	//-------------------------------------------------------------------------
	inline void setTitleBar(Image i){this->title_bar = i;}; inline void setTitleBar(string path){this->setTitleBar(kernel->graphicsMgr->loadImage(path));}
	inline void setBG      (Image i){this->content_bg = i;} inline void setBG      (string path){this->setBG      (kernel->graphicsMgr->loadImage(path));}
	void setTitleWidget(WidgetPtr w);
	void setTitle(string t);
	void setCloseButton(shared_ptr<WButton> b);

	// MISC
	//-------------------------------------------------------------------------
	void close();

protected:
	virtual void _draw();
};

#endif // WIDGET_WINDOW_HPP_INCLUDED
