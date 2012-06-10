#ifndef SPRITEMULTIIMAGE_HPP_INCLUDED
#define SPRITEMULTIIMAGE_HPP_INCLUDED

#include "managers/sprites/SpriteImage.hpp"

class SpriteMultiImage : public SpriteImage
{
protected:
//---------- ATTRIBUTES --------------------------------------------------------
	vector<Box> frames;
	unsigned int default_frame;

//---------- HELPERS -----------------------------------------------------------

public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteMultiImage(Image image,
				boost::shared_ptr<GraphicsManager> gmgr = boost::shared_ptr<GraphicsManager>())
	 :	SpriteImage(image, gmgr), default_frame(0)
		{};

	SpriteMultiImage(boost::shared_ptr<GraphicsManager> gmgr = boost::shared_ptr<GraphicsManager>())
	 :	SpriteImage(gmgr), default_frame(0)
		{};

//---------- SET ---------------------------------------------------------------
	void set(ptree params);
	void setDefaultFrame(unsigned int f){this->default_frame = f;}
	void addFrame(Box f){this->frames.push_back(f);}

//---------- GET ---------------------------------------------------------------
	string getType(){return "MultiImage";}
	inline vector<Box>& getFrames(){return this->frames;}
	Box getFrame(unsigned int nr);

//---------- DRAW --------------------------------------------------------------
	void draw(Vect pos);
	void draw(Vect pos, unsigned int framenr);
	void draw(Vect pos, Box frame);
};

#endif // SPRITEMULTIIMAGE_HPP_INCLUDED
