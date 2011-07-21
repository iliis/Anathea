#include "profiler.hpp"

//------------------------------------------------------------------------------
Profiler::Profiler()
{
    memset(&data, 0 , sizeof(data));
    total = 0;
    lastAddTime = 0;
};
//------------------------------------------------------------------------------
void
Profiler::add(int where)
{
    Uint32 actTime = SDL_GetTicks();
    Uint32 diff = actTime - lastAddTime;
    data[where] += diff;
    total += diff;
    lastAddTime = actTime;
};
//------------------------------------------------------------------------------
