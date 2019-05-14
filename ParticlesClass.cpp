#include "ParticlesClass.hpp"

ParticleSystem::ParticleSystem()
{
	srand(time(NULL));

	this->Capacity = 1000;
	this->MaxVertexCount = this->Capacity * 3; // Each particle is one triangle, hence 3 vertices
	this->CurrentParticleCount = 0;
	this->LastKeyTime = 0.0f;
	this->CurrentTime = 0.0f;

	this->PropertiesArray = new ParticleProperties[this->Capacity];		// Create an array for all particle properties
	this->VertexArray = new ParticleVertexData[this->MaxVertexCount];	// Create vertex array for particles to be rendered
}

ParticleSystem::~ParticleSystem()
{

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ParticleSystem::ReleaseAll()
{
	delete[] this->VertexArray;
}

void ParticleSystem::InitializeAll(
	ID3D11Device*	*Device
)
{
	this->InitializeShaders(Device);

	this->InitializeBuffers(Device);
}

void ParticleSystem::UpdateSystem(
	ID3D11DeviceContext* *DeviceContext,
	float dt,
	Camera* theCamera
)
{
	this->CurrentTime = dt;
	float TempTimeDifference = this->CurrentTime - this->LastKeyTime;

	if (TempTimeDifference > 0.1f)
	{
		this->CreateParticle(theCamera->GetCameraPositionFloat3());
		this->LastKeyTime = this->CurrentTime;
	}

	this->UpdateParticlePositions
	(
		theCamera->GetCameraPositionFloat3()
	);

	this->UpdateVertexBuffer(
		DeviceContext,
		theCamera
	);
}

void ParticleSystem::SetShadingContext(
	ID3D11DeviceContext* *DeviceContext
)
{
	// Set InputLayout & PrimitiveTopology
	(*DeviceContext)->IASetInputLayout(this->VertexLayout);
	(*DeviceContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	
																					
	(*DeviceContext)->VSSetShader(this->VertexShader, nullptr, 0);
	(*DeviceContext)->HSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->DSSetShader(nullptr, nullptr, 0);
	(*DeviceContext)->GSSetShader(this->GeometryShader, nullptr, 0);
	(*DeviceContext)->PSSetShader(this->PixelShader, nullptr, 0);
}

void ParticleSystem::RenderParticles(
	ID3D11DeviceContext* *DeviceContext
)
{
	// Set the object's VertexBuffer to the IA.
	UINT32 VertexSize = sizeof(ParticleVertexData);	// float3_pos + float2_uv + float3_color
	UINT32 offset = 0;
	(*DeviceContext)->IASetVertexBuffers(
		0,
		1,
		&this->VertexBuffer,
		&VertexSize,
		&offset
	);

	// Render the geometry
	(*DeviceContext)->Draw((UINT)(this->CurrentParticleCount * 3), 0);
}






//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ParticleSystem::InitializeShaders(
	ID3D11Device* *Device
)
{
	HRESULT hr;
	//C R E A T E		V E R T E X			S H A D E R
	ID3DBlob* pVS = nullptr;
	D3DCompileFromFile(
		L"Particle_Vertex.hlsl",
		nullptr,
		nullptr,
		"PARTICLES_VS_main",
		"vs_5_0",
		0,
		0,
		&pVS,
		nullptr
	);
	(*Device)->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &this->VertexShader);

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = (*Device)->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &this->VertexLayout);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'Vertex Shader'.", L"ERROR: 'InitialiseShaders() - ParticlesClass.cpp'", MB_OK);

	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	// C R E A T E		G E O M E T R Y		S H A D E R
	ID3DBlob* pGS = nullptr;
	ID3DBlob* error_GS = nullptr;
	D3DCompileFromFile(
		L"Particle_Geometry.hlsl",
		nullptr,
		nullptr,
		"PARTICLES_GS_main",
		"gs_5_0",
		D3D10_SHADER_DEBUG,
		0,
		&pGS,
		&error_GS
	);

	if (error_GS)
	{
		OutputDebugStringA((char*)error_GS->GetBufferPointer());
		error_GS->Release();
	}

	hr = (*Device)->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(),
		nullptr, &this->GeometryShader);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'Geometry Shader'.", L"ERROR: 'InitialiseShaders() - ParticlesClass.cpp'", MB_OK);

	//We no longer need this COM object and therefore release it
	pGS->Release();

	//----------------------------------- Pixel Shader
	ID3DBlob* pPS = nullptr;
	D3DCompileFromFile(
		L"Particle_Pixel.hlsl",
		nullptr,
		nullptr,
		"PARTICLES_PS_main",
		"ps_5_0",
		D3D10_SHADER_DEBUG,
		0,
		&pPS,
		nullptr
	);
	hr = (*Device)->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &this->PixelShader);
	if (FAILED(hr))
		MessageBox(NULL, L"Failed to create 'Pixel Shader'.", L"ERROR: 'InitialiseShaders() - ParticlesClass.cpp'", MB_OK);
	pPS->Release();
}

