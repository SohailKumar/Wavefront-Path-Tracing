#pragma once
#include <chrono>

class Timer {
public:
	Timer();
	float Mark();
	float Peek() const;
	void UpdateWindowTitleWithTimer();
private:
	std::chrono::steady_clock::time_point timerStart;
};