#ifndef TIMED_EVENT_HPP_INCLUDED
#define TIMED_EVENT_HPP_INCLUDED

#include "managers/time_mgr.hpp"
#include "tools/ordered_list.hpp"

#include <boost/function.hpp>

/// provides simple Timeline-functionality


class Event
{
	boost::function<void()> func_ptr;

public:
	Event(boost::function<void()> func_ptr)
	 : func_ptr(func_ptr){};

	inline void execute(){this->func_ptr();}
};

class EventManager
{
	SortedList<TimeVal, Event> data;
public:
	void addEvent(TimeVal when, boost::function<void()> what)
		{this->data.insert(when, Event(what));}

	void execute(TimeVal now)
	{
		BOOST_FOREACH(Event e, this->data.getAndRemoveElementsSEq(now))
			e.execute();
	}
};

#endif // TIMED_EVENT_HPP_INCLUDED
