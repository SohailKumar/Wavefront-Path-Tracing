#include <string>
#include <sstream>
#include <iomanip>
#include "window.h"
#include "timer.h"
//#include "renderer.h"

Timer timer;

// Helper function to convert const char* to std::wstring
inline std::wstring ToWide(const char* str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], len);
    // Remove the null terminator for MessageBox
    if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back();
    return wstr;
}

void Update() {
	const float t = timer.Peek();
	std::wostringstream oss{}; // Initialize oss
	oss << L"Time Elapsed: " << std::setprecision(1) << std::fixed << t << L" sec";
	Window::SetTitle(oss.str());
}

int WINAPI wWinMain(
    HINSTANCE hInst, 
    HINSTANCE hInstPrev, 
    PWSTR cmdline, 
    int cmdshow)
{
    timer = Timer();

    const int width = 1280;
    const int height = 720;

    try {
        Window::Create(hInst, width, height, L"Nami Window Name Here", false, NULL);
        // Game Loop
        while(true)
        {
            if (const auto exitCode = Window::ProcessMessages()) {
                return *exitCode;
            }
            Update();
        }
    }
    catch (std::exception& e) {
        MessageBox(nullptr, ToWide(e.what()).c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
}

