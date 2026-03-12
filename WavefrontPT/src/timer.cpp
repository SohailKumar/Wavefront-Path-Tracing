#include "timer.h"

using namespace std::chrono;

Timer::Timer()
{
    timerStart = steady_clock::now();
}

// Returns duration of timer and resets timerStart
float Timer::Mark()
{
    const auto old = timerStart;
    timerStart = steady_clock::now(); // reset 
    const duration<float>  frameTime = timerStart - old;
    return frameTime.count();
}

// Returns duration of timer and DOESN'T reset timerStart
float Timer::Peek() const
{
    return duration<float> (steady_clock::now() - timerStart).count();
}
