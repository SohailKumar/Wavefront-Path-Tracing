#include "Renderer.h"
#include <exception>

void Renderer::Init(HWND winHandle) {
	DXGI_SWAP_CHAIN_DESC sd = {};

	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = winHandle;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //TODO: Look into Preset1() and DXGI Flip model: https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-flip-model
	sd.Flags = 0;

	HRESULT hr = S_OK;
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&sd,
		SwapChain.GetAddressOf(),
		(ID3D11Device**)Device.GetAddressOf(),
		NULL,
		(ID3D11DeviceContext**)Context.GetAddressOf()
	);
	if (FAILED(hr)) {
		{
			throw std::exception("D3D11CreateDeviceAndSwapChain Failed");
		} 
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr)) {
		throw std::exception("GetBuffer for backBuffer Failed");
	}

	Device->CreateRenderTargetView(backBuffer.Get(), NULL, RTView.GetAddressOf());

}

void Renderer::ClearBuffer(float red, float green, float blue) {
	const float color[] = { red, green, blue, 1.0f };
	Context.Get()->ClearRenderTargetView(RTView.Get(), color);
}

void Renderer::Destroy() {

}

void Renderer::FinishFrame()
{
	SwapChain->Present(1, 0);
}
