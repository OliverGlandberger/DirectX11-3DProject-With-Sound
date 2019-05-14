#include "ConstantBufferClass.hpp"

ConstantBufferClass::ConstantBufferClass()
{
	this->ConstantBuffer = nullptr;
}
ConstantBufferClass::~ConstantBufferClass()
{

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ConstantBufferClass::ReleaseAll()
{
	this->ConstantBuffer->Release();
}





MatrixBufferStored *ConstantBufferClass::GetFormattedMatrixData()
{
	return &this->FormattedMatrixData;
}
ID3D11Buffer* *ConstantBufferClass::GetConstantBuffer()
{
	return &this->ConstantBuffer;
}





void ConstantBufferClass::Initialize(
	ID3D11Device*		 *Device,
	ID3D11DeviceContext* *DeviceContext
)
{
	this->InitializeConstantMatrices();
	this->MatrixToFloat4X4Reformat();
	this->CreateSetConstantBuffers(
		Device,
		DeviceContext
	);
}





void ConstantBufferClass::MatrixToFloat4X4Reformat()
{
	DirectX::XMStoreFloat4x4(&this->FormattedMatrixData.world, this->UnformattedMatrixData.world);
	DirectX::XMStoreFloat4x4(&this->FormattedMatrixData.view, this->UnformattedMatrixData.view);
	DirectX::XMStoreFloat4x4(&this->FormattedMatrixData.projection, this->UnformattedMatrixData.projection);
}





void ConstantBufferClass::AddMaterialDataToFormattedStruct(
	ObjectDataClass objectData
)
{
	this->FormattedMatrixData.Kd =
	{ objectData.KdValues.x,
		objectData.KdValues.y,
		objectData.KdValues.z,
		1.0f };
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PRIVATE FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void ConstantBufferClass::InitializeConstantMatrices()
{
	//WORLD MATRIX
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

	//PROJECTION MATRIX
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(PI*ANGLE,
		SCREEN_WIDTH / SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);

	//Here we supply the global constant buffer with data
	this->UnformattedMatrixData.world = worldMatrix;
	this->UnformattedMatrixData.projection = projectionMatrix;
}





void ConstantBufferClass::CreateSetConstantBuffers(
	ID3D11Device*			*Device,
	ID3D11DeviceContext*	*DeviceContext
)
{
	// BUFFER DESCRIPTION: 'Settings'
	D3D11_BUFFER_DESC cbDesc;
	memset(&cbDesc, 0, sizeof(cbDesc));
	cbDesc.ByteWidth = sizeof(MatrixBufferStored);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;						// Needs to be DYNAMIC so that we can Map/Unmap,
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			// via ~AlterConstantBuffers()
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// SUBRESOURCE DATA: Package the data
	D3D11_SUBRESOURCE_DATA InitData;
	memset(&InitData, 0, sizeof(InitData));
	InitData.pSysMem = &this->FormattedMatrixData;				// Meant to recieve data - not create.
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Create buffer
	(*Device)->CreateBuffer(&cbDesc, &InitData, &this->ConstantBuffer);

	// Set buffer
	(*DeviceContext)->VSSetConstantBuffers(0, 1, &this->ConstantBuffer);
	(*DeviceContext)->GSSetConstantBuffers(0, 1, &this->ConstantBuffer);
	(*DeviceContext)->PSSetConstantBuffers(0, 1, &this->ConstantBuffer);
	// Current GS-ConstantBuffer slots occupied:
	// 0 - ConstantBufferClass (above)
	// 1 - LightBuffer.
	// 2 - Empty, etc...
}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									NON-CLASS FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void AlterConstantBuffers(
	ID3D11Buffer* TargetBuffer,
	MatrixBufferStored TargetStruct,
	ID3D11DeviceContext* *DeviceContext
)
{
	D3D11_MAPPED_SUBRESOURCE MappedBuffer;
	(*DeviceContext)->Map(TargetBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedBuffer);
	memcpy(MappedBuffer.pData, &TargetStruct, sizeof(TargetStruct));
	(*DeviceContext)->Unmap(TargetBuffer, 0);
}