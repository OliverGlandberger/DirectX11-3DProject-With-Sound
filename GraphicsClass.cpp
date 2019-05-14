#include "GraphicsClass.hpp"

GraphicsClass::GraphicsClass()
{
	// All class types automatically set internal data to null.
	// MatrixBufferLoaded & MatrixBufferStored also automatically set internal data to null.
}
GraphicsClass::~GraphicsClass()
{

}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void GraphicsClass::ReleaseAll()
{
	this->D3D.ReleaseAll();
	this->ConstantBufferManager.ReleaseAll();

	this->LightManager.ReleaseAll();

	this->DeferredBuffers.ReleaseAll();
	this->DeferredShader.ReleaseAll();
	this->LightShader.ReleaseAll();

	this->QuadTree.ReleaseQuadTree();
}




Direct3DContext* GraphicsClass::GetD3D()
{
	return &this->D3D;
}

ID3D11Device* *GraphicsClass::GetDevice()
{
	return this->D3D.GetDevice();
}

ID3D11DeviceContext* *GraphicsClass::GetDeviceContext()
{
	return this->D3D.GetDeviceContext();
}
IDXGISwapChain* *GraphicsClass::GetSwapChain()
{
	return this->D3D.GetSwapChain();
}

ID3D11RenderTargetView* *GraphicsClass::GetDeferredBackBufferRTV()
{
	return this->LightShader.GetBackBufferRTV();
}

Camera *GraphicsClass::GetCamera()
{
	return &this->MainCamera;
}

MatrixBufferStored *GraphicsClass::GetFormattedCBMatrix()
{
	return this->ConstantBufferManager.GetFormattedMatrixData();
}

ID3D11Buffer* *GraphicsClass::GetConstantBuffer()
{
	return this->ConstantBufferManager.GetConstantBuffer();
}

CB_Lights_Stored *GraphicsClass::GetLightHandlerStruct()
{
	return this->LightManager.GetFormattedStruct();
}

ID3D11Buffer* *GraphicsClass::GetLightHandlerBuffer()
{
	return this->LightManager.GetLightBuffer();
}

ID3D11Texture2D* GraphicsClass::GetColorIDTexture()
{
	return this->DeferredBuffers.GetColorIDTexture();
}


void GraphicsClass::CreateQuadTree(ObjectClass *AllObjects)
{
	this->QuadTree.InitializeGrid(AllObjects);	// InitializeGrid
	this->QuadTree.InitializeNodes();		// InitializeNodes
	this->QuadTree.AssignObjectsToNodes(AllObjects);	// AssignObjectsToNodes
	this->QuadTree.InitializeHoverCam();
}



void GraphicsClass::Initialize(HWND	wndHandle)
{
	// Everything Global.
	this->D3D.Initialize(wndHandle);
	this->ConstantBufferManager.Initialize(
		this->D3D.GetDevice(),
		this->D3D.GetDeviceContext()
	);

	// Everything Lights
	this->LightManager.InitializeLightBufferStruct();
	this->LightManager.InitializeLightBuffer(
		this->D3D.GetDevice(),
		this->D3D.GetDeviceContext()
	);


	// Everything Deferred.
	this->DeferredBuffers.InitializeBuffers(
		this->D3D.GetDevice(),
		SCREEN_WIDTH,
		SCREEN_HEIGHT
	);
	this->DeferredShader.InitialiseShaders(this->D3D.GetDevice());
	this->LightShader.InitializeAll(this->D3D.GetDevice());

	// Everything ShadowMap related
	this->ShadowMap.InitializeAll(
		this->D3D.GetDevice(),
		this->D3D.GetDeviceContext(),
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		this->LightManager.GetFormattedStruct()->PointLightArray[0]
	);

	// Everything ParticleSystem related
	this->Particles.InitializeAll(
		this->D3D.GetDevice()
	);
}




