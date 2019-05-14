#include "DeferredBuffersClass.hpp"

DeferredBuffersClass::DeferredBuffersClass()
{
	for (int i = 0; i < BUFFER_COUNT; i++) {
		this->RenderTargetTextureArray[i] = nullptr;
		this->RenderTargetViewArray[i] = nullptr;
		this->ShaderResourceViewArray[i] = nullptr;
	}
	this->DepthStencilBuffer = nullptr;
	this->DepthStencilView = nullptr;
}
DeferredBuffersClass::~DeferredBuffersClass()
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void DeferredBuffersClass::ReleaseAll()
{
	for (int i = 0; i < BUFFER_COUNT; i++) {
		this->RenderTargetTextureArray[i]->Release();
		this->RenderTargetTextureArray[i] = nullptr;

		this->RenderTargetViewArray[i]->Release();
		this->RenderTargetViewArray[i] = nullptr;

		this->ShaderResourceViewArray[i]->Release();
		this->ShaderResourceViewArray[i] = nullptr;
	}
	this->ShaderResourceViewArray[BUFFER_COUNT]->Release();
	this->ShaderResourceViewArray[BUFFER_COUNT] = nullptr;

	this->DepthStencilBuffer->Release();
	this->DepthStencilBuffer = nullptr;

	this->DepthStencilView->Release();
	this->DepthStencilView = nullptr;
}





ID3D11ShaderResourceView* *DeferredBuffersClass::GetGBuffers()
{
	return this->ShaderResourceViewArray;
}


ID3D11Texture2D* DeferredBuffersClass::GetColorIDTexture()
{
	return this->RenderTargetTextureArray[BUFFER_COUNT - 1];
}




