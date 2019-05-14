#ifndef APPLICATIONCLASS_HPP
#define ApplicationClass_HPP

#include "GraphicsClass.hpp"
#include "InputClass.hpp"

#include "Direct3D.hpp"
#include "objLoader.hpp"
#include "Camera.hpp"
#include "TimerClass.hpp"

#include "ObjectHandlerClass.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


// NON-CLASS FUNCTION
// LRESULT CALLBACK WindowsProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class ApplicationClass {
private:
	MSG msg;
	HWND wndHandle;

	GraphicsClass		GraphicsManager;
	ObjectHandlerClass	ObjectManager;
	InputClass			InputManager;
	TimerClass			GameTimer;

	void InitiateWindow(HINSTANCE hInstance, LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM));
	


	bool HoverCamActivationStatus = false;
	static ApplicationClass*	SelfPtr;



public:
	ApplicationClass();
	~ApplicationClass();
	/* ------------- COMMENTS -------------
	Releases GraphicsClass
	--- Releases Direct3DContext
	--- --- Releases SwapChain
	--- --- Releases Device
	--- --- Releases DeviceContext
	--- Releases BasicShader
	--- --- Releases VertexShader
	--- --- Releases GeometryShader
	--- --- Releases PixelShader
	--- --- Releases BackBufferRTV
	--- --- Releases DepthStencil
	--- --- Releases DepthStencilView
	--- Releases ConstantBuffer
	Destroys internal windowHandle
	*/
	void Shutdown();
	
	/* ------------- COMMENTS -------------
	Initializes Window.
	Initializes Input,
	--- Imports Object Files
	Initializes Graphics,
	--- Initializes Direct3DContext
	--- --- Creates 3DContext
	--- --- Creates Viewport
	---	---	Creates DepthStencilView
	--- Initializes BasicShader
	--- --- Creates VertexShader
	--- --- Creates GeometryShader
	--- --- Creates PixelShader
	--- Initializes ConstantBuffers
	--- --- Initializes ConstantMatrices
	--- --- Creates & Sets ConstantBuffers [GS]
	*/
	void Initialize(HINSTANCE hInstance);

	/* ------------- COMMENTS -------------
	Pulls object- & calculation-data from InputManager's earlier called 'importObjFile'
	and sets these into Graphics->ObjectManager's respective Vertex/Index-Buffers
	*/
	void SetObjectData();

	/* ------------- COMMENTS -------------

	*/
	void CreateQuadTree();

	/* ------------- COMMENTS -------------
	Checks which leafnodes the frustum relies in through plane/plane-intersections.
	Checks which objectClones in these leafnodes who rely in the frustum with p/p-intersection.
	Only render the objects which can be "seen" by the frustum.
	I
	[OBS: NEEDS QUAD TREE TO BE CREATED IN ADVANCE]
	*/
	void FrustumCull();

	void SetHeightMapData();

	/* ------------- COMMENTS -------------
	Sets the LightShaders internal RenderTargetView to be the main
	BackBufferRTV, which is displayed by the window.
	*/
	void SetMainBackBufferRTV();


	HWND GetWNDHandle();
	MSG *GetInputMessage();

	/* ------------- COMMENTS -------------
	Updates CursorMovement with new cursor coordinates based on the difference between new & old coordinates.
	Then updates the camera based on keyboard input through CharacterMessage and the updated cursor coordinates.
	*/
	void UpdateCamera();

	/* ------------- COMMENTS -------------
	Renders the ShadowMap using the ShadowMapping pipeline.
	This is stored into a Texture2D that is later used
	during the deferred rendering stage.
	*/
	void RenderShadowMap();

	/* ------------- COMMENTS -------------
	Renders using Deferred Rendering.
	*/
	void RenderDeferredAll();

	/* ------------- COMMENTS -------------
	Renders all the 'falling snow' particles. This is
	done after/'on top' of the deferred rendering and
	as such is completely independant.
	*/
	void RenderParticles();

	/* ------------- COMMENTS -------------
	Swaps front(window)-buffer & back-buffer
	*/
	void Present();


	/* ------------- COMMENTS -------------

	*/
	void CheckPicking();

	/*
	Switches between displaying from the MainCamera & HoverCamera,
	by switching the View&Projection matrices in the ConstantBuffer.
	*/
	void SwitchCamera();

	static LRESULT CALLBACK WindowsProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);




	// GET FUNCTIONS

	TimerClass* getTimer();
};



#endif