void GraphicsClass::RenderShadowMapTexture(
	ObjectClass*				ObjectArray,
	ID3D11Buffer*				**VertexBufferArray
)
{
	this->UnbindAllPSShaderResourceViews(6);

	this->ShadowMap.SetDepthRenderTarget(this->D3D.GetDeviceContext());
	this->ShadowMap.SetShadingContext(this->D3D.GetDeviceContext());

	for (int i = 0; i < OBJ_COUNT; i++)
	{
		int loopcount = ObjectArray[i].GetCloneCount();
		for (int j = 0; j < loopcount; j++) {

			// Update the CloneWorldMatrix in the constant buffer.
			this->ConstantBufferManager.GetFormattedMatrixData()->world = ObjectArray[i].GetClonesWorldMatrixArray()[j];
			AlterConstantBuffers(
				*this->ConstantBufferManager.GetConstantBuffer(),
				*this->ConstantBufferManager.GetFormattedMatrixData(),
				this->D3D.GetDeviceContext()
			);
			// Render the same object, but with different world matrices.
			this->ShadowMap.RenderObjectShadowMap(
				this->D3D.GetDeviceContext(),
				&(*VertexBufferArray)[i],
				(ObjectArray[i].getObjectData()->faceCount * 3)
			);

		}
	}

	// Unbinds the ShadowMap's Depth buffer as a render target so that
	// it can instead be bound as a ShaderResource below.
	ID3D11DepthStencilView* UnBinderDepth = nullptr;
	(*this->D3D.GetDeviceContext())->OMSetRenderTargets(
		0,
		nullptr,
		UnBinderDepth
	);

	this->ShadowMap.SetShaderResourceView(this->D3D.GetDeviceContext());
}





void GraphicsClass::RenderParticleSystem(
	ID3D11DeviceContext* *DeviceContext,
	float dt
)
{
	// Set BackBufferRTV as the Render Target, so that output can be presented to the window.
	(*this->D3D.GetDeviceContext())->OMSetRenderTargets(
		1,
		this->LightShader.GetBackBufferRTV(),
		nullptr
	);

	this->Particles.UpdateSystem(
		DeviceContext,
		dt,
		this->GetCamera()
	);

	this->Particles.SetShadingContext(DeviceContext);
	this->Particles.RenderParticles(DeviceContext);
}





void GraphicsClass::DeferredRenderAllObjects(
	ObjectClass*				ObjectArray,
	ID3D11Buffer*				**VertexBufferArray,
	ID3D11ShaderResourceView*	*TextureResourceViews,
	ID3D11Buffer*				*HeightMapVertexBuffer,
	float						FaceCount,
	ID3D11ShaderResourceView*	*HeightMapResourceView
)
{
	// First Rendering Pass
	this->RenderSceneToTextures(
		ObjectArray,
		VertexBufferArray,
		TextureResourceViews
	);

	// Has to be called between first and second rendering pass
	this->RenderHeightMap(
		this->D3D.GetDeviceContext(),
		(*HeightMapVertexBuffer),
		FaceCount,
		HeightMapResourceView
	);

	// Second Rendering Pass
	this->SecondRenderPass(
		this->DeferredBuffers.GetGBuffers()
	);
}





void GraphicsClass::SetMainBackBufferRTV()
{
	// get the address of the back buffer
	ID3D11Texture2D* pBackBuffer = nullptr;
	(*this->D3D.GetSwapChain())->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	(*this->D3D.GetDevice())->CreateRenderTargetView(pBackBuffer, NULL, this->LightShader.GetBackBufferRTV());
	pBackBuffer->Release();
}






int GraphicsClass::QuadTreeCornerDive(DirectX::XMVECTOR Corner)
{
	return this->QuadTree.CornerDive(Corner);
}





