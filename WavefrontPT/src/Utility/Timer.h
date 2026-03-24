#pragma once
#include <chrono>

class Timer {
public:
	Timer();
	float Mark();
	float Peek() const;
	void UpdateWindowTitleWithTimer(bool mark);
	std::wstring GetStringTime(bool mark);
private:
	std::chrono::steady_clock::time_point timerStart;
};