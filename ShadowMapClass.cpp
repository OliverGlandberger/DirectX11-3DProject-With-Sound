#include "ShadowMapClass.hpp"



ShadowMapClass::ShadowMapClass()
{
	this->VertexLayout = nullptr;
	this->VertexShader = nullptr;
	this->PixelShader = nullptr;

	this->DepthStencilBuffer = nullptr;
	this->DepthStencilView = nullptr;
	this->DepthShaderResourceView = nullptr;

	this->SamplerStateCLAMP = nullptr;

	this->ConstantBuffer = nullptr;
}

ShadowMapClass::~ShadowMapClass()
{
	if (this->VertexLayout != nullptr)
		this->VertexLayout->Release();
	this->VertexLayout = nullptr;

	if (this->VertexShader != nullptr)
		this->VertexShader->Release();
	this->VertexLayout = nullptr;

	if (this->PixelShader != nullptr)
		this->PixelShader->Release();
	this->PixelShader = nullptr;

	if (this->DepthStencilBuffer != nullptr)
		this->DepthStencilBuffer->Release();
	this->DepthStencilBuffer = nullptr;

	if (this->DepthStencilView != nullptr)
		this->DepthStencilView->Release();
	this->DepthStencilView = nullptr;

	if (this->DepthShaderResourceView != nullptr)
		this->DepthShaderResourceView->Release();
	this->DepthShaderResourceView = nullptr;

	if (this->SamplerStateCLAMP != nullptr)
		this->SamplerStateCLAMP->Release();
	this->SamplerStateCLAMP = nullptr;

	if (this->ConstantBuffer != nullptr)
		this->ConstantBuffer->Release();
	this->ConstantBuffer = nullptr;

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ShadowMapClass::InitializeAll(
	ID3D11Device*			*Device,
	ID3D11DeviceContext*	*DeviceContext,
	UINT					ScreenWidth,
	UINT					ScreenHeight,
	DirectX::XMFLOAT4		LightPosition
)
{
	this->InitializeShaders(
		Device
	);

	this->InitializeSampler(
		Device
	);

	this->InitializeBuffers(
		Device,
		ScreenWidth,
		ScreenHeight
	);
	this->InitializeConstantBuffer(
		Device,
		DeviceContext,
		LightPosition
	);
}








void ShadowMapClass::SetDepthRenderTarget(
	ID3D11DeviceContext*	*DeviceContext
)
{
	(*DeviceContext)->OMSetRenderTargets(
		0,
		NULL, // Potentially 'nullptr' instead
		this->DepthStencilView
	);

	// Clears the DepthStencilView.
	(*DeviceContext)->ClearDepthStencilView(
		this->DepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0
	);
}









void ShadowMapClass::SetShaderResourceView(
	ID3D11DeviceContext* *DeviceContext
)
{
	(*DeviceContext)->PSSetShaderResources(5, 1, &this->DepthShaderResourceView);

}










void ShadowMapClass::SetShadingContext(
	ID3D11DeviceContext* *DeviceContext
)
{
	// Set InputLayout & PrimitiveTopology
	(*DeviceContext)->IASetInputLayout(this->VertexLayout);
	(*DeviceContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// LightShader->RenderObject() is affected by a different topology.
																						// Set Vertex and Pixel shaders that will be used to render
	(*DeviceContext)->VSSetShader(this->VertexShader, nullptr, 0);
	(*DeviceContext)->HSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->DSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->GSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->PSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->PSSetSamplers(1, 1, &this->SamplerStateCLAMP);
}







void ShadowMapClass::RenderObjectShadowMap(
	ID3D11DeviceContext*		*DeviceContext,
	ID3D11Buffer*				*VertexBuffer,
	int							VerticeCount
)
{
	// Set the object's VertexBuffer to the IA.
	UINT32 VertexSize = sizeof(VertexDataClass);	// float3_pos + float2_uv + float3_normal
	UINT32 offset = 0;
	(*DeviceContext)->IASetVertexBuffers(
		0,
		1,
		VertexBuffer,
		&VertexSize,
		&offset
	);

	// Render the geometry
	(*DeviceContext)->Draw((UINT)VerticeCount, 0);
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ShadowMapClass::InitializeShaders(
	ID3D11Device* *Device
)
{
	HRESULT hr;
	//C R E A T E     V E R T E X     S H A D E R
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"ShadowMapping_Vertex.hlsl",
		nullptr,
		nullptr,
		"SHADOW_VS_main",
		"vs_5_0",
		0,
		0,
		&pVS,
		nullptr
	);
	(*Device)->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &this->VertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = (*Device)->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &this->VertexLayout);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'Vertex Shader'.", L"ERROR: 'InitialiseShaders() - DeferredShaderClass'", MB_OK);

	// we do not need anymore this COM object, so we release it.
	pVS->Release();
}






void ShadowMapClass::InitializeSampler(
	ID3D11Device* *Device
)
{
	D3D11_SAMPLER_DESC SamplerState_Description;

	SamplerState_Description.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerState_Description.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerState_Description.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerState_Description.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SamplerState_Description.MipLODBias = 0.0f;
	SamplerState_Description.MaxAnisotropy = 1.0f;
	SamplerState_Description.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamplerState_Description.BorderColor[4] = (1.0f, 1.0f, 1.0f, 1.0f);
	SamplerState_Description.MinLOD = -3.402823466e+38F;
	SamplerState_Description.MaxLOD = 3.402823466e+38F;

	HRESULT hr = (*Device)->CreateSamplerState(&SamplerState_Description, &this->SamplerStateCLAMP);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'SamplerStateCLAMP'.", L"ERROR: 'InitializeSampler() in ShadowMapClass.cpp'", MB_OK);
}






void ShadowMapClass::InitializeBuffers(
	ID3D11Device* *Device,
	UINT ScreenWidth,
	UINT ScreenHeight
)
{
	// Description for the DepthBuffer
	D3D11_TEXTURE2D_DESC DepthBuffer_Description;
	ZeroMemory(&DepthBuffer_Description, sizeof(DepthBuffer_Description));
	DepthBuffer_Description.Width = SHADOWMAP_WIDTH;
	DepthBuffer_Description.Height = SHADOWMAP_HEIGHT;
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
	HRESULT hr = (*Device)->CreateTexture2D(
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


	/*
	
	"The texture resource for the depth buffer is created using the
	R24G8_TYPELESS texture format and the depth/stencil view is created
	with the D24_UNORM_S8_UINT texture format. When accessing the
	depth buffer in the pixel shader, the shader resource view is created
	using the R24_UNORM_X8_TYPELESS texture format since the stencil value is unused."

	*/

	D3D11_SHADER_RESOURCE_VIEW_DESC ShaderResourceView_Description;
	ZeroMemory(&ShaderResourceView_Description, sizeof(ShaderResourceView_Description));
	ShaderResourceView_Description.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;		// This specific format is needesd so that.. same reason as format in depthBuffer & DepthView.
	ShaderResourceView_Description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	ShaderResourceView_Description.Texture2D.MostDetailedMip = 0;
	ShaderResourceView_Description.Texture2D.MipLevels = 1;

	// Create ShaderResourceView for the DepthBuffer
	(*Device)->CreateShaderResourceView(
		this->DepthStencilBuffer,
		&ShaderResourceView_Description,
		&this->DepthShaderResourceView
	);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to Create the ShaderResourceVIew connected to the DepthBuffer'.", L"ERROR: 'InitialiseBuffers() - DeferredBuffersClass'", MB_OK);
}










void ShadowMapClass::InitializeConstantBuffer(
	ID3D11Device*			*Device,
	ID3D11DeviceContext*	*DeviceContext,
	DirectX::XMFLOAT4		LightPositionArray
)
{
	// Initializes the new ViewMatrix
	//DirectX::XMMATRIX UnformattedViewMatrix = DirectX::XMMatrixLookToLH(
	//	DirectX::XMVectorSet(
	//		LightPositionArray.x,
	//		LightPositionArray.y,
	//		LightPositionArray.z,
	//		LightPositionArray.w
	//	),
	//	DirectX::XMVector4Normalize(DirectX::XMVectorSet(5.0f, -1.0f, -1.0f, 0.0f)),
	//	DirectX::XMVector4Normalize(DirectX::XMVectorSet(-5.0f, -26.0f, 1.0f, 0.0f))
	//);

	DirectX::XMMATRIX UnformattedViewMatrix = DirectX::XMMatrixLookToLH(
		DirectX::XMVectorSet(-10.0f, 20.0f, -10.0f, 1.0f),
		DirectX::XMVectorSet(0.5f, -0.2f, 0.5f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	);

	DirectX::XMStoreFloat4x4(
		&this->ConstantBufferData.ShadowViewMatrix,
		UnformattedViewMatrix
	);


	// Initializes the new ProjectionMatrix
	DirectX::XMMATRIX UnformattedProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
		PI*0.45f,
		(SCREEN_WIDTH * 0.2) / (SCREEN_HEIGHT * 0.2),
		NEAR_PLANE,
		FAR_PLANE * HEIGHTMAP_WIDTH_MULTIPLIER * 2
	);

	DirectX::XMStoreFloat4x4(
		&this->ConstantBufferData.ShadowProjectionMatrix,
		UnformattedProjectionMatrix
	);

	D3D11_BUFFER_DESC cbDesc;
	memset(&cbDesc, 0, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(ShadowMapConstantBuffer);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	memset(&InitData, 0, sizeof(InitData));
	InitData.pSysMem = &this->ConstantBufferData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	(*Device)->CreateBuffer(&cbDesc, &InitData, &this->ConstantBuffer);

	(*DeviceContext)->VSSetConstantBuffers(1, 1, &this->ConstantBuffer);

	(*DeviceContext)->PSSetConstantBuffers(2, 1, &this->ConstantBuffer);
}

