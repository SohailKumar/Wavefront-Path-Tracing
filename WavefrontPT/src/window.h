#pragma once

#include <Windows.h>
#include <string>

namespace Window
{
	//Getters
	unsigned int GetWidth();
	unsigned int GetHeight();
	HWND GetHandle();
	bool HasFocus();
	bool IsMinimized();

	//Window-related
	HRESULT Create(
		HINSTANCE appInstance,
		unsigned int width,
		unsigned int height,
		std::wstring titleBarText,
		bool statsInTitleBar,
		void (*resizeCallback)());
	//void UpdateStats(float totalTime);
	void Quit();

	LRESULT ProcessMessage(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);

}