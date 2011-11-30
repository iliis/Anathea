#ifndef WIDGET_HPP_INCLUDED
#define WIDGET_HPP_INCLUDED

#include <map>
#include <string>
#include <typeinfo>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
using boost::shared_ptr;
using boost::weak_ptr;
using boost::dynamic_pointer_cast;

#include "tools/slots.hpp"
#include "tools/shape.hpp"
#include "tools/expression.hpp"
#include "managers/definitions.hpp"
#include "managers/input_mgr.hpp"
#include "managers/animation_mgr.hpp"


//------------------------------------------------------------------------------
class Widget;
class Kernel;

typedef shared_ptr<Widget>	WidgetPtr;
typedef weak_ptr<Widget>	WidgetWeakPtr;
//------------------------------------------------------------------------------
/** Ein GUI-Objekt.
  * Hat einen eindeutigen Namen, eine Position, Grösse
  * Transparenz und ein Sichtbar-Flag. Ist ausderdem Teil einer
  * Hierarchie.
  */
class Widget : public boost::enable_shared_from_this<Widget>
{
public:
	/// Eigenschaften
	Expression<string>  name;
	Expression<FNumber> rel_x, rel_y,
					    abs_x, abs_y,
	                    width, height;

	Expression<bool>   visible, draw_bounding_box;
	Expression<FNumber> alpha;

	virtual const char* getType() const = 0;

protected:
	Slots slots;

	/// Für den Zugriff auf das ganze restliche System...
	Kernel* kernel;

	/// Vater- und Kind-Widgets
	WidgetWeakPtr	parent;
	list<WidgetPtr>	childs;

	friend class GuiManager;
	Widget(string Name, Kernel* k);

public:
	virtual ~Widget() {this->slots.call("deleted"); cout << "Widget '" << this->name.get() << "' destroyed." << endl;};

//------------- GET ------------------------------------------------------------
	inline Vect    getSize()             const {return Vect(width, height);};
	inline FNumber getSize(int dim)      const {return get_dim2(dim, width.get(), height.get());}
	inline Vect    getRelPos()           const {return Vect(rel_x, rel_y);}
	inline FNumber getRelPos(int dim)    const {return get_dim2(dim, rel_x.get(), rel_y.get());}
	inline Vect    getAbsPos()           const {return Vect(abs_x, abs_y);};
	inline FNumber getAbsPos(int dim)    const {return (dim==1?abs_x:abs_y);}
	inline FNumber getAbsOuter(int dim)  const {return (dim==1?abs_x+width:abs_y+height);}

	inline Box     getBB()               const {return Box(this->getAbsPos(), this->getSize());}
	inline bool    isVisible()           const {return this->visible and this->alpha != A_TRANSPARENT;}

	inline WidgetPtr  getParent()       const {return this->parent.lock();}
	inline bool       hasParent()        const {return !this->parent.expired();}
	unsigned int      getNumberOfChilds()const {return this->childs.size();}
	inline bool       hasChild(WidgetPtr child);
	WidgetPtr         getChild(string path, bool throwIfNotFound = true);

	inline list<WidgetPtr> getChilds()   const {return this->childs;};

	inline Slots::SignalPtr getSlot(string name) const {return slots[name];}
	inline const Slots&     getSlots()    const {return this->slots;}

//------------- SET ------------------------------------------------------------
	inline void    setAbsPos(FNumber n, int dim) {assert(valid_dim2(dim)); if(dim==1)this->abs_x = n; else this->abs_y = n;}
	inline void    setRelPos(FNumber n, int dim) {assert(valid_dim2(dim)); if(dim==1)this->rel_x = n; else this->rel_y = n;}
	inline void    setRelPos(Vect p)             {this->rel_x = p.x, this->rel_y = p.y;}
	inline void    setSize  (FNumber n, int dim) {assert(valid_dim2(dim)); if(dim==1)this->width = n; else this->height = n;}
	inline void    setSize  (Vect s) {this->width = s.x; this->height = s.y;}

	        void    setRelativeTo      (Align horiz, bool h_inside, Vlign vert, bool v_inside, WidgetPtr relativeTo);
	inline void     setRelativeToParent(Align horiz, bool h_inside, Vlign vert, bool v_inside)
				{if(this->hasParent()) setRelativeTo(horiz,h_inside,vert,v_inside,this->parent.lock());
				else throw Error("illegalOperation","Can't set relativeToParent: There is no parent.");}

	inline  void   show(){this->visible = true;}
	inline  void   hide(){this->visible = false;}

protected:
	virtual void   _set(ptree n) {};
	virtual void   _draw(){}; ///< for normal implementations (gets called by draw())
//------------- INPUT ----------------------------------------------------------
	/// werden von abgeleiteten Objekten überschrieben.
	/// @return Ob Input verarbeitet wurde
	virtual bool handleKeyEvent  (KEY key, bool state){return false;}
	virtual bool handlePointMovEvent(PointKey oldP, PointKey newP){return false;}
	virtual bool handlePointHitEvent(PointKey pk){return false;}
public:
	virtual bool keyListener(KEY name, bool state);
	virtual bool pointMovListener(PointKey oldPoint, PointKey newPoint);
	virtual bool pointHitListener(PointKey pk);
//------------- DIV ------------------------------------------------------------
	virtual  void  draw();
	virtual  void  drawChilds();

	virtual  void  addChild(WidgetPtr child);
			 void  addChildDuringConstructor(WidgetPtr child);
	virtual  void  setParent(WidgetPtr new_parent);

	virtual  void  removeChildren();
	virtual  void  removeChild(WidgetPtr child);
	virtual  void  removeChild(string child_name);
	virtual  void  removeParent();

	virtual  void  moveToFront(); ///< Bewegt Objekt ans Ende der Liste, so dass es zuletzt, also zuoberst, gezeichnet wird.
	virtual  void  moveToBG   (); ///< Bewegt Objekt an den Anfang der Liste, so dass es zuerst, also zuunters, gezeichnet wird.
	virtual  void  moveChildToFront(WidgetPtr obj);
	virtual  void  moveChildToBG   (WidgetPtr obj);
	virtual  void  deleteMe(bool deleteFromMgr = true);
	void clearSlots();
	void deactivateSlots(){this->slots.deactivate(); BOOST_FOREACH(WidgetPtr c, this->childs) c->deactivateSlots();}

	virtual  void  set(ptree n);


	friend void setParentChild(WidgetPtr parent, WidgetPtr child);

	template <typename T>
	inline shared_ptr<T> cast(){return dynamic_pointer_cast<T>(shared_from_this());}

//---------- ANIMATIONS --------------------------------------------------------
	/*TimelinePtr fadeOut(FNumber sec = 0.5);
	TimelinePtr fadeOutAndDelete(FNumber sec = 0.5, bool deleteFromMgr = true);
	TimelinePtr fadeIn (FNumber sec = 0.5);

	TimelinePtr moveAnim(Vect dest, FNumber sec = 0.5);*/
};
//------------------------------------------------------------------------------
bool order_by_name(WidgetPtr& A, WidgetPtr& B);
//------------------------------------------------------------------------------

#endif // WIDGET_HPP_INCLUDED