void GraphicsClass::FrustumCull()
{
	DirectX::XMMATRIX View = this->MainCamera.View;
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(
		PI*ANGLE,
		SCREEN_WIDTH / SCREEN_HEIGHT,
		NEAR_PLANE,
		FAR_PLANE
	);

	DirectX::XMMATRIX LoadedM = DirectX::XMMatrixMultiply(View, Projection);
	DirectX::XMFLOAT4X4 StoredM;
	DirectX::XMStoreFloat4x4(&StoredM, LoadedM);

	// Create the planes out of the matrix M.
	Plane Frustum_left, Frustum_right, Frustum_top, Frustum_bottom, Frustum_near, Frustum_far;
	Frustum_left.Normal.x = -(StoredM._14 + StoredM._11);
	Frustum_left.Normal.y = -(StoredM._24 + StoredM._21);
	Frustum_left.Normal.z = -(StoredM._34 + StoredM._31);
	Frustum_left.Distance = -(StoredM._44 + StoredM._41);

	Frustum_right.Normal.x = -(StoredM._14 - StoredM._11);
	Frustum_right.Normal.y = -(StoredM._24 - StoredM._21);
	Frustum_right.Normal.z = -(StoredM._34 - StoredM._31);
	Frustum_right.Distance = -(StoredM._44 - StoredM._41);

	Frustum_top.Normal.x = -(StoredM._14 - StoredM._12);
	Frustum_top.Normal.y = -(StoredM._24 - StoredM._22);
	Frustum_top.Normal.z = -(StoredM._34 - StoredM._32);
	Frustum_top.Distance = -(StoredM._44 - StoredM._42);

	Frustum_bottom.Normal.x = -(StoredM._14 + StoredM._12);
	Frustum_bottom.Normal.y = -(StoredM._24 + StoredM._22);
	Frustum_bottom.Normal.z = -(StoredM._34 + StoredM._32);
	Frustum_bottom.Distance = -(StoredM._44 + StoredM._42);

	Frustum_near.Normal.x = -(StoredM._14 + StoredM._13);
	Frustum_near.Normal.y = -(StoredM._24 + StoredM._23);
	Frustum_near.Normal.z = -(StoredM._34 + StoredM._33);
	Frustum_near.Distance = -(StoredM._44 + StoredM._43);

	Frustum_far.Normal.x = -(StoredM._14 - StoredM._13);
	Frustum_far.Normal.y = -(StoredM._24 - StoredM._23);
	Frustum_far.Normal.z = -(StoredM._34 - StoredM._33);
	Frustum_far.Distance = -(StoredM._44 - StoredM._43);

	Plane AllFrustumPlanes[6];
	AllFrustumPlanes[0] = Frustum_left;
	AllFrustumPlanes[1] = Frustum_right;
	AllFrustumPlanes[2] = Frustum_near;
	AllFrustumPlanes[3] = Frustum_far;
	AllFrustumPlanes[4] = Frustum_top;
	AllFrustumPlanes[5] = Frustum_bottom;

	this->QuadTree.TestFrustumIntersection(AllFrustumPlanes);
}





