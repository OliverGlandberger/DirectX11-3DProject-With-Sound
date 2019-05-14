#ifndef SHADOWMAPCLASS_HPP
#define SHADOWMAPCLASS_HPP

//#include <d3d11.h>
//#include <d3dcompiler.h>
//#include <DirectXMath.h>

#include "ConstantBufferClass.hpp"

struct ShadowMapConstantBuffer
{
	DirectX::XMFLOAT4X4 ShadowViewMatrix;
	DirectX::XMFLOAT4X4 ShadowProjectionMatrix;
};

class ShadowMapClass
{
private:
	ID3D11InputLayout*			VertexLayout;
	ID3D11VertexShader*			VertexShader;
	ID3D11PixelShader*			PixelShader;

	ID3D11Texture2D*			DepthStencilBuffer;
	ID3D11DepthStencilView*		DepthStencilView;
	ID3D11ShaderResourceView*	DepthShaderResourceView;

	ID3D11SamplerState*			SamplerStateCLAMP;

	ID3D11Buffer*				ConstantBuffer;
	ShadowMapConstantBuffer		ConstantBufferData;



	

	// PRIVATE FUNCTIONS

	void InitializeShaders(
		ID3D11Device* *Device
	);

	void InitializeSampler(
		ID3D11Device* *Device
	);

	void InitializeBuffers(
		ID3D11Device* *Device,
		UINT ScreenWidth,
		UINT ScreenHeight
	);

	void InitializeConstantBuffer(
		ID3D11Device*			*Device,
		ID3D11DeviceContext*	*DeviceContext,
		DirectX::XMFLOAT4		LightPositionArray
	);


public:
	ShadowMapClass();
	~ShadowMapClass();


	void InitializeAll(
		ID3D11Device*			*Device,
		ID3D11DeviceContext*	*DeviceContext,
		UINT					ScreenWidth,
		UINT					ScreenHeight,
		DirectX::XMFLOAT4		LightPosition
	);

	void SetDepthRenderTarget(
		ID3D11DeviceContext* *DeviceContext
	);

	void SetShaderResourceView(
		ID3D11DeviceContext* *Device
	);

	void SetShadingContext(
		ID3D11DeviceContext* *DeviceContext
	);

	void RenderObjectShadowMap(
		ID3D11DeviceContext*		*DeviceContext,
		ID3D11Buffer*				*VertexBuffer,
		int							VerticeCount
	);
};

#endif