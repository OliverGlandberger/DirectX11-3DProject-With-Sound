#include "Direct3D.hpp"

Direct3DContext::Direct3DContext()
{
	this->SwapChain = nullptr;
	this->Device = nullptr;
	this->DeviceContext = nullptr;
}
Direct3DContext::~Direct3DContext()
{

}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Direct3DContext::ReleaseAll()
{
	this->SwapChain->Release();
	this->Device->Release();
	this->DeviceContext->Release();
}





ID3D11Device* *Direct3DContext::GetDevice()
{
	return &this->Device;
}

ID3D11DeviceContext* *Direct3DContext::GetDeviceContext()
{
	return &this->DeviceContext;
}

IDXGISwapChain* *Direct3DContext::GetSwapChain()
{
	return &this->SwapChain;
}





void Direct3DContext::Initialize(HWND	wndHandle)
{
	this->CreateDirect3DContext(
		wndHandle
	);
	
	this->InitializeRasterizer();
	this->SetRasterizer();
	
	this->InitializeViewports();
	this->SetStandardViewport();
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void Direct3DContext::CreateDirect3DContext(
	HWND	wndHandle
)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&this->SwapChain,
		&this->Device,
		NULL,
		&this->DeviceContext
	);
}




void Direct3DContext::InitializeRasterizer()
{
	//this->RasterizerDescription.FillMode = D3D11_FILL_WIREFRAME;	// Will not work due to rendering passes
	this->RasterizerDescription.FillMode = D3D11_FILL_SOLID;		// Cool setting to mess with
	this->RasterizerDescription.CullMode = D3D11_CULL_NONE;
	//this->RasterizerDescription.CullMode = D3D11_CULL_BACK;
	this->RasterizerDescription.FrontCounterClockwise = FALSE;		// Good to know that this setting exists
	this->RasterizerDescription.DepthBias = 0;
	this->RasterizerDescription.DepthBiasClamp = 0.0f;
	this->RasterizerDescription.SlopeScaledDepthBias = 0.0f;
	this->RasterizerDescription.DepthClipEnable = TRUE;
	this->RasterizerDescription.MultisampleEnable = FALSE;			// Another setting to try
	this->RasterizerDescription.AntialiasedLineEnable = FALSE;		// Requires MultisampleEnabled to be FALSE
}





void Direct3DContext::InitializeViewports()
{
	// Standard Viewport
	this->Viewport.Width = (float)SCREEN_WIDTH;
	this->Viewport.Height = (float)SCREEN_HEIGHT;
	this->Viewport.MinDepth = 0.0f;
	this->Viewport.MaxDepth = 1.0f;
	this->Viewport.TopLeftX = 0;
	this->Viewport.TopLeftY = 0;

	// Viewport Used While Creating The ShadowMap
	this->ShadowMapViewPort.Width = (float)SHADOWMAP_WIDTH;
	this->ShadowMapViewPort.Height = (float)SHADOWMAP_HEIGHT;
	this->ShadowMapViewPort.MinDepth = 0.0f;
	this->ShadowMapViewPort.MaxDepth = 1.0f;
	this->ShadowMapViewPort.TopLeftX = 0;
	this->ShadowMapViewPort.TopLeftY = 0;

}




void Direct3DContext::SetRasterizer()
{
	this->Device->CreateRasterizerState(&this->RasterizerDescription, &this->RasterizerState);
	this->DeviceContext->RSSetState(this->RasterizerState);
}





void Direct3DContext::SetStandardViewport()
{
	this->DeviceContext->RSSetViewports(1, &this->Viewport);
}

void Direct3DContext::SetShadowMapViewport()
{
	this->DeviceContext->RSSetViewports(1, &this->ShadowMapViewPort);
}