void ParticleSystem::InitializeBuffers(
	ID3D11Device* *Device
)
{
	HRESULT hr;

	memset(this->VertexArray, 0, (sizeof(ParticleVertexData) * this->MaxVertexCount));

	D3D11_BUFFER_DESC VertexBufferDesc;

	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertexBufferDesc.ByteWidth = sizeof(ParticleVertexData) * this->MaxVertexCount;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA VertexData;

	VertexData.pSysMem = this->VertexArray;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;

	hr = (*Device)->CreateBuffer(&VertexBufferDesc, &VertexData, &this->VertexBuffer);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to Create a VertexBuffer.", L"ERROR: 'InitialiseBuffers() - ParticlesClass.cpp'", MB_OK);
}

void ParticleSystem::UpdateVertexBuffer(
	ID3D11DeviceContext* *DeviceContext,
	Camera* theCamera
)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	ParticleVertexData* VerticesPtr;
	int MaxVertexCount = this->Capacity * 3;

	memset(this->VertexArray, 0, (sizeof(ParticleVertexData)) * MaxVertexCount);

	for (int i = 0; i < this->CurrentParticleCount; i++)
	{

		DirectX::XMVECTOR ParticleToCamera = DirectX::XMVectorSubtract(
			DirectX::XMVECTOR{
			this->PropertiesArray[i].Position.x,
			this->PropertiesArray[i].Position.y,
			this->PropertiesArray[i].Position.z },
			theCamera->CameraPosition);
		ParticleToCamera = DirectX::XMVector3Normalize(ParticleToCamera);

		DirectX::XMVECTOR TempCross1, TempCross2;

		TempCross1 = DirectX::XMVector3Cross(ParticleToCamera, theCamera->CameraUpDirection);
		TempCross1 = DirectX::XMVector3Normalize(TempCross1);

		TempCross2 = DirectX::XMVector3Cross(TempCross1, ParticleToCamera);
		TempCross2 = DirectX::XMVector3Normalize(TempCross2);


		DirectX::XMFLOAT3 ParticleRightVector, ParticleUpVector;

		DirectX::XMStoreFloat3(&ParticleRightVector, TempCross1);
		DirectX::XMStoreFloat3(&ParticleUpVector, TempCross2);



		// BOTTOM LEFT
		this->VertexArray[(i * 3)].Position.x = this->PropertiesArray[i].Position.x - (ParticleRightVector.x * PARTICLE_SIZE);
		this->VertexArray[(i * 3)].Position.y = this->PropertiesArray[i].Position.y - (ParticleRightVector.y * PARTICLE_SIZE);
		this->VertexArray[(i * 3)].Position.z = this->PropertiesArray[i].Position.z - (ParticleRightVector.z * PARTICLE_SIZE);
		
		this->VertexArray[(i * 3)].TexCoord = DirectX::XMFLOAT2(0.0f, 1.0f);

		this->VertexArray[(i * 3)].Color = this->PropertiesArray[i].Color;

		// TOP MIDDLE
		this->VertexArray[((i * 3) + 1)].Position.x = this->PropertiesArray[i].Position.x + (ParticleUpVector.x * PARTICLE_SIZE);
		this->VertexArray[((i * 3) + 1)].Position.y = this->PropertiesArray[i].Position.y + (ParticleUpVector.y * PARTICLE_SIZE);
		this->VertexArray[((i * 3) + 1)].Position.z = this->PropertiesArray[i].Position.z + (ParticleUpVector.z * PARTICLE_SIZE);

		this->VertexArray[((i * 3) + 1)].TexCoord = DirectX::XMFLOAT2(0.5f, 0.0f);

		this->VertexArray[((i * 3) + 1)].Color = this->PropertiesArray[i].Color;

		// BOTTOM RIGHT
		this->VertexArray[((i * 3) + 2)].Position.x = this->PropertiesArray[i].Position.x + (ParticleRightVector.x * PARTICLE_SIZE);
		this->VertexArray[((i * 3) + 2)].Position.y = this->PropertiesArray[i].Position.y + (ParticleRightVector.y * PARTICLE_SIZE);
		this->VertexArray[((i * 3) + 2)].Position.z = this->PropertiesArray[i].Position.z + (ParticleRightVector.z * PARTICLE_SIZE);

		this->VertexArray[((i * 3) + 2)].TexCoord = DirectX::XMFLOAT2(1.0f, 1.0f);

		this->VertexArray[((i * 3) + 2)].Color = this->PropertiesArray[i].Color;
	}

	hr = (*DeviceContext)->Map(this->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	if (FAILED(hr))
		MessageBox(NULL, L"Failed to Map a VertexBuffer.", L"ERROR: 'UpdateVertexBuffer() - ParticlesClass.cpp'", MB_OK);

	VerticesPtr = (ParticleVertexData*)MappedResource.pData;

	memcpy(VerticesPtr, (void*)this->VertexArray, (sizeof(ParticleVertexData) * this->MaxVertexCount));

	(*DeviceContext)->Unmap(this->VertexBuffer, 0);
}

