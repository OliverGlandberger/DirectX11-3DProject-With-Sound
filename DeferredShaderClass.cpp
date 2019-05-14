#include "DeferredShaderClass.hpp"


DeferredShaderClass::DeferredShaderClass()
{
	this->VertexLayout = nullptr;
	this->VertexShader = nullptr;
	this->GeometryShader = nullptr;
	this->PixelShader = nullptr;
	this->SamplerStateWrap = nullptr;
}
DeferredShaderClass::~DeferredShaderClass()
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void DeferredShaderClass::ReleaseAll()
{
	this->VertexLayout->Release();
	this->VertexShader->Release();
	this->GeometryShader->Release();
	this->PixelShader->Release();
	this->SamplerStateWrap->Release();
}





void DeferredShaderClass::InitialiseShaders(ID3D11Device* *Device)
{
	HRESULT hr;

	//----------------------------------- Vertex Shader
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Def_Vertex.hlsl",
		nullptr,
		nullptr,
		"DEF_VS_main",
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
	pVS->Release();


	//----------------------------------- Geometry Shader
	ID3DBlob* pGS = nullptr;
	ID3DBlob* error_GS = nullptr;
	D3DCompileFromFile(
		L"Def_Geometry.hlsl",
		nullptr,
		nullptr,
		"DEF_GS_main",
		"gs_5_0",
		0,
		0,
		&pGS,
		&error_GS
	);

	if (error_GS)
	{
		OutputDebugStringA((char*)error_GS->GetBufferPointer());
		error_GS->Release();
	}

	hr = (*Device)->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(),
		nullptr, &this->GeometryShader);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'Geometry Shader'.", L"ERROR: 'CreateShaders()'", MB_OK);

	//We no longer need this COM object and therefore release it
	pGS->Release();
	
	//----------------------------------- Pixel Shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Def_Pixel.hlsl",
		nullptr,
		nullptr,
		"DEF_PS_main",
		"ps_5_0",
		0,
		0,
		&pPS,
		nullptr
	);
	hr = (*Device)->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->PixelShader);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'Pixel Shader'.", L"ERROR: InitialiseShaders() - DeferredShaderClass", MB_OK);
	pPS->Release();


	//----------------------------------- SamplerState
	D3D11_SAMPLER_DESC Sampler_Description;
	Sampler_Description.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// Linear interpolation for Minification,Magnification,Mip-level-Sampling.
	Sampler_Description.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	// Keep repeating the texture
	Sampler_Description.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	// in case of values not between
	Sampler_Description.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;	// 0-1.
	Sampler_Description.MipLODBias = 0.0f;	// Offset from the calculated MipMap level.
	Sampler_Description.MaxAnisotropy = 1;	// Only relevant if .Filter = ANISOTROPIC|COMPARISON_ANISOTROPIC
	Sampler_Description.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// Comparison will always pass; Difference between source- & destination-data is irrelevant. 
	Sampler_Description.BorderColor[0] = 0;	// BorderColour to be used 
	Sampler_Description.BorderColor[1] = 0;	// if .AdressX is set to 
	Sampler_Description.BorderColor[2] = 0;	// TEXTURE_ADRESS_BORDER
	Sampler_Description.BorderColor[3] = 0;	//
	Sampler_Description.MinLOD = 0;					// Lower end of the mipmap range to clamp access to. (0 is the largest and most detailed mipmaplevel and higher levels are smaller and less detailed)
	Sampler_Description.MaxLOD = D3D11_FLOAT32_MAX;	// Upper end of the mipmap range to clamp access to. (No upper limit = D3D11_FLOAT32_MAX

	hr = (*Device)->CreateSamplerState(&Sampler_Description, &this->SamplerStateWrap);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create SamplerState.", L"ERROR: 'InitialiseShaders() - DeferredShaderClass'", MB_OK);
}





void DeferredShaderClass::SetShadingContext(ID3D11DeviceContext* *DeviceContext)
{
	// Set InputLayout & PrimitiveTopology
	(*DeviceContext)->IASetInputLayout(this->VertexLayout);
	(*DeviceContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// LightShader->RenderObject() is affected by a different topology.

	// Set Vertex and Pixel shaders that will be used to render
	(*DeviceContext)->VSSetShader(this->VertexShader, nullptr, 0);
	(*DeviceContext)->HSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->DSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->GSSetShader(this->GeometryShader, nullptr, 0);
	(*DeviceContext)->PSSetShader(this->PixelShader, nullptr, 0);

	// Set sampler states in the pixel shader
	(*DeviceContext)->PSSetSamplers(0, 1, &this->SamplerStateWrap);
}





void DeferredShaderClass::RenderObject(
	ID3D11DeviceContext*		*DeviceContext,
	ID3D11Buffer*				*VertexBuffer,
	ID3D11ShaderResourceView*	*TextureResourceView,
	int							VerticeCount
)
{
	// Set the texture related to that object index.
	(*DeviceContext)->PSSetShaderResources(0, 1, TextureResourceView);

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