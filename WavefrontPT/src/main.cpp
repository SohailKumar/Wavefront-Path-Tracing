#include <string>
#include <sstream>
#include <iomanip>
#include "window.h"
#include "Timer.h"
#include "GraphicsDx11.h"
#include "App.h"
#include <iostream>

Timer timer;
Timer fpsTimer;
//#define TIMER_ANALYSIS

#define WIDTH = 1280
#define HEIGHT = 7200.


void CUDAChecks() {
    cudaDeviceProp deviceProp;
    // Get properties for device 0 (the first GPU)
    cudaGetDeviceProperties(&deviceProp, 0);
    int dev = 0;
    int supportsCoopLaunch = 0;
    cudaDeviceGetAttribute(&supportsCoopLaunch, cudaDevAttrCooperativeLaunch, dev);

    std::cout << "Device name: " << deviceProp.name << std::endl;
    std::cout << "Number of multiprocessors (SMs): " << deviceProp.multiProcessorCount << std::endl;
    std::cout << "Supports coop launch: " << supportsCoopLaunch << std::endl << "DEV: " << dev << std::endl;
}

void Update() {
    // timer.UpdateWindowTitleWithTimer();
    Timer updateTimer = Timer();
    GraphicsDx11::ClearBuffer(0.1, 0.0, 0.2);
#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
    std::wcout << "\tClear Buffer: " << updateTimer.GetStringTime(true) << std::endl;
#endif

    GraphicsDx11::CUDARender();
#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
    std::wcout << "\tCUDA Render: " << updateTimer.GetStringTime(true) << std::endl;
#endif

    GraphicsDx11::FinishFrame();
#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
    std::wcout << "\tFinih Frame: " << updateTimer.GetStringTime(true) << std::endl;
#endif
}

// Helper function to convert const char* to std::wstring
inline std::wstring ToWide(const char* str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], len);
    // Remove the null terminator for MessageBox
    if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back();
    return wstr;
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInst,
    _In_ HINSTANCE hInstPrev,
    _In_ PWSTR cmdline,
    _In_ int cmdshow)
{
    timer = Timer();

#if defined(DEBUG) | defined(_DEBUG)
    Window::CreateConsoleWindow(500, 120, 32, 120);
#endif

    const int width = 1280;
    const int height = 720;

    try {
        Window::Create(hInst, width, height, L"Nami Window Name Here", false, NULL);
        GraphicsDx11::Init(Window::GetHandle());

        CUDAChecks();

        GraphicsDx11::ContinueInit();
        GraphicsDx11::InitTexturesAndRegisterWithCUDA();

        //Init Scene and Renderer
        Camera cam = Camera(float3(0, 0.5, 10), float3(0, 0.5, 0), 3.5555, 2, Window::GetWidth(), Window::GetHeight());
		Scene scene = Scene(cam);
		Renderer rendererBegin = Renderer(Window::GetWidth(), Window::GetHeight());
		
        App::CreateApp(rendererBegin, scene);
        Renderer* renderer = &App::GetRenderer();
        renderer->Initialize(App::GetScene());

        fpsTimer = Timer(); 
#if defined(DEBUG) | defined(_DEBUG)
        std::wcout << "\n\n";
#endif
        // Game Loop
        Update();

        while(true)
        {
            if (const auto exitCode = Window::ProcessMessages()) {
                GraphicsDx11::Destroy();
                return *exitCode;
            }
            //Update();
            //fpsTimer.UpdateWindowTitleWithTimer(false);
#if defined(DEBUG) | defined(_DEBUG)
            //std::wcout << fpsTimer.GetStringTime(true) << std::endl;

#endif
        }
    }
    catch (std::exception& e) {
        MessageBox(nullptr, ToWide(e.what()).c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...) {
        MessageBox(nullptr, L"idk", L"Other Exception", MB_OK | MB_ICONEXCLAMATION);
    }
}

