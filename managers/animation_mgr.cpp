#include "animation_mgr.hpp"


//------------------------------------------------------------------------------
void
Timeline::calc(TimeVal now)
{
	if(!paused and !ended)
	{
	    now = now - starttime;

		assert(now >= 0);
		assert(this->size() >= 2);

		/// zum nächsten Keyframe gehen?
		if((*next_keyframe).t <= now)
			last_keyframe = next_keyframe++;

		/// ist Animation beendet?
		if(next_keyframe == keyframes.end())
		{
			if(set_func) set_func((*last_keyframe).x);

			if(not this->repeat)
            {
                this->ended = true;

                if(this->end_event_func) this->end_event_func();
            }
			else
			{
			    if(pingpong) this->reverse();

				this->start(now+starttime);
			}

			return;
		}

		assert(next_keyframe->t > last_keyframe->t);

		/// eigentliche Werte bestimmen
		FNumber y = 0;
		TimeVal delta = (*next_keyframe).t - (*last_keyframe).t;
		TimeVal x     = now - (*last_keyframe).t;
		FNumber pos   = FNumber(x)/FNumber(delta);
		switch(interpolation)
		{
			case LINEAR:
				/// y1 + (y2-y1) * x/delta
				y = (*last_keyframe).x + ((*next_keyframe).x-(*last_keyframe).x)*pos;
				break;

			case COS:
				/// y1 + (y2-y1) * (1-cos( x/delta * PI ))/2
				y = (*last_keyframe).x + ((*next_keyframe).x-(*last_keyframe).x) * (1-cos(pos*PI))/2;
				break;

			case EXP:
			case LOG:
			case RAND:
				throw ERROR("notImplemented","Only linear and cosinus interpolation are valid for now.");
				y = 0; break; /// NOT YET IMPLEMENTED!
		}

		/// und setzten
		this->value = y;
		if(set_func) set_func(y);
	};
};
//------------------------------------------------------------------------------
void
Timeline::start(TimeVal now)
{
    starttime = now;
	paused    = false;
	ended     = false;

	if(this->size() < 2 or this->length() <= 0) ///< zu wenige keyframes
	{
		ended = true;

		if(this->end_event_func) this->end_event_func();
	}
	else
	{
		next_keyframe = keyframes.begin();
		last_keyframe = next_keyframe++;
	}
}
//------------------------------------------------------------------------------
void
Timeline::reverse()
{
    TimeVal l = this->length();

    BOOST_FOREACH(Keyframe& k, this->keyframes)
        k.t = l - k.t;

    this->keyframes.reverse();
};
//------------------------------------------------------------------------------
//---------- AnimManager -------------------------------------------------------
//------------------------------------------------------------------------------
TimeVal
AnimationManager::letTimeElapse()
{
	TimeVal now = this->time_mgr->now()-inittime;

	BOOST_FOREACH(TimelinePtr tl, this->timelines)
		tl->calc(now);

	this->timelines.remove_if(boost::bind(&Timeline::hasEnded,_1));

	BOOST_FOREACH(boost::function<void(TimeVal)>& f, this->custom_anim)
		f(now);

	TimeVal elapsed = now - last;
	last = now;

	return elapsed;
};
//------------------------------------------------------------------------------
TimelinePtr
AnimationManager::from_to(FNumber from, FNumber to, TimeVal duration, bool start) // TODO: add interpolation mode
{
	assert(duration > 0);

	TimelinePtr tl(new Timeline());

	tl->addKeyframe(Keyframe(0,        from));
	tl->addKeyframe(Keyframe(duration ,to));

	this->addAnim(tl, start);

	return tl;
};
//------------------------------------------------------------------------------
