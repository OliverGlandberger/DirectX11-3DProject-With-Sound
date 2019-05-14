#ifndef DIRECT3D_HPP
#define DIRECT3D_HPP

#include <d3d11.h>
#include "Globals.hpp"	// InitialiseDepthStencilAndView(), CreateViewport()

class Direct3DContext {
private:
	IDXGISwapChain* SwapChain;
	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;

	D3D11_RASTERIZER_DESC RasterizerDescription;
	ID3D11RasterizerState* RasterizerState;

	D3D11_VIEWPORT Viewport;
	D3D11_VIEWPORT ShadowMapViewPort;

	/* ------------- COMMENTS -------------
	Initializes Device, DeviceContext, and SwapChain.
	*/
	void CreateDirect3DContext(HWND	wndHandle);

	/* ------------- COMMENTS -------------
	Initializes the default Rasterizer with customized settings.
	*/
	void InitializeRasterizer();

	/* ------------- COMMENTS -------------
	Initializes the internal viewport with data.
	*/
	void InitializeViewports();

	/* ------------- COMMENTS -------------
	Sets the Rasterizer with already initialized data.
	*/
	void SetRasterizer();



public:
	Direct3DContext();
	~Direct3DContext();
	void ReleaseAll();

	/* ------------- COMMENTS -------------
	Sets the internal viewport to the Rasterizer stage
	(standard viewport for the camera).
	*/
	void SetStandardViewport();

	/* ------------- COMMENTS -------------
	Sets the internal viewport to the Rasterizer stage
	(viewport for the when creating the high res ShadowMap).
	*/
	void SetShadowMapViewport();

	ID3D11Device* *GetDevice();
	ID3D11DeviceContext* *GetDeviceContext();
	IDXGISwapChain* *GetSwapChain();

	/* ------------- COMMENTS -------------
	Initializes Device, DeviceContext, and SwapChain.
	Initializes internal Viewport.
	Sets internal viewport to the Rasterizer stage in the GPU-pipeline
	*/
	void Initialize(HWND wndHandle);

};







#endif