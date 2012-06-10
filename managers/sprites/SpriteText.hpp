#ifndef SPRITETEXT_HPP_INCLUDED
#define SPRITETEXT_HPP_INCLUDED

#include <string>

#include "managers/sprite_mgr.hpp"
#include "managers/definitions.hpp"

using namespace std;

class SpriteText : public Sprite
{
//---------- ATTRIBUTES --------------------------------------------------------
	Image buffer;

	string text;
	Font font;

	Color text_color, background_color;

	Align align;
	Font::Quality quality;
	bool utf;

//---------- HELPERS -----------------------------------------------------------
protected:
	void render();
	void setSizeToImage();
public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteText(boost::shared_ptr<GraphicsManager> gmgr)
	 :	Sprite(gmgr),
		buffer(gmgr->getFallbackImage()), ///< nur damit \the buffer irgendwie initialisiert wird
		text(""), font(gmgr->getFallbackFont()),
		text_color(BLACK), background_color(WHITE),
		align(LEFT), quality(Font::BLENDED), utf(false)
		{
			/// da wird \the buffer sowieso überschrieben
			// this->render();

			buffer.color.setA(this->alpha);
		};

//---------- GET ---------------------------------------------------------------
	string getText()   {return this->text;}
	Font   getFont()   {return this->font;}
	Color  getTextCol(){return this->text_color;}
	Color  getBGCol()  {return this->background_color;}
	Align  getAlign()  {return this->align;}
	Font::Quality getQuality(){return this->quality;}
	bool  usesUTF()   {return this->utf;}

	Vect	getMinSize();
	Vect	getMaxSize();

	string getType(){return "text";};

//---------- SET ---------------------------------------------------------------
	void setText(string t){this->text = t; this->render();}
	void setFont(Font f)  {this->font = f; this->render();}
	void setTextCol(Color c){this->text_color = c; this->render();}
	void setBGCol(Color c)  {this->background_color = c; this->render();}
	void setAlign(Align a){this->align = a; this->render();}
	void setQuality(Font::Quality q){this->quality = q; this->render();}
	void setUTF(bool u){this->utf = u; this->render();}

	void set(ptree params);

//---------- DRAW --------------------------------------------------------------
	void draw(Vect pos = Vect(0,0));
};

#endif // SPRITETEXT_HPP_INCLUDED
