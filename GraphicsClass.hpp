#ifndef GRAPHICSCLASS_HPP
#define GRAPHICSCLASS_HPP

#include "Direct3D.hpp"
#include "Camera.hpp"
#include "ObjectHandlerClass.hpp"
#include "ConstantBufferClass.hpp"

#include "LightHandlerClass.hpp"

#include "DeferredBuffersClass.hpp"
#include "DeferredShaderClass.hpp"
#include "LightShaderClass.hpp"

#include "ShadowMapClass.hpp"

#include "ParticlesClass.hpp"

#include "QuadTreeClass.hpp"

class GraphicsClass {
private:
	Direct3DContext		D3D;
	
	DeferredBuffersClass DeferredBuffers;
	DeferredShaderClass	 DeferredShader;
	LightShaderClass	 LightShader;

	ShadowMapClass		ShadowMap;

	ParticleSystem			Particles;

	Camera				MainCamera;
	LightHandlerClass	LightManager;
	ConstantBufferClass ConstantBufferManager;

	QuadTreeClass		QuadTree;





	// PRIVATE FUNCTIONS

	/* ------------- COMMENTS -------------
	First pass of deferred rendering.
	Renders the scene to different G-Buffers:
	- Color.
	- Normal.
	- WorldPos.
	- Depth.
	*/
	void RenderSceneToTextures(
		ObjectClass*				ObjectArray,
		ID3D11Buffer*				**VertexBufferArray,
		ID3D11ShaderResourceView*	*TextureResourceView
	);

	void RenderHeightMap(
		ID3D11DeviceContext*		*DeviceContext,
		ID3D11Buffer*				VertexBuffer,
		float						FaceCount,
		ID3D11ShaderResourceView*	*ShaderResourceView
	);

	/* ------------- COMMENTS -------------
	Samples the input from the G-Buffers which RenderSceneToTexture renders to
	onto a fullscreen quad which is rendered to the BackBufferRTV.
	*/
	void SecondRenderPass(ID3D11ShaderResourceView*	*GBufferShaderResourceViews);

	/* ------------- COMMENTS -------------
	Unbinds rendertargets so that they can be set as shaderResources
	*/
	void UnbindAllRenderTargets();

	/* ------------- COMMENTS -------------
	Unbinds shaderresources so that they can be set as renderTargets.
	*/
	void UnbindAllPSShaderResourceViews(int ShaderResourceView_Count);

	/* ------------- COMMENTS -------------
	Sets renderTargets.
	Sets viewport.
	*/
	void SetDeferredRenderingContext();



public:
	GraphicsClass();
	~GraphicsClass();
	void ReleaseAll();

	Direct3DContext			*GetD3D();
	ID3D11Device*			*GetDevice();
	ID3D11DeviceContext*	*GetDeviceContext();
	IDXGISwapChain*			*GetSwapChain();

	ID3D11RenderTargetView* *GetDeferredBackBufferRTV();

	Camera *GetCamera();

	MatrixBufferStored *GetFormattedCBMatrix();
	ID3D11Buffer* *GetConstantBuffer();

	CB_Lights_Stored *GetLightHandlerStruct();
	ID3D11Buffer* *GetLightHandlerBuffer();

	ID3D11Texture2D* GetColorIDTexture();


	/* ------------- COMMENTS -------------
	Currently initializes everything but LIGHTSHADER.
	*/
	void Initialize(HWND wndHandle);

	/* ------------- COMMENTS -------------
	Creates the ShadowDepthMap, storing all the values onto
	a texture 2D 'canvas'.
	*/
	void RenderShadowMapTexture(
		ObjectClass*				ObjectArray,
		ID3D11Buffer*				**VertexBufferArray
	);

	void RenderParticleSystem(
		ID3D11DeviceContext* *DeviceContext,
		float dt
	);

	void CreateQuadTree(ObjectClass *AllObjects);

	/* ------------- COMMENTS -------------
	Renders all objects that has been set, with deferred rendering.
	Also renders the height map.
	*/
	void DeferredRenderAllObjects(
		ObjectClass*				ObjectArray,
		ID3D11Buffer*				**VertexBufferArray,
		ID3D11ShaderResourceView*	*TextureResourceViews,
		ID3D11Buffer*				*HeightMapVertexBuffer,
		float						FaceCount,
		ID3D11ShaderResourceView*	*HeightMapResourceView
	);

	/* ------------- COMMENTS -------------
	Sets LightShader's internal RenderTargetView as the BackBufferRTV, so that 
	data rendered to it will be displayed by the application window.
	I
	Also sets the same BackBufferRTV and LightShader's DepthStencilView as 
	render targets to the output-merger stage so that Clear() can be called at the
	start of rendercalls without a need for 'if-cases'.
	*/
	void SetMainBackBufferRTV();


	/* ------------- COMMENTS -------------
	*/
	int QuadTreeCornerDive(DirectX::XMVECTOR Corner);

	/* ------------- COMMENTS -------------
	Checks which leafnodes the frustum relies in through plane/plane-intersections.
	Checks which objectClones in these leafnodes who rely in the frustum with p/p-intersection.
	Only render the objects which can be "seen" by the frustum.
	I
	[OBS: NEEDS QUAD TREE TO BE CREATED IN ADVANCE]
	*/
	void FrustumCull();


	void SwitchCamera(
		bool HoverCamActivationStatus,
		TCHAR CharacterMessage,
		POINT MouseCoordinates
		);


};

#endif