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
    const duration<float>  frameTime = timerStart - old;
    return frameTime.count();
}

// Returns duration of timer and DOESN'T reset timerStart
float Timer::Peek() const
{
    return duration<float> (steady_clock::now() - timerStart).count();
}

void Timer::UpdateWindowTitleWithTimer()
{
    const float t = this->Peek();
    std::wostringstream oss{}; // Initialize oss
    oss << L"Time Elapsed: " << std::setprecision(1) << std::fixed << t << L" sec";
    Window::SetTitle(oss.str());
}
