#include "GraphicsDx11.h"
#include <exception>
#include <iterator> //for std::size
#include <sstream>
#include "Window.h"
#include "Camera.h"
#include <vector_types.h>
// #include "Renderer.h"
#include "App.h"
#include "Timer.h"
#include <iostream>

//
// Vertex and Pixel shaders here : VS() & PS()
//
static const char g_simpleShaders[] = "cbuffer cbuf \n"
"{ \n"
"  float4 g_vQuadRect; \n"
"} \n"
"Texture2D g_Texture2D; \n"
"\n"
"SamplerState samLinear{ \n"
"    Filter = MIN_MAG_LINEAR_MIP_POINT; \n"
"};\n"
"\n"
"struct Fragment{ \n"
"    float4 Pos : SV_POSITION;\n"
"    float3 Tex : TEXCOORD0; };\n"
"\n"
"Fragment VS( uint vertexId : SV_VertexID )\n"
"{\n"
"    Fragment f;\n"
"    f.Tex = float3( 0.f, 0.f, 0.f); \n"
"    if (vertexId == 1) f.Tex.x = 1.f; \n"
"    else if (vertexId == 2) f.Tex.y = 1.f; \n"
"    else if (vertexId == 3) f.Tex.xy = float2(1.f, 1.f); \n"
"    \n"
"    f.Pos = float4( g_vQuadRect.xy + f.Tex * g_vQuadRect.zw, 0, 1);\n"
"    \n"
"    return f;\n"
"}\n"
"\n"
"float4 PS( Fragment f ) : SV_Target\n"
"{\n"
"    return g_Texture2D.Sample( samLinear, f.Tex.xy ); "
"\n"
"}\n"
"\n";

struct ConstantBuffer
{
	float vQuadRect[4];
	int   UseCase;
};

Microsoft::WRL::ComPtr<ID3D11Buffer> g_pConstantBuffer;
Microsoft::WRL::ComPtr<ID3D11SamplerState> g_pSamplerState;

//extern "C"
//{
//	bool cuda_texture_2d(void* surface, size_t width, size_t height, size_t pitch, float t);
//	bool cuda_SendRays(void* surface, size_t width, size_t height, size_t pitch);
//}

void GraphicsDx11::Init(HWND winHandle) {
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

	//==============UNCOMMENT TO CHECK ADAPTER==============
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
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (LPVOID*)(backBuffer.GetAddressOf()));
	if (FAILED(hr)) {throw std::exception("GetBuffer for backBuffer Failed");}

	Device->CreateRenderTargetView(backBuffer.Get(), NULL, RTView.GetAddressOf());

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
}

void GraphicsDx11::ContinueInit() {
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> pErrorMsgs;
	HRESULT hr = {};
	//VERTEX SHADER
	{
		hr = D3DCompile(g_simpleShaders,
			strlen(g_simpleShaders),
			"Memory",
			NULL,
			NULL, //TIP: if shader has an include, change this!
			"VS",
			"vs_4_0",
			0 /*Flags1*/,
			0 /*Flags2*/,
			&shaderBlob,
			&pErrorMsgs);

		if (FAILED(hr)) {
			const char* pStr = (const char*)pErrorMsgs->GetBufferPointer();
			throw std::exception(pStr);
		}

		hr = Device->CreateVertexShader(
			shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &VertexShader);
		if (FAILED(hr)) {
			throw std::exception("CreateVertexShader Failed");
		}
		// Let's bind it now : no other vtx shader will replace it...
		Context->VSSetShader(VertexShader.Get(), NULL, 0);
		// hr = g_pd3dDevice->CreateInputLayout(...pShader used for signature...) No
		// need
	}

	// PIXEL SHADER
	{
		hr = D3DCompile(g_simpleShaders,
			strlen(g_simpleShaders),
			"Memory",
			NULL,
			NULL, //TIP: if shader has an include, chang ethis!
			"PS",
			"ps_4_0",
			0 /*Flags1*/,
			0 /*Flags2*/,
			&shaderBlob,
			&pErrorMsgs);

		if (FAILED(hr)) {
			const char* pStr = (const char*)pErrorMsgs->GetBufferPointer();
			throw std::exception(pStr);
		}

		hr = Device->CreatePixelShader(
			shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &PixelShader);
		if (FAILED(hr)) {
			throw std::exception("CreatePixelShader Failed");
		}
		// Let's bind it now : no other pix shader will replace it...
		Context->PSSetShader(PixelShader.Get(), NULL, 0);
	}

	// Create the constant buffer that both the VS and PS use? 
	{
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;  //TIP: https://learn.microsoft.com/en-us/windows/win32/direct3d11/how-to--use-dynamic-resources
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // D3D11_BIND_SHADER_RESOURCE;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.ByteWidth = 16 * ((sizeof(ConstantBuffer) + 16) / 16);
		// cbDesc.StructureByteStride = 0;
		hr = Device->CreateBuffer(&cbDesc, NULL, &g_pConstantBuffer);
		if (FAILED(hr)) {
			throw std::exception("Create Constant Buffer Failed");
		}
		// Assign the buffer now : nothing in the code will interfere with this
		// (very simple sample)
		Context->VSSetConstantBuffers(0, 1, g_pConstantBuffer.GetAddressOf());
		Context->PSSetConstantBuffers(0, 1, g_pConstantBuffer.GetAddressOf());
	}

	// SAMPLER STATE
	{
		D3D11_SAMPLER_DESC sDesc;
		sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sDesc.MinLOD = 0;
		sDesc.MaxLOD = 8;
		sDesc.MipLODBias = 0;
		sDesc.MaxAnisotropy = 1;
		hr = Device->CreateSamplerState(&sDesc, &g_pSamplerState);
		if (FAILED(hr)) {
			throw std::exception("Create Sampler State Failed");
		}

		Context->PSSetSamplers(0, 1, &g_pSamplerState);
	}

	// Setup no Input Layout
	Context->IASetInputLayout(0);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_FRONT;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = false;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	Device->CreateRasterizerState(&rasterizerState, &g_pRasterState);
	Context->RSSetState(g_pRasterState.Get());
}

