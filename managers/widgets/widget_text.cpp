#include "widget_text.hpp"

//------------------------------------------------------------------------------
void
WText::_set(ptree n)
{
	if(n.get_child_optional("text.content"))
		this->setText(n.get("text.content", this->text));

	if(n.get_child_optional("text.padding"))
		this->padding = n.get<FNumber>("text.padding");

	if(n.get_child_optional("text.font"))
	{
		if(n.get_child_optional("text.font.path") && n.get_child_optional("text.font.size"))
			this->setFont(this->kernel->graphicsMgr->loadFont(
													n.get<string>("text.font.path"),
													n.get<int>  ("text.font.size")));

		else if(n.get_child_optional("text.font.size"))
			this->setFontSize(n.get<int>("text.font.size"));

		else if(n.get_child_optional("text.font.path"))
			this->setFont(this->kernel->graphicsMgr->loadFont(
													n.get<string>("text.font.path"),
													this->font.getSize()));

		if(n.get_child_optional("text.font.color"))
			this->setColor(Color(n.get<string>("text.font.color")));

		if(n.get_child_optional("text.font.background_color"))
			this->setBG(Color(n.get<string>("text.font.background_color")));

		/// TODO: transparent [bool]

		/// TODO: alignment
	}
};
//------------------------------------------------------------------------------
void
WText::setFont(Font f)
{
	this->font = f;
	this->render();
};
//------------------------------------------------------------------------------
void
WText::setFontSize(int s)
{
	if(s != this->font.getSize())
		this->setFont(this->kernel->graphicsMgr->loadFont(this->font.getPath(), s));
};
//------------------------------------------------------------------------------
void
WText::setText(string t)
{
	if(t != this->text)
	{
		/// copy text in order to prevent invalid iterators (WTextInput)
		this->text.assign(t.begin(), t.end());
		this->render();
	}
};
//------------------------------------------------------------------------------
void
WText::_draw()
{
	if(this->text.empty()) return;

	this->buffer.color.a = this->alpha;

	if(this->font.isValid())
	{
		/// round the coordinates to integer, so the text always is
		/// nicely aligned to pixels (otherwise, blur can occur)

		Box there = this->getBB();
		there.pos.x = floor(there.pos.x+0.5+padding);
		there.pos.y = floor(there.pos.y+0.5+padding);
		there.size.x -= padding*2; // padding is already added twice in render
		there.size.y -= padding*2;
		this->buffer.draw(there);
	}
	else
		throw ERROR("draw","WText::draw(): Font isn't valid.");
};
//------------------------------------------------------------------------------
void
WText::render()
{
	if(!this->font.isValid())
		throw ERROR("draw","WText::render(): font isn't valid!");

	if(!this->text.empty())
	{
		this->buffer = this->font.renderMultiline(this->text,this->kernel->graphicsMgr,
									this->color,this->background,this->align,
									(this->transparent?Font::BLENDED:Font::SHADED),false);

		this->width  = buffer.getSize().x+padding.ref()*2;
		this->height = buffer.getSize().y+padding.ref()*2;
	}
	else
	{
		this->width  = padding.ref()*2;
		this->height = padding.ref()*2;
	}
};
//------------------------------------------------------------------------------
