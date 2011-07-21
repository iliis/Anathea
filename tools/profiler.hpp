#ifndef PROFILER_HPP_INCLUDED
#define PROFILER_HPP_INCLUDED

#include <SDL/SDL.h>

class Profiler
{
private:
    Uint32 lastAddTime;
public:
    Uint32 data[10];
    Uint32 total;
    Profiler();
    void add(int where);
};

#endif // PROFILER_HPP_INCLUDED
