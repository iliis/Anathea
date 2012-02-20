#ifndef SPRITE_MGR_HPP_INCLUDED
#define SPRITE_MGR_HPP_INCLUDED

#include <boost/foreach.hpp>

#include "managers/image_mgr.hpp"
#include "tools/slots.hpp"

#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
template <typename T>
inline ptree mkPtree(string key, T val){ptree pt; pt.put(key,val); return pt;}


//------------------------------------------------------------------------------
/** Ein beliebiges, konkretes Grafikelement.
  Kann alles von einem bunten Rechteck über Text bis zu einer Komposition
  verschiedener Bilder sein. Hat nur eine Grösse.
  */
class Sprite
{
protected:
	boost::shared_ptr<GraphicsManager> graphicsMgr;
	Slots slots;
//---------- ATTRIBUTES --------------------------------------------------------
public:
	Expression<bool>   visible;
	Expression<ColVal> alpha;
	Expression<Vect::T> width, height;

	virtual string getType() = 0;

//---------- CONSTRUCTOR -------------------------------------------------------
	Sprite(boost::shared_ptr<GraphicsManager> gmgr = boost::shared_ptr<GraphicsManager>())
	 :	graphicsMgr(gmgr), visible(true), alpha(A_OPAQUE) //, width(0), height(0)
		{
			//slots.add("size");
			//slots.add("visibility");
		};
	virtual ~Sprite(){};

//---------- DRAW --------------------------------------------------------------
	virtual void  draw(Vect pos=Vect(0,0)) = 0;

//---------- SET ---------------------------------------------------------------
	virtual void setSize(Vect s)          {width = s.x; height = s.y;}
	//inline  void setVisibility(bool vis)  {if(vis!=visible){this->visible = vis; slots.call("visibility");}}
	//virtual void setAlpha(ColVal a)       {this->alpha = a;}
	inline  void toggleVisibility()       {this->visible = !this->visible;}
	inline  void setGraphicsManager(boost::shared_ptr<GraphicsManager> gm)
	                                      {this->graphicsMgr = gm;}
	virtual void set(ptree params) = 0;

//---------- GET ---------------------------------------------------------------
	inline Vect	getSize()  {return Vect(width,height);}
	virtual Vect	getMinSize() = 0;
	virtual Vect	getMaxSize() = 0;
	inline bool	isVisible(){return this->visible and this->alpha > 0;}
	inline	ColVal	getAlpha(){return this->alpha;}
	inline	Slots::SignalPtr getSlot(string name){return slots[name];}
};

typedef boost::shared_ptr<Sprite> SpritePtr;



//------------------------------------------------------------------------------
//---------- SpriteManager -----------------------------------------------------
//------------------------------------------------------------------------------
class SpriteManager
{
//---------- ATTRIBUTES --------------------------------------------------------
	list<SpritePtr> sprites;
	boost::shared_ptr<GraphicsManager> graphicsMgr;
	SpritePtr nullSprite;

public:
//---------- CONSTRUCTOR -------------------------------------------------------
	SpriteManager(boost::shared_ptr<GraphicsManager> gm);

//---------- MANAGE ------------------------------------------------------------
	void addSprite(SpritePtr sprite);
	void removeSprite(SpritePtr sprite);

//---------- FACTORY -----------------------------------------------------------
	/// Erzeugt ein neues Sprite, entweder aus einem String, oder einem Typ als Parameter
	SpritePtr createSprite(string typ, ptree params = ptree());
	SpritePtr createSprite(ptree pt){return this->createSprite(pt.get_value<string>(), pt);}

	template <typename T>
	boost::shared_ptr<T> createSprite(ptree params = ptree())
	{
		boost::shared_ptr<T> newSprite(new T(graphicsMgr));

		if(!params.empty())
			newSprite->set(params);

		this->addSprite(newSprite);
		return newSprite;
	};

	/// Setzt Parameter und erzeugt Sprite falls nötig
	void createOrSetSprite(SpritePtr& sprite, ptree& params, string child_name, bool force_creation=false);
//---------- MISC --------------------------------------------------------------
	SpritePtr getFallback(){return this->nullSprite;}
};
//------------------------------------------------------------------------------

#endif // SPRITE_MGR_HPP_INCLUDED
