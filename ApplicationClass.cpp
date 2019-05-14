#include "ApplicationClass.hpp"

ApplicationClass* ApplicationClass::SelfPtr = nullptr;

ApplicationClass::ApplicationClass()
{
	SelfPtr = this;
}
ApplicationClass::~ApplicationClass()
{

}
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ApplicationClass::Shutdown()
{
	this->GraphicsManager.ReleaseAll();
	this->ObjectManager.ReleaseAll();
	this->InputManager.ReleaseAll();
	DestroyWindow(this->wndHandle);
}



HWND ApplicationClass::GetWNDHandle()
{
	return this->wndHandle;
}
MSG *ApplicationClass::GetInputMessage()
{
	return &this->msg;
}






void ApplicationClass::Initialize(HINSTANCE hInstance)
{
	// Initialize Window
	this->InitiateWindow(hInstance, WindowsProcedure);
	
	// Initialize Graphics
	this->GraphicsManager.Initialize(this->wndHandle);

	// Transfer data from files into ObjectManagers AllObjects
	this->InputManager.Initialize(
		*this->ObjectManager.GetAllObjects(),
		this->ObjectManager.GetHeightMap(),
		this->GraphicsManager.GetDevice(),
		this->GraphicsManager.GetDeviceContext()
	);
}





void ApplicationClass::SetObjectData()
{
	this->ObjectManager.SetAllObjectData(
		this->GraphicsManager.GetDevice(),
		*this->ObjectManager.GetAllObjects()
	);
}




void ApplicationClass::CreateQuadTree()
{
	this->GraphicsManager.CreateQuadTree(*this->ObjectManager.GetAllObjects());
}




void ApplicationClass::FrustumCull()
{
	this->GraphicsManager.FrustumCull();
}



void ApplicationClass::SetHeightMapData()
{
	this->ObjectManager.SetHeightMapVertexData(
		this->GraphicsManager.GetDevice(),
		this->ObjectManager.GetHeightMap()->GetHeightMapVertexDataArray(),
		this->InputManager.GetHeightMapLoader()
	);
}





void ApplicationClass::SetMainBackBufferRTV()
{
	this->GraphicsManager.SetMainBackBufferRTV();
}




void ApplicationClass::UpdateCamera()
{
	// Update the camera based on mouse/keyboard input
	this->GraphicsManager.GetCamera()->UpdateCamera(
		this->msg.wParam,
		this->InputManager.GetCursorMovement(this->msg.pt),
		this->GraphicsManager.GetFormattedCBMatrix(),
		this->GraphicsManager.GetConstantBuffer(),
		this->GraphicsManager.GetDeviceContext(),
		this->HoverCamActivationStatus
	);

	// Creating variables for the following if-statement
	// Reasons:
	//		- Converting super long names into shorter more concise names; easier to read
	//		- Saves time (very minimul) by only calling 'GetFunctions' once
	float TempCameraFloatX = this->GraphicsManager.GetCamera()->GetCameraPositionFloat3().x;
	float TempCameraFloatY;	// To be determined
	float TempCameraFloatZ = this->GraphicsManager.GetCamera()->GetCameraPositionFloat3().z;
	int TempHeightMapWidth = this->InputManager.GetHeightMapLoader()->GetHeightMapWidth();
	int TempHeightMapHeight = this->InputManager.GetHeightMapLoader()->GetHeightMapHeight();

	// HEIGHTMAP: Terrain Walking
	// Check if the camera is within the heightmap; if true -> adjust Y-Coordinate
	if (TempCameraFloatX > 0.0f &&
		TempCameraFloatX < (float)(TempHeightMapWidth * HEIGHTMAP_WIDTH_MULTIPLIER) &&
		TempCameraFloatZ > 0.0f &&
		TempCameraFloatZ < (float)(TempHeightMapHeight * HEIGHTMAP_WIDTH_MULTIPLIER))
	{
		DirectX::XMVECTOR TempCameraPositionVector;

		int TempXIndex = int(TempCameraFloatX / HEIGHTMAP_WIDTH_MULTIPLIER);
		int TempZIndex = int(TempCameraFloatZ / HEIGHTMAP_WIDTH_MULTIPLIER);

		// Converting the Z-Index into an 'array form' (for each '1' height, multiply by the width)
		TempZIndex *= this->InputManager.GetHeightMapLoader()->GetHeightMapWidth();

		TempCameraFloatY = this->InputManager.GetHeightMapLoader()->GetVertexHeightArray()[(TempZIndex + TempXIndex)];
		TempCameraFloatY = ((TempCameraFloatY * HEIGHTMAP_HEIGHT_MULTIPLIER) + 5.0f);

		TempCameraPositionVector = DirectX::XMVectorSet(TempCameraFloatX, TempCameraFloatY, TempCameraFloatZ, 1);

		this->GraphicsManager.GetCamera()->CameraPosition = TempCameraPositionVector;
	}

	// Update the LightBuffer with new camera position so that specular can be done in the GPU
	DirectX::XMStoreFloat4(
		&this->GraphicsManager.GetLightHandlerStruct()->CameraPosition,
		this->GraphicsManager.GetCamera()->CameraPosition
	);
	D3D11_MAPPED_SUBRESOURCE MappedBuffer;
	(*this->GraphicsManager.GetDeviceContext())->Map(*this->GraphicsManager.GetLightHandlerBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedBuffer);
	memcpy(MappedBuffer.pData, this->GraphicsManager.GetLightHandlerStruct(), sizeof(CB_Lights_Stored));
	(*this->GraphicsManager.GetDeviceContext())->Unmap(*this->GraphicsManager.GetLightHandlerBuffer(), 0);
	/*
	This can be condensed into a single function if other things doesn't have higher priority.
	*/
}





