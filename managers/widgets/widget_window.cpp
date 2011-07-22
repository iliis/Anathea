#include "widget_window.hpp"

//------------------------------------------------------------------------------
WWindow::WWindow(string name, Kernel* k)
 : Widget(name, k),
   title_bar(k->graphicsMgr->getFallbackImage()),
   content_bg(k->graphicsMgr->getFallbackImage())
{
	this->padding_bottom = padding.ref();
	this->padding_top    = padding.ref();
	this->padding_left   = padding.ref();
	this->padding_right  = padding.ref();

	this->content = kernel->guiMgr->createWidget<WContainer>(name+"_content");
	this->content->abs_x  = this->abs_x.ref() + border.ref() + padding_left.ref();
	this->content->abs_y  = this->abs_y.ref() + border.ref() + padding_top.ref() + title_bar_height.ref();
	this->content->width  = this->width.ref()  - 2*border.ref() - padding_left.ref() - padding_right.ref();
	this->content->height = this->height.ref() - 3*border.ref() - padding_bottom.ref() - padding_top.ref() - title_bar_height.ref();
	this->content->removeParent(); // only WE are responsible for this widget

	this->slots.add("closed");
};
//------------------------------------------------------------------------------
void
WWindow::setTitleWidget(WidgetPtr w)
{
	if(title) title->removeParent();

	this->title = w;
	this->title_bar_height = this->title->height.ref() + 2*border.ref();
	this->title->setParent(shared_from_this());

	title->abs_x = this->abs_x.ref()+border.ref();
	title->abs_y = this->abs_y.ref()+border.ref();
};
//------------------------------------------------------------------------------
void
WWindow::setTitle(string t)
{
	shared_ptr<WText> w = kernel->guiMgr->createWidget<WText>(this->name.get()+"_title");
	w->setText(t);

	this->setTitleWidget(w);
};
//------------------------------------------------------------------------------
void
WWindow::setCloseButton(shared_ptr<WButton> b)
{
	if(close_button)
	{
		close_connection.disconnect(); // disregard any events from old button
		close_button->removeParent();
	}
	close_button = b;
	close_button->getSlot("clicked")->connect(boost::bind(&WWindow::close, this));
	close_button->setParent(shared_from_this());

	close_button->abs_x = this->abs_x.ref() + this->width.ref() - this->border.ref() - close_button->width.ref();
	close_button->abs_y = this->abs_y.ref() + this->border.ref();
	close_button->height = this->title_bar_height.ref() - 2*border.ref();
};
//------------------------------------------------------------------------------
void
WWindow::close()
{
	this->visible = false;
	this->slots.call("closed");
};
//------------------------------------------------------------------------------
void
WWindow::_draw()
{
	this->content_bg.draw(Box(abs_x,abs_y+title_bar_height,width,height-title_bar_height));
	this->title_bar .draw(Box(abs_x,abs_y,width,title_bar_height));

	//this->close_button->draw();
	//this->title->draw();
	this->content->draw();
};
//------------------------------------------------------------------------------
