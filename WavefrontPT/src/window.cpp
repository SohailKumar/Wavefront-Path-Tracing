#include "window.h"
#pragma comment(linker, "/subsystem:windows")

namespace Window{
    //anonymous namespace for variables only accessible in this file
    namespace {
        unsigned int _windowWidth;
        unsigned int _windowHeight;
        HWND _winHandle;
        bool _hasFocus;
        bool _isMinimized;
    }
}
unsigned int Window::GetWidth() { return _windowWidth; }
unsigned int Window::GetHeight() { return _windowHeight; }
HWND Window::GetHandle() { return _winHandle; }
bool Window::HasFocus() { return _hasFocus; }
bool Window::IsMinimized() { return _isMinimized; }

LRESULT Window::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_DESTROY:
        //Quit our program;
        return 0;
    case WM_CLOSE:
        PostQuitMessage(69); // Posts quit message to Message Queue. Returning with code 69.
        break;
        //case WM_LBUTTONDOWN:
        //    const POINTS pt = MAKEPOINTS(lParam);
        //    std::string windowMsg = "Point: ( " + std::to_string(pt.x) + ", " + std::to_string(pt.y) + " )";

        //    SetWindowText(hWnd, windowMsg);
        //    break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

std::optional<int> Window::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return msg.wParam; // return PostQuitMessage()'s exit code
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return std::nullopt;
}

void Window::SetTitle(const std::wstring& title)
{
    SetWindowText(_winHandle, title.c_str());
}

HRESULT Window::Create(
    HINSTANCE appInstance, 
    unsigned int width, 
    unsigned int height, 
    std::wstring titleBarText, 
    bool statsInTitleBar, 
    void(*resizeCallback)())
{

    _windowWidth = width;
    _windowHeight = height;

    const wchar_t _className[] = L"Nami Window Class";

    // INITIALIZE WINDOW CLASS
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ProcessMessage;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = appInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _className;

    // REGISTER CLASS
    if (!RegisterClassEx(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS)
            return HRESULT_FROM_WIN32(error);
    }

    // SET WINDOW SIZE
    RECT clientRect;
    SetRect(&clientRect, 0, 0, width, height);
    AdjustWindowRect(&clientRect,   // adjust total window size so client size is width x height
        WS_OVERLAPPEDWINDOW,       // title bar, border, min/max buttons, etc.
        false);                     // no menu bar

    // CENTER WINDOW ON SCREEN
    RECT desktopRect;
    GetClientRect(GetDesktopWindow(), &desktopRect);
    unsigned int centeredX = (desktopRect.right / 2) - (clientRect.right / 2); // get left of window top-left
    unsigned int centeredY = (desktopRect.bottom / 2) - (clientRect.bottom / 2); // get top of window top-left


    // CREATE WINDOW
    _winHandle = CreateWindowEx(
        0,
        _className,
        titleBarText.c_str(),
        WS_OVERLAPPEDWINDOW,
        centeredX,
        centeredY,
        clientRect.right - clientRect.left, // total window width
        clientRect.bottom - clientRect.top, // total window height
        0,
        0,
        appInstance,
        0
    );

    if (_winHandle == NULL) {
        DWORD error = GetLastError();
        return HRESULT_FROM_WIN32(error);
    }

    ShowWindow(_winHandle, SW_SHOW);

    return S_OK;

}

//void Window::UpdateStats(float totalTime)
//{
//}

void Window::Quit()
{
    PostMessage(_winHandle, WM_CLOSE, 0, 0);
}
