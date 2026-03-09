#include <windows.h>

LRESULT CALLBACK ProcessMessage(
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam) 
{
	switch (msg) {
	case WM_CLOSE:
		PostQuitMessage(69); // Posts quit message to Message Queue. Returning with code 69.
		break;
	case WM_LBUTTONDOWN:
		const POINTS pt = MAKEPOINTS(lParam);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

static int WINAPI WinMain(
	HINSTANCE hInst, 
	HINSTANCE hInstPrev, 
	PSTR cmdline, 
	int cmdshow)
{
	const int width = 1280;
	const int height = 720;


	//register window class
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = ProcessMessage;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "GraphicsWindowClass";
	
	if (!RegisterClassEx(&wc)) {
		DWORD error = GetLastError();

		if (error != ERROR_CLASS_ALREADY_EXISTS)
			return HRESULT_FROM_WIN32(error);
	}

	RECT clientRect;
	SetRect( &clientRect, 0, 0, width, height );
	AdjustWindowRect(&clientRect, 
		WS_OVERLAPPEDWINDOW, //Has a title bar, border, min and max buttons, etc.
		false); // no menu bar

	RECT desktopRect;
	GetClientRect(GetDesktopWindow(), &desktopRect);
	int centeredX = (desktopRect.right / 2) - (clientRect.right / 2);
	int centeredY = (desktopRect.bottom / 2) - (clientRect.bottom / 2);

	HWND windowHandle = CreateWindowEx(0, wc.lpszClassName, "Window Name", WS_OVERLAPPEDWINDOW, centeredX, centeredY, width, height, 0, 0, hInst, 0);

	if (windowHandle == NULL) {
		DWORD error = GetLastError();
		return HRESULT_FROM_WIN32(error);
	}

	ShowWindow(windowHandle, SW_SHOW);
	

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