void GraphicsClass::SwitchCamera(
	bool HoverCamActivationStatus,
	TCHAR CharacterMessage,
	POINT MouseCoordinates
)
{
	if (HoverCamActivationStatus) {
		
		// Switches so MainCam's viewmatrix gets rendered from instantly,
		// rather than after 'h' and then another input
		this->MainCamera.UpdateCamera(
			CharacterMessage,
			MouseCoordinates,
			this->ConstantBufferManager.GetFormattedMatrixData(),
			this->ConstantBufferManager.GetConstantBuffer(),
			this->D3D.GetDeviceContext(),
			HoverCamActivationStatus
		);

		// Switches the ProjectionMatrix back to the MainCamera's one. 
		DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(PI*ANGLE,
			SCREEN_WIDTH / SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
		DirectX::XMStoreFloat4x4(&this->ConstantBufferManager.GetFormattedMatrixData()->projection, Projection);
		AlterConstantBuffers(
			*this->ConstantBufferManager.GetConstantBuffer(),
			*this->ConstantBufferManager.GetFormattedMatrixData(),
			this->D3D.GetDeviceContext()
		);
	}
	else {
		// Switch to HoverCam

		// Convert QuadTree's view&Projection into the XMFLOAT4X4 view/proj in the CB struct.
		DirectX::XMStoreFloat4x4(&this->ConstantBufferManager.GetFormattedMatrixData()->view, this->QuadTree.GetHoverCamView());
		DirectX::XMStoreFloat4x4(&this->ConstantBufferManager.GetFormattedMatrixData()->projection, this->QuadTree.GetHoverCamProjection());

		AlterConstantBuffers(
			*this->ConstantBufferManager.GetConstantBuffer(),
			*this->ConstantBufferManager.GetFormattedMatrixData(),
			this->D3D.GetDeviceContext()
		);


	}
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

/* ------------- COMMENTS -------------
Sets the following to the GPU-pipeline
Internal DeferredBuffers:
--- RenderTargetViewArray (G-Buffers) as Render Targets.
Internal DeferredShader's:
--- VertexLayout + PrimitiveTopology (TraingleList)
--- Vertex & Pixel-shaders (Removes current GeometryShader)
--- SamplerState 
I 
For each object in the ObjectArray:
--- Update the constant buffer with the object's 'Kd' value
--- Render the object:
--- --- Sets the objects Texture to the PixelShader
--- --- Sets the VertexBuffer to the GPU-pipeline
--- --- Calls Draw()
*/
void GraphicsClass::RenderSceneToTextures(
	ObjectClass*				ObjectArray,
	ID3D11Buffer*				**VertexBufferArray,
	ID3D11ShaderResourceView*	*TextureResourceViews
)
{
	// Unbinds the GBuffers from ShaderResourceViews, so that they can be set as RenderTargets
	this->UnbindAllPSShaderResourceViews(GBUFFER_SHADERRESOURCE_COUNT);


	// Now draws to G-Buffers.
	this->DeferredBuffers.SetAllRenderTargets(this->D3D.GetDeviceContext());
	this->DeferredBuffers.ClearAllRenderTargets(
		this->D3D.GetDeviceContext(),
		0,
		0,
		0,
		1
	);

	// Set new shading context
	this->DeferredShader.SetShadingContext(this->D3D.GetDeviceContext());

	
	// Render each unique object's clones individually to the Texture2D RenderTargets.
	for (int i = 0; i < OBJ_COUNT; i++) {

		// Update the constant buffer with the current object's color.
		ConstantBufferManager.AddMaterialDataToFormattedStruct(
			*ObjectArray[i].getObjectData()
		);
		AlterConstantBuffers(
			*this->ConstantBufferManager.GetConstantBuffer(),
			*this->ConstantBufferManager.GetFormattedMatrixData(),
			this->D3D.GetDeviceContext()
		);


		//				FRUSTUM CULLED ADAPTATION
		// --------------------------------------
		int LoopCount = this->QuadTree.GetPassedCloneCounts()[i];
		int CloneWorldMatrixIndex = -1;

		for (int j = 0; j < LoopCount; j++) {

			// Picked is '1' if it's been picked; otherwise '0'
			float Picked = 0;

			// Fetch the CloneID
			int CloneWorldMatrixIndex = this->QuadTree.GetPassedCloneIDs()[i][j];

			// Check if the current clone's picked-status
			if ((*ObjectArray[i].ModifyPickedArray(CloneWorldMatrixIndex)) == true) {
				Picked = 1;
			}

			// Update the ColorID (AlterConstantBuffers gets called below)
			float Red = ((float)i / 255);
			float Green = ((float)CloneWorldMatrixIndex / 255);
			float Blue = 1;
			this->ConstantBufferManager.GetFormattedMatrixData()->ColorID = DirectX::XMFLOAT4(Red, Green, Blue, Picked);


			// Update the CloneWorldMatrix in the constant buffer.
			this->ConstantBufferManager.GetFormattedMatrixData()->world = ObjectArray[i].GetClonesWorldMatrixArray()[CloneWorldMatrixIndex];
			AlterConstantBuffers(
				*this->ConstantBufferManager.GetConstantBuffer(),
				*this->ConstantBufferManager.GetFormattedMatrixData(),
				this->D3D.GetDeviceContext()
			);
			// Render the same object, but with different world matrices.
			this->DeferredShader.RenderObject(
				this->D3D.GetDeviceContext(),
				&(*VertexBufferArray)[i],
				&TextureResourceViews[i],
				(ObjectArray[i].getObjectData()->faceCount * 3)
			);

		}

	}
	
	// Reset Z so that other things aren't looked at as objects (Heightmap)
	this->ConstantBufferManager.GetFormattedMatrixData()->ColorID.z = 0;
	// Reset Picked so that it doesn't affect anything else
	this->ConstantBufferManager.GetFormattedMatrixData()->ColorID.w = 0;
	// Reset the worldmatrix so that it doesn't affect anything else
	DirectX::XMStoreFloat4x4(&this->ConstantBufferManager.GetFormattedMatrixData()->world, DirectX::XMMatrixIdentity());
	// Apply changes to constant buffer.
	AlterConstantBuffers(
		*this->ConstantBufferManager.GetConstantBuffer(),
		*this->ConstantBufferManager.GetFormattedMatrixData(),
		this->D3D.GetDeviceContext()
	);
}





void GraphicsClass::RenderHeightMap(
	ID3D11DeviceContext*		*DeviceContext,
	ID3D11Buffer*				VertexBuffer,
	float						FaceCount,
	ID3D11ShaderResourceView*	*ShaderResourceView
)
{
	for (int i = 0; i < HEIGHTMAP_TEXTURE_COUNT; i++)
		(*DeviceContext)->PSSetShaderResources(i, 1, &ShaderResourceView[i]);

	UINT32 vertexSize = (sizeof(HeightMapVertexData));	//Data amount per INDIVIDUAL VERTEX
	UINT32 offset = 0;
	(*DeviceContext)->IASetVertexBuffers(
		0,
		1,
		&VertexBuffer,
		&vertexSize,
		&offset
	);
	(*DeviceContext)->Draw((FaceCount * 3), 0);
}




/* ------------- COMMENTS -------------
Sets the standard BackBuffer as the only Render Target.
Sets the LightShader's shading context:
--- Sets LightShader's VertexLayout to the GPU-Pipeline
--- Sets LightShader's internal shaders to the GPU-Pipeline
--- Sets GBufferShaderResourceViews parameter to the Pixel Shader.
*/
void GraphicsClass::SecondRenderPass(
	ID3D11ShaderResourceView*	*GBufferShaderResourceViews
)
{
	// Unbind all old Render Targets so they can be bound as ShaderResourceViews
	this->UnbindAllRenderTargets();

	// Set Lightshaders internal shader pipeline to the gpu.
	this->LightShader.SetPipeline(
		this->D3D.GetDeviceContext()
	);

	// Set Texture2D's (From the first render call)
	(*this->D3D.GetDeviceContext())->PSSetShaderResources(
		0,
		GBUFFER_SHADERRESOURCE_COUNT,
		GBufferShaderResourceViews
	);

	// Set BackBufferRTV as the Render Target, so that output can be presented to the window.
	(*this->D3D.GetDeviceContext())->OMSetRenderTargets(
		1,
		this->LightShader.GetBackBufferRTV(),
		nullptr
	);


	this->LightShader.Render(
		this->D3D.GetDeviceContext()
	);



}





void GraphicsClass::UnbindAllRenderTargets()
{
	ID3D11RenderTargetView* UnBinder[6] = { nullptr };
	ID3D11DepthStencilView* UnBinderDepth = nullptr;
	(*this->D3D.GetDeviceContext())->OMSetRenderTargets(
		6,
		UnBinder,
		UnBinderDepth
	);

}





void GraphicsClass::UnbindAllPSShaderResourceViews(int ShaderResourceView_Count)
{
	// Allocate memory for the empty container.
	ID3D11ShaderResourceView* *UnBinder;
	UnBinder = new ID3D11ShaderResourceView*[ShaderResourceView_Count];

	// Make sure container is empty
	for (int i = 0; i < ShaderResourceView_Count; i++)
		UnBinder[i] = nullptr;

	// Set empty container to shaderresources.
	(*this->D3D.GetDeviceContext())->PSSetShaderResources(
		0,
		ShaderResourceView_Count,
		UnBinder
	);
}





/* ------------- COMMENTS -------------
Replaces current Render Targets with DeferredBuffers internal array of 
RenderTargets (Also Clears these Render Targets).
Replaces current Vertex/Pixel-shaders, InputLayout & SamplerState.
Does not affect ConstantBuffer.
*/
void GraphicsClass::SetDeferredRenderingContext()
{
	this->DeferredBuffers.SetAllRenderTargets(this->D3D.GetDeviceContext());
	this->DeferredBuffers.ClearAllRenderTargets(
		this->D3D.GetDeviceContext(),
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);

	this->DeferredShader.SetShadingContext(this->D3D.GetDeviceContext());
}

