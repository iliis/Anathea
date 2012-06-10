#ifndef SPRITEBOX_HPP_INCLUDED
#define SPRITEBOX_HPP_INCLUDED

#include "managers/sprite_mgr.hpp"

//#include "SDL_gfxPrimitives.h"

class SpriteBox : public Sprite
{
//---------- ATTRIBUTES --------------------------------------------------------
	bool drawFill;
	float borderSize;
	Color borderCol, fillCol;
public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteBox(boost::shared_ptr<GraphicsManager> gmgr = boost::shared_ptr<GraphicsManager>())
	 : Sprite(gmgr),
		drawFill(true), borderSize(0),
		borderCol(BLACK), fillCol(WHITE)
		{};
//---------- DRAW --------------------------------------------------------------
	virtual void draw(Vect pos=Vect(0,0));
//---------- GET ---------------------------------------------------------------
	bool		isBorderEnabled(){return this->borderSize > 0;}
	float		getBorderSize(){return this->borderSize;}
	Color		getBorderColor() {return this->borderCol;}

	bool		isFillEnabled(){return this->drawFill;}
	Color		getFillColor() {return this->fillCol;}

	Vect getMinSize(){return Vect(0,0);};
	Vect getMaxSize(){return Vect(0,0);};

	string      getType(){return "Box";}
//---------- SET ---------------------------------------------------------------
	void setBorderSize(float s)	{this->borderSize = s;}
	void setBorderColor(Color c)	{this->borderCol = c;}

	void setFill(bool enabled)		{this->drawFill = enabled;}
	void setFillColor(Color c)		{this->fillCol = c;}

	void setAlpha(ColVal a)			{this->alpha = a; fillCol.setSecondaryAlpha(a); borderCol.setSecondaryAlpha(a);}

	void set(ptree params);
//------------------------------------------------------------------------------
};

#endif // SPRITEBOX_HPP_INCLUDED
