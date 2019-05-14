#ifndef DEFERREDSHADERCLASS_HPP
#define DEFERREDSHADERCLASS_HPP

#include <d3d11.h>
#include <d3dcompiler.h>

#include "ConstantBufferClass.hpp"	// Needs MatrixBufferStored


class DeferredShaderClass {
private:
	ID3D11InputLayout*		VertexLayout;
	ID3D11VertexShader*		VertexShader;
	ID3D11GeometryShader*	GeometryShader;
	ID3D11PixelShader*		PixelShader;
	ID3D11SamplerState*		SamplerStateWrap; // Wraps Address.X


public:
	DeferredShaderClass();
	~DeferredShaderClass();
	void ReleaseAll();

	/* ------------- COMMENTS -------------
	Creates internal Vertex/Pixel shaders.
	Creates internal SamplerState.
	Creates internal MatrixBuffer.
	*/
	void InitialiseShaders(ID3D11Device* *Device);

	/* ------------- COMMENTS -------------
	Sets these to the pipeline:
	VertexLayout.
	VertexShader.
	GeometryShader.
	PixelShader.
	SamplerState.
	*/
	void SetShadingContext(ID3D11DeviceContext* *DeviceContext);

	/* ------------- COMMENTS -------------
	Sets the VertexBuffer parameter to the Input Assembly then
	calls Draw().
	*/
	void RenderObject(
		ID3D11DeviceContext*		*DeviceContext,
		ID3D11Buffer*				*VertexBuffer,
		ID3D11ShaderResourceView*	*TextureResourceView,
		int							VerticeCount
	);

};




#endif