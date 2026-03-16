#include "Renderer.h"
#include <exception>
#include <iterator> //for std::size
#include <sstream>
#include <cuda_runtime.h>

void Renderer::Init(HWND winHandle) {
	//Fill Swap Chain Descriptor
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	//sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = winHandle;
	sd.Windowed = TRUE;
	//sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //TODO: Look into Preset1() and DXGI Flip model: https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-flip-model
	//sd.Flags = 0;

	UINT deviceFlags = 0;
#if defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = S_OK;
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		deviceFlags,
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

	//==============UNCOMMENT TO CHECK DEVICE==============
	//Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
	//if (SUCCEEDED(Device.As(&dxgiDevice))) {
	//	// Get the adapter for this device
	//	Microsoft::WRL::ComPtr<IDXGIAdapter> activeAdapter;
	//	if (SUCCEEDED(dxgiDevice->GetAdapter(&activeAdapter))) {
	//		DXGI_ADAPTER_DESC adapterDesc;
	//		activeAdapter->GetDesc(&adapterDesc);
	//		std::wstringstream ss{};
	//		ss << L": " << adapterDesc.Description << L"\n";
	//		MessageBoxW(NULL, ss.str().c_str(), L"AdapterCurr", MB_OK | MB_ICONINFORMATION);
	//	}
	//}

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

void Renderer::DrawTestTriangle() {

	//Create Vertex Buffer
	struct Vertex {
		float x;
		float y;
	};

	const Vertex vertices[] =
	{
		{	-1.0f,	-1.0f	},
		{	-1.0f,	1.0f	},
		{	1.0f,	-1.0f	},
		{	1.0f,	1.0f	}
	};

	//Populate Vertex Buffer Description
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA subRData = {};
	subRData.pSysMem = vertices;

	HRESULT hr = Device->CreateBuffer(&bufferDesc, &subRData, &VertexBuffer);
	if (FAILED(hr)) { throw std::exception("Create Buffer Failed"); }

	//Set Vertex Buffer
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	Context->IASetVertexBuffers(0, 1, VertexBuffer.GetAddressOf(), &stride, &offset);

	Microsoft::WRL::ComPtr<ID3DBlob> Blob;

	// Create Pixel Shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
	hr = D3DReadFileToBlob(L"PixelShader.cso", &Blob);
	if (FAILED(hr)) { throw std::exception("D3DReadFileToBlob PS Failed"); }

	hr = Device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &PixelShader);
	if (FAILED(hr)) { throw std::exception("Create Pixel Shader Failed"); }
	// Bind PS
	Context->PSSetShader(PixelShader.Get(), NULL, 0);

	//Create Vertex Shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
	hr = D3DReadFileToBlob(L"VertexShader.cso", &Blob);
	if (FAILED(hr)) { throw std::exception("D3DReadFileToBlob VS Failed"); }

	hr = Device->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &VertexShader);
	if (FAILED(hr)) { throw std::exception("Create Vertex Shader Failed"); }
	// Bind VS
	Context->VSSetShader(VertexShader.Get(), NULL, 0);

	//Input Layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  InputLayout;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = Device->CreateInputLayout(inputElementDesc, std::size(inputElementDesc), Blob->GetBufferPointer(), Blob->GetBufferSize(), &InputLayout);
	if (FAILED(hr)) { throw std::exception("Create Input Layout Failed"); }
	Context->IASetInputLayout(InputLayout.Get());

	// Bind RT
	Context->OMSetRenderTargets(1u, RTView.GetAddressOf(), NULL);

	//Configure Viewport
	D3D11_VIEWPORT vp;
	vp.Width = 1270;
	vp.Height = 710;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 5;
	vp.TopLeftY = 5;
	Context->RSSetViewports(1u, &vp);

	// Set primitive topology to triangle list
	Context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Context->Draw(std::size(vertices), 0u);
}

void Renderer::PrintAllAdapterNames()
{
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	CreateDXGIFactory(IID_PPV_ARGS(&factory));

	std::wstringstream ss{};
	ss << L"Found the following adapters:\n\n";

	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		ss << i << L": " << desc.Description << L"\n";
	}

	MessageBoxW(NULL, ss.str().c_str(), L"Adapter List", MB_OK | MB_ICONINFORMATION);
}

void Renderer::Destroy() {

}

void Renderer::FinishFrame()
{
	SwapChain->Present(1, 0);
}
