#ifndef ANIMATION_MGR_HPP_INCLUDED
#define ANIMATION_MGR_HPP_INCLUDED

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#include "managers/time_mgr.hpp"

#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
struct Keyframe
{
	TimeVal t;
	double x;

	Keyframe(TimeVal T, double X) : t(T), x(X) {};
	Keyframe(double X) : t(0), x(X) {};
	Keyframe() : t(0), x(0) {};
};
//------------------------------------------------------------------------------
class Timeline
{
//---------- ATTRIBUTES --------------------------------------------------------
	list<Keyframe> keyframes;
	boost::function<void(double)> set_func;
	boost::function<void(void)>   end_event_func;
	enum Interpolation {LINEAR, COS, LOG, EXP, RAND} interpolation;
//---------- DATA --------------------------------------------------------------
	list<Keyframe>::iterator last_keyframe, next_keyframe;
	bool paused, ended, pingpong, repeat;
	TimeVal starttime, pausetime;

public:
	Timeline(boost::function<void(double)> set, Interpolation interp = COS)
	 : set_func(set), interpolation(interp),
	 last_keyframe(keyframes.begin()), next_keyframe(keyframes.begin()),
	 paused(true),ended(false),pingpong(false),repeat(false),
	 starttime(0),pausetime(0) {};

    void setEndEvent(boost::function<void(void)> f){this->end_event_func = f;}

	void calc   (TimeVal now);
	void addKeyframe(Keyframe kf){assert(this->empty() or kf.t > keyframes.back().t); this->keyframes.push_back(kf);}

	void pause  (TimeVal now)	{pausetime = now; paused = true;};
	void unpause(TimeVal now)	{starttime += now-pausetime; paused = false;};
	void start  (TimeVal now);
	void reverse();

	inline bool hasEnded(){return this->ended;}

	inline void setPingPong(bool pp){this->pingpong = pp;}
	inline void setRepeat  (bool rep){this->repeat = rep;}

	inline int  size() {return this->keyframes.size();}
	inline bool empty(){return this->keyframes.empty();}
	inline TimeVal length(){return this->keyframes.back().t - this->keyframes.front().t;}
};

typedef boost::shared_ptr<Timeline> TimelinePtr;
/*//------------------------------------------------------------------------------
class HasEnded
{
public:
	bool operator()(TimelinePtr& t){return t->hasEnded();}
};*/
//------------------------------------------------------------------------------
class AnimationManager
{
	list<TimelinePtr> timelines;
	list<boost::function<void(TimeVal)> > custom_anim;

	boost::shared_ptr<TimeManager> time_mgr;
	TimeVal inittime, last;

public:
	AnimationManager(boost::shared_ptr<TimeManager> tm) : time_mgr(tm), inittime(tm->now()), last(0) {};

	void addAnim(TimelinePtr tl, bool start=false){this->timelines.push_back(tl); if(start){tl->start(time_mgr->now()-inittime);}}
	void addCustomAnim(boost::function<void(TimeVal)> f){this->custom_anim.push_back(f);}

	TimeVal letTimeElapse();
	void resetInitTime(){this->inittime = time_mgr->now();}
};
//------------------------------------------------------------------------------

#endif // ANIMATION_MGR_HPP_INCLUDED
