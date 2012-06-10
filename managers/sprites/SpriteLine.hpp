#ifndef SPRITELINE_HPP_INCLUDED
#define SPRITELINE_HPP_INCLUDED

#include "managers/sprite_mgr.hpp"

//#include "SDL_gfxPrimitives.h"

//------------------------------------------------------------------------------
class SpriteLine : public Sprite
{
//---------- ATTRIBUTES --------------------------------------------------------
	Vect to;
	Color col;
	bool AA;
public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteLine(boost::shared_ptr<GraphicsManager> gmgr)
	 : Sprite(gmgr), to(Vect(0,0)), col(WHITE), AA(true)
		{};

//---------- GET ---------------------------------------------------------------
	inline Vect  getDest(){return to;}
	inline Color getColor(){return col;}
	inline bool hasAA(){return AA;}
	inline float getLength(){return this->to.length();}

	Vect getMinSize();
	Vect getMaxSize();

	string getType(){return "Line";}

//---------- SET ---------------------------------------------------------------
	inline void setEndPos(Vect p){this->to = p;}
	inline void setColor(Color c){this->col = c;}
	inline void setAA(bool aa){this->AA = aa;}
	void setAlpha(ColVal a) {this->alpha = a; col.setSecondaryAlpha(a);}

	void set(ptree params);

//---------- DRAW --------------------------------------------------------------
	void draw(Vect pos=Vect(0,0));
};
//------------------------------------------------------------------------------

#endif // SPRITELINE_HPP_INCLUDED