void ParticleSystem::UpdateParticlePositions
(
	DirectX::XMFLOAT3 CameraPosition
)
{
	for (int i = 0; i < this->CurrentParticleCount; i++)
	{
		this->PropertiesArray[i].Position.x = CameraPosition.x +
			this->PropertiesArray[i].CameraRelativePosition.x;

		this->PropertiesArray[i].Position.y = (this->PropertiesArray[i].CameraRelativePosition.y -= 0.1f);

		this->PropertiesArray[i].Position.z = CameraPosition.z +
			this->PropertiesArray[i].CameraRelativePosition.z;
	}
}

void ParticleSystem::CreateParticle(DirectX::XMFLOAT3 CameraPosition)
{
	if (this->CurrentParticleCount == this->Capacity)
		this->CurrentParticleCount = 0;
	
	//static int test = 0;


	//if (test < this->Capacity)
	//	test++;
	//else
	//	test = 0;

	// [this->CurrentParticleCount % this->Capacity

	this->PropertiesArray[this->CurrentParticleCount].Active = true;

	this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.x = ((float)(rand() % 100 + 1) * 0.2);
	this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.y = ((float)(rand() % 100 + 1) * 0.5);
	this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.z = ((float)(rand() % 100 + 1) * 0.2);

	if (rand() % 2 + 1 == 2)
		this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.x *= -1.0f;
	if (rand() % 2 + 1 == 2)
		this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.z *= -1.0f;

	this->PropertiesArray[this->CurrentParticleCount].Position.x = CameraPosition.x +
		this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.x;

	this->PropertiesArray[this->CurrentParticleCount].Position.y = CameraPosition.y +
		this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.y;

	this->PropertiesArray[this->CurrentParticleCount].Position.z = CameraPosition.z +
		this->PropertiesArray[this->CurrentParticleCount].CameraRelativePosition.z;
	this->PropertiesArray[this->CurrentParticleCount].Color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	this->PropertiesArray[this->CurrentParticleCount].Velocity = 1.0f;

	if (this->CurrentParticleCount < this->Capacity)
		this->CurrentParticleCount++;
}