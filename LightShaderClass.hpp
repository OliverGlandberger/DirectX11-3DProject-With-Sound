#ifndef LIGHTSHADERCLASS_HPP
#define LIGHTSHADERCLASS_HPP

#include <d3d11.h>
#include <d3dcompiler.h>
#include "DeferredBuffersClass.hpp"	// BUFFER_COUNT, SetShadersAndShaderResources

class LightShaderClass {
private:
	ID3D11VertexShader*		VertexShader;
	ID3D11PixelShader*		PixelShader;

	ID3D11SamplerState*		SamplerStateCLAMP;

	ID3D11RenderTargetView*	BackBufferRTV;

public:

	LightShaderClass();
	~LightShaderClass();


	ID3D11RenderTargetView* *GetBackBufferRTV();



	/* ------------- COMMENTS -------------
	Initializes internal shaders.
	Initializes internal sampler.
	Initializes internal screenquad.
	*/
	void InitializeAll(ID3D11Device* *Device);

	/* ------------- COMMENTS -------------
	Removes input layout and vertexBuffers, this function draws
	the screen through an id-generated quad in the VertexShader which gets
	input from the earlier rendered G-Buffers.
	*/
	void Render(
		ID3D11DeviceContext*		*DeviceContext
	);


	void InitializeShaders(ID3D11Device* *Device);
	void InitializeSampler(ID3D11Device* *Device);

	/* ------------- COMMENTS -------------

	*/
	void SetRenderTargets(
		ID3D11DeviceContext*	*DeviceContext,
		ID3D11RenderTargetView* *BackBufferRTV,
		ID3D11DepthStencilView*	*DepthStencilView
	
	);
	/* ------------- COMMENTS -------------
	Sets internal VertexLayout to the GPU-pipeline.
	Sets internal Shaders to the GPU-pipeline.
	Sets internal SamplerState to the GPU-pipeline
	*/
	void SetPipeline(
		ID3D11DeviceContext*		*DeviceContext
	);

	void ReleaseAll();

};



#endif