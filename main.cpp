


//--------------------------------------------------------------------------------------
//	DirectX 11 Starting Template Created by Stefan Petersson (BTH) 2014
//	Project Coded by Daniel Fredriksson & Oliver Glandberger.
//--------------------------------------------------------------------------------------
// APPLICATION
#include "ApplicationClass.hpp"

// FOR MEMORY LEAKS
#include <crtdbg.h>


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	CoInitialize(NULL);	// "Initialize COM Library", needed for 'CreateWICTextureFromFile'

	bool ActiveHoverCam = false;

	ApplicationClass *Application;
	Application = new ApplicationClass;

	Application->Initialize(hInstance);		// Initialize classes
	Application->SetMainBackBufferRTV();	// Uses LightShader's BackBufferRT
	Application->SetObjectData();			// Load object data to classes
	Application->CreateQuadTree();			// Requires ObjectData to be set
	Application->SetHeightMapData();		// Load the height map

	ShowWindow(Application->GetWNDHandle(), nCmdShow);

	while (Application->GetInputMessage()->message != WM_QUIT)
	{

		if (PeekMessage(Application->GetInputMessage(), nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(Application->GetInputMessage());	// Calls WindowProcedure in some part of its internal process
			DispatchMessage(Application->GetInputMessage());

			// Windows procedure exists in application.cpp below "non-class functions"

		}
		else
		{

			Application->FrustumCull();

			Application->getTimer()->Update();
			
			Application->RenderShadowMap();			// Create the ShadowMap Texture2D

			Application->RenderDeferredAll();	// Requires FrustumCull to be called.

			Application->RenderParticles();

			Application->Present();


		

		}
	}

	Application->Shutdown();
	delete Application;

	return (int)(*Application->GetInputMessage()).wParam;
}

