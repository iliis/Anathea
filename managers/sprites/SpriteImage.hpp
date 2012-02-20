#ifndef SPRITEIMAGE_HPP_INCLUDED
#define SPRITEIMAGE_HPP_INCLUDED

#include "managers/sprite_mgr.hpp"

class SpriteImage : public Sprite
{
protected:
//---------- ATTRIBUTES --------------------------------------------------------
	Image img;

//---------- HELPERS -----------------------------------------------------------
	void setSizeToImage(){this->width = this->img.getSize().x; this->height = this->img.getSize().y;}
public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteImage(Image image,
				boost::shared_ptr<GraphicsManager> gmgr = boost::shared_ptr<GraphicsManager>())
	 :	Sprite(gmgr),
		img(image)
			{this->setSizeToImage(); img.alpha = this->alpha;};

	SpriteImage(string image,
				boost::shared_ptr<GraphicsManager> gmgr)
	 :	Sprite(gmgr), img(gmgr->loadImage(image))
			{this->setSizeToImage(); img.alpha = this->alpha;};

	SpriteImage(boost::shared_ptr<GraphicsManager> gmgr)
	 :	Sprite(gmgr), img(gmgr->getFallbackImage())
			{this->setSizeToImage(); img.alpha = this->alpha;};

//---------- SET ---------------------------------------------------------------
	void set(ptree params);
	void setImage(Image i){this->img = i; this->setSizeToImage();}

//---------- GET ---------------------------------------------------------------
	Vect getMinSize(){return this->getSize();};
	Vect getMaxSize(){return this->getSize();};

	string getType(){return "Image";}

//---------- DRAW --------------------------------------------------------------
	void draw(Vect pos);
};

#endif // SPRITEIMAGE_HPP_INCLUDED
