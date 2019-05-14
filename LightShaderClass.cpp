#include "LightShaderClass.hpp"



LightShaderClass::LightShaderClass()
{
	this->VertexShader = nullptr;
	this->PixelShader = nullptr;
	this->BackBufferRTV = nullptr;
}
LightShaderClass::~LightShaderClass()
{

}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

ID3D11RenderTargetView* *LightShaderClass::GetBackBufferRTV()
{
	return &this->BackBufferRTV;
}









void LightShaderClass::InitializeAll(ID3D11Device* *Device)
{
	this->InitializeShaders(Device);
	this->InitializeSampler(Device);
}



void LightShaderClass::Render(
	ID3D11DeviceContext*		*DeviceContext
)
{
	// Set inputlayout and vertexbuffers to null so that we can use vertexId to generate the quad
	// in the vertexshader.
	(*DeviceContext)->IASetInputLayout(nullptr);
	(*DeviceContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// SUPER IMPORTANT!
	(*DeviceContext)->IASetVertexBuffers(
		0,
		0,
		nullptr,
		nullptr,
		nullptr
	);

	(*DeviceContext)->Draw(4, 0);
}







void LightShaderClass::SetRenderTargets(
	ID3D11DeviceContext* *DeviceContext,
	ID3D11RenderTargetView* *BackBufferRTV,
	ID3D11DepthStencilView*	*DepthStencilView
)
{
	(*DeviceContext)->OMSetRenderTargets(
		1,
		BackBufferRTV,
		*DepthStencilView
	);
}





void LightShaderClass::SetPipeline(
	ID3D11DeviceContext*		*DeviceContext
)
{
	// Set Shaders
	(*DeviceContext)->VSSetShader(this->VertexShader, nullptr, 0);
	(*DeviceContext)->HSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->DSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->GSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->PSSetShader(this->PixelShader, nullptr, 0);
	// Set Sampler
	(*DeviceContext)->PSSetSamplers(0, 1, &this->SamplerStateCLAMP);
}

void LightShaderClass::ReleaseAll()
{
	this->VertexShader->Release();
	this->PixelShader->Release();
	this->SamplerStateCLAMP->Release();
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


void LightShaderClass::InitializeShaders(ID3D11Device* *Device)
{
	HRESULT hr;
	//C R E A T E     V E R T E X     S H A D E R
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Light_Vertex.hlsl",
		nullptr,
		nullptr,
		"LIGHT_VS_main",
		"vs_5_0",
		0,
		0,
		&pVS,
		nullptr
	);
	(*Device)->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &this->VertexShader);

	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	//----------------------------------------------------------------------------------------------------------------------------------
	//C R E A T E     P I X E L     S H A D E R
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Light_Pixel.hlsl",
		nullptr,
		nullptr,
		"LIGHT_PS_main",
		"ps_5_0",
		0,
		0,
		&pPS,
		nullptr
	);

	hr = (*Device)->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->PixelShader);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'Pixel Shader'.", L"ERROR: 'CreateShaders()'", MB_OK);

	// we do not need anymore this COM object, so we release it.
	pPS->Release();

}




void LightShaderClass::InitializeSampler(ID3D11Device* *Device)
{
	D3D11_SAMPLER_DESC SamplerState_Description;

	SamplerState_Description.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerState_Description.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerState_Description.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerState_Description.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerState_Description.MipLODBias = 0.0f;
	SamplerState_Description.MaxAnisotropy = 1.0f;
	SamplerState_Description.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SamplerState_Description.BorderColor[4] = (1.0f, 1.0f, 1.0f, 1.0f);
	SamplerState_Description.MinLOD = -3.402823466e+38F;
	SamplerState_Description.MaxLOD = 3.402823466e+38F;

	(*Device)->CreateSamplerState(&SamplerState_Description, &this->SamplerStateCLAMP);
}

