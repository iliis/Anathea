#include "time_mgr.hpp"

//--------------------------------------------------------------------
double toSeconds(TimeVal t)
	{return ((double)t)/1000/1000;}
	//--------------------------------------------------------------------
TimeVal fromSeconds(double sec)
	{return sec*1000000;}
//--------------------------------------------------------------------
TimeVal timeFromString(string val, string delimiter)
{
	val = trimString(val);

	int bb = atoi(getLeftSubstring(val, delimiter).c_str());

	string	ll_string		= getRightSubstring(val, delimiter);
	int		ll				= atoi(fillStringRight(ll_string, 6, '0').c_str());

	return bb*1000000+ll;
};
//--------------------------------------------------------------------
int TimeManager::getCurSeconds()
{
    time_t raw_now;
	struct tm * timeinfo;
	char now[200];


	//get current day of the year
	time(&raw_now);
	timeinfo = localtime(&raw_now);
	strftime(now,200,"%S",timeinfo);
	return atoi(now);
};
//--------------------------------------------------------------------
int TimeManager::getCurMinutes()
{
    time_t raw_now;
	struct tm * timeinfo;
	char now[200];


	//get current day of the year
	time(&raw_now);
	timeinfo = localtime(&raw_now);
	strftime(now,200,"%M",timeinfo);
	return atoi(now);
};
//--------------------------------------------------------------------
int TimeManager::getCurHours()
{
    time_t raw_now;
	struct tm * timeinfo;
	char now[200];


	//get current day of the year
	time(&raw_now);
	timeinfo = localtime(&raw_now);
	strftime(now,200,"%H",timeinfo);
	return atoi(now);
};
//--------------------------------------------------------------------
/*int TimeManager::getCurMiliSeconds()
{

#	ifdef _WIN32
	LONGLONG time, freq;
	QueryPerformanceCounter((LARGE_INTEGER*)&time);
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	//double d_time = ((double)time) / ((double)freq);
	//return d_time;
	return (TimeVal)((long double)(time)/(long double)(freq))*1000*1000;
#   else
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_usec / 1000;
#   endif
}
//--------------------------------------------------------------------
int TimeManager::getCurMicroSeconds()
{
    struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_usec - (tv.tv_usec/1000)*1000;
};*/
//--------------------------------------------------------------------
TimeVal TimeManager::getCurTime()
{
	//see: http://nebuladevice.cubik.org/documentation/nebula2/ntime__main_8cc-source.shtml#l00164 (17.11.08)
	// or: http://cplus.about.com/od/howtodothingsi2/a/timing.htm (22.02.11)
#	ifdef _WIN32
	LARGE_INTEGER time, freq;
	QueryPerformanceCounter(&time);
	QueryPerformanceFrequency(&freq);
	//double d_time = ((double)time) / ((double)freq);
	//return d_time;
	return (TimeVal)((long double)(time.QuadPart*1000*1000)/(long double)(freq.QuadPart));
#   else
	struct timeval tv;
	gettimeofday(&tv,NULL);
	//return timeVal(tv.tv_sec, tv.tv_usec);
	return ((TimeVal)(tv.tv_sec))*1000*1000+tv.tv_usec;
#	endif
};
//--------------------------------------------------------------------
