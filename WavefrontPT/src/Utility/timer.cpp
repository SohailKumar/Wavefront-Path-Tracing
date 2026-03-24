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
    return duration<float, std::milli> (steady_clock::now() - timerStart).count();
}

void Timer::UpdateWindowTitleWithTimer(bool mark)
{
    float t = 0;
    if(mark){
        t = this->Mark();
    }else{
        t = this->Peek();
    }

    std::wostringstream oss{}; // Initialize oss
    oss << L"Time: " << std::setprecision(1) << std::fixed << t << L" ms";
    Window::SetTitle(oss.str());
}

std::wstring Timer::GetStringTime(bool mark) {
    float t = 0;
    if (mark) {
        t = this->Mark();
    }
    else {
        t = this->Peek();
    }

    std::wostringstream oss{}; // Initialize oss
    oss << std::setprecision(1) << std::fixed << t << L" ms";
    return oss.str();
}
