#ifndef INPUT_MGR_HPP_INCLUDED
#define INPUT_MGR_HPP_INCLUDED

#include <boost/signal.hpp>
#include "managers/definitions.hpp"
#include "tools/shape.hpp"

bool isAMouseKey(KEY name);
string getMouseName(KEY name);
//KEY getMouseKey(string name);

typedef string PointName;

struct PointKey
{
	PointName name;
	bool state, visible;
	Point point;

	PointKey() : name(),state(false),visible(true),point(Vect(0,0)) {};
	PointKey(PointName name, bool state, Point p) : name(name), state(state), point(p) {};
};

class InputManager
{
protected:
	map<KEY, bool> keys;
	map<PointName, PointKey> points;

	boost::signal<void (KEY, bool)>           keyListeners;
	boost::signal<void (PointKey, PointKey)>  pointMovListeners;
	boost::signal<void (PointKey)>            pointHitListeners;
public:
	inline	bool		getKeyState(KEY name)	{return keys[name];};
	inline	PointKey	getPoint(PointName name){return points[name];};
	inline map<string, PointKey>& getPoints() {return this->points;}

	inline	bool	isShiftOrCapsPressed()	{return this->getKeyState(KEY_LSHIFT) || this->getKeyState(KEY_RSHIFT) || this->getKeyState(KEY_CAPSLOCK);}

	inline boost::signals::connection
		addKeyListener(boost::signal<void (KEY, bool)>::slot_function_type subscriber)
			{return keyListeners.connect(subscriber);};

	inline boost::signals::connection
		addPointMovListener(boost::signal<void (PointKey, PointKey)>::slot_function_type subscriber)
			{return pointMovListeners.connect(subscriber);};

	inline boost::signals::connection
		addPointHitListener(boost::signal<void (PointKey)>::slot_function_type subscriber)
			{return pointHitListeners.connect(subscriber);}

	inline void removeListener(boost::signals::connection subscriber)
		{subscriber.disconnect();};

	inline void inputKeyDown(KEY name) {this->keys[name] = true;  keyListeners(name, true); };
	inline void inputKeyUp  (KEY name) {this->keys[name] = false; keyListeners(name, false);};

	inline void inputPointDown(PointName name){this->points[name].state = true;  pointHitListeners(points[name]);}
	inline void inputPointUp  (PointName name){this->points[name].state = false; pointHitListeners(points[name]);}

	void inputPoint (PointKey p){PointKey old = points[p.name]; points[p.name] = p; pointMovListeners(old, p);};
	void inputPoint2(PointName name, int x, int y)
	{
		this->inputPoint(PointKey(name, points[name].state, Vect(x,y)));
	}

	inline void addPointKey(PointName name){this->points[name] = PointKey(name,false,Point(Vect(0,0)));}

	void hideCursor(bool hide=true){SDL_ShowCursor(hide?SDL_DISABLE:SDL_ENABLE);}
	void grabCursor(bool grab=true){SDL_WM_GrabInput(grab?SDL_GRAB_ON:SDL_GRAB_OFF);}
};

#endif // INPUT_MGR_HPP_INCLUDED