void GraphicsDx11::InitTexturesAndRegisterWithCUDA() {
	Texture2D.width = 1280;
	Texture2D.height = 720;

	D3D11_TEXTURE2D_DESC texture2DDesc = {};
	texture2DDesc.Width = Texture2D.width;
	texture2DDesc.Height = Texture2D.height;
	texture2DDesc.MipLevels = 1;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(Device->CreateTexture2D(&texture2DDesc, NULL, &Texture2D.pTexture))) {
		throw std::exception("Creating Texture Failed");
	}

	if (FAILED(Device->CreateShaderResourceView((Texture2D.pTexture.Get()), NULL, &Texture2D.pSRView))) {
		throw std::exception("Creating SRV Failed");
	}
	Context->PSSetShaderResources(0, 1, Texture2D.pSRView.GetAddressOf());

	//Registering D3D11 Texture with CUDA
	cudaError_t ce = cudaGraphicsD3D11RegisterResource(
		&(Texture2D.cudaResource), Texture2D.pTexture.Get(), cudaGraphicsRegisterFlagsNone);

	if (ce != cudaSuccess) { throw std::exception("CUDA D3D11 Register Resource registration failed"); }

	//Allocate memory in GPU for the texture
	ce = cudaMallocPitch(&Texture2D.cudaLinearMemory,
		&(Texture2D.pitch),
		Texture2D.width * sizeof(float) * 4,
		Texture2D.height);
	if (ce != cudaSuccess) {throw std::exception("CUDA Malloc Pitch failed"); }

	// Fill allocated memory with 1s
	ce = cudaMemset(Texture2D.cudaLinearMemory, 1, Texture2D.pitch * Texture2D.height);
	if (ce != cudaSuccess) { throw std::exception("CUDA Memset failed");}
}

