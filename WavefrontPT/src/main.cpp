#include <string>
#include "window.h"

static int WINAPI WinMain(
	HINSTANCE hInst, 
	HINSTANCE hInstPrev, 
	PSTR cmdline, 
	int cmdshow)
{
	const int width = 1280;
	const int height = 720;

	Window::Create(hInst, width, height, L"Nami Window Name Here", false, NULL);
	
	// Message Loop for input:
	MSG msg;
	BOOL gResult;
	while ((gResult = GetMessage(&msg, NULL, 0, 0)) > 0) {
		TranslateMessage(&msg); // Generates char message along with key down message
		DispatchMessage(&msg); // Pass to process message
	}

	if (gResult == -1) {
		return -1;
	}
	else {
		return msg.wParam;
	}
}

