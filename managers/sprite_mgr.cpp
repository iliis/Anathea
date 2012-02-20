#include "managers/sprite_mgr.hpp"


//------------------------------------------------------------------------------
/*void
Sprite::drawToScreen(Vect pos)
{
	if(this->visible)
	{
		if(this->graphicsMgr != boost::shared_ptr<GraphicsManager>())
			this->drawTo(this->graphicsMgr->getScreen(), pos);
		else
		{
			Image scr(SDL_Surface_ptr(SDL_GetVideoSurface(), null_deleter()));
			this->drawTo(scr, pos);
		}
	}
};*/
//------------------------------------------------------------------------------
//---------- SpriteManager -----------------------------------------------------
//------------------------------------------------------------------------------
#include "managers/sprites/SpriteImage.hpp"
SpriteManager::SpriteManager(boost::shared_ptr<GraphicsManager> gm)
	 : graphicsMgr(gm)
{
	boost::shared_ptr<SpriteImage> nullImg = this->createSprite<SpriteImage>();
	nullImg->setImage(gm->getFallbackImage());

	this->nullSprite = nullImg;
};
//------------------------------------------------------------------------------
void
SpriteManager::addSprite(SpritePtr sprite)
{
	/// Ist \the sprite bereits im Speicher?
	if(find(this->sprites.begin(), this->sprites.end(), sprite) == this->sprites.end())
	{
		this->sprites.push_back(sprite);
		sprite->setGraphicsManager(this->graphicsMgr);
	}
};
//------------------------------------------------------------------------------
void
SpriteManager::removeSprite(SpritePtr sprite)
{
	this->sprites.remove(sprite);
};
//------------------------------------------------------------------------------
#include "managers/sprites/all_sprites.hpp"
SpritePtr
SpriteManager::createSprite(string typ, ptree params)
{
	     if(typ == "line")          return this->createSprite<SpriteLine>(params);
	else if(typ == "box")           return this->createSprite<SpriteBox>(params);
	else if(typ == "image")         return this->createSprite<SpriteImage>(params);
	else if(typ == "dynamic_image") return this->createSprite<SpriteDynamicImage>(params);
	else if(typ == "multi_image")   return this->createSprite<SpriteMultiImage>(params);
	else if(typ == "text")          return this->createSprite<SpriteText>(params);
	else
		throw Error("notFound", "SpriteManager can't create sprite '"+typ+"'.");
};
//------------------------------------------------------------------------------
void
SpriteManager::createOrSetSprite(SpritePtr& sprite, ptree& params, string child_name, bool force_creation)
{
	if(params.get_child_optional("<children>."+child_name))
	{
		if(force_creation or sprite == SpritePtr())
			sprite = this->createSprite(params.get<string>("<children>."+child_name+".type"), params.get_child("<children>."+child_name));
		else
			sprite->set(params.get_child("<children>."+child_name));
	}
};
//------------------------------------------------------------------------------
