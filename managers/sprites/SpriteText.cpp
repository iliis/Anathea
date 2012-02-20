#include "SpriteText.hpp"

//------------------------------------------------------------------------------
Vect
SpriteText::getMinSize()
{
	/// Momentan noch kein Support für dynamisches Resizing
	return this->getSize();
};
//------------------------------------------------------------------------------
Vect
SpriteText::getMaxSize()
{
	return this->getSize();
};
//------------------------------------------------------------------------------
void
SpriteText::draw(Vect pos)
{
	if(this->isVisible() && this->text != "")
	{
		this->buffer.draw(pos);
	}
};
//------------------------------------------------------------------------------
void
SpriteText::setSizeToImage()
{
	if(this->text == "")
		this->setSize(Vect(0,0));
	else
		this->setSize(this->buffer.getSize().cast<Vect::T>());
};
//------------------------------------------------------------------------------
#include "managers/kernel.hpp"
void
SpriteText::set(ptree params)
{
	this->text             = params.get("text", this->text);
	this->text_color       = params.get("text_color", this->text_color.getString());
	this->background_color = params.get("bg_color",   this->background_color.getString());
	this->align            = getAlignByName(params.get("align", getAlignName(this->align)));
	this->quality          = getFontQualityByName(params.get("quality", getFontQualityName(this->quality)));
	setBoolFromPT(params, "utf", this->utf);

	/// load Font
	int fntsize = this->font.getSize();
	fntsize = params.get("size", fntsize);
	string fnt = params.get("font", this->font.getPath());
	this->font = this->graphicsMgr->loadFont(fnt, fntsize);

	this->render();
};
//------------------------------------------------------------------------------
void
SpriteText::render()
{
	 if(this->text != "") ///< Ansonsten reklamiert SDL
		this->buffer = this->font.renderMultiline(
										this->text,
										this->graphicsMgr,
										this->text_color,
										this->background_color,
										this->align,
										this->quality,
										this->utf);
	this->setSizeToImage();
	//this->buffer.setAlpha(this->alpha);
};
//------------------------------------------------------------------------------
