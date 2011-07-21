#ifndef TIME_MGR_HPP_INCLUDED
#define TIME_MGR_HPP_INCLUDED

#include <time.h>
#include "tools/stringhelpers.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

using namespace std;

typedef long long	TimeVal; /// usually in microseconds

double toSeconds	(TimeVal t);
TimeVal fromSeconds	(double sec);

TimeVal timeFromString(string val, string delimiter = ".");

class TimeManager
{
	TimeVal startTime;
public:
	TimeManager() {this->startTime = this->now();}
	int getCurSeconds();
	int getCurMinutes();
	int getCurHours();
	//int getCurMiliSeconds();
	//int getCurMicroSeconds();
	TimeVal getCurTime();
	inline TimeVal now()    {return this->getCurTime();}
	//inline TimeVal getTime(){return this->getTimeSinceStart();}
	inline TimeVal getTimeSinceStart(){return this->now() - this->startTime;}
	inline void reset(){this->startTime = this->now();}
};

#endif // TIME_MGR_HPP_INCLUDED