void ApplicationClass::RenderShadowMap()
{
	this->GraphicsManager.GetD3D()->SetShadowMapViewport();

	this->GraphicsManager.RenderShadowMapTexture(
		*this->ObjectManager.GetAllObjects(),
		this->ObjectManager.GetVertexBuffers()
	);

	this->GraphicsManager.GetD3D()->SetStandardViewport();
}





void ApplicationClass::RenderParticles()
{
	this->GraphicsManager.RenderParticleSystem(
		this->GraphicsManager.GetD3D()->GetDeviceContext(),
		this->GameTimer.get_dt()
	);
}





void ApplicationClass::RenderDeferredAll()
{
	this->GraphicsManager.DeferredRenderAllObjects(
		*this->ObjectManager.GetAllObjects(),
		this->ObjectManager.GetVertexBuffers(),
		this->InputManager.GetShaderResourceViewArray(),
		this->ObjectManager.GetHeightMapVertexBuffer(),
		this->InputManager.GetHeightMapLoader()->GetFaceCount(),
		this->InputManager.GetHeightMapShaderResourceView()
	);
}





void ApplicationClass::Present()
{
	(*this->GraphicsManager.GetSwapChain())->Present(1, 0);
}



void ApplicationClass::CheckPicking()
{
	// Fetch mouse coordinates.
	POINT Coordinates = this->msg.pt;
	LPPOINT TempPointer = &Coordinates;
	ScreenToClient(this->wndHandle, TempPointer);	// Convers coordinates from the WindowsScreen-space to ClientScreen-space.
	
	// Create a temporary Texture2D which we have CPU-Access(read) to.
	ID3D11Texture2D* TempTexture;
	D3D11_TEXTURE2D_DESC TempDesc;
	TempDesc.Width = SCREEN_WIDTH;
	TempDesc.Height = SCREEN_HEIGHT;
	TempDesc.MipLevels = 0;
	TempDesc.ArraySize = 1;
	TempDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	TempDesc.SampleDesc.Count = 1;
	TempDesc.SampleDesc.Quality = 0;
	TempDesc.Usage = D3D11_USAGE_STAGING;
	TempDesc.BindFlags = 0;
	TempDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	TempDesc.MiscFlags = 0;
	HRESULT hr = (*this->GraphicsManager.GetDevice())->CreateTexture2D(&TempDesc, NULL, &TempTexture);
	// Copy to the new Texture2D (Which we can map) the values from the Texture2D which gets rendered to
	(*this->GraphicsManager.GetDeviceContext())->CopyResource(TempTexture, this->GraphicsManager.GetColorIDTexture());


	// Map the new Texture2D so we can access its data.
	D3D11_MAPPED_SUBRESOURCE MappedBuffer;
	(*this->GraphicsManager.GetDeviceContext())->Map(TempTexture, 0, D3D11_MAP_READ, 0, &MappedBuffer);


	// Typecast the data as XMFLOAT4 pointer, so that we can treat the entire Texture2D as an array.
	DirectX::XMFLOAT4 *FloatPointer;
	FloatPointer = (DirectX::XMFLOAT4*)MappedBuffer.pData;


	// "Sample" the array index based on the mousecoordinates
	int Index = Coordinates.x + SCREEN_WIDTH * Coordinates.y;	//
	DirectX::XMFLOAT4 SampledValue = FloatPointer[Index];


	// Z currently is 1 if the pixel belongs to an object; otherwise zero.
	if (SampledValue.z > 0.9 && SampledValue.z < 1.1) {
		
		// Find which object through the ID, so convert it into from 0-1 to 0-255
		int ObjectID = (SampledValue.x * 255) + 0.5;	// '+ 0.5' to make it round up instead of down.
		int CloneID = (SampledValue.y * 255) + 0.5;		//

		// bool picked = !picked   is the line below simplified.
		ObjectClass TempHolder = (*this->ObjectManager.GetAllObjects())[ObjectID];
		(*TempHolder.ModifyPickedArray(CloneID)) = !(*TempHolder.ModifyPickedArray(CloneID));

	}
	
	// Don't leak dat memory!
	TempTexture->Release();
	
}



void ApplicationClass::SwitchCamera()
{
	this->GraphicsManager.SwitchCamera(
		this->HoverCamActivationStatus,
		this->msg.wParam,
		this->InputManager.GetCursorMovement(this->msg.pt)
	);
	this->HoverCamActivationStatus = !this->HoverCamActivationStatus;
}






// GET FUNCTIONS

TimerClass* ApplicationClass::getTimer()
{
	return &this->GameTimer;
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+





void ApplicationClass::InitiateWindow(HINSTANCE hInstance, LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM)
)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_D3D_DEMO";
	if (!RegisterClassEx(&wcex))
		MessageBox(NULL, L"SOMETHING HAS GONE TERRIBLY WRONG", L"ApplicationClass::InitializeWindow()", MB_OK);

	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		L"BTH_D3D_DEMO",
		L"pls work",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	this->wndHandle = handle;

}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									NON-CLASS FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

LRESULT CALLBACK ApplicationClass::WindowsProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam
)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN: {
		if (wParam == 'H')
			SelfPtr->SwitchCamera();

		SelfPtr->UpdateCamera();	// Updates constantbuffer with new view matrix
		break;
	}

	case WM_LBUTTONDOWN: {
		
		SelfPtr->CheckPicking();

		break;
	}
	case WM_MOUSEMOVE: {
		SelfPtr->UpdateCamera();	// Updates constantbuffer with new view matrix
		break;
	}

	}

	return DefWindowProc(hWnd, message, wParam, lParam);	// If we process SYSKEYSTROKES, which
															// are ALT+KeyStroke, this function
															// allows windows to keep processing normal
															// SYSKEYSTROKES, such as ALT-TAB.
}