void GraphicsDx11::DrawSceneTexture2D() {
	//GraphicsDx11::ClearBuffer(0.1, 0.1f, 0.2f);
	float quadRect[4] = { -1.0f, -1.0f, 2.0f, 2.0f };

	HRESULT                  hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBuffer* pcb;
	hr = Context->Map(g_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {throw std::exception("Context->Map Failed");}
	pcb = (ConstantBuffer*)mappedResource.pData;
	{
		memcpy(pcb->vQuadRect, quadRect, sizeof(float) * 4);
		pcb->UseCase = 0;
	}
	Context->Unmap(g_pConstantBuffer.Get(), 0);
	Context->Draw(4, 0);
}

void GraphicsDx11::CUDARender() 
{
	Timer timer = Timer();

	// Map Resources so D3D11 can't access them
	cudaStream_t stream = 0;
	const int nbResources = 1;
	cudaGraphicsResource* ppResources[nbResources] = {
		Texture2D.cudaResource
	};
	cudaError_t ce = cudaGraphicsMapResources(nbResources, ppResources, stream);
	if (ce != cudaSuccess) { throw std::exception("CUDAGraphicsMapResources failed"); }

	cudaArray* cuArray;
	ce = cudaGraphicsSubResourceGetMappedArray(&cuArray, Texture2D.cudaResource, 0, 0);
	if (ce != cudaSuccess) { throw std::exception("cudaGraphicsSubResourceGetMappedArray failed"); }

#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
	std::wcout << "\t\tMapped Resources = " << timer.GetStringTime(true) << std::endl;
#endif

	// Run relevant kernels
	{
		static float t = 0.0f;
		cudaError_t err = cudaSuccess;

		Renderer* renderer = &App::GetRenderer();

		//////////////////////////////////////////


		renderer->Initialize(App::GetScene());

		renderer->GenerateCameraRays(App::GetCamera().camDetails);
		renderer->IntersectionKernel(App::GetScene().sphereRadii, App::GetScene().sphereCenters, App::GetScene().sphereCount);
		renderer->LogicKernel();
		renderer->RunMaterialShaders();
		
		renderer->PostProcess(Texture2D.cudaLinearMemory, Texture2D.pitch);

		//App::GetRenderer().InitializeRays(Texture2D.cudaLinearMemory, Texture2D.pitch, App::GetCamera().camDetails, t);
		//App::GetRenderer().TextureTest(Texture2D.cudaLinearMemory, Texture2D.width, Texture2D.height, Texture2D.pitch);

		//////////////////////////////////////////

		err = cudaGetLastError();
		if (err != cudaSuccess) { throw std::exception("Kernel launch error: %s\n"); }

		err = cudaDeviceSynchronize();
		if (err != cudaSuccess) { throw std::exception("Synchronization error: %s\n");	}
		
		t +=0.00f;
	}

#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
	std::wcout << "\t\tRan Renderer = " << timer.GetStringTime(true) << std::endl;
#endif

	// Copy cudaLinearMemory to the D3D texture using its mapped form : cudaArray
	ce = cudaMemcpy2DToArray(cuArray, // dst array
		0,
		0, // offset
		Texture2D.cudaLinearMemory,
		Texture2D.pitch, // src
		Texture2D.width * 4 * sizeof(float),
		Texture2D.height,       // extent
		cudaMemcpyDeviceToDevice); // kind
	if (ce != cudaSuccess) { throw std::exception("cudaMemcpy2DToArray failed"); }

#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
	std::wcout << "\t\tCopied memory to texture = " << timer.GetStringTime(true) << std::endl;
#endif

	cudaGraphicsUnmapResources(nbResources, ppResources, stream);

#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
	std::wcout << "\t\tUnmapped resources = " << timer.GetStringTime(true) << std::endl;
#endif

	DrawSceneTexture2D();

#if (defined(DEBUG) | defined(_DEBUG)) && defined(TIMER_ANALYSIS)
	std::wcout << "\t\tDrew Scene Texture = " << timer.GetStringTime(true) << std::endl;
#endif
}

void GraphicsDx11::ClearBuffer(float red, float green, float blue) {
	const float color[] = { red, green, blue, 1.0f };
	Context.Get()->ClearRenderTargetView(RTView.Get(), color);
}

void GraphicsDx11::DrawTestTriangle() {

	//Create Vertex Buffer
	struct Vertex {
		float x;
		float y;
	};

	//const Vertex vertices[] =
	//{
	//	{	 0.0f,	 0.5f	},
	//	{	 0.5f,	-0.5f	},
	//	{	-0.5f,	-0.5f	},
	//};

	const Vertex vertices[] =
	{
		{	-1.0f,	-1.0f	},
		{	-1.0f,	1.0f	},
		{	1.0f,	-1.0f	},
		//{	1.0f,	1.0f	}
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
	//Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader;
	hr = D3DReadFileToBlob(L"PixelShader.cso", &Blob);
	if (FAILED(hr)) { throw std::exception("D3DReadFileToBlob PS Failed"); }

	hr = Device->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, &PixelShader);
	if (FAILED(hr)) { throw std::exception("Create Pixel Shader Failed"); }
	// Bind PS
	Context->PSSetShader(PixelShader.Get(), NULL, 0);

	//Create Vertex Shader
	//Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader;
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

	// Set primitive topology to triangle list
	Context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Context->Draw(std::size(vertices), 0u);
}


void GraphicsDx11::PrintAllAdapterNames()
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

void GraphicsDx11::Destroy() {

}

void GraphicsDx11::FinishFrame()
{
	SwapChain->Present(1, 0);
}
