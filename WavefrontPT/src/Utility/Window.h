#pragma once

#include <Windows.h>
#include <string>
#include <optional>
#include "GraphicsDx11.h"

namespace Window
{
	//Getters
	unsigned int GetWidth();
	unsigned int GetHeight();
	HWND GetHandle();
	bool HasFocus();
	bool IsMinimized();
	void SetTitle(const std::wstring& title);

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

	std::optional<int> ProcessMessages();

	LRESULT ProcessMessage(
		HWND hWnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);

	void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns);

	void Update(int frameCount);

}