void DeferredBuffersClass::InitializeBuffers(
	ID3D11Device* *Device,
	UINT ScreenWidth,
	UINT ScreenHeight
)
{
	HRESULT hr;
	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//																					//
	//		RenderTargetTexture's,	 RenderTargetView's,	ShaderResourceView's		//
	//																					//
	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	// Description for all RenderTargetTexture's
	D3D11_TEXTURE2D_DESC Texture2D_Description;
	ZeroMemory(&Texture2D_Description, sizeof(Texture2D_Description));
	Texture2D_Description.Width = ScreenWidth;
	Texture2D_Description.Height = ScreenHeight;
	Texture2D_Description.MipLevels = 0;	// '0' generates a full set of subtextures.
	Texture2D_Description.ArraySize = 1;	// We're only using '1' texture.
	Texture2D_Description.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	// The datatype per pixel, here with four components.
	Texture2D_Description.SampleDesc.Count = 1;	// Multisamples per pixel.
	Texture2D_Description.SampleDesc.Quality = 0;		// Image quality level.
	Texture2D_Description.Usage = D3D11_USAGE_DEFAULT;	// Will only be read/written to by GPU.
	Texture2D_Description.BindFlags = D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;	// This texture will be used as a render target & will be used by shaders
	Texture2D_Description.CPUAccessFlags = 0;
	Texture2D_Description.MiscFlags = 0;
	
	// Description for all RenderTargetView's
	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetView_Description;
	ZeroMemory(&RenderTargetView_Description, sizeof(RenderTargetView_Description));
	RenderTargetView_Description.Format = Texture2D_Description.Format;
	RenderTargetView_Description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RenderTargetView_Description.Texture2D.MipSlice = 0;
	
	// Description for all ShaderResourceView's
	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceView_Description;
	ZeroMemory(&ShaderResourceView_Description, sizeof(ShaderResourceView_Description));
	ShaderResourceView_Description.Format = Texture2D_Description.Format;
	ShaderResourceView_Description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceView_Description.Texture2D.MostDetailedMip = 0;
	ShaderResourceView_Description.Texture2D.MipLevels = 1;

	for (int i = 0; i < BUFFER_COUNT;	 i++) {	// '-1' because the final ShaderResourceView - DepthBuffer, needs special snowflake treatment

		// Create all Texture2D's
		hr = (*Device)->CreateTexture2D(
			&Texture2D_Description,
			NULL,
			&this->RenderTargetTextureArray[i]
		);
		if (FAILED(hr))
			MessageBox(NULL, L"Failed to Create a Texture2D'.", L"ERROR: 'InitialiseBuffers() - DeferredBuffersClass'", MB_OK);

		// Create all RenderTargetView's
		hr = (*Device)->CreateRenderTargetView(
			this->RenderTargetTextureArray[i],
			&RenderTargetView_Description,
			&this->RenderTargetViewArray[i]
		);
		if (FAILED(hr))
			MessageBox(NULL, L"Failed to Create a RenderTargetView'.", L"ERROR: 'InitialiseBuffers() - DeferredBuffersClass'", MB_OK);

		// Create all ShaderResourceView's
		(*Device)->CreateShaderResourceView(
			this->RenderTargetTextureArray[i],
			&ShaderResourceView_Description,
			&this->ShaderResourceViewArray[i]
		);
		if (FAILED(hr))
			MessageBox(NULL, L"Failed to Create a ShaderResourceView'.", L"ERROR: 'InitialiseBuffers() - DeferredBuffersClass'", MB_OK);
	}



	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	//																					//
	//							DepthBuffer,	DepthStencilView						//			
	//																					//
	//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	
	// Description for the DepthBuffer
	D3D11_TEXTURE2D_DESC DepthBuffer_Description;
	ZeroMemory(&DepthBuffer_Description, sizeof(DepthBuffer_Description));
	DepthBuffer_Description.Width = ScreenWidth;
	DepthBuffer_Description.Height = ScreenHeight;
	DepthBuffer_Description.MipLevels = 1;
	DepthBuffer_Description.ArraySize = 1;
	DepthBuffer_Description.Format = DXGI_FORMAT_R24G8_TYPELESS;	// This specific format needed so that it can later be used as a shader resource.
	DepthBuffer_Description.SampleDesc.Count = 1;
	DepthBuffer_Description.SampleDesc.Quality = 0;
	DepthBuffer_Description.Usage = D3D11_USAGE_DEFAULT;
	DepthBuffer_Description.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	DepthBuffer_Description.CPUAccessFlags = 0;							// Same as CPU_READ
	DepthBuffer_Description.MiscFlags = 0;
	// Creation of the DepthBuffer
	hr = (*Device)->CreateTexture2D(
		&DepthBuffer_Description,
		NULL,
		&this->DepthStencilBuffer
	);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to Create a DepthBuffer.", L"ERROR: 'InitialiseBuffers() - DeferredBuffersClass'", MB_OK);
	
	
	// Description for the DepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilView_Description;
	ZeroMemory(&DepthStencilView_Description, sizeof(DepthStencilView_Description));
	DepthStencilView_Description.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// Needed so that it can work with the format specified above, and for the same reasons.
	DepthStencilView_Description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilView_Description.Texture2D.MipSlice = 0;

	// Creation of the DepthStencilView
	hr = (*Device)->CreateDepthStencilView(
		this->DepthStencilBuffer,
		&DepthStencilView_Description,
		&this->DepthStencilView
	);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to Create a DepthStencilView'.", L"ERROR: 'InitialiseBuffers() - DeferredBuffersClass'", MB_OK);

	// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ Everything related to the last ShaderResourceView-Index, The Depth Buffer. +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	/* Necessary formats so that Depth can be used as a GBuffer:

	"The texture resource for the depth buffer is created using the
	R24G8_TYPELESS texture format and the depth/stencil view is created
	with the D24_UNORM_S8_UINT texture format. When accessing the
	depth buffer in the pixel shader, the shader resource view is created
	using the R24_UNORM_X8_TYPELESS texture format since the stencil value is unused."

	*/

	ZeroMemory(&ShaderResourceView_Description, sizeof(ShaderResourceView_Description));
	ShaderResourceView_Description.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;		// This specific format is needesd so that.. same reason as format in depthBuffer & DepthView.
	ShaderResourceView_Description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceView_Description.Texture2D.MostDetailedMip = 0;
	ShaderResourceView_Description.Texture2D.MipLevels = 1;

	// Create ShaderResourceView for the DepthBuffer
	(*Device)->CreateShaderResourceView(
		this->DepthStencilBuffer,
		&ShaderResourceView_Description,
		&this->ShaderResourceViewArray[BUFFER_COUNT]	// If we have for.ex. 2 buffers (not including depth), their index will 
	);													// be 0,1 and index 2 will be for the depth.
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to Create the ShaderResourceVIew connected to the DepthBuffer'.", L"ERROR: 'InitialiseBuffers() - DeferredBuffersClass'", MB_OK);
}





void DeferredBuffersClass::SetAllRenderTargets(ID3D11DeviceContext* *DeviceContext)
{
	(*DeviceContext)->OMSetRenderTargets(
		BUFFER_COUNT,
		this->RenderTargetViewArray,
		this->DepthStencilView
	);
}





void DeferredBuffersClass::ClearAllRenderTargets(
	ID3D11DeviceContext*	*DeviceContext,
	float					ClearColor_Red,
	float					ClearColor_Blue,
	float					ClearColor_Green,
	float					ClearColor_Alpha
)
{
	/*
	Clears all RenderTarget's.
	Clears the DepthStencilView.
	*/

	// Sets up the background color
	float colour[4];
	colour[0] = ClearColor_Red;
	colour[1] = ClearColor_Blue;
	colour[2] = ClearColor_Green;
	colour[3] = ClearColor_Alpha;

	// Clears all RenderTarget's
	for (int i = 0; i < BUFFER_COUNT; i++) {
		(*DeviceContext)->ClearRenderTargetView(this->RenderTargetViewArray[i], colour);
	}

	// Clears the DepthStencilView.
	(*DeviceContext)->ClearDepthStencilView(
		this->DepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
