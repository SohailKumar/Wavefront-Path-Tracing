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
        bool consoleCreated;
    }
}
unsigned int Window::GetWidth() { return _windowWidth; }
unsigned int Window::GetHeight() { return _windowHeight; }
HWND Window::GetHandle() { return _winHandle; }
bool Window::HasFocus() { return _hasFocus; }
bool Window::IsMinimized() { return _isMinimized; }

void Window::Update(int frameCount) {
    {
        GraphicsDx11::ClearBuffer(0.1, 0.0, 0.2);
#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
        std::wcout << "\tClear Buffer: " << updateTimer.GetStringTime(true) << std::endl;
#endif

        GraphicsDx11::CUDARender(frameCount);
#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
        std::wcout << "\tCUDA Render: " << updateTimer.GetStringTime(true) << std::endl;
#endif

        GraphicsDx11::FinishFrame();
#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
        std::wcout << "\tFinih Frame: " << updateTimer.GetStringTime(true) << std::endl;
#endif
    }
}

LRESULT Window::ProcessMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_DESTROY:
        //Quit our program;
        return 0;
    case WM_CLOSE:
        PostQuitMessage(69); // Posts quit message to Message Queue. Returning with code 69.
        break;
    case WM_LBUTTONDOWN:
        //const POINTS pt = MAKEPOINTS(lParam);
        //std::wstring windowMsg = L"Point: ( " + std::to_wstring(pt.x) + L", " + std::to_wstring(pt.y) + L" )";
        //SetWindowText(hWnd, windowMsg.c_str());

        for (int i = 0; i < 10; ++i) {
            Update(++GraphicsDx11::frameCount);
        }

        SetWindowText(hWnd, std::to_wstring(GraphicsDx11::frameCount).c_str());

        break;
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

// --------------------------------------------------------
// Allocates a console window we can print to for debugging
// 
// bufferLines   - Number of lines in the overall console buffer
// bufferColumns - Numbers of columns in the overall console buffer
// windowLines   - Number of lines visible at once in the window
// windowColumns - Number of columns visible at once in the window
// --------------------------------------------------------
void Window::CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns)
{
    // Only allow this once
    if (consoleCreated)
        return;

    // Our temp console info struct
    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    // Get the console info and set the number of lines
    AllocConsole();
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = bufferLines;
    coninfo.dwSize.X = bufferColumns;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    SMALL_RECT rect = {};
    rect.Left = 0;
    rect.Top = 0;
    rect.Right = windowColumns;
    rect.Bottom = windowLines;
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

    FILE* stream;
    freopen_s(&stream, "CONIN$", "r", stdin);
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);

    // Prevent accidental console window close
    HWND consoleHandle = GetConsoleWindow();
    HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
    EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);

    // Get the current console mode and append options that allow colored output
    DWORD currentMode = 0;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &currentMode);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),
        currentMode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    consoleCreated = true;
}

void Window::Quit()
{
    PostMessage(_winHandle, WM_CLOSE, 0, 0);
}
