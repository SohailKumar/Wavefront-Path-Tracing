#include <string>
#include <sstream>
#include <iomanip>
#include "window.h"
#include "Timer.h"
#include "Renderer.h"
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

    Renderer::FinishFrame();
    Renderer::ClearBuffer(1, 0, 0);
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInst,
    _In_ HINSTANCE hInstPrev,
    _In_ PWSTR cmdline,
    _In_ int cmdshow)
{
    timer = Timer();

    const int width = 1280;
    const int height = 720;

    try {
        Window::Create(hInst, width, height, L"Nami Window Name Here", false, NULL);
        Renderer::Init(Window::GetHandle());

        // Game Loop
        while(true)
        {
            if (const auto exitCode = Window::ProcessMessages()) {
                Renderer::Destroy();
                return *exitCode;
            }
            Update();
        }
    }
    catch (std::exception& e) {
        MessageBox(nullptr, ToWide(e.what()).c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...) {
        MessageBox(nullptr, L"idk", L"Other Exception", MB_OK | MB_ICONEXCLAMATION);
    }
}

