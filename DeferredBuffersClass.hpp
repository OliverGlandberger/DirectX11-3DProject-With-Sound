#ifndef DEFERREDBUFFERSCLASS_HPP
#define DEFERREDBUFFERSCLASS_HPP

// The DeferredBufferClass is just the "Render-To-Texture" Class re-written to handle an array
// of "Render-To-Texture"'s instead of just a single one. The outputs of the Deferred Shader
// will be writing to the texture buffers (render targets) of thiss class.

#include <d3d11.h>
#include <DirectXMath.h>
#include "Globals.hpp"	// For ColorIDBuffer Description.

#define BUFFER_COUNT 4
#define GBUFFER_SHADERRESOURCE_COUNT (BUFFER_COUNT+1)
//	ColorBuffer
//	NormalBuffer
//	DepthBuffer


class DeferredBuffersClass {
private:
	// Used as rendertargets to which data will be outputted from Deferred Shader
	ID3D11Texture2D* RenderTargetTextureArray[BUFFER_COUNT];	// DepthBuffer Texture2D is defined elsewhere
	// Used as a lens to work with the RenderTargetTextureArray above.
	ID3D11RenderTargetView* RenderTargetViewArray[BUFFER_COUNT];

	ID3D11ShaderResourceView* ShaderResourceViewArray[GBUFFER_SHADERRESOURCE_COUNT];	// The final index is for the DepthStencilBuffer.

	ID3D11Texture2D* DepthStencilBuffer;
	ID3D11DepthStencilView* DepthStencilView;


public:
	DeferredBuffersClass();
	~DeferredBuffersClass();
	void ReleaseAll();

	ID3D11ShaderResourceView* *GetGBuffers();

	ID3D11Texture2D* GetColorIDTexture();

	/* ------------- COMMENTS -------------
	Creates RenderTargetTexture's & RenderTargetView's & ShaderResourceView's.
	Creates DepthBuffer & DepthStencilView & Viewport.
	*/
	void InitializeBuffers(
		ID3D11Device* *Device,
		UINT ScreenWidth,
		UINT ScreenHeight
	);

	/* ------------- COMMENTS -------------
	Sets all the internal RenderTargetView's to the OutputMerger stage in the GPU-pipeline.
	The order of the RenderTargets in the array is the order of "SV_TargetX".
	Also sets the internal DepthStenciLView to the pipeline.
	*/
	void SetAllRenderTargets(ID3D11DeviceContext* *DeviceContext);

	/* ------------- COMMENTS -------------
	Clears G-Buffers including the depth buffer.
	*/
	void ClearAllRenderTargets(
		ID3D11DeviceContext*	*DeviceContext,
		float					ClearColor_Red,
		float					ClearColor_Blue,
		float					ClearColor_Green,
		float					ClearColor_Alpha
	);


};












#endif