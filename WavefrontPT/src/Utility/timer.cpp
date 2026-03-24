#include "timer.h"
#include "Window.h"

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
    const duration<float, std::milli>  frameTime = timerStart - old;
    return frameTime.count();
}

// Returns duration of timer and DOESN'T reset timerStart
float Timer::Peek() const
{
    return duration<float> (steady_clock::now() - timerStart).count();
}

void Timer::UpdateWindowTitleWithTimer(bool mark)
{
    if(mark){
        const float t = this->Mark();
    }else{
        const float t = this->Peek();
    }

    std::wostringstream oss{}; // Initialize oss
    oss << L"Time: " << std::setprecision(1) << std::fixed << t << L" ms";
    Window::SetTitle(oss.str());
}
