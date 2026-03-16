#pragma once

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

namespace Renderer {
	inline Microsoft::WRL::ComPtr<ID3D11Device> Device; //Used to create buffers
	inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context; // Set different resources for rendering
	inline Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain; // 
	inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView;


	void Init(HWND  winHandle);
	void Destroy();

	void FinishFrame();
	void ClearBuffer(float red, float green, float blue);
	void DrawTestTriangle();
	void PrintAllAdapterNames();
}