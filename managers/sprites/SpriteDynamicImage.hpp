#ifndef SPRITEDYNAMICIMAGE_HPP_INCLUDED
#define SPRITEDYNAMICIMAGE_HPP_INCLUDED

#include "managers/sprites/SpriteImage.hpp"

class SpriteDynamicImage : public SpriteImage
{
//---------- ATTRIBUTES --------------------------------------------------------
	Vect p1,p2,imgsize;

//---------- HELPERS -----------------------------------------------------------

public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteDynamicImage(string image,
				boost::shared_ptr<GraphicsManager> gmgr)
	 :	SpriteImage(image, gmgr) {};

	SpriteDynamicImage(boost::shared_ptr<GraphicsManager> gmgr)
	 :	SpriteImage(gmgr) {};

//---------- SET ---------------------------------------------------------------
	inline void setGeometry(Vect P1, Vect P2){ this->p1 = P1; this->p2 = P2; }
	void set(ptree params);

//---------- GET ---------------------------------------------------------------
	Vect getMinSize(){return this->img.getSize().cast<Vect::T>();};
	Vect getMaxSize(){return Vect(0,0);};

	string getType(){return "DynamicImage";}

//---------- DRAW --------------------------------------------------------------
	void draw(Vect pos);
};

#endif // SPRITEDYNAMICIMAGE_HPP_INCLUDED
