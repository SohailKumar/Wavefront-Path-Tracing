#pragma once

#include <d3d11.h>
#include <Windows.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <cuda_d3d11_interop.h>
#include <cuda_runtime_api.h>

namespace Renderer {
	inline Microsoft::WRL::ComPtr<ID3D11Device> Device; //Used to create buffers
	inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context; // Set different resources for rendering
	inline Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain; // 
	inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView;
	inline Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	inline Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
	inline Microsoft::WRL::ComPtr<ID3D11RasterizerState> g_pRasterState = NULL;

	// Data structure for 2D texture shared between DX11 and CUDA
	struct
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRView;
		cudaGraphicsResource* cudaResource;
		void* cudaLinearMemory;
		size_t                    pitch;
		int                       width;
		int                       height;
	} Texture2D;

	void Init(HWND  winHandle);
	void ContinueInit();
	void Destroy();

	void FinishFrame();
	void ClearBuffer(float red, float green, float blue);
	void DrawTestTriangle();

	void PrintAllAdapterNames();
	void InitTextures();
	void CUDASetupStuff();
	void CUDARender();
	void DrawSceneTexture2D